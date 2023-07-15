#include "timerNode.h"

bool timerNode::isRegistered = []()
{
	NODE_FACTORY.RegisterNodeType("timerNode",
		[]() -> VisualNode* {
			return new timerNode();
		},

		[](const VisualNode& Node) -> VisualNode* {
			const timerNode& NodeToCopy = static_cast<const timerNode&>(Node);
			return new timerNode(NodeToCopy);
		}
	);

	return true;
}();

timerNode::timerNode() : basicLogicNode()
{
	Type = "timerNode";

	SetStyle(VISUAL_NODE_STYLE_DEFAULT);

	SetSize(ImVec2(300, 120));
	SetName("Timer node");

	TitleBackgroundColor = ImColor(31, 117, 208);
	TitleBackgroundColorHovered = ImColor(35, 145, 255);

	AddSocket(new NodeSocket(this, "EXECUTE", "Check time", false));
	AddSocket(new NodeSocket(this, "EXECUTE", "Set time", false));
	AddSocket(new NodeSocket(this, "INT", "time input", false));

	AddSocket(new NodeSocket(this, "INT", "Time left", true));
	Output[0]->SetFunctionToOutputData(IntDataGetter);
	//AddSocket(new TimerLeftSocket(this, "INT", "Time left", true));
	AddSocket(new NodeSocket(this, "EXECUTE", "Finished", true));
	AddSocket(new NodeSocket(this, "EXECUTE", "Not finished", true));
}

timerNode::timerNode(const timerNode& Src) : basicLogicNode(Src)
{
	SetStyle(VISUAL_NODE_STYLE_DEFAULT);
	Data = Src.Data;

	// Here I am restoring the output data function.
	// Because the function is not serializable, I have to set it manually.
	Output[0]->SetFunctionToOutputData(IntDataGetter);
}

Json::Value timerNode::ToJson()
{
	Json::Value Result = VisualNode::ToJson();
	Result["timerNode_Data"] = Data;
	return Result;
}

void timerNode::FromJson(Json::Value Json)
{
	VisualNode::FromJson(Json);
	Data = Json["timerNode_Data"].asInt();

	// Here I am restoring the output data function.
	// Because the function is not serializable, I have to set it manually.
	Output[0]->SetFunctionToOutputData(IntDataGetter);
}

int timerNode::GetTimeLeft()
{
	Data -= TIME.EndTimeStamp(GetID());
	if (Data < 0)
	{
		Data = 0;
	}
	else
	{
		TIME.BeginTimeStamp(GetID());
	}
		
	return Data;
}

void timerNode::SetTimeLeft(int TimeInMS)
{
	if (TimeInMS < 1)
		TimeInMS = 1;

	Data = TimeInMS;

	TIME.BeginTimeStamp(GetID());
}

void timerNode::Draw()
{	
	VisualNode::Draw();
}

void timerNode::SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, VISUAL_NODE_SOCKET_EVENT EventType)
{
	VisualNode::SocketEvent(OwnSocket,  ConnectedSocket, EventType);

	if (EventType == VISUAL_NODE_SOCKET_EXECUTE)
	{
		if (OwnSocket == Input[0])
		{
			int TimeLeft = GetTimeLeft();

			if (TimeLeft == 0)
			{
				if (Output[1]->GetConnections().size() > 0)
					ParentArea->TriggerSocketEvent(Output[1], Output[1]->GetConnections()[0], VISUAL_NODE_SOCKET_EXECUTE);
			}
			else
			{
				if (Output[2]->GetConnections().size() > 0)
					ParentArea->TriggerSocketEvent(Output[2], Output[2]->GetConnections()[0], VISUAL_NODE_SOCKET_EXECUTE);
			}
		}
		else if (OwnSocket == Input[1])
		{
			if (Input[2]->GetConnections().size() > 0)
			{
				void* TempData = Input[2]->GetConnections()[0]->GetData();
				if (TempData != nullptr)
				{
					int Time = reinterpret_cast<int*>(TempData)[0];
					SetTimeLeft(Time);
				}
			}
		}
	}
}

bool timerNode::CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser)
{
	if (!VisualNode::CanConnect(OwnSocket, CandidateSocket, nullptr))
		return false;

	return true;
}

basicLogicNode* timerNode::GetNextNode()
{
	//if (Output.size() > 0 && Output[0]->GetConnections().size() > 0)
	//	return reinterpret_cast<basicLogicNode*>(Output[0]->GetConnections()[0]->GetParent());
	
	return nullptr;
}