#include "ImageLoadNode.h"
#include "ImageLiteralNode.h"
#include "../../../../ThirdParty/lodepng/lodepng.h"
using namespace VisNodeSys;

bool ImageLoadNode::bIsRegistered = []()
{
	auto Creator = []() -> Node* {
		return new ImageLoadNode();
	};

	auto Copier = [](const Node& CurrentNode) -> Node* {
		const ImageLoadNode& NodeToCopy = static_cast<const ImageLoadNode&>(CurrentNode);
		return new ImageLoadNode(NodeToCopy);
	};

	NODE_FACTORY.RegisterNodeType("ImageLoadNode", Creator, Copier);
	return true;
}();

ImageLoadNode::ImageLoadNode() : BaseExecutionFlowNode()
{
	Type = "ImageLoadNode";

	SetStyle(DEFAULT);
	SetName("Image Load");

	TitleBackgroundColor = ImColor(31, 117, 208);
	TitleBackgroundColorHovered = ImColor(35, 145, 255);

	AddSocket(new NodeSocket(this, "STRING", "Path", NodeSocket::SocketFlow::Input));

	AddSocket(new NodeSocket(this, "EXECUTE", "", NodeSocket::SocketFlow::Output));
	AddSocket(new NodeSocket(this, "IMAGE", "Image", NodeSocket::SocketFlow::Output));

	SetSize(ImVec2(350, 200));
	Output[1]->SetFunctionToOutputData(ImageDataGetter);
}

ImageLoadNode::ImageLoadNode(const ImageLoadNode& Other) : BaseExecutionFlowNode(Other)
{
	SetStyle(DEFAULT);
	Data = Other.Data;
	// LoadedImage is intentionally not copied.

	// Here I am restoring the output data function.
	// Because the function is not serializable, I have to set it manually.
	Output[1]->SetFunctionToOutputData(ImageDataGetter);
}

ImageLoadNode::~ImageLoadNode()
{
	delete LoadedImage;
}

Json::Value ImageLoadNode::ToJson()
{
	Json::Value Result = Node::ToJson();
	Result["Path"] = Data;
	return Result;
}

bool ImageLoadNode::FromJson(Json::Value Json)
{
	bool bResult = Node::FromJson(Json);
	if (!bResult)
		return false;

	if (!Json.isMember("Path") || !Json["Path"].isString())
		return false;

	Data = Json["Path"].asString();

	if (Output.size() < 2 || Output[1] == nullptr)
		return false;

	Output[1]->SetFunctionToOutputData(ImageDataGetter);
	return true;
}

bool ImageLoadNode::SetPath(std::string Path)
{
	Data = Path;
	return true;
}

void ImageLoadNode::LoadImageFromCurrentPath()
{
	delete LoadedImage;
	LoadedImage = nullptr;

	if (Data.empty())
		return;

	std::vector<unsigned char> ImageRawData;
	unsigned ImageWidth = 0;
	unsigned ImageHeight = 0;
	int ErrorCode = lodepng::decode(ImageRawData, ImageWidth, ImageHeight, Data);
	if (ErrorCode != 0)
		return;

	unsigned char* TemporaryRawData = new unsigned char[ImageWidth * ImageHeight * 4];
	memcpy_s(TemporaryRawData, ImageWidth * ImageHeight * 4, ImageRawData.data(), ImageWidth * ImageHeight * 4);
	LoadedImage = new FETPImage(TemporaryRawData, ImageWidth, ImageHeight);
	delete[] TemporaryRawData;
}

void ImageLoadNode::Draw()
{
	ImVec2 NodeLeftTop = ImGui::GetCursorScreenPos();
	Node::Draw();

	float Zoom = ParentArea->GetZoomFactor();
	float StartX = NodeLeftTop.x + 25.0f * Zoom;
	float StartY = NodeLeftTop.y + 48.0f * Zoom;

	bool bInputOverrides = Input.size() > 1 && Input[1]->GetConnectedSockets().size() > 0;

	static char TemporaryTextBuffer[4096];
	strcpy_s(TemporaryTextBuffer, Data.c_str());

	ImGui::SetCursorScreenPos(ImVec2(StartX, StartY));
	if (bInputOverrides)
		ImGui::BeginDisabled();

	if (ImGui::InputTextMultiline("##PathInput", TemporaryTextBuffer, IM_ARRAYSIZE(TemporaryTextBuffer), ImVec2(290, 70) * Zoom, ImGuiInputTextFlags_AllowTabInput))
	{
		Data = TemporaryTextBuffer;
	}

	if (bInputOverrides)
		ImGui::EndDisabled();

	float ButtonWidth = ImGui::CalcTextSize("Browse...").x + ImGui::GetStyle().FramePadding.x * 2.0f;
	float ButtonX = StartX + (290.0f * Zoom - ButtonWidth) * 0.5f;
	ImGui::SetCursorScreenPos(ImVec2(ButtonX, StartY + 78.0f * Zoom));
	if (ImGui::Button("Browse..."))
	{
		std::string NewPath;
		std::string InitialDirectory;
		if (!Data.empty() && FocalEngine::FILE_SYSTEM.DoesFileExist(Data))
			InitialDirectory = FocalEngine::FILE_SYSTEM.GetDirectoryPath(Data);
		FocalEngine::FILE_SYSTEM.ShowFileOpenDialog(NewPath, PNGLoadFilter, 1, InitialDirectory);
		if (!NewPath.empty())
			SetPath(NewPath);
	}
}

void ImageLoadNode::SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, NODE_SOCKET_EVENT EventType)
{
	Node::SocketEvent(OwnSocket, ConnectedSocket, EventType);

	if (EventType == EXECUTE)
	{
		if (Input[1]->GetConnectedSockets().size() > 0)
		{
			void* TemporaryData = Input[1]->GetConnectedSockets()[0]->GetData();
			if (TemporaryData != nullptr)
			{
				std::string InputPath = *reinterpret_cast<std::string*>(TemporaryData);
				if (!InputPath.empty())
					Data = InputPath;
			}
		}

		LoadImageFromCurrentPath();

		if (Output[0]->GetConnectedSockets().size() > 0)
			ParentArea->TriggerSocketEvent(Output[0], Output[0]->GetConnectedSockets()[0], EXECUTE);
	}
}

bool ImageLoadNode::CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser)
{
	if (!Node::CanConnect(OwnSocket, CandidateSocket, nullptr))
		return false;

	return true;
}