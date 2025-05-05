#include "TimerNode.h"
using namespace VisNodeSys;
using namespace FocalEngine;

bool TimerNode::bIsRegistered = []()
{
	NODE_FACTORY.RegisterNodeType("TimerNode",
		[]() -> Node* {
			return new TimerNode();
		},

		[](const Node& CurrentNode) -> Node* {
			const TimerNode& NodeToCopy = static_cast<const TimerNode&>(CurrentNode);
			return new TimerNode(NodeToCopy);
		}
	);

	return true;
}();

TimerNode::TimerNode() : BaseExecutionFlowNode()
{
	Type = "TimerNode";

	SetStyle(DEFAULT);

	SetSize(ImVec2(300, 120));
	SetName("Timer node");

	TitleBackgroundColor = ImColor(31, 117, 208);
	TitleBackgroundColorHovered = ImColor(35, 145, 255);

	AddSocket(new NodeSocket(this, "EXECUTE", "Check time", false));
	AddSocket(new NodeSocket(this, "EXECUTE", "Set time", false));
	AddSocket(new NodeSocket(this, "INT", "time input", false));

	AddSocket(new NodeSocket(this, "INT", "Time left", true));
	Output[0]->SetFunctionToOutputData(IntDataGetter);
	AddSocket(new NodeSocket(this, "EXECUTE", "Finished", true));
	AddSocket(new NodeSocket(this, "EXECUTE", "Not finished", true));
}

TimerNode::TimerNode(const TimerNode& Other) : BaseExecutionFlowNode(Other)
{
	SetStyle(DEFAULT);
	Data = Other.Data;

	// Here I am restoring the output data function.
	// Because the function is not serializable, I have to set it manually.
	Output[0]->SetFunctionToOutputData(IntDataGetter);
}

Json::Value TimerNode::ToJson()
{
	Json::Value Result = Node::ToJson();
	Result["TimerNode_Data"] = Data;
	return Result;
}

bool TimerNode::FromJson(Json::Value Json)
{
	bool bResult = Node::FromJson(Json);
	if (!bResult)
		return false;

	if (!Json.isMember("TimerNode_Data"))
		return false;

	Data = Json["TimerNode_Data"].asInt();

	// Here I am restoring the output data function.
	// Because the function is not serializable, I have to set it manually.
	if (Output.size() < 1 || Output[0] == nullptr)
		return false;

	Output[0]->SetFunctionToOutputData(IntDataGetter);

	return true;
}

int TimerNode::GetTimeLeft()
{
	Data -= static_cast<int>(TIME.EndTimeStamp(GetID()));
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

void TimerNode::SetTimeLeft(int TimeInMS)
{
	if (TimeInMS < 1)
		TimeInMS = 1;

	Data = TimeInMS;

	TIME.BeginTimeStamp(GetID());
}

void TimerNode::Draw()
{	
	Node::Draw();
}

void TimerNode::SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, NODE_SOCKET_EVENT EventType)
{
	Node::SocketEvent(OwnSocket,  ConnectedSocket, EventType);

	if (EventType == EXECUTE)
	{
		if (OwnSocket == Input[0])
		{
			int TimeLeft = GetTimeLeft();

			if (TimeLeft == 0)
			{
				if (Output[1]->GetConnectedSockets().size() > 0)
					ParentArea->TriggerSocketEvent(Output[1], Output[1]->GetConnectedSockets()[0], EXECUTE);
			}
			else
			{
				if (Output[2]->GetConnectedSockets().size() > 0)
					ParentArea->TriggerSocketEvent(Output[2], Output[2]->GetConnectedSockets()[0], EXECUTE);
			}
		}
		else if (OwnSocket == Input[1])
		{
			if (Input[2]->GetConnectedSockets().size() > 0)
			{
				void* TempData = Input[2]->GetConnectedSockets()[0]->GetData();
				if (TempData != nullptr)
				{
					int Time = reinterpret_cast<int*>(TempData)[0];
					SetTimeLeft(Time);
				}
			}
		}
	}
}

bool TimerNode::CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser)
{
	if (!Node::CanConnect(OwnSocket, CandidateSocket, nullptr))
		return false;

	return true;
}