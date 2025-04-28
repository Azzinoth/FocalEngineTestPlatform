#include "boolNode.h"
using namespace VisNodeSys;

bool BoolNode::bIsRegistered = []()
{
	NODE_FACTORY.RegisterNodeType("BoolNode",
		[]() -> Node* {
			return new BoolNode();
		},

		[](const Node& CurrentNode) -> Node* {
			const BoolNode& NodeToCopy = static_cast<const BoolNode&>(CurrentNode);
			return new BoolNode(NodeToCopy);
		}
	);

	return true;
}();

BoolNode::BoolNode() : BasicLogicNode()
{
	Type = "BoolNode";

	SetStyle(DEFAULT);

	SetSize(ImVec2(150, 78));
	SetName("bool node");

	TitleBackgroundColor = ImColor(31, 117, 208);
	TitleBackgroundColorHovered = ImColor(35, 145, 255);

	AddSocket(new NodeSocket(this, "BOOL", "Out", true));
	Output[0]->SetFunctionToOutputData(BoolDataGetter);
}

BoolNode::BoolNode(const BoolNode& Other) : BasicLogicNode(Other)
{
	SetStyle(DEFAULT);
	bData = Other.bData;

	// Here I am restoring the output data function.
	// Because the function is not serializable, I have to set it manually.
	Output[0]->SetFunctionToOutputData(BoolDataGetter);
}

Json::Value BoolNode::ToJson()
{
	Json::Value Result = Node::ToJson();
	Result["boolNode_Data"] = bData;
	return Result;
}

bool BoolNode::FromJson(Json::Value Json)
{
	bool bResult = Node::FromJson(Json);
	if (!bResult)
		return false;

	if (!Json.isMember("boolNode_Data"))
		return false;

	bData = Json["boolNode_Data"].asBool();

	// Here I am restoring the output data function.
	// Because the function is not serializable, I have to set it manually.
	if (Output.size() < 1)
		return false;

	if (Output[0] == nullptr)
		return false;

	Output[0]->SetFunctionToOutputData(BoolDataGetter);

	return true;
}

void BoolNode::Draw()
{	
	Node::Draw();

	float Zoom = ParentArea->GetZoomFactor();

	ImGui::SetCursorScreenPos(ImVec2(ImGui::GetCursorScreenPos().x + 30.0f * Zoom, ImGui::GetCursorScreenPos().y + 45.0f * Zoom));
	ImGui::Checkbox("True", &bData);
}

void BoolNode::SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, NODE_SOCKET_EVENT EventType)
{
	Node::SocketEvent(OwnSocket,  ConnectedSocket, EventType);
}

bool BoolNode::CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser)
{
	if (!Node::CanConnect(OwnSocket, CandidateSocket, nullptr))
		return false;

	return true;
}

BasicLogicNode* BoolNode::GetNextNode()
{
	return nullptr;
}