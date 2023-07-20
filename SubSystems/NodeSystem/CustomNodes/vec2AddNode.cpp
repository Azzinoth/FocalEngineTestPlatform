#include "vec2AddNode.h"

bool vec2AddNode::isRegistered = []()
{
	NODE_FACTORY.RegisterNodeType("vec2AddNode",
		[]() -> VisualNode* {
			return new vec2AddNode();
		},

		[](const VisualNode& Node) -> VisualNode* {
			const vec2AddNode& NodeToCopy = static_cast<const vec2AddNode&>(Node);
			return new vec2AddNode(NodeToCopy);
		}
	);

	return true;
}();

vec2AddNode::vec2AddNode() : basicLogicNode()
{
	Type = "vec2AddNode";

	SetStyle(VISUAL_NODE_STYLE_DEFAULT);

	SetSize(ImVec2(90, 78));
	SetName("Add vec2");

	TitleBackgroundColor = ImColor(31, 117, 208);
	TitleBackgroundColorHovered = ImColor(35, 145, 255);

	AddSocket(new NodeSocket(this, "VEC2", "", false));
	AddSocket(new NodeSocket(this, "VEC2", "", false));
	//Input[0]->SetFunctionToOutputData(Vec2DataGetter);

	AddSocket(new NodeSocket(this, "VEC2", "", true));
	Output[0]->SetFunctionToOutputData(Vec2AddDataGetter);
}

vec2AddNode::vec2AddNode(const vec2AddNode& Src) : basicLogicNode(Src)
{
	SetStyle(VISUAL_NODE_STYLE_DEFAULT);

	// Here I am restoring the output data function.
	// Because the function is not serializable, I have to set it manually.
	//Input[0]->SetFunctionToOutputData(Vec2DataGetter);
	Output[0]->SetFunctionToOutputData(Vec2AddDataGetter);
}

void vec2AddNode::Draw()
{	
	VisualNode::Draw();

	/*ImGui::SetCursorScreenPos(ImVec2(ImGui::GetCursorScreenPos().x + 10.0f, ImGui::GetCursorScreenPos().y + 45.0f));

	ImGui::SetNextItemWidth(140);
	static int position[] = { 0, 0 };
	position[0] = Data.x;
	position[1] = Data.y;

	if (ImGui::InputInt2("##Position", position))
	{
		Data = glm::vec2(position[0], position[1]);

		if (Output[0]->GetConnections().size() > 0)
			ParentArea->TriggerSocketEvent(Output[0], Output[0]->GetConnections()[0], VISUAL_NODE_SOCKET_UPDATE);
	}*/
}

void vec2AddNode::SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, VISUAL_NODE_SOCKET_EVENT EventType)
{
	VisualNode::SocketEvent(OwnSocket,  ConnectedSocket, EventType);
}

bool vec2AddNode::CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser)
{
	if (!VisualNode::CanConnect(OwnSocket, CandidateSocket, nullptr))
		return false;

	return true;
}

basicLogicNode* vec2AddNode::GetNextNode()
{
	return nullptr;
}