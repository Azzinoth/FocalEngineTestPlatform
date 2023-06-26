#include "regionNode.h"

VISUAL_NODE_CHILD_CPP(regionNode)
FETPImage* regionNode::RegionIcon = nullptr;

regionNode::regionNode() : VisualNode()
{
	SetStyle(VISUAL_NODE_STYLE_CIRCLE);
	Type = "regionNode";
	Data = NODE_SYSTEM.CreateNodeArea();

	Begin = new beginNode();
	Begin->SetPosition(ImVec2(300.0f, 350.0f));
	Data->AddNode(Begin);

	End = new endNode();
	End->SetPosition(Begin->GetPosition() + ImVec2(600.0f, 0.0f));
	Data->AddNode(End);

	AddSocket(new NodeSocket(this, "FLOAT", "", false));
	AddSocket(new NodeSocket(this, "FLOAT", "", true));
}

regionNode::regionNode(const regionNode& Src) : VisualNode(Src)
{
	Data = Src.Data;
	Begin = reinterpret_cast<beginNode*>(Data->GetNodesByType("beginNode")[0]);
	End = reinterpret_cast<endNode*>(Data->GetNodesByType("endNode")[0]);
	SetStyle(VISUAL_NODE_STYLE_CIRCLE);
}

void regionNode::Draw()
{	
	VisualNode::Draw();

	if (GetStyle() == VISUAL_NODE_STYLE_DEFAULT)
	{
		ImGui::SetCursorScreenPos(ImVec2(ImGui::GetCursorScreenPos().x + 10.0f, ImGui::GetCursorScreenPos().y + NODE_TITLE_HEIGHT + 13.0f));
		ImGui::SetNextItemWidth(140);

		ImGui::Text("REGION NODE");
	}
	else if (GetStyle() == VISUAL_NODE_STYLE_CIRCLE)
	{
		CheckIcons();

		ImGui::SetCursorScreenPos(ImVec2(ImGui::GetCursorScreenPos().x - 4.0f, ImGui::GetCursorScreenPos().y - 4.0f));
		RenderIcon();

		if (ParentArea->IsMouseHovered() && IsHovered() && !actionEditPopup::getInstance().isOpened() && !textInputPopup::getInstance().isOpened())
			ShowTooltip();
	}
}

void regionNode::SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, VISUAL_NODE_SOCKET_EVENT EventType)
{
	VisualNode::SocketEvent(OwnSocket,  ConnectedSocket, EventType);
}

VisualNodeArea* regionNode::GetData()
{
	return Data;
}

bool regionNode::CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser)
{
	if (!VisualNode::CanConnect(OwnSocket, CandidateSocket, nullptr))
		return true;

	return true;
}

VisualNode* regionNode::GetNextNode()
{
	End->NextNode = nullptr;
	if (Output.size() > 0 && Output[0]->GetConnections().size() > 0)
		End->NextNode = Output[0]->GetConnections()[0]->GetParent();
	
	return Begin;
}

VisualNode* regionNode::GetLogicallyNextNode()
{
	return GetNextNode();
}

void regionNode::CheckIcons()
{
	if (RegionIcon == nullptr)
		RegionIcon = new FETPImage("Resources//regionIcon.png");
}

void regionNode::RenderIcon()
{
	ImGui::SetCursorScreenPos(ImVec2(ImGui::GetCursorScreenPos().x + 8.0f, ImGui::GetCursorScreenPos().y + 8.0f));
	ImGui::Image((void*)(intptr_t)RegionIcon->getTextureID(), ImVec2(100.0f, 100.0f), ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f));
}

void regionNode::ShowTooltip()
{
	std::string textToShow = "Region : " + GetName();
	textToShow += "\nNode count : " + std::to_string(Data->GetNodeCount());

	ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
	ImGui::BeginTooltip();
	ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
	ImGui::TextUnformatted(textToShow.c_str());
	ImGui::PopTextWrapPos();
	ImGui::EndTooltip();
	ImGui::PopFont();
}

Json::Value regionNode::ToJson()
{
	Json::Value result = VisualNode::ToJson();
	result["nodeArea"] = Data->ToJson();

	return result;
}

void regionNode::FromJson(Json::Value Json)
{
	VisualNode::FromJson(Json);
	Data = VisualNodeArea::FromJson(Json["nodeArea"].asCString());
}