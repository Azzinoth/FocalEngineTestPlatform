#include "MouseMoveNode.h"
using namespace VisNodeSys;

bool MouseMoveNode::bIsRegistered = []()
{
	NODE_FACTORY.RegisterNodeType("MouseMoveNode",
		[]() -> Node* {
			return new MouseMoveNode();
		},

		[](const Node& CurrentNode) -> Node* {
			const MouseMoveNode& NodeToCopy = static_cast<const MouseMoveNode&>(CurrentNode);
			return new MouseMoveNode(NodeToCopy);
		}
	);

	return true;
}();

MouseMoveNode::MouseMoveNode() : BaseExecutionFlowNode()
{
	Type = "MouseMoveNode";

	SetStyle(DEFAULT);

	SetSize(ImVec2(250, 120));
	SetName("Mouse Move");

	TitleBackgroundColor = ImColor(31, 117, 208);
	TitleBackgroundColorHovered = ImColor(35, 145, 255);

	AddSocket(new NodeSocket(this, "VEC2", "Position", NodeSocket::SocketFlow::Input));
	AddSocket(new NodeSocket(this, "INT", "Monitor", NodeSocket::SocketFlow::Input));

	AddSocket(new NodeSocket(this, "EXECUTE", "Out", NodeSocket::SocketFlow::Output));
}

MouseMoveNode::MouseMoveNode(const MouseMoveNode& Other) : BaseExecutionFlowNode(Other)
{
	SetStyle(DEFAULT);
	Data = Other.Data;
	Monitor = Other.Monitor;
}

Json::Value MouseMoveNode::ToJson()
{
	Json::Value Result = Node::ToJson();

	if (Input[1]->GetConnectedSockets().empty())
	{
		Result["MouseMoveNode_Data_x"] = Data.x;
		Result["MouseMoveNode_Data_y"] = Data.y;
	}

	if (Input[2]->GetConnectedSockets().empty())
		Result["MouseMoveNode_Monitor"] = Monitor;
	
	return Result;
}

bool MouseMoveNode::FromJson(Json::Value Json)
{
	bool bResult = Node::FromJson(Json);
	if (!bResult)
		return false;

	if (Json.isMember("MouseMoveNode_Data_x"))
		Data.x = Json["MouseMoveNode_Data_x"].asFloat();

	if (Json.isMember("MouseMoveNode_Data_y"))
		Data.y = Json["MouseMoveNode_Data_y"].asFloat();

	if (Json.isMember("MouseMoveNode_Monitor"))
		Monitor = Json["MouseMoveNode_Monitor"].asInt();

	return true;
}

void MouseMoveNode::Draw()
{	
	Node::Draw();

	float Zoom = ParentArea->GetZoomFactor();

	ImGui::SetCursorScreenPos(ImVec2(ImGui::GetCursorScreenPos().x + 50.0f * Zoom, ImGui::GetCursorScreenPos().y + 45.0f * Zoom));

	ImGui::SetNextItemWidth(140 * Zoom);
	static int Position[] = { 0, 0 };
	Position[0] = static_cast<int>(Data.x);
	Position[1] = static_cast<int>(Data.y);

	ImGui::BeginDisabled(Input[1]->GetConnectedSockets().size() != 0);
	if (ImGui::InputInt2("##Position", Position))
	{
		Data = glm::vec2(Position[0], Position[1]);
	}
	ImGui::EndDisabled();
}

void MouseMoveNode::SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, NODE_SOCKET_EVENT EventType)
{
	Node::SocketEvent(OwnSocket,  ConnectedSocket, EventType);

	if (EventType == UPDATE || EventType == EXECUTE)
	{
		if (Input[1]->GetConnectedSockets().size() > 0)
		{
			void* TemporaryData = Input[1]->GetConnectedSockets()[0]->GetData();
			if (TemporaryData != nullptr)
				Data = *reinterpret_cast<glm::vec2*>(TemporaryData);
		}

		if (Input[2]->GetConnectedSockets().size() > 0)
		{
			void* TemporaryData = Input[2]->GetConnectedSockets()[0]->GetData();
			if (TemporaryData != nullptr)
				Monitor = *reinterpret_cast<int*>(TemporaryData);
		}
	}

	if (EventType == EXECUTE)
	{
		INPUT_SYSTEM.SimulateMouseMoveTo(static_cast<int>(Data.x), static_cast<int>(Data.y), Monitor);

		if (Output[0]->GetConnectedSockets().size() > 0)
			ParentArea->TriggerSocketEvent(Output[0], Output[0]->GetConnectedSockets()[0], EXECUTE);
	}
}

bool MouseMoveNode::CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser)
{
	if (!Node::CanConnect(OwnSocket, CandidateSocket, nullptr))
		return false;

	return true;
}

glm::vec2 MouseMoveNode::GetMouseTargetPosition() const
{
	return Data;
}

void MouseMoveNode::SetMouseTargetPosition(glm::vec2 NewValue)
{
	Data = NewValue;
}