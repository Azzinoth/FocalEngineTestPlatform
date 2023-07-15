#include "imageSearchNode.h"

bool imageSearchNode::isRegistered = []()
{
	NODE_FACTORY.RegisterNodeType("imageSearchNode",
		[]() -> VisualNode* {
			return new imageSearchNode();
		},

		[](const VisualNode& Node) -> VisualNode* {
			const imageSearchNode& NodeToCopy = static_cast<const imageSearchNode&>(Node);
			return new imageSearchNode(NodeToCopy);
		}
	);

	return true;
}();

imageSearchNode::imageSearchNode() : basicLogicNode()
{
	Type = "imageSearchNode";

	SetStyle(VISUAL_NODE_STYLE_DEFAULT);

	SetSize(ImVec2(230, 220));
	SetName("image search node");

	TitleBackgroundColor = ImColor(31, 117, 208);
	TitleBackgroundColorHovered = ImColor(35, 145, 255);

	AddSocket(new NodeSocket(this, "EXECUTE", "", false));
	AddSocket(new NodeSocket(this, "IMAGE", "Image", false));

	AddSocket(new NodeSocket(this, "EXECUTE", "Out", true));
	//AddSocket(new BoolSocket(this, "BOOL", "Found", true));
	AddSocket(new NodeSocket(this, "BOOL", "Found", true));
	Output[1]->SetFunctionToOutputData(BoolDataGetter);
	//AddSocket(new Vec2Socket(this, "VEC2", "Position", true));
	AddSocket(new NodeSocket(this, "VEC2", "Position", true));
	Output[2]->SetFunctionToOutputData(Vec2DataGetter);
}

imageSearchNode::imageSearchNode(const imageSearchNode& Src) : basicLogicNode(Src)
{
	SetStyle(VISUAL_NODE_STYLE_DEFAULT);

	// Here I am restoring the output data function.
	// Because the function is not serializable, I have to set it manually.
	Output[1]->SetFunctionToOutputData(BoolDataGetter);
	Output[2]->SetFunctionToOutputData(Vec2DataGetter);
}

Json::Value imageSearchNode::ToJson()
{
	Json::Value Result = VisualNode::ToJson();
	return Result;
}

void imageSearchNode::FromJson(Json::Value Json)
{
	VisualNode::FromJson(Json);

	// Here I am restoring the output data function.
	// Because the function is not serializable, I have to set it manually.
	Output[1]->SetFunctionToOutputData(BoolDataGetter);
	Output[2]->SetFunctionToOutputData(Vec2DataGetter);
}

void imageSearchNode::Draw()
{	
	VisualNode::Draw();

	/*int xPosition = ImGui::GetCursorScreenPos().x + 75.0f;
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
	}*/
}

void imageSearchNode::SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, VISUAL_NODE_SOCKET_EVENT EventType)
{
	VisualNode::SocketEvent(OwnSocket,  ConnectedSocket, EventType);

	if (EventType == VISUAL_NODE_SOCKET_EXECUTE)
	{
		FETPImage* ImageToLookFor = nullptr;

		if (Input[1]->GetConnections().size() > 0)
		{
			void* TempData = Input[1]->GetConnections()[0]->GetData();
			if (TempData != nullptr)
			{
				ImageToLookFor = reinterpret_cast<FETPImage*>(TempData);

				std::vector<unsigned char> tempScreenshoot;
				tempScreenshoot.resize(ImageToLookFor->getWidth() * ImageToLookFor->getHeight() * 4);

				std::vector<unsigned char> tempDifferenceData;
				tempDifferenceData.resize(tempScreenshoot.size());

				FETPImage* TestScreenShoot = nullptr;
				TestScreenShoot = SCREEN_SYSTEM.GetScreenDataAsImage();

				int similarity = 0;
				size_t x = 0;
				size_t y = 0;

				bool found = false;
				glm::vec2 Position = COMPUTE_SHADER_COMPARE.FindSubImage(TestScreenShoot, ImageToLookFor, 90.0f/*float(action->imagesInfo[i]->correctnessThreshold)*/, 8/*action->imagesInfo[i]->maxColorShift*/);
				found = Position.x != -1 && Position.y != -1;
				x = Position.x;
				y = Position.y;

				FoundPosition = Position;
				bFound = found;

				if (found)
				{
					int y = 0;
					y++;
				}

				delete TestScreenShoot;

				if (Output[0]->GetConnections().size() > 0)
					ParentArea->TriggerSocketEvent(Output[0], Output[0]->GetConnections()[0], VISUAL_NODE_SOCKET_EXECUTE);
			}
		}
	}
}

bool imageSearchNode::CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser)
{
	if (!VisualNode::CanConnect(OwnSocket, CandidateSocket, nullptr))
		return false;

	return true;
}

basicLogicNode* imageSearchNode::GetNextNode()
{
	return nullptr;
}