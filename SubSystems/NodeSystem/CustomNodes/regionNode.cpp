#include "regionNode.h"
using namespace VisNodeSys;

FETPImage* regionNode::RegionIcon = nullptr;

bool regionNode::isRegistered = []()
{
	NODE_FACTORY.RegisterNodeType("regionNode",
		[]() -> Node* {
			return new regionNode();
		},

		[](const Node& CurrentNode) -> Node* {
			const regionNode& NodeToCopy = static_cast<const regionNode&>(CurrentNode);
			return new regionNode(NodeToCopy);
		}
	);

	return true;
}();

regionNode::regionNode() : basicLogicNode()
{
	SetStyle(CIRCLE);
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

regionNode::regionNode(const regionNode& Src) : basicLogicNode(Src)
{
	Data = Src.Data;
	Begin = reinterpret_cast<beginNode*>(Data->GetNodesByType("beginNode")[0]);
	End = reinterpret_cast<endNode*>(Data->GetNodesByType("endNode")[0]);
	SetStyle(CIRCLE);
}

void regionNode::Draw()
{	
	Node::Draw();

	if (GetStyle() == DEFAULT)
	{
		ImGui::SetCursorScreenPos(ImVec2(ImGui::GetCursorScreenPos().x + 10.0f, ImGui::GetCursorScreenPos().y + NODE_TITLE_HEIGHT + 13.0f));
		ImGui::SetNextItemWidth(140);

		ImGui::Text("REGION NODE");
	}
	else if (GetStyle() == CIRCLE)
	{
		CheckIcons();

		ImGui::SetCursorScreenPos(ImVec2(ImGui::GetCursorScreenPos().x - 4.0f, ImGui::GetCursorScreenPos().y - 4.0f));
		RenderIcon();

		if (ParentArea->IsMouseHovered() && IsHovered() && !actionEditPopup::getInstance().isOpened() && !textInputPopup::getInstance().isOpened())
			ShowTooltip();
	}
}

void regionNode::SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, NODE_SOCKET_EVENT EventType)
{
	Node::SocketEvent(OwnSocket,  ConnectedSocket, EventType);
}

NodeArea* regionNode::GetData()
{
	return Data;
}

bool regionNode::CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser)
{
	if (!Node::CanConnect(OwnSocket, CandidateSocket, nullptr))
		return true;

	return true;
}

basicLogicNode* regionNode::GetNextNode()
{
	End->NextNode = nullptr;
	if (Output.size() > 0 && Output[0]->GetConnectedSockets().size() > 0)
		End->NextNode = reinterpret_cast<basicLogicNode*>(Output[0]->GetConnectedSockets()[0]->GetParent());
	
	return Begin;
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
	Json::Value result = Node::ToJson();
	result["nodeArea"] = Data->ToJson();

	return result;
}

void regionNode::FromJson(Json::Value Json)
{
	Node::FromJson(Json);
	Data = NodeArea::FromJson(Json["nodeArea"].asCString());
}