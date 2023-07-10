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

	AddSocket(new ImageSocket(this, "IMAGE", "Out", true));
}

imageNode::imageNode(const imageNode& Src) : basicLogicNode(Src)
{
	SetStyle(VISUAL_NODE_STYLE_DEFAULT);
	Data = Src.Data;

	delete Output[0];
	Output.clear();
	AddSocket(new ImageSocket(this, "IMAGE", "Out", true));
}

Json::Value imageNode::ToJson()
{
	Json::Value Result = VisualNode::ToJson();

	if (Data != nullptr)
	{
		unsigned char* tempRawData = Data->getRawData();
		// One possibility why it is empty is that user copy node to clipboard.
		if (Data->getFullPath() == "")
		{
			// So we need to save it to temp location.
			std::string tempDirectory = FocalEngine::FILE_SYSTEM.getDirectoryPath(FocalEngine::FILE_SYSTEM.getApplicationPath().c_str());
			std::string fileName = tempDirectory;
			fileName += GetID();
			fileName += ".png";
			Data->setFullPath(fileName);
		}
		lodepng::encode(Data->getFullPath(), tempRawData, Data->getWidth(), Data->getHeight());
		delete[] tempRawData;
	}

	return Result;
}

void imageNode::FromJson(Json::Value Json)
{
	VisualNode::FromJson(Json);

	std::string fileName = Json["DirectoryPath"].asString();
	fileName += GetID();
	fileName += ".png";

	if (FILE_SYSTEM.checkFile(fileName.c_str()))
	{
		std::vector<unsigned char> rawData;
		unsigned uWidth, uHeight;
		lodepng::decode(rawData, uWidth, uHeight, fileName);

		unsigned char* tempData = new unsigned char[uWidth * uHeight * 4];
		memcpy_s(tempData, uWidth * uHeight * 4, rawData.data(), uWidth * uHeight * 4);
		Data = new FETPImage(tempData, uWidth, uHeight);
		delete[] tempData;
	}
}

void imageNode::Draw()
{	
	VisualNode::Draw();

	int xPosition = ImGui::GetCursorScreenPos().x + 75.0f;
	int yPosition = ImGui::GetCursorScreenPos().y + 115.0f;
	
	if (Data == nullptr)
	{
		ImGui::SetCursorScreenPos(ImVec2(xPosition, yPosition));
		ImGui::Text("NO IMAGE");

		xPosition -= 8.0f;
		yPosition += 75.0f;
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
		xPosition -= 25.0f;
		yPosition -= 60.0f;
		ImGui::SetCursorScreenPos(ImVec2(xPosition, yPosition));
		ImGui::Image((void*)(intptr_t)Data->getTextureID(), ImVec2(128.0f, 128.0f), ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f));

		xPosition -= 20.0f;
		yPosition += 136.0f;
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