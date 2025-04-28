#include "SequenceNode.h"
using namespace VisNodeSys;

bool SequenceNode::bIsRegistered = []()
{
	NODE_FACTORY.RegisterNodeType("SequenceNode",
		[]() -> Node* {
			return new SequenceNode();
		},

		[](const Node& CurrentNode) -> Node* {
			const SequenceNode& NodeToCopy = static_cast<const SequenceNode&>(CurrentNode);
			return new SequenceNode(NodeToCopy);
		}
	);

	return true;
}();

SequenceNode::SequenceNode() : BasicLogicNode()
{
	Type = "SequenceNode";

	SetStyle(DEFAULT);

	SetSize(ImVec2(150, 78));
	SetName("sequence node");

	TitleBackgroundColor = ImColor(31, 117, 208);
	TitleBackgroundColorHovered = ImColor(35, 145, 255);

	AddSocket(new NodeSocket(this, "EXECUTE", "", false));
}

SequenceNode::SequenceNode(const SequenceNode& Other) : BasicLogicNode(Other)
{
	SetStyle(DEFAULT);
	Data = Other.Data;
}

void SequenceNode::AddSequenceOutput()
{
	AddSocket(new NodeSocket(this, "EXECUTE", "Then " + std::to_string(Output.size()), true));
}

Json::Value SequenceNode::ToJson()
{
	Json::Value Result = Node::ToJson();
	Result["sequenceNode_Data"] = Data;
	return Result;
}

bool SequenceNode::FromJson(Json::Value Json)
{
	bool bResult = Node::FromJson(Json);
	if (!bResult)
		return false;

	if (!Json.isMember("sequenceNode_Data"))
		return false;

	Data = Json["sequenceNode_Data"].asInt();

	return true;
}

void SequenceNode::Draw()
{	
	Node::Draw();

	float Zoom = ParentArea->GetZoomFactor();

	ImGui::SetCursorScreenPos(ImVec2(ImGui::GetCursorScreenPos().x + 30.0f * Zoom, ImGui::GetCursorScreenPos().y + (Size.y - 25.0f) * Zoom));
	if (ImGui::Button("Add"))
	{
		Data++;
		AddSequenceOutput();
	}

	SetSize(ImVec2(Size.x, 100 + Output.size() * 30.0f));
}

void SequenceNode::SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, NODE_SOCKET_EVENT EventType)
{
	Node::SocketEvent(OwnSocket,  ConnectedSocket, EventType);

	if (EventType == EXECUTE)
	{
		for (size_t i = 0; i < Data; i++)
		{
			if (Output[i]->GetConnectedSockets().size() > 0)
				ParentArea->TriggerSocketEvent(Output[i], Output[i]->GetConnectedSockets()[0], EXECUTE);
		}
	}
}

bool SequenceNode::CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser)
{
	if (!Node::CanConnect(OwnSocket, CandidateSocket, nullptr))
		return false;

	return true;
}

BasicLogicNode* SequenceNode::GetNextNode()
{
	return nullptr;
}