#include "ScreenshotNode.h"
using namespace VisNodeSys;
#include "../../../../FETestPlatform.h"

bool ScreenshotNode::bIsRegistered = []()
{
	auto Creator = []() -> Node* {
		return new ScreenshotNode();
	};

	auto Copier = [](const Node& CurrentNode) -> Node* {
		const ScreenshotNode& NodeToCopy = static_cast<const ScreenshotNode&>(CurrentNode);
		return new ScreenshotNode(NodeToCopy);
	};

	NODE_FACTORY.RegisterNodeType("ScreenshotNode", Creator, Copier);
	return true;
}();

ScreenshotNode::ScreenshotNode() : BaseExecutionFlowNode()
{
	Type = "ScreenshotNode";

	SetStyle(DEFAULT);

	SetSize(ImVec2(250, 220));
	SetName("Screenshot");

	TitleBackgroundColor = ImColor(31, 117, 208);
	TitleBackgroundColorHovered = ImColor(35, 145, 255);

	AddSocket(new NodeSocket(this, "INT", "Monitor", NodeSocket::SocketFlow::Input));

	AddSocket(new NodeSocket(this, "EXECUTE", "Out", NodeSocket::SocketFlow::Output));
	AddSocket(new NodeSocket(this, "IMAGE", "Image", NodeSocket::SocketFlow::Output));
	Output[1]->SetFunctionToOutputData(ImageDataGetter);
}

ScreenshotNode::ScreenshotNode(const ScreenshotNode& Other) : BaseExecutionFlowNode(Other)
{
	SetStyle(DEFAULT);

	MonitorIndex = Other.MonitorIndex;

	// Here I am restoring the output data function.
	// Because the function is not serializable, I have to set it manually.
	Output[1]->SetFunctionToOutputData(ImageDataGetter);
}

ScreenshotNode::~ScreenshotNode()
{
	delete StoredImage;
}

Json::Value ScreenshotNode::ToJson()
{
	Json::Value Result = Node::ToJson();

	if (Input[1]->GetConnectedSockets().empty())
		Result["MonitorIndex"] = MonitorIndex;

	return Result;
}

bool ScreenshotNode::FromJson(Json::Value Json)
{
	bool bResult = Node::FromJson(Json);
	if (!bResult)
		return false;

	if (!Json.isMember("MonitorIndex"))
		return false;

	MonitorIndex = Json["MonitorIndex"].asInt();
	if (MonitorIndex < 0)
		MonitorIndex = 0;

	// Here I am restoring the output data function.
	// Because the function is not serializable, I have to set it manually.
	if (Output.size() < 2 || Output[0] == nullptr || Output[1] == nullptr)
		return false;

	Output[1]->SetFunctionToOutputData(ImageDataGetter);

	return true;
}

void ScreenshotNode::Draw()
{
	Node::Draw();

	float Zoom = ParentArea->GetZoomFactor();

	ImVec2 BodyOrigin = ImGui::GetCursorScreenPos();
	ImVec2 MonitorSocketPos = ParentArea->SocketToPosition(this, Input[1]->GetID());

	float MonitorFieldX = BodyOrigin.x + 92.0f * Zoom;
	float MonitorFieldY = MonitorSocketPos.y - ImGui::GetFrameHeight() / 2.0f;

	ImGui::BeginDisabled(Input[1]->GetConnectedSockets().size() != 0);
	ImGui::SetCursorScreenPos(ImVec2(MonitorFieldX, MonitorFieldY));
	ImGui::SetNextItemWidth(35.0f * Zoom);
	int MonitorCount = int(FocalEngine::APPLICATION.GetMonitors().size());
	int TemporaryMonitorIndex = MonitorIndex;
	ImGui::DragInt("##MonitorIndex", &TemporaryMonitorIndex, 1, 0, 100);

	if (TemporaryMonitorIndex >= MonitorCount)
		TemporaryMonitorIndex = MonitorCount - 1;
	if (TemporaryMonitorIndex < 0)
		TemporaryMonitorIndex = 0;

	MonitorIndex = TemporaryMonitorIndex;
	ImGui::EndDisabled();

	if (StoredImage == nullptr)
	{
		float TextX = BodyOrigin.x + 75.0f * Zoom;
		float TextY = BodyOrigin.y + 115.0f * Zoom;
		ImGui::SetCursorScreenPos(ImVec2(TextX, TextY));
		ImGui::Text("(NO IMAGE)");
	}
	else
	{
		float CenterOffsetX = (GetSize().x - 128.0f) * 0.5f;
		float ImageX = BodyOrigin.x + CenterOffsetX * Zoom;
		float ImageY = BodyOrigin.y + 65.0f * Zoom;
		ImGui::SetCursorScreenPos(ImVec2(ImageX, ImageY));
		ImGui::Image(StoredImage->GetTextureID(), ImVec2(128.0f, 128.0f) * Zoom, ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f));
	}
}

void ScreenshotNode::SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, NODE_SOCKET_EVENT EventType)
{
	Node::SocketEvent(OwnSocket, ConnectedSocket, EventType);

	if (EventType == UPDATE)
	{
		if (Input[1]->GetConnectedSockets().size() > 0)
		{
			void* TemporaryData = Input[1]->GetConnectedSockets()[0]->GetData();
			if (TemporaryData != nullptr)
			{
				int Incoming = *reinterpret_cast<int*>(TemporaryData);
				int MonitorCount = int(FocalEngine::APPLICATION.GetMonitors().size());
				if (Incoming < 0)
					Incoming = 0;
				if (Incoming >= MonitorCount)
					Incoming = MonitorCount - 1;
				MonitorIndex = Incoming;
			}
		}
	}

	if (EventType == EXECUTE)
	{
		delete StoredImage;
		StoredImage = SCREEN_SYSTEM.GetScreenDataAsImage(unsigned int(MonitorIndex));

		if (Output[0]->GetConnectedSockets().size() > 0)
			ParentArea->TriggerSocketEvent(Output[0], Output[0]->GetConnectedSockets()[0], EXECUTE);
	}
}

bool ScreenshotNode::CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser)
{
	if (!Node::CanConnect(OwnSocket, CandidateSocket, nullptr))
		return false;

	return true;
}
