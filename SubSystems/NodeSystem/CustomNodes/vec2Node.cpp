#include "Vec2Node.h"
using namespace VisNodeSys;

bool Vec2Node::bIsRegistered = []()
{
	NODE_FACTORY.RegisterNodeType("Vec2Node",
		[]() -> Node* {
			return new Vec2Node();
		},

		[](const Node& CurrentNode) -> Node* {
			const Vec2Node& NodeToCopy = static_cast<const Vec2Node&>(CurrentNode);
			return new Vec2Node(NodeToCopy);
		}
	);

	return true;
}();

Vec2Node::Vec2Node() : BasicLogicNode()
{
	Type = "Vec2Node";

	SetStyle(DEFAULT);

	SetSize(ImVec2(210, 78));
	SetName("vec2 node");

	TitleBackgroundColor = ImColor(31, 117, 208);
	TitleBackgroundColorHovered = ImColor(35, 145, 255);

	AddSocket(new NodeSocket(this, "VEC2", "Out", true));
	Output[0]->SetFunctionToOutputData(Vec2DataGetter);
}

Vec2Node::Vec2Node(const Vec2Node& Other) : BasicLogicNode(Other)
{
	SetStyle(DEFAULT);
	Data = Other.Data;

	// Here I am restoring the output data function.
	// Because the function is not serializable, I have to set it manually.
	Output[0]->SetFunctionToOutputData(Vec2DataGetter);
}

Json::Value Vec2Node::ToJson()
{
	Json::Value Result = Node::ToJson();
	Result["Vec2Node_Data_X"] = Data.x;
	Result["Vec2Node_Data_Y"] = Data.y;
	return Result;
}

bool Vec2Node::FromJson(Json::Value Json)
{
	bool bResult = Node::FromJson(Json);
	if (!bResult)
		return false;

	if (!Json.isMember("Vec2Node_Data_X") || !Json.isMember("Vec2Node_Data_Y"))
		return false;

	Data.x = Json["Vec2Node_Data_X"].asFloat();
	Data.y = Json["Vec2Node_Data_Y"].asFloat();

	// Here I am restoring the output data function.
	// Because the function is not serializable, I have to set it manually.
	if (Output.size() < 1 || Output[0] == nullptr)
		return false;

	Output[0]->SetFunctionToOutputData(Vec2DataGetter);

	return true;
}

void Vec2Node::Draw()
{	
	Node::Draw();

	float Zoom = ParentArea->GetZoomFactor();

	ImGui::SetCursorScreenPos(ImVec2(ImGui::GetCursorScreenPos().x + 10.0f * Zoom, ImGui::GetCursorScreenPos().y + 45.0f * Zoom));

	ImGui::SetNextItemWidth(140 * Zoom);
	static int position[] = { 0, 0 };
	position[0] = static_cast<int>(Data.x);
	position[1] = static_cast<int>(Data.y);

	if (ImGui::InputInt2("##Position", position))
	{
		Data = glm::vec2(position[0], position[1]);

		if (Output[0]->GetConnectedSockets().size() > 0)
			ParentArea->TriggerSocketEvent(Output[0], Output[0]->GetConnectedSockets()[0], UPDATE);
	}
}

void Vec2Node::SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, NODE_SOCKET_EVENT EventType)
{
	Node::SocketEvent(OwnSocket,  ConnectedSocket, EventType);
}

bool Vec2Node::CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser)
{
	if (!Node::CanConnect(OwnSocket, CandidateSocket, nullptr))
		return false;

	return true;
}

BasicLogicNode* Vec2Node::GetNextNode()
{
	return nullptr;
}