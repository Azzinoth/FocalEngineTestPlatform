#include "ImageSearchNode.h"
using namespace VisNodeSys;

bool ImageSearchNode::bIsRegistered = []()
{
	NODE_FACTORY.RegisterNodeType("ImageSearchNode",
		[]() -> Node* {
			return new ImageSearchNode();
		},

		[](const Node& CurrentNode) -> Node* {
			const ImageSearchNode& NodeToCopy = static_cast<const ImageSearchNode&>(CurrentNode);
			return new ImageSearchNode(NodeToCopy);
		}
	);

	return true;
}();

ImageSearchNode::ImageSearchNode() : BaseExecutionFlowNode()
{
	Type = "ImageSearchNode";

	SetStyle(DEFAULT);

	SetSize(ImVec2(290, 220));
	SetName("Image Search");

	TitleBackgroundColor = ImColor(31, 117, 208);
	TitleBackgroundColorHovered = ImColor(35, 145, 255);

	AddSocket(new NodeSocket(this, "IMAGE", "Image", NodeSocket::SocketFlow::Input));
	AddSocket(new NodeSocket(this, "FLOAT", "Similarity", NodeSocket::SocketFlow::Input));
	AddSocket(new NodeSocket(this, "INT", "Color shift", NodeSocket::SocketFlow::Input));
	AddSocket(new NodeSocket(this, "INT", "Monitor", NodeSocket::SocketFlow::Input));

	AddSocket(new NodeSocket(this, "EXECUTE", "Out", NodeSocket::SocketFlow::Output));
	AddSocket(new NodeSocket(this, "BOOL", "Found", NodeSocket::SocketFlow::Output));
	Output[1]->SetFunctionToOutputData(BoolDataGetter);
	AddSocket(new NodeSocket(this, "VEC2", "Position", NodeSocket::SocketFlow::Output));
	Output[2]->SetFunctionToOutputData(Vec2DataGetter);
	AddSocket(new NodeSocket(this, "INT", "Monitor", NodeSocket::SocketFlow::Output));
	Output[3]->SetFunctionToOutputData(MonitorIndexDataGetter);
}

ImageSearchNode::ImageSearchNode(const ImageSearchNode& Other) : BaseExecutionFlowNode(Other)
{
	SetStyle(DEFAULT);

	Similarity = Other.Similarity;
	MaxColorShift = Other.MaxColorShift;
	MonitorIndex = Other.MonitorIndex;

	// Here I am restoring the output data function.
	// Because the function is not serializable, I have to set it manually.
	Output[1]->SetFunctionToOutputData(BoolDataGetter);
	Output[2]->SetFunctionToOutputData(Vec2DataGetter);
	Output[3]->SetFunctionToOutputData(MonitorIndexDataGetter);
}

Json::Value ImageSearchNode::ToJson()
{
	Json::Value Result = Node::ToJson();

	if (Input[2]->GetConnectedSockets().empty())
		Result["Similarity"] = Similarity;
	
	if (Input[3]->GetConnectedSockets().empty())
		Result["MaxColorShift"] = MaxColorShift;

	if (Input[4]->GetConnectedSockets().empty())
		Result["MonitorIndex"] = MonitorIndex;

	return Result;
}

bool ImageSearchNode::FromJson(Json::Value Json)
{
	bool bResult = Node::FromJson(Json);
	if (!bResult)
		return false;

	if (!Json.isMember("Similarity") || !Json.isMember("MaxColorShift") || !Json.isMember("MonitorIndex"))
		return false;

	Similarity = Json["Similarity"].asFloat();
	MaxColorShift = Json["MaxColorShift"].asInt();
	MonitorIndex = Json["MonitorIndex"].asInt();

	// Here I am restoring the output data function.
	// Because the function is not serializable, I have to set it manually.
	if (Output.size() < 4 || Output[0] == nullptr || Output[1] == nullptr || Output[2] == nullptr || Output[3] == nullptr)
		return false;

	Output[1]->SetFunctionToOutputData(BoolDataGetter);
	Output[2]->SetFunctionToOutputData(Vec2DataGetter);
	Output[3]->SetFunctionToOutputData(MonitorIndexDataGetter);

	return true;
}

void ImageSearchNode::Draw()
{	
	Node::Draw();

	float Zoom = ParentArea->GetZoomFactor();

	float XPosition = ImGui::GetCursorScreenPos().x + 115.0f * Zoom;
	float YPosition = ImGui::GetCursorScreenPos().y + 115.0f * Zoom;

	ImGui::BeginDisabled(Input[2]->GetConnectedSockets().size() != 0);
	ImGui::SetCursorScreenPos(ImVec2(XPosition, YPosition));
	ImGui::SetNextItemWidth(35.0f * Zoom);
	ImGui::DragFloat("##Similarity", &Similarity, 0.1f, 0.0f, 100.0f);
	if (Similarity > 100.0f)
		Similarity = 100.0f;

	if (Similarity < 0.0f)
		Similarity = 0.0f;

	ImGui::EndDisabled();

	XPosition += 7 * Zoom;
	YPosition += 38 * Zoom;
	ImGui::SetCursorScreenPos(ImVec2(XPosition, YPosition));
	ImGui::SetNextItemWidth(35.0f * Zoom);
	ImGui::DragInt("##MaxColorShift", &MaxColorShift, 1, 0, 100);
	if (MaxColorShift < 0)
		MaxColorShift = 0;

	XPosition -= 30 * Zoom;
	YPosition += 38 * Zoom;
	ImGui::SetCursorScreenPos(ImVec2(XPosition, YPosition));
	ImGui::SetNextItemWidth(35.0f * Zoom);
	size_t MonitorCount = FocalEngine::APPLICATION.GetMonitors().size();
	int TemporaryMonitorIndex = MonitorIndex;
	ImGui::DragInt("##MonitorIndex", &TemporaryMonitorIndex, 1, -1, 100);

	if (MonitorIndex >= MonitorCount)
		MonitorIndex = int(MonitorCount - 1);

	if (TemporaryMonitorIndex != MonitorIndex)
	{
		MonitorIndex = TemporaryMonitorIndex;
		//ParentArea->TriggerSocketEvent(Input[4], Input[4]->GetConnectedSockets()[0], UPDATE);
	}
}

void ImageSearchNode::SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, NODE_SOCKET_EVENT EventType)
{
	Node::SocketEvent(OwnSocket,  ConnectedSocket, EventType);

	if (EventType == UPDATE)
	{
		if (Input[2]->GetConnectedSockets().size() > 0)
		{
			void* TemporaryData = Input[2]->GetConnectedSockets()[0]->GetData();
			if (TemporaryData != nullptr)
				Similarity = *reinterpret_cast<float*>(TemporaryData);
		}

		if (Input[3]->GetConnectedSockets().size() > 0)
		{
			void* TemporaryData = Input[3]->GetConnectedSockets()[0]->GetData();
			if (TemporaryData != nullptr)
				MaxColorShift = *reinterpret_cast<int*>(TemporaryData);
		}

		if (Input[4]->GetConnectedSockets().size() > 0)
		{
			void* TemporaryData = Input[4]->GetConnectedSockets()[0]->GetData();
			if (TemporaryData != nullptr)
			{
				int MonitorCount = int(FocalEngine::APPLICATION.GetMonitors().size());
				unsigned int TemporaryMonitorIndex = *reinterpret_cast<unsigned int*>(TemporaryData);

				if (TemporaryMonitorIndex >= unsigned int(MonitorCount))
					TemporaryMonitorIndex = unsigned int(MonitorCount - 1);

				MonitorIndex = TemporaryMonitorIndex;
			}
		}
	}

	if (EventType == EXECUTE)
	{
		FETPImage* ImageToLookFor = nullptr;

		if (Input[1]->GetConnectedSockets().size() > 0)
		{
			void* TemporaryData = Input[1]->GetConnectedSockets()[0]->GetData();
			if (TemporaryData != nullptr)
			{
				ImageToLookFor = reinterpret_cast<FETPImage*>(TemporaryData);

				if (ImageToLookFor == nullptr)
					return;

				if (MonitorIndex == -1)
				{
					std::vector<FocalEngine::MonitorInfo> Monitors = FocalEngine::APPLICATION.GetMonitors();
					for (size_t i = 0; i < Monitors.size(); i++)
					{
						FETPImage* CurrentScreenshot = nullptr;
						CurrentScreenshot = SCREEN_SYSTEM.GetScreenDataAsImage(unsigned int(i));

						glm::vec2 Position = glm::vec2(-1.0f);
						if (CurrentScreenshot != nullptr)
							Position = COMPUTE_SHADER_COMPARE.FindSubImage(CurrentScreenshot, ImageToLookFor, Similarity, MaxColorShift);

						bFound = Position.x != -1 && Position.y != -1;
						FoundPosition = Position;

						delete CurrentScreenshot;

						if (bFound)
						{
							FoundMonitorIndex = int(i);
							break;
						}
					}
				}
				else
				{
					FETPImage* CurrentScreenshot = nullptr;
					CurrentScreenshot = SCREEN_SYSTEM.GetScreenDataAsImage(MonitorIndex);

					glm::vec2 Position = glm::vec2(-1.0f);
					if (CurrentScreenshot != nullptr)
						Position = COMPUTE_SHADER_COMPARE.FindSubImage(CurrentScreenshot, ImageToLookFor, Similarity, MaxColorShift);

					bFound = Position.x != -1 && Position.y != -1;
					FoundPosition = Position;

					delete CurrentScreenshot;
				}

				if (Output[0]->GetConnectedSockets().size() > 0)
					ParentArea->TriggerSocketEvent(Output[0], Output[0]->GetConnectedSockets()[0], EXECUTE);
			}
		}
	}
}

bool ImageSearchNode::CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser)
{
	if (!Node::CanConnect(OwnSocket, CandidateSocket, nullptr))
		return false;

	return true;
}