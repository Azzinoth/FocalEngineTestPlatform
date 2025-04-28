#include "IntNode.h"
using namespace VisNodeSys;

bool IntNode::bIsRegistered = []()
{
	NODE_FACTORY.RegisterNodeType("IntNode",
		[]() -> Node* {
			return new IntNode();
		},

		[](const Node& CurrentNode) -> Node* {
			const IntNode& NodeToCopy = static_cast<const IntNode&>(CurrentNode);
			return new IntNode(NodeToCopy);
		}
	);

	return true;
}();

IntNode::IntNode() : BasicLogicNode()
{
	Type = "IntNode";

	SetStyle(DEFAULT);

	SetSize(ImVec2(170, 78));
	SetName("int node");

	TitleBackgroundColor = ImColor(31, 117, 208);
	TitleBackgroundColorHovered = ImColor(35, 145, 255);

	AddSocket(new NodeSocket(this, "INT", "Out", true));
	Output[0]->SetFunctionToOutputData(IntDataGetter);
}

IntNode::IntNode(const IntNode& Other) : BasicLogicNode(Other)
{
	SetStyle(DEFAULT);
	Data = Other.Data;

	// Here I am restoring the output data function.
	// Because the function is not serializable, I have to set it manually.
	Output[0]->SetFunctionToOutputData(IntDataGetter);
}

Json::Value IntNode::ToJson()
{
	Json::Value Result = Node::ToJson();
	Result["intNode_Data"] = Data;
	return Result;
}

bool IntNode::FromJson(Json::Value Json)
{
	bool bResult = Node::FromJson(Json);
	if (!bResult)
		return false;

	if (!Json.isMember("intNode_Data"))
		return false;

	Data = Json["intNode_Data"].asInt();

	// Here I am restoring the output data function.
	// Because the function is not serializable, I have to set it manually.
	if (Output.size() < 1)
		return false;

	if (Output[0] == nullptr)
		return false;

	Output[0]->SetFunctionToOutputData(IntDataGetter);

	return true;
}

void IntNode::Draw()
{	
	Node::Draw();

	float Zoom = ParentArea->GetZoomFactor();

	ImGui::SetCursorScreenPos(ImVec2(ImGui::GetCursorScreenPos().x + 30.0f * Zoom, ImGui::GetCursorScreenPos().y + 45.0f * Zoom));

	float XPosition = ImGui::GetCursorScreenPos().x - 17.0f * Zoom;
	float YPosition = ImGui::GetCursorScreenPos().y + 0.0f * Zoom;

	ImGui::SetCursorScreenPos(ImVec2(XPosition, YPosition));
	ImGui::SetNextItemWidth(100 * Zoom);
	if (ImGui::InputInt("##value", &Data))
	{
		if (Output[0]->GetConnectedSockets().size() > 0)
			ParentArea->TriggerSocketEvent(Output[0], Output[0]->GetConnectedSockets()[0], UPDATE);
	}
}

void IntNode::SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, NODE_SOCKET_EVENT EventType)
{
	Node::SocketEvent(OwnSocket,  ConnectedSocket, EventType);
}

bool IntNode::CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser)
{
	if (!Node::CanConnect(OwnSocket, CandidateSocket, nullptr))
		return false;

	return true;
}

BasicLogicNode* IntNode::GetNextNode()
{
	return nullptr;
}