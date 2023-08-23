#include "vec2Node.h"
using namespace VisNodeSys;

bool vec2Node::isRegistered = []()
{
	NODE_FACTORY.RegisterNodeType("vec2Node",
		[]() -> Node* {
			return new vec2Node();
		},

		[](const Node& CurrentNode) -> Node* {
			const vec2Node& NodeToCopy = static_cast<const vec2Node&>(CurrentNode);
			return new vec2Node(NodeToCopy);
		}
	);

	return true;
}();

vec2Node::vec2Node() : basicLogicNode()
{
	Type = "vec2Node";

	SetStyle(DEFAULT);

	SetSize(ImVec2(210, 78));
	SetName("vec2 node");

	TitleBackgroundColor = ImColor(31, 117, 208);
	TitleBackgroundColorHovered = ImColor(35, 145, 255);

	AddSocket(new NodeSocket(this, "VEC2", "Out", true));
	Output[0]->SetFunctionToOutputData(Vec2DataGetter);
}

vec2Node::vec2Node(const vec2Node& Src) : basicLogicNode(Src)
{
	SetStyle(DEFAULT);
	Data = Src.Data;

	// Here I am restoring the output data function.
	// Because the function is not serializable, I have to set it manually.
	Output[0]->SetFunctionToOutputData(Vec2DataGetter);
}

Json::Value vec2Node::ToJson()
{
	Json::Value Result = Node::ToJson();
	Result["vec2Node_Data_x"] = Data.x;
	Result["vec2Node_Data_y"] = Data.y;
	return Result;
}

void vec2Node::FromJson(Json::Value Json)
{
	Node::FromJson(Json);
	Data.x = Json["vec2Node_Data_x"].asFloat();
	Data.y = Json["vec2Node_Data_y"].asFloat();

	// Here I am restoring the output data function.
	// Because the function is not serializable, I have to set it manually.
	Output[0]->SetFunctionToOutputData(Vec2DataGetter);
}

void vec2Node::Draw()
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

void vec2Node::SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, NODE_SOCKET_EVENT EventType)
{
	Node::SocketEvent(OwnSocket,  ConnectedSocket, EventType);
}

bool vec2Node::CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser)
{
	if (!Node::CanConnect(OwnSocket, CandidateSocket, nullptr))
		return false;

	return true;
}

basicLogicNode* vec2Node::GetNextNode()
{
	return nullptr;
}