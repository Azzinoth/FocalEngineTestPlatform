#include "imageSearchNode.h"
using namespace VisNodeSys;

bool imageSearchNode::isRegistered = []()
{
	NODE_FACTORY.RegisterNodeType("imageSearchNode",
		[]() -> Node* {
			return new imageSearchNode();
		},

		[](const Node& CurrentNode) -> Node* {
			const imageSearchNode& NodeToCopy = static_cast<const imageSearchNode&>(CurrentNode);
			return new imageSearchNode(NodeToCopy);
		}
	);

	return true;
}();

imageSearchNode::imageSearchNode() : basicLogicNode()
{
	Type = "imageSearchNode";

	SetStyle(DEFAULT);

	SetSize(ImVec2(290, 220));
	SetName("image search node");

	TitleBackgroundColor = ImColor(31, 117, 208);
	TitleBackgroundColorHovered = ImColor(35, 145, 255);

	AddSocket(new NodeSocket(this, "EXECUTE", "", false));
	AddSocket(new NodeSocket(this, "IMAGE", "Image", false));
	AddSocket(new NodeSocket(this, "FLOAT", "Simularity", false));
	AddSocket(new NodeSocket(this, "INT", "Color shift", false));
	AddSocket(new NodeSocket(this, "INT", "Monitor", false));

	AddSocket(new NodeSocket(this, "EXECUTE", "Out", true));
	AddSocket(new NodeSocket(this, "BOOL", "Found", true));
	Output[1]->SetFunctionToOutputData(BoolDataGetter);
	AddSocket(new NodeSocket(this, "VEC2", "Position", true));
	Output[2]->SetFunctionToOutputData(Vec2DataGetter);
}

imageSearchNode::imageSearchNode(const imageSearchNode& Src) : basicLogicNode(Src)
{
	SetStyle(DEFAULT);

	Simularity = Src.Simularity;
	MaxColorShift = Src.MaxColorShift;
	MonitorIndex = Src.MonitorIndex;

	// Here I am restoring the output data function.
	// Because the function is not serializable, I have to set it manually.
	Output[1]->SetFunctionToOutputData(BoolDataGetter);
	Output[2]->SetFunctionToOutputData(Vec2DataGetter);
}

Json::Value imageSearchNode::ToJson()
{
	Json::Value Result = Node::ToJson();

	if (Input[2]->GetConnectedSockets().empty())
		Result["Simularity"] = Simularity;
	
	if (Input[3]->GetConnectedSockets().empty())
		Result["MaxColorShift"] = MaxColorShift;

	if (Input[4]->GetConnectedSockets().empty())
		Result["MonitorIndex"] = MonitorIndex;

	return Result;
}

void imageSearchNode::FromJson(Json::Value Json)
{
	Node::FromJson(Json);

	if (Json.isMember("Simularity"))
		Simularity = Json["Simularity"].asFloat();

	if (Json.isMember("MaxColorShift"))
		MaxColorShift = Json["MaxColorShift"].asInt();

	if (Json.isMember("MonitorIndex"))
		MonitorIndex = Json["MonitorIndex"].asInt();

	// Here I am restoring the output data function.
	// Because the function is not serializable, I have to set it manually.
	Output[1]->SetFunctionToOutputData(BoolDataGetter);
	Output[2]->SetFunctionToOutputData(Vec2DataGetter);
}

void imageSearchNode::Draw()
{	
	Node::Draw();

	float Zoom = ParentArea->GetZoomFactor();

	float xPosition = ImGui::GetCursorScreenPos().x + 115.0f * Zoom;
	float yPosition = ImGui::GetCursorScreenPos().y + 115.0f * Zoom;

	ImGui::BeginDisabled(Input[2]->GetConnectedSockets().size() != 0);
	ImGui::SetCursorScreenPos(ImVec2(xPosition, yPosition));
	ImGui::SetNextItemWidth(35.0f * Zoom);
	ImGui::DragFloat("##Simularity", &Simularity, 0.1f, 0.0f, 100.0f);
	if (Simularity > 100.0f)
		Simularity = 100.0f;

	if (Simularity < 0.0f)
		Simularity = 0.0f;

	ImGui::EndDisabled();

	xPosition += 7 * Zoom;
	yPosition += 38 * Zoom;
	ImGui::SetCursorScreenPos(ImVec2(xPosition, yPosition));
	ImGui::SetNextItemWidth(35.0f * Zoom);
	ImGui::DragInt("##MaxColorShift", &MaxColorShift, 1, 0, 100);
	if (MaxColorShift < 0)
		MaxColorShift = 0;

	xPosition -= 30 * Zoom;
	yPosition += 38 * Zoom;
	ImGui::SetCursorScreenPos(ImVec2(xPosition, yPosition));
	ImGui::SetNextItemWidth(35.0f * Zoom);
	int MonitorCount = FocalEngine::APPLICATION.GetMonitors().size();
	int TemporaryMonitorIndex = MonitorIndex;
	ImGui::DragInt("##MonitorIndex", &TemporaryMonitorIndex, 1, 0, 100);
	if (MonitorIndex < 0)
		MonitorIndex = 0;

	if (MonitorIndex >= MonitorCount)
		MonitorIndex = MonitorCount - 1;

	if (TemporaryMonitorIndex != MonitorIndex)
	{
		MonitorIndex = TemporaryMonitorIndex;
		//ParentArea->TriggerSocketEvent(Input[4], Input[4]->GetConnectedSockets()[0], UPDATE);
	}
}

void imageSearchNode::SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, NODE_SOCKET_EVENT EventType)
{
	Node::SocketEvent(OwnSocket,  ConnectedSocket, EventType);

	if (EventType == UPDATE)
	{
		if (Input[2]->GetConnectedSockets().size() > 0)
		{
			void* TempData = Input[2]->GetConnectedSockets()[0]->GetData();
			if (TempData != nullptr)
				Simularity = *reinterpret_cast<float*>(TempData);
		}

		if (Input[3]->GetConnectedSockets().size() > 0)
		{
			void* TempData = Input[3]->GetConnectedSockets()[0]->GetData();
			if (TempData != nullptr)
				MaxColorShift = *reinterpret_cast<int*>(TempData);
		}

		if (Input[4]->GetConnectedSockets().size() > 0)
		{
			void* TempData = Input[4]->GetConnectedSockets()[0]->GetData();
			if (TempData != nullptr)
			{
				int MonitorCount = FocalEngine::APPLICATION.GetMonitors().size();
				unsigned int TemporaryMonitorIndex = *reinterpret_cast<unsigned int*>(TempData);

				if (TemporaryMonitorIndex >= MonitorCount)
					TemporaryMonitorIndex = MonitorCount - 1;

				MonitorIndex = TemporaryMonitorIndex;
			}
		}
	}

	if (EventType == EXECUTE)
	{
		FETPImage* ImageToLookFor = nullptr;

		if (Input[1]->GetConnectedSockets().size() > 0)
		{
			void* TempData = Input[1]->GetConnectedSockets()[0]->GetData();
			if (TempData != nullptr)
			{
				ImageToLookFor = reinterpret_cast<FETPImage*>(TempData);

				FETPImage* CurrentScreenshot = nullptr;
				CurrentScreenshot = SCREEN_SYSTEM.GetScreenDataAsImage(MonitorIndex);

				glm::vec2 Position = glm::vec2(-1.0f);
				if (CurrentScreenshot != nullptr)
					Position = COMPUTE_SHADER_COMPARE.FindSubImage(CurrentScreenshot, ImageToLookFor, Simularity, MaxColorShift);

				bFound = Position.x != -1 && Position.y != -1;
				FoundPosition = Position;

				delete CurrentScreenshot;

				if (Output[0]->GetConnectedSockets().size() > 0)
					ParentArea->TriggerSocketEvent(Output[0], Output[0]->GetConnectedSockets()[0], EXECUTE);
			}
		}
	}
}

bool imageSearchNode::CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser)
{
	if (!Node::CanConnect(OwnSocket, CandidateSocket, nullptr))
		return false;

	return true;
}

basicLogicNode* imageSearchNode::GetNextNode()
{
	return nullptr;
}