#include "ImageNode.h"
using namespace VisNodeSys;

bool ImageNode::bIsRegistered = []()
{
	NODE_FACTORY.RegisterNodeType("ImageNode",
		[]() -> Node* {
			return new ImageNode();
		},

		[](const Node& CurrentNode) -> Node* {
			const ImageNode& NodeToCopy = static_cast<const ImageNode&>(CurrentNode);
			return new ImageNode(NodeToCopy);
		}
	);

	return true;
}();

ImageNode::ImageNode() : BaseExecutionFlowNode()
{
	Type = "ImageNode";

	SetStyle(DEFAULT);

	SetSize(ImVec2(220, 220));
	SetName("image node");

	TitleBackgroundColor = ImColor(31, 117, 208);
	TitleBackgroundColorHovered = ImColor(35, 145, 255);

	AddSocket(new NodeSocket(this, "IMAGE", "Out", true));
	Output[0]->SetFunctionToOutputData(ImageDataGetter);
}

ImageNode::ImageNode(const ImageNode& Other) : BaseExecutionFlowNode(Other)
{
	SetStyle(DEFAULT);
	Data = Other.Data;

	// Here I am restoring the output data function.
	// Because the function is not serializable, I have to set it manually.
	Output[0]->SetFunctionToOutputData(ImageDataGetter);
}

Json::Value ImageNode::ToJson()
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

bool ImageNode::FromJson(Json::Value Json)
{
	bool bResult = Node::FromJson(Json);
	if (!bResult)
		return false;

	if (!Json.isMember("ImageWidth") || !Json.isMember("ImageHeight") || !Json.isMember("ImageData"))
		return false;

	int ImageWidth = Json["ImageWidth"].asInt();
	int ImageHeight = Json["ImageHeight"].asInt();

	std::string base64String = Json["ImageData"].asString();

	if (Data != nullptr)
		delete Data;

	Data = new FETPImage();
	Data->DecodeBase64ToRawData(base64String, ImageWidth, ImageHeight);

	// Here I am restoring the output data function.
	// Because the function is not serializable, I have to set it manually.
	if (Output.size() < 1 || Output[0] == nullptr)
		return false;

	Output[0]->SetFunctionToOutputData(ImageDataGetter);

	return true;
}

bool ImageNode::SetImage(FETPImage* Image)
{
	if (Image == nullptr)
		return false;

	if (Data != nullptr)
		delete Data;

	Data = Image;
	return true;
}

void ImageNode::Draw()
{	
	Node::Draw();

	float Zoom = ParentArea->GetZoomFactor();

	float XPosition = ImGui::GetCursorScreenPos().x + 75.0f * Zoom;
	float YPosition = ImGui::GetCursorScreenPos().y + 115.0f * Zoom;
	
	if (Data == nullptr)
	{
		ImGui::SetCursorScreenPos(ImVec2(XPosition, YPosition));
		ImGui::Text("NO IMAGE");

		XPosition -= 8.0f * Zoom;
		YPosition += 75.0f * Zoom;
		ImGui::SetCursorScreenPos(ImVec2(XPosition, YPosition));
		if (ImGui::Button("Load Image"))
		{
			std::string Path;
			FocalEngine::FILE_SYSTEM.ShowFileOpenDialog(Path, PNGLoadFilter, 1);

			if (Path != "")
			{
				std::vector<unsigned char> rawData;
				unsigned uWidth, uHeight;
				int error = lodepng::decode(rawData, uWidth, uHeight, Path);

				if (error == 0)
				{
					unsigned char* tempData = new unsigned char[uWidth * uHeight * 4];
					memcpy_s(tempData, uWidth * uHeight * 4, rawData.data(), uWidth * uHeight * 4);
					SetImage(new FETPImage(tempData, uWidth, uHeight));
					delete[] tempData;
				}
			}
		}
	}
	else
	{
		XPosition -= 25.0f * Zoom;
		YPosition -= 60.0f * Zoom;
		ImGui::SetCursorScreenPos(ImVec2(XPosition, YPosition));
		ImGui::Image((void*)(intptr_t)Data->GetTextureID(), ImVec2(128.0f, 128.0f) * Zoom, ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f));

		XPosition -= 20.0f * Zoom;
		YPosition += 136.0f * Zoom;
		ImGui::SetCursorScreenPos(ImVec2(XPosition, YPosition));
		if (ImGui::Button("Load different image"))
		{
			std::string Path;
			FocalEngine::FILE_SYSTEM.ShowFileOpenDialog(Path, PNGLoadFilter, 1);

			if (Path != "")
			{
				std::vector<unsigned char> rawData;
				unsigned uWidth, uHeight;
				int error = lodepng::decode(rawData, uWidth, uHeight, Path);

				if (error == 0)
				{
					delete Data;

					unsigned char* tempData = new unsigned char[uWidth * uHeight * 4];
					memcpy_s(tempData, uWidth * uHeight * 4, rawData.data(), uWidth * uHeight * 4);
					SetImage(new FETPImage(tempData, uWidth, uHeight));
					delete[] tempData;
				}
			}
		}
	}
}

void ImageNode::SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, NODE_SOCKET_EVENT EventType)
{
	Node::SocketEvent(OwnSocket,  ConnectedSocket, EventType);
}

bool ImageNode::CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser)
{
	if (!Node::CanConnect(OwnSocket, CandidateSocket, nullptr))
		return false;

	return true;
}