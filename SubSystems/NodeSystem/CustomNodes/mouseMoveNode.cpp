#include "mouseMoveNode.h"

bool mouseMoveNode::isRegistered = []()
{
	NODE_FACTORY.RegisterNodeType("mouseMoveNode",
		[]() -> VisualNode* {
			return new mouseMoveNode();
		},

		[](const VisualNode& Node) -> VisualNode* {
			const mouseMoveNode& NodeToCopy = static_cast<const mouseMoveNode&>(Node);
			return new mouseMoveNode(NodeToCopy);
		}
	);

	return true;
}();

mouseMoveNode::mouseMoveNode() : basicLogicNode()
{
	Type = "mouseMoveNode";

	SetStyle(VISUAL_NODE_STYLE_DEFAULT);

	SetSize(ImVec2(250, 90));
	SetName("Mouse move node");

	TitleBackgroundColor = ImColor(31, 117, 208);
	TitleBackgroundColorHovered = ImColor(35, 145, 255);

	AddSocket(new NodeSocket(this, "EXECUTE", "", false));
	AddSocket(new NodeSocket(this, "VEC2", "Position", false));

	AddSocket(new NodeSocket(this, "EXECUTE", "Out", true));
}

mouseMoveNode::mouseMoveNode(const mouseMoveNode& Src) : basicLogicNode(Src)
{
	SetStyle(VISUAL_NODE_STYLE_DEFAULT);
	Data = Src.Data;
}

Json::Value mouseMoveNode::ToJson()
{
	Json::Value Result = VisualNode::ToJson();

	if (Input[1]->GetConnectedSockets().empty())
	{
		Result["mouseMoveNode_Data_x"] = Data.x;
		Result["mouseMoveNode_Data_y"] = Data.y;
	}
	
	return Result;
}

void mouseMoveNode::FromJson(Json::Value Json)
{
	VisualNode::FromJson(Json);

	if (Json.isMember("mouseMoveNode_Data_x") && Json.isMember("mouseMoveNode_Data_y"))
	{
		Data.x = Json["mouseMoveNode_Data_x"].asInt();
		Data.y = Json["mouseMoveNode_Data_y"].asInt();
	}
}

void mouseMoveNode::Draw()
{	
	VisualNode::Draw();

	float Zoom = ParentArea->GetZoomFactor();

	ImGui::SetCursorScreenPos(ImVec2(ImGui::GetCursorScreenPos().x + 50.0f * Zoom, ImGui::GetCursorScreenPos().y + 45.0f * Zoom));

	ImGui::SetNextItemWidth(140 * Zoom);
	static int position[] = { 0, 0 };
	position[0] = Data.x;
	position[1] = Data.y;

	ImGui::BeginDisabled(Input[1]->GetConnectedSockets().size() != 0);
	if (ImGui::InputInt2("##Position", position))
	{
		Data = glm::vec2(position[0], position[1]);
	}
	ImGui::EndDisabled();
}

void mouseMoveNode::SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, VISUAL_NODE_SOCKET_EVENT EventType)
{
	VisualNode::SocketEvent(OwnSocket,  ConnectedSocket, EventType);

	if (EventType == VISUAL_NODE_SOCKET_UPDATE || EventType == VISUAL_NODE_SOCKET_EXECUTE)
	{
		if (Input[1]->GetConnectedSockets().size() > 0)
		{
			void* TempData = Input[1]->GetConnectedSockets()[0]->GetData();
			if (TempData != nullptr)
				Data = *reinterpret_cast<glm::vec2*>(TempData);
		}
	}

	if (EventType == VISUAL_NODE_SOCKET_EXECUTE)
	{
		INPUT_SYSTEM.mouseMoveTo(Data.x, Data.y);

		if (Output[0]->GetConnectedSockets().size() > 0)
			ParentArea->TriggerSocketEvent(Output[0], Output[0]->GetConnectedSockets()[0], VISUAL_NODE_SOCKET_EXECUTE);
	}
}

bool mouseMoveNode::CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser)
{
	if (!VisualNode::CanConnect(OwnSocket, CandidateSocket, nullptr))
		return false;

	return true;
}

basicLogicNode* mouseMoveNode::GetNextNode()
{
	return nullptr;
}