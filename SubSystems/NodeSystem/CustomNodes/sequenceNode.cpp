#include "sequenceNode.h"

bool sequenceNode::isRegistered = []()
{
	NODE_FACTORY.RegisterNodeType("sequenceNode",
		[]() -> VisualNode* {
			return new sequenceNode();
		},

		[](const VisualNode& Node) -> VisualNode* {
			const sequenceNode& NodeToCopy = static_cast<const sequenceNode&>(Node);
			return new sequenceNode(NodeToCopy);
		}
	);

	return true;
}();

sequenceNode::sequenceNode() : basicLogicNode()
{
	Type = "sequenceNode";

	SetStyle(VISUAL_NODE_STYLE_DEFAULT);

	SetSize(ImVec2(150, 78));
	SetName("sequence node");

	TitleBackgroundColor = ImColor(31, 117, 208);
	TitleBackgroundColorHovered = ImColor(35, 145, 255);

	AddSocket(new NodeSocket(this, "EXECUTE", "", false));
}

sequenceNode::sequenceNode(const sequenceNode& Src) : basicLogicNode(Src)
{
	SetStyle(VISUAL_NODE_STYLE_DEFAULT);
	Data = Src.Data;
}

void sequenceNode::AddSequenceOutput()
{
	AddSocket(new NodeSocket(this, "EXECUTE", "Then " + std::to_string(Output.size()), true));
}

Json::Value sequenceNode::ToJson()
{
	Json::Value Result = VisualNode::ToJson();
	Result["sequenceNode_Data"] = Data;
	return Result;
}

void sequenceNode::FromJson(Json::Value Json)
{
	VisualNode::FromJson(Json);
	Data = Json["sequenceNode_Data"].asInt();

	/*for (size_t i = 0; i < Output.size(); i++)
	{
		delete Output[i];
	}
	Output.clear();

	for (size_t i = 0; i < Data; i++)
	{
		AddSequenceOutput();
	}*/
}

void sequenceNode::Draw()
{	
	VisualNode::Draw();

	ImGui::SetCursorScreenPos(ImVec2(ImGui::GetCursorScreenPos().x + 30.0f, ImGui::GetCursorScreenPos().y + Size.y - 25.0f));
	if (ImGui::Button("Add"))
	{
		Data++;
		AddSequenceOutput();
	}

	SetSize(ImVec2(Size.x, 100 + Output.size() * 30.0f));
}

void sequenceNode::SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, VISUAL_NODE_SOCKET_EVENT EventType)
{
	VisualNode::SocketEvent(OwnSocket,  ConnectedSocket, EventType);

	if (EventType == VISUAL_NODE_SOCKET_EXECUTE)
	{
		for (size_t i = 0; i < Data; i++)
		{
			if (Output[i]->GetConnections().size() > 0)
				ParentArea->TriggerSocketEvent(Output[i], Output[i]->GetConnections()[0], VISUAL_NODE_SOCKET_EXECUTE);
		}
	}
}

bool sequenceNode::CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser)
{
	if (!VisualNode::CanConnect(OwnSocket, CandidateSocket, nullptr))
		return false;

	return true;
}

basicLogicNode* sequenceNode::GetNextNode()
{
	return nullptr;
}