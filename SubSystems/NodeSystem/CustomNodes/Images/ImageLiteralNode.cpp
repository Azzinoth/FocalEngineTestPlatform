#include "ImageLiteralNode.h"
using namespace VisNodeSys;

bool ImageLiteralNode::bIsRegistered = []()
{
	auto Creator = []() -> Node* {
		return new ImageLiteralNode();
	};

	auto Copier = [](const Node& CurrentNode) -> Node* {
		const ImageLiteralNode& NodeToCopy = static_cast<const ImageLiteralNode&>(CurrentNode);
		return new ImageLiteralNode(NodeToCopy);
	};

	NODE_FACTORY.RegisterNodeType("ImageLiteralNode", Creator, Copier);
	return true;
}();

ImageLiteralNode::ImageLiteralNode() : BaseExecutionFlowNode(false)
{
	Type = "ImageLiteralNode";

	SetStyle(DEFAULT);

	SetSize(ImVec2(250, 220));
	SetName("Image Literal");

	TitleBackgroundColor = ImColor(31, 117, 208);
	TitleBackgroundColorHovered = ImColor(35, 145, 255);

	AddSocket(new NodeSocket(this, "IMAGE", "", NodeSocket::SocketFlow::Output));
	AddSocket(new NodeSocket(this, "VEC2", "Size", NodeSocket::SocketFlow::Output));

	Output[0]->SetFunctionToOutputData(ImageDataGetter);
	Output[1]->SetFunctionToOutputData(ImageSizeGetter);
}

ImageLiteralNode::ImageLiteralNode(const ImageLiteralNode& Other) : BaseExecutionFlowNode(Other)
{
	SetStyle(DEFAULT);
	Data = Other.Data;
	ImageSize = Other.ImageSize;

	// Here I am restoring the output data function.
	// Because the function is not serializable, I have to set it manually.
	Output[0]->SetFunctionToOutputData(ImageDataGetter);
	Output[1]->SetFunctionToOutputData(ImageSizeGetter);
}

Json::Value ImageLiteralNode::ToJson()
{
	Json::Value Result = Node::ToJson();

	if (Data != nullptr)
	{
		Result["ImageWidth"] = Data->GetWidth();
		Result["ImageHeight"] = Data->GetHeight();
		Result["ImageData"] = Data->EncodeRawDataToBase64();

	}
	else
	{
		Result["ImageID"] = "NONE";
	}

	return Result;
}

bool ImageLiteralNode::FromJson(Json::Value Json)
{
	bool bResult = Node::FromJson(Json);
	if (!bResult)
		return false;

	if (!Json.isMember("ImageWidth") || !Json.isMember("ImageHeight") || !Json.isMember("ImageData"))
		return false;

	int ImageWidth = Json["ImageWidth"].asInt();
	int ImageHeight = Json["ImageHeight"].asInt();

	std::string Base64String = Json["ImageData"].asString();

	this->Output[0]->SetName("");

	if (Data != nullptr)
		delete Data;

	Data = new FETPImage();
	Data->DecodeBase64ToRawData(Base64String, ImageWidth, ImageHeight);
	ImageSize = glm::vec2((float)ImageWidth, (float)ImageHeight);

	// Here I am restoring the output data function.
	// Because the function is not serializable, I have to set it manually.
	if (Output.size() < 2 || Output[0] == nullptr || Output[1] == nullptr)
		return false;

	Output[0]->SetFunctionToOutputData(ImageDataGetter);
	Output[1]->SetFunctionToOutputData(ImageSizeGetter);

	return true;
}

bool ImageLiteralNode::SetImage(FETPImage* Image)
{
	if (Image == nullptr)
		return false;

	if (Data != nullptr)
		delete Data;

	Data = Image;
	ImageSize = glm::vec2((float)Image->GetWidth(), (float)Image->GetHeight());

	return true;
}

void ImageLiteralNode::Draw()
{	
	Node::Draw();

	float Zoom = ParentArea->GetZoomFactor();

	float XPosition = ImGui::GetCursorScreenPos().x + 75.0f * Zoom;
	float YPosition = ImGui::GetCursorScreenPos().y + 115.0f * Zoom;
	
	if (Data == nullptr)
	{
		ImGui::SetCursorScreenPos(ImVec2(XPosition, YPosition));
		ImGui::Text("(NO IMAGE)");

		XPosition -= 8.0f * Zoom;
		YPosition += 75.0f * Zoom;
		ImGui::SetCursorScreenPos(ImVec2(XPosition, YPosition));
		if (ImGui::Button("Load Image"))
		{
			std::string Path;
			FocalEngine::FILE_SYSTEM.ShowFileOpenDialog(Path, PNGLoadFilter, 1);

			if (Path != "")
			{
				std::vector<unsigned char> ImageRawData;
				unsigned ImageWidth, ImageHeight;
				int ErrorCode = lodepng::decode(ImageRawData, ImageWidth, ImageHeight, Path);

				if (ErrorCode == 0)
				{
					unsigned char* TemporaryData = new unsigned char[ImageWidth * ImageHeight * 4];
					memcpy_s(TemporaryData, ImageWidth * ImageHeight * 4, ImageRawData.data(), ImageWidth * ImageHeight * 4);
					SetImage(new FETPImage(TemporaryData, ImageWidth, ImageHeight));
					delete[] TemporaryData;
				}
			}
		}
	}
	else
	{
		float CenterOffsetX = (GetSize().x - 128.0f) * 0.5f;
		XPosition = ImGui::GetCursorScreenPos().x + CenterOffsetX * Zoom;
		YPosition -= 60.0f * Zoom;
		ImGui::SetCursorScreenPos(ImVec2(XPosition, YPosition));
		ImGui::Image(Data->GetTextureID(), ImVec2(128.0f, 128.0f) * Zoom, ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f));

		XPosition -= 20.0f * Zoom;
		YPosition += 136.0f * Zoom;
		ImGui::SetCursorScreenPos(ImVec2(XPosition, YPosition));
		if (ImGui::Button("Load different image"))
		{
			std::string Path;
			FocalEngine::FILE_SYSTEM.ShowFileOpenDialog(Path, PNGLoadFilter, 1);

			if (Path != "")
			{
				std::vector<unsigned char> ImageRawData;
				unsigned ImageWidth, ImageHeight;
				int ErrorCode = lodepng::decode(ImageRawData, ImageWidth, ImageHeight, Path);

				if (ErrorCode == 0)
				{
					unsigned char* TemporaryData = new unsigned char[ImageWidth * ImageHeight * 4];
					memcpy_s(TemporaryData, ImageWidth * ImageHeight * 4, ImageRawData.data(), ImageWidth * ImageHeight * 4);
					SetImage(new FETPImage(TemporaryData, ImageWidth, ImageHeight));
					delete[] TemporaryData;
				}
			}
		}
	}
}

void ImageLiteralNode::SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, NODE_SOCKET_EVENT EventType)
{
	Node::SocketEvent(OwnSocket,  ConnectedSocket, EventType);
}

bool ImageLiteralNode::CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser)
{
	if (!Node::CanConnect(OwnSocket, CandidateSocket, nullptr))
		return false;

	return true;
}