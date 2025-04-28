#include "FloatNode.h"
using namespace VisNodeSys;

bool FloatNode::bIsRegistered = []()
{
	NODE_FACTORY.RegisterNodeType("FloatNode",
		[]() -> Node* {
			return new FloatNode();
		},

		[](const Node& CurrentNode) -> Node* {
			const FloatNode& NodeToCopy = static_cast<const FloatNode&>(CurrentNode);
			return new FloatNode(NodeToCopy);
		}
	);

	return true;
}();

FloatNode::FloatNode() : BasicLogicNode()
{
	Type = "FloatNode";

	SetStyle(DEFAULT);

	SetSize(ImVec2(170, 78));
	SetName("Float node");

	TitleBackgroundColor = ImColor(31, 117, 208);
	TitleBackgroundColorHovered = ImColor(35, 145, 255);

	AddSocket(new NodeSocket(this, "FLOAT", "Out", true));
	Output[0]->SetFunctionToOutputData(FloatDataGetter);
}

FloatNode::FloatNode(const FloatNode& Other) : BasicLogicNode(Other)
{
	SetStyle(DEFAULT);
	Data = Other.Data;

	// Here I am restoring the output data function.
	// Because the function is not serializable, I have to set it manually.
	Output[0]->SetFunctionToOutputData(FloatDataGetter);
}

Json::Value FloatNode::ToJson()
{
	Json::Value Result = Node::ToJson();
	Result["floatNode_Data"] = Data;
	return Result;
}

bool FloatNode::FromJson(Json::Value Json)
{
	bool bResult = Node::FromJson(Json);
	if (!bResult)
		return false;

	if (!Json.isMember("floatNode_Data"))
		return false;

	Data = Json["floatNode_Data"].asFloat();

	// Here I am restoring the output data function.
	// Because the function is not serializable, I have to set it manually.
	if (Output.size() < 1 || Output[0] == nullptr)
		return false;

	Output[0]->SetFunctionToOutputData(FloatDataGetter);

	return true;
}

void FloatNode::Draw()
{	
	Node::Draw();

	float Zoom = ParentArea->GetZoomFactor();

	ImGui::SetCursorScreenPos(ImVec2(ImGui::GetCursorScreenPos().x + 30.0f * Zoom, ImGui::GetCursorScreenPos().y + 45.0f * Zoom));

	float XPosition = ImGui::GetCursorScreenPos().x - 17.0f * Zoom;
	float YPosition = ImGui::GetCursorScreenPos().y + 0.0f * Zoom;

	ImGui::SetCursorScreenPos(ImVec2(XPosition, YPosition));
	ImGui::SetNextItemWidth(100.0f * Zoom);
	if (ImGui::InputFloat("##value", &Data))
	{
		if (Output[0]->GetConnectedSockets().size() > 0)
			ParentArea->TriggerSocketEvent(Output[0], Output[0]->GetConnectedSockets()[0], UPDATE);
	}
}

void FloatNode::SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, NODE_SOCKET_EVENT EventType)
{
	Node::SocketEvent(OwnSocket,  ConnectedSocket, EventType);
}

bool FloatNode::CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser)
{
	if (!Node::CanConnect(OwnSocket, CandidateSocket, nullptr))
		return false;

	return true;
}

BasicLogicNode* FloatNode::GetNextNode()
{
	return nullptr;
}