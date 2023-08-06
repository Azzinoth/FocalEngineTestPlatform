#include "floatNode.h"

bool floatNode::isRegistered = []()
{
	NODE_FACTORY.RegisterNodeType("floatNode",
		[]() -> VisualNode* {
			return new floatNode();
		},

		[](const VisualNode& Node) -> VisualNode* {
			const floatNode& NodeToCopy = static_cast<const floatNode&>(Node);
			return new floatNode(NodeToCopy);
		}
	);

	return true;
}();

floatNode::floatNode() : basicLogicNode()
{
	Type = "floatNode";

	SetStyle(VISUAL_NODE_STYLE_DEFAULT);

	SetSize(ImVec2(170, 78));
	SetName("float node");

	TitleBackgroundColor = ImColor(31, 117, 208);
	TitleBackgroundColorHovered = ImColor(35, 145, 255);

	AddSocket(new NodeSocket(this, "FLOAT", "Out", true));
	Output[0]->SetFunctionToOutputData(FloatDataGetter);
}

floatNode::floatNode(const floatNode& Src) : basicLogicNode(Src)
{
	SetStyle(VISUAL_NODE_STYLE_DEFAULT);
	Data = Src.Data;

	// Here I am restoring the output data function.
	// Because the function is not serializable, I have to set it manually.
	Output[0]->SetFunctionToOutputData(FloatDataGetter);
}

Json::Value floatNode::ToJson()
{
	Json::Value Result = VisualNode::ToJson();
	Result["floatNode_Data"] = Data;
	return Result;
}

void floatNode::FromJson(Json::Value Json)
{
	VisualNode::FromJson(Json);
	Data = Json["floatNode_Data"].asInt();

	// Here I am restoring the output data function.
	// Because the function is not serializable, I have to set it manually.
	Output[0]->SetFunctionToOutputData(FloatDataGetter);
}

void floatNode::Draw()
{	
	VisualNode::Draw();

	float Zoom = ParentArea->GetZoomFactor();

	ImGui::SetCursorScreenPos(ImVec2(ImGui::GetCursorScreenPos().x + 30.0f * Zoom, ImGui::GetCursorScreenPos().y + 45.0f * Zoom));

	int xPosition = ImGui::GetCursorScreenPos().x - 17.0f * Zoom;
	int yPosition = ImGui::GetCursorScreenPos().y + 0.0f * Zoom;

	ImGui::SetCursorScreenPos(ImVec2(xPosition, yPosition));
	ImGui::SetNextItemWidth(100 * Zoom);
	if (ImGui::InputFloat("##value", &Data))
	{
		if (Output[0]->GetConnectedSockets().size() > 0)
			ParentArea->TriggerSocketEvent(Output[0], Output[0]->GetConnectedSockets()[0], VISUAL_NODE_SOCKET_UPDATE);
	}
}

void floatNode::SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, VISUAL_NODE_SOCKET_EVENT EventType)
{
	VisualNode::SocketEvent(OwnSocket,  ConnectedSocket, EventType);
}

bool floatNode::CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser)
{
	if (!VisualNode::CanConnect(OwnSocket, CandidateSocket, nullptr))
		return false;

	return true;
}

basicLogicNode* floatNode::GetNextNode()
{
	return nullptr;
}