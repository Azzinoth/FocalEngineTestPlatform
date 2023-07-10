#include "intNode.h"

bool intNode::isRegistered = []()
{
	NODE_FACTORY.RegisterNodeType("intNode",
		[]() -> VisualNode* {
			return new intNode();
		},

		[](const VisualNode& Node) -> VisualNode* {
			const intNode& NodeToCopy = static_cast<const intNode&>(Node);
			return new intNode(NodeToCopy);
		}
	);

	return true;
}();

intNode::intNode() : basicLogicNode()
{
	Type = "intNode";

	SetStyle(VISUAL_NODE_STYLE_DEFAULT);

	SetSize(ImVec2(170, 78));
	SetName("int node");

	TitleBackgroundColor = ImColor(31, 117, 208);
	TitleBackgroundColorHovered = ImColor(35, 145, 255);

	AddSocket(new IntSocket(this, "INT", "Out", true));
}

intNode::intNode(const intNode& Src) : basicLogicNode(Src)
{
	SetStyle(VISUAL_NODE_STYLE_DEFAULT);
	Data = Src.Data;

	delete Output[0];
	Output.clear();
	AddSocket(new IntSocket(this, "INT", "Out", true));
}

Json::Value intNode::ToJson()
{
	Json::Value Result = VisualNode::ToJson();
	Result["intNode_Data"] = Data;
	return Result;
}

void intNode::FromJson(Json::Value Json)
{
	VisualNode::FromJson(Json);
	Data = Json["intNode_Data"].asInt();
}

void intNode::Draw()
{	
	VisualNode::Draw();

	ImGui::SetCursorScreenPos(ImVec2(ImGui::GetCursorScreenPos().x + 30.0f, ImGui::GetCursorScreenPos().y + 45.0f));

	int xPosition = ImGui::GetCursorScreenPos().x - 17.0f;
	int yPosition = ImGui::GetCursorScreenPos().y + 0.0f;

	ImGui::SetCursorScreenPos(ImVec2(xPosition, yPosition));
	ImGui::SetNextItemWidth(100);
	ImGui::InputInt("##value", &Data);
}

void intNode::SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, VISUAL_NODE_SOCKET_EVENT EventType)
{
	VisualNode::SocketEvent(OwnSocket,  ConnectedSocket, EventType);
}

bool intNode::CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser)
{
	if (!VisualNode::CanConnect(OwnSocket, CandidateSocket, nullptr))
		return false;

	return true;
}

basicLogicNode* intNode::GetNextNode()
{
	return nullptr;
}