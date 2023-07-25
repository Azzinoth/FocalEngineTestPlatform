#include "imageNode.h"

bool imageNode::isRegistered = []()
{
	NODE_FACTORY.RegisterNodeType("imageNode",
		[]() -> VisualNode* {
			return new imageNode();
		},

		[](const VisualNode& Node) -> VisualNode* {
			const imageNode& NodeToCopy = static_cast<const imageNode&>(Node);
			return new imageNode(NodeToCopy);
		}
	);

	return true;
}();

imageNode::imageNode() : basicLogicNode()
{
	Type = "imageNode";

	SetStyle(VISUAL_NODE_STYLE_DEFAULT);

	SetSize(ImVec2(220, 220));
	SetName("image node");

	TitleBackgroundColor = ImColor(31, 117, 208);
	TitleBackgroundColorHovered = ImColor(35, 145, 255);

	AddSocket(new NodeSocket(this, "IMAGE", "Out", true));
	Output[0]->SetFunctionToOutputData(ImageDataGetter);
}

imageNode::imageNode(const imageNode& Src) : basicLogicNode(Src)
{
	SetStyle(VISUAL_NODE_STYLE_DEFAULT);
	Data = Src.Data;

	// Here I am restoring the output data function.
	// Because the function is not serializable, I have to set it manually.
	Output[0]->SetFunctionToOutputData(ImageDataGetter);
}

Json::Value imageNode::ToJson()
{
	Json::Value Result = VisualNode::ToJson();

	if (Data != nullptr)
	{
		Result["ImageWidth"] = Data->getWidth();
		Result["ImageHeight"] = Data->getHeight();
		Result["ImageData"] = Data->EncodeRawDataToBase64();

	}
	else
	{
		Result["ImageID"] = "NONE";
	}

	return Result;
}

void imageNode::FromJson(Json::Value Json)
{
	VisualNode::FromJson(Json);

	int ImageWidth = Json["ImageWidth"].asInt();
	int ImageHeight = Json["ImageHeight"].asInt();

	std::string base64String = Json["ImageData"].asString();

	if (Data != nullptr)
		delete Data;

	Data = new FETPImage();
	Data->DecodeBase64ToRawData(base64String, ImageWidth, ImageHeight);

	// Here I am restoring the output data function.
	// Because the function is not serializable, I have to set it manually.
	Output[0]->SetFunctionToOutputData(ImageDataGetter);
}

void imageNode::Draw()
{	
	VisualNode::Draw();

	float Zoom = ParentArea->GetZoomFactor();

	int xPosition = ImGui::GetCursorScreenPos().x + 75.0f * Zoom;
	int yPosition = ImGui::GetCursorScreenPos().y + 115.0f * Zoom;
	
	if (Data == nullptr)
	{
		ImGui::SetCursorScreenPos(ImVec2(xPosition, yPosition));
		ImGui::Text("NO IMAGE");

		xPosition -= 8.0f * Zoom;
		yPosition += 75.0f * Zoom;
		ImGui::SetCursorScreenPos(ImVec2(xPosition, yPosition));
		if (ImGui::Button("Load Image"))
		{
			std::string path;
			FocalEngine::FILE_SYSTEM.showFileOpenDialog(path, pngLoadFilter, 1);

			if (path != "")
			{
				std::vector<unsigned char> rawData;
				unsigned uWidth, uHeight;
				int error = lodepng::decode(rawData, uWidth, uHeight, path);

				if (error == 0)
				{
					unsigned char* tempData = new unsigned char[uWidth * uHeight * 4];
					memcpy_s(tempData, uWidth * uHeight * 4, rawData.data(), uWidth * uHeight * 4);
					Data = new FETPImage(tempData, uWidth, uHeight);
					delete[] tempData;
				}
			}
		}
	}
	else
	{
		xPosition -= 25.0f * Zoom;
		yPosition -= 60.0f * Zoom;
		ImGui::SetCursorScreenPos(ImVec2(xPosition, yPosition));
		ImGui::Image((void*)(intptr_t)Data->getTextureID(), ImVec2(128.0f, 128.0f) * Zoom, ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f));

		xPosition -= 20.0f * Zoom;
		yPosition += 136.0f * Zoom;
		ImGui::SetCursorScreenPos(ImVec2(xPosition, yPosition));
		if (ImGui::Button("Load different image"))
		{
			std::string path;
			FocalEngine::FILE_SYSTEM.showFileOpenDialog(path, pngLoadFilter, 1);

			if (path != "")
			{
				std::vector<unsigned char> rawData;
				unsigned uWidth, uHeight;
				int error = lodepng::decode(rawData, uWidth, uHeight, path);

				if (error == 0)
				{
					delete Data;

					unsigned char* tempData = new unsigned char[uWidth * uHeight * 4];
					memcpy_s(tempData, uWidth * uHeight * 4, rawData.data(), uWidth * uHeight * 4);
					Data = new FETPImage(tempData, uWidth, uHeight);
					delete[] tempData;
				}
			}
		}
	}
}

void imageNode::SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, VISUAL_NODE_SOCKET_EVENT EventType)
{
	VisualNode::SocketEvent(OwnSocket,  ConnectedSocket, EventType);
}

bool imageNode::CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser)
{
	if (!VisualNode::CanConnect(OwnSocket, CandidateSocket, nullptr))
		return false;

	return true;
}

basicLogicNode* imageNode::GetNextNode()
{
	return nullptr;
}