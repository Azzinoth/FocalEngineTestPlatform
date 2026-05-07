#include "ImageSearchNode.h"
using namespace VisNodeSys;
#include "../../../FETestPlatform.h"

// Define this to also run the CPU implementation on a failed GPU search.
//#define FETP_DEBUG_CPU_IMAGE_SEARCH

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

	SetSize(ImVec2(290, 260));
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

ImageSearchNode::~ImageSearchNode()
{
	delete LastResult.CroppedRegion;
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
	ImVec2 PositionBeforeDraw = ImGui::GetCursorScreenPos();

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

	XPosition += 7.0f * Zoom;
	YPosition += 38.0f * Zoom;
	ImGui::SetCursorScreenPos(ImVec2(XPosition, YPosition));
	ImGui::SetNextItemWidth(35.0f * Zoom);
	ImGui::DragInt("##MaxColorShift", &MaxColorShift, 1, 0, 100);
	if (MaxColorShift < 0)
		MaxColorShift = 0;

	XPosition -= 30.0f * Zoom;
	YPosition += 38.0f * Zoom;
	ImGui::SetCursorScreenPos(ImVec2(XPosition, YPosition));
	ImGui::SetNextItemWidth(35.0f * Zoom);
	size_t MonitorCount = FocalEngine::APPLICATION.GetMonitors().size();
	int TemporaryMonitorIndex = MonitorIndex;
	ImGui::DragInt("##MonitorIndex", &TemporaryMonitorIndex, 1, -1, 100);

	if (MonitorIndex >= MonitorCount)
		MonitorIndex = int(MonitorCount - 1);

	if (TemporaryMonitorIndex != MonitorIndex)
		MonitorIndex = TemporaryMonitorIndex;
	
	float IconSize = 24.0f * Zoom;
	float NodeCenterX = PositionBeforeDraw.x + GetSize().x / 2.0f * Zoom;
	float InfoButtonX = NodeCenterX - IconSize / 2.0f;
	float InfoButtonY = PositionBeforeDraw.y + IconSize / 2.0f + 24.0f * Zoom;

	FETPImage* CurrentIcon = nullptr;
	switch (Status)
	{
		case ACTION_NODE_STATUS::WasNotExecuted:
		{
			CurrentIcon = TEST_PLATFORM.GetInfoIconWhite();
			break;
		}
		case ACTION_NODE_STATUS::Success:
		{
			CurrentIcon = TEST_PLATFORM.GetInfoIconGreen();
			break;
		}
		case ACTION_NODE_STATUS::Failure:
		{
			CurrentIcon = TEST_PLATFORM.GetInfoIconRed();
			break;
		}
		case ACTION_NODE_STATUS::Warning:
		{
			CurrentIcon = TEST_PLATFORM.GetInfoIconYellow();
			break;
		}
		default:
			CurrentIcon = TEST_PLATFORM.GetInfoIconWhite();
			break;
	}

	ImGui::SetCursorScreenPos(ImVec2(InfoButtonX, InfoButtonY));
	if (ImGui::ImageButton(("##Show Info " + GetID()).c_str(), CurrentIcon->GetTextureID(), ImVec2(IconSize, IconSize)))
		ImGui::OpenPopup("##ImageSearchLastResult");

	if (ImGui::BeginPopup("##ImageSearchLastResult"))
	{
		if (LastResult.CroppedRegion == nullptr)
		{
			ImGui::Text("No search has been performed yet.");
		}
		else
		{
			ImGui::Text("Status: %s", LastResult.bMatchFound ? "Match found" : "No match (best similarity)");
			ImGui::Text("Position: (%.0f, %.0f)", LastResult.Position.x, LastResult.Position.y);
			ImGui::Text("Score: %.2f%%", LastResult.BestMatchScore * 100.0f);
			ImGui::Text("Monitor: %d", LastResult.MonitorIndex);
			ImGui::Separator();

			float MaxWidth = 400.0f;
			float ImageWidth = float(LastResult.CroppedRegion->GetWidth());
			float ImageHeight = float(LastResult.CroppedRegion->GetHeight());
			if (ImageWidth > MaxWidth)
			{
				ImageHeight *= MaxWidth / ImageWidth;
				ImageWidth = MaxWidth;
			}
			ImGui::Image(LastResult.CroppedRegion->GetTextureID(), ImVec2(ImageWidth, ImageHeight), ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f));
		}
		ImGui::EndPopup();
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
		if (Input[1]->GetConnectedSockets().empty())
			return;

		void* TemporaryData = Input[1]->GetConnectedSockets()[0]->GetData();
		if (TemporaryData == nullptr)
			return;

		FETPImage* ImageToLookFor = reinterpret_cast<FETPImage*>(TemporaryData);
		if (ImageToLookFor == nullptr)
			return;

		auto MakeCroppedRegion = [&](FETPImage* Source, GLuint OriginX, GLuint OriginY) -> FETPImage* {
			if (Source == nullptr)
				return nullptr;
			int RegionX = int(OriginX);
			int RegionY = int(OriginY);
			int RegionWidth = ImageToLookFor->GetWidth();
			int RegionHeight = ImageToLookFor->GetHeight();
			if (RegionX < 0 || RegionY < 0 || RegionX + RegionWidth > Source->GetWidth() || RegionY + RegionHeight > Source->GetHeight())
				return nullptr;
			return Source->GetRegion(RegionX, RegionY, RegionWidth, RegionHeight);
		};

		std::vector<int> MonitorsToTry;
		if (MonitorIndex == -1)
		{
			size_t MonitorCount = FocalEngine::APPLICATION.GetMonitors().size();
			for (size_t i = 0; i < MonitorCount; i++)
				MonitorsToTry.push_back(int(i));
		}
		else
		{
			MonitorsToTry.push_back(MonitorIndex);
		}

		delete LastResult.CroppedRegion;
		LastResult = LastSearchResult();
		bFound = false;
		FoundPosition = glm::vec2(-1.0f);

		for (int i = 0; i < MonitorsToTry.size(); i++)
		{
			FETPImage* CurrentScreenshot = SCREEN_SYSTEM.GetScreenDataAsImage(unsigned int(MonitorsToTry[i]));

			FETPComputeShaderCompare::ComparisonResult ComparisonData = {};
			if (CurrentScreenshot != nullptr)
				ComparisonData = COMPUTE_SHADER_COMPARE.FindSubImage(CurrentScreenshot, ImageToLookFor, Similarity, MaxColorShift);

			FETPImage* TestScreenshotFor = new FETPImage("C:/Users/kberegovyi/Downloads/FocalEngineTestPlatform_DEV_05_05_2026_2/Screen_test.png");
			FETPComputeShaderCompare::ComparisonResult ComparisonData_TEST = {};
			ComparisonData_TEST = COMPUTE_SHADER_COMPARE.FindSubImage(TestScreenshotFor, ImageToLookFor, Similarity, MaxColorShift);

			bool bMatch = ComparisonData.MatchFound != 0;

#if defined(FETP_DEBUG_CPU_IMAGE_SEARCH)
			if (!bMatch && CurrentScreenshot != nullptr)
			{
				FETPComputeShaderCompare::ComparisonResult CPUComparisonData = COMPUTE_SHADER_COMPARE.FindSubImageOnCPU(CurrentScreenshot, ImageToLookFor, Similarity, MaxColorShift);
			}
#endif
			float Score = float(ComparisonData.BestMatchScore) / 1000000.0f;
			GLuint MatchPositionX = bMatch ? ComparisonData.MatchPosition[0] : ComparisonData.BestMatchPosition[0];
			GLuint MatchPositionY = bMatch ? ComparisonData.MatchPosition[1] : ComparisonData.BestMatchPosition[1];

			// Keep the best result we have seen across monitors (or just the only one in single-monitor mode).
			if (bMatch || Score > LastResult.BestMatchScore)
			{
				delete LastResult.CroppedRegion;
				LastResult.bMatchFound = bMatch;
				LastResult.Position = glm::vec2(float(MatchPositionX), float(MatchPositionY));
				LastResult.BestMatchScore = Score;
				LastResult.MonitorIndex = MonitorsToTry[i];
				LastResult.CroppedRegion = MakeCroppedRegion(CurrentScreenshot, MatchPositionX, MatchPositionY);
			}

			if (bMatch)
			{
				bFound = true;
				FoundPosition = glm::vec2(float(MatchPositionX), float(MatchPositionY));
				FoundMonitorIndex = MonitorsToTry[i];
				delete CurrentScreenshot;
				break;
			}

			delete CurrentScreenshot;
		}

		Status = bFound ? ACTION_NODE_STATUS::Success : ACTION_NODE_STATUS::Failure;

		if (Output[0]->GetConnectedSockets().size() > 0)
			ParentArea->TriggerSocketEvent(Output[0], Output[0]->GetConnectedSockets()[0], EXECUTE);
	}
}

bool ImageSearchNode::CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser)
{
	if (!Node::CanConnect(OwnSocket, CandidateSocket, nullptr))
		return false;

	return true;
}

ACTION_NODE_STATUS ImageSearchNode::GetStatus() const
{
	return Status;
}

void ImageSearchNode::ResetToDefaultStatus()
{
	Status = ACTION_NODE_STATUS::WasNotExecuted;
}