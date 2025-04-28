#include "RegionNode.h"
using namespace VisNodeSys;

FETPImage* RegionNode::RegionIcon = nullptr;

bool RegionNode::bIsRegistered = []()
{
	NODE_FACTORY.RegisterNodeType("RegionNode",
		[]() -> Node* {
			return new RegionNode();
		},

		[](const Node& CurrentNode) -> Node* {
			const RegionNode& NodeToCopy = static_cast<const RegionNode&>(CurrentNode);
			return new RegionNode(NodeToCopy);
		}
	);

	return true;
}();

RegionNode::RegionNode() : BasicLogicNode()
{
	SetStyle(CIRCLE);
	Type = "RegionNode";
	Data = NODE_SYSTEM.CreateNodeArea();

	Begin = new BeginNode();
	Begin->SetPosition(ImVec2(300.0f, 350.0f));
	Data->AddNode(Begin);

	End = new EndNode();
	End->SetPosition(Begin->GetPosition() + ImVec2(600.0f, 0.0f));
	Data->AddNode(End);

	AddSocket(new NodeSocket(this, "FLOAT", "", false));
	AddSocket(new NodeSocket(this, "FLOAT", "", true));
}

RegionNode::RegionNode(const RegionNode& Other) : BasicLogicNode(Other)
{
	Data = Other.Data;
	Begin = reinterpret_cast<BeginNode*>(Data->GetNodesByType("BeginNode")[0]);
	End = reinterpret_cast<EndNode*>(Data->GetNodesByType("EndNode")[0]);
	SetStyle(CIRCLE);
}

void RegionNode::Draw()
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

		if (ParentArea->IsMouseHovered() && IsHovered() && !ActionEditPopup::GetInstance().IsOpened() && !TextInputPopup::GetInstance().IsOpened())
			ShowTooltip();
	}
}

void RegionNode::SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, NODE_SOCKET_EVENT EventType)
{
	Node::SocketEvent(OwnSocket,  ConnectedSocket, EventType);
}

NodeArea* RegionNode::GetData()
{
	return Data;
}

bool RegionNode::CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser)
{
	if (!Node::CanConnect(OwnSocket, CandidateSocket, nullptr))
		return true;

	return true;
}

BasicLogicNode* RegionNode::GetNextNode()
{
	End->NextNode = nullptr;
	if (Output.size() > 0 && Output[0]->GetConnectedSockets().size() > 0)
		End->NextNode = reinterpret_cast<BasicLogicNode*>(Output[0]->GetConnectedSockets()[0]->GetParent());
	
	return Begin;
}

void RegionNode::CheckIcons()
{
	if (RegionIcon == nullptr)
		RegionIcon = new FETPImage("Resources//regionIcon.png");
}

void RegionNode::RenderIcon()
{
	ImGui::SetCursorScreenPos(ImVec2(ImGui::GetCursorScreenPos().x + 8.0f, ImGui::GetCursorScreenPos().y + 8.0f));
	ImGui::Image((void*)(intptr_t)RegionIcon->GetTextureID(), ImVec2(100.0f, 100.0f), ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f));
}

void RegionNode::ShowTooltip()
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

Json::Value RegionNode::ToJson()
{
	Json::Value result = Node::ToJson();
	result["nodeArea"] = Data->ToJson();

	return result;
}

bool RegionNode::FromJson(Json::Value Json)
{
	bool bResult = Node::FromJson(Json);
	if (!bResult)
		return false;

	if (!Json.isMember("nodeArea"))
		return false;

	NODE_SYSTEM.DeleteNodeArea(Data);
	Data = new NodeArea();
	Data->LoadFromJson(Json["nodeArea"].asCString());

	return true;
}