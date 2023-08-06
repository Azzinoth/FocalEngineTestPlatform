#include "beginNode.h"

bool beginNode::isRegistered = []()
{
	NODE_FACTORY.RegisterNodeType("beginNode",
		[]() -> VisualNode* {
			return new beginNode();
		},

		[](const VisualNode& Node) -> VisualNode* {
			const beginNode& NodeToCopy = static_cast<const beginNode&>(Node);
			return new beginNode(NodeToCopy);
		}
	);

	return true;
}();

beginNode::beginNode() : basicLogicNode()
{
	Type = "beginNode";
	bCouldBeDestroyed = false;

	SetStyle(VISUAL_NODE_STYLE_CIRCLE);

	SetSize(ImVec2(220, 78));
	SetName("beginNode");

	TitleBackgroundColor = ImColor(31, 117, 208);
	TitleBackgroundColorHovered = ImColor(35, 145, 255);
	
	AddSocket(new NodeSocket(this, "EXECUTE", "out", true));

	if (Icon == nullptr)
		Icon = new FETPImage("Resources//beginNodeIcon.png");
}

beginNode::beginNode(const beginNode& Src) : basicLogicNode(Src)
{
	Data = Src.Data;
	bCouldBeDestroyed = false;

	SetStyle(VISUAL_NODE_STYLE_CIRCLE);

	if (Icon == nullptr)
		Icon = new FETPImage("Resources//beginNodeIcon.png");
}

void beginNode::Draw()
{	
	VisualNode::Draw();

	float Zoom = ParentArea->GetZoomFactor();

	ImGui::SetCursorScreenPos(ImVec2(ImGui::GetCursorScreenPos().x - 4.0f * Zoom, ImGui::GetCursorScreenPos().y - 4.0f * Zoom));
	ImGui::Image((void*)(intptr_t)Icon->getTextureID(), ImVec2(116.0f, 116.0f) * Zoom, ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f));
}

void beginNode::SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, VISUAL_NODE_SOCKET_EVENT EventType)
{
	VisualNode::SocketEvent(OwnSocket,  ConnectedSocket, EventType);

	if (EventType == VISUAL_NODE_SOCKET_EXECUTE)
	{
		if (Output[0]->GetConnectedSockets().size() > 0)
			ParentArea->TriggerSocketEvent(Output[0], Output[0]->GetConnectedSockets()[0], VISUAL_NODE_SOCKET_EXECUTE);
	}
}

float beginNode::GetData()
{
	return Data;
}

bool beginNode::CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser)
{
	if (!VisualNode::CanConnect(OwnSocket, CandidateSocket, nullptr))
		return false;

	return false;
}

basicLogicNode* beginNode::GetNextNode()
{
	if (Output.size() > 0 && Output[0]->GetConnectedSockets().size() > 0)
		return reinterpret_cast<basicLogicNode*>(Output[0]->GetConnectedSockets()[0]->GetParent());
	
	return nullptr;
}