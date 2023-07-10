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
	AddSocket(new BoolSocket(this, "BOOL", "Found", true));
	AddSocket(new Vec2Socket(this, "VEC2", "Position", true));
}

imageSearchNode::imageSearchNode(const imageSearchNode& Src) : basicLogicNode(Src)
{
	SetStyle(VISUAL_NODE_STYLE_DEFAULT);

	for (size_t i = 0; i < Input.size(); i++)
	{
		delete Input[i];
	}
	Input.clear();

	AddSocket(new NodeSocket(this, "EXECUTE", "", false));
	AddSocket(new NodeSocket(this, "IMAGE", "Image", false));

	for (size_t i = 0; i < Output.size(); i++)
	{
		delete Output[i];
	}
	Output.clear();

	AddSocket(new NodeSocket(this, "EXECUTE", "Out", true));
	AddSocket(new BoolSocket(this, "BOOL", "Found", true));
	AddSocket(new Vec2Socket(this, "VEC2", "Position", true));
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

				//FETPImage* image = imageToUse(action->imagesInfo[i]);
				/*if (image == nullptr)
				{
					currentTestResult->failReason = FE_TEST_FAIL_SCREENSHOOT_COMPARE;
					currentTestResult->failedAction = action;
					continue;
				}*/

				std::vector<unsigned char> tempScreenshoot;
				tempScreenshoot.resize(ImageToLookFor->getWidth() * ImageToLookFor->getHeight() * 4);

				std::vector<unsigned char> tempDifferenceData;
				tempDifferenceData.resize(tempScreenshoot.size());

				FETPImage* TestScreenShoot = nullptr;

				//if (action->imagesInfo[i]->partialImage != nullptr)
				//{
					//if (!action->bUseGPU)
					//{
					//	SCREEN_SYSTEM.getScreenRegion(tempScreenshoot.data(), action->imagesInfo[i]->partialImageLeft, action->imagesInfo[i]->partialImageTop, image->getWidth(), image->getHeight());
					//}
					//else
					//{
						TestScreenShoot = SCREEN_SYSTEM.GetScreenDataAsImage();
					//}
				/*}
				else
				{
					SCREEN_SYSTEM.getScreenRegion(tempScreenshoot.data(), 0, 0, image->getWidth(), image->getHeight());
				}*/
						int similarity = 0;
						/*unsigned char* tempRawData = image->getRawData();
						if (action->imagesInfo[i]->screenSearch != nullptr && action->imagesInfo[i]->partialImage != nullptr)
						{*/
							size_t x = 0;
							size_t y = 0;

							//TIME.BeginTimeStamp("M");
							bool found = false;
							/*if (!action->bUseGPU)
							{
								found = SCREEN_SYSTEM.searchOnScreen(image->getWidth(), image->getHeight(), tempRawData, x, y, float(action->imagesInfo[i]->correctnessThreshold), action->imagesInfo[i]->maxColorShift);
							}
							else
							{*/
								glm::vec2 Position = COMPUTE_SHADER_COMPARE.FindSubImage(TestScreenShoot, ImageToLookFor/*action->imagesInfo[i]->partialImage*/, 90.0f/*float(action->imagesInfo[i]->correctnessThreshold)*/, 4/*action->imagesInfo[i]->maxColorShift*/);
								found = Position.x != -1 && Position.y != -1;
								x = Position.x;
								y = Position.y;

								FoundPosition = Position;
								bFound = found;
							//}
							//auto time = TIME.EndTimeStamp("M");

							if (found)
							{
								int y = 0;
								y++;
								/*INPUT_SYSTEM.mouseMoveTo(x + action->imagesInfo[i]->screenSearch->getXShiftFromFound(),
									y + action->imagesInfo[i]->screenSearch->getYShiftFromFound());
								action->imagesInfo[i]->lastRunResult = true;
								return true;*/
							}

							if (Output[0]->GetConnections().size() > 0)
								ParentArea->TriggerSocketEvent(Output[0], Output[0]->GetConnections()[0], VISUAL_NODE_SOCKET_EXECUTE);
						/*}
						else
						{
							similarity = SCREEN_SYSTEM.compare(image->getWidth(), image->getHeight(), tempScreenshoot.data(), tempRawData, tempDifferenceData.data(), action->imagesInfo[i]->maxColorShift);
						}
						delete[] tempRawData;*/

						//if (similarity < action->imagesInfo[i]->correctnessThreshold)
						//{
						//	if (!action->imagesInfo[i]->severalAttempts)
						//	{
						//		currentTestResult->failReason = FE_TEST_FAIL_SCREENSHOOT_COMPARE;
						//		currentTestResult->failedAction = action;

						//		FETPImage* currentScreenshoot = new FETPImage(tempScreenshoot.data(), image->getWidth(), image->getHeight());
						//		FETPImage* diffMap = new FETPImage(tempDifferenceData.data(), image->getWidth(), image->getHeight());
						//		currentTestResult->setScreenshootCompareResult(new FETestScreenshootCompareResult(image, currentScreenshoot, diffMap, similarity));

						//		continue;
						//	}
						//	else
						//	{
						//		DWORD beginTime = GetTickCount();
						//		while (similarity < action->imagesInfo[i]->correctnessThreshold)
						//		{
						//			Sleep(10);
						//			if (action->imagesInfo[i]->partialImage != nullptr)
						//			{
						//				if (!action->bUseGPU)
						//				{
						//					SCREEN_SYSTEM.getScreenRegion(tempScreenshoot.data(), action->imagesInfo[i]->partialImageLeft, action->imagesInfo[i]->partialImageTop, image->getWidth(), image->getHeight());
						//				}
						//				else
						//				{
						//					delete TestScreenShoot;
						//					TestScreenShoot = SCREEN_SYSTEM.GetScreenDataAsImage();
						//				}
						//			}
						//			else
						//			{
						//				SCREEN_SYSTEM.getScreenRegion(tempScreenshoot.data(), 0, 0, image->getWidth(), image->getHeight());
						//			}

						//			//int similarity = SCREEN_SYSTEM.compare(image->getWidth(), image->getHeight(), tempScreenshoot.data(), tempRawData, tempDifferenceData.data());
						//			int similarity = 0;
						//			unsigned char* tempRawData = image->getRawData();
						//			if (action->imagesInfo[i]->screenSearch != nullptr && action->imagesInfo[i]->partialImage != nullptr)
						//			{
						//				size_t x = 0;
						//				size_t y = 0;

						//				bool found = false;
						//				if (!action->bUseGPU)
						//				{
						//					found = SCREEN_SYSTEM.searchOnScreen(image->getWidth(), image->getHeight(), tempRawData, x, y, float(action->imagesInfo[i]->correctnessThreshold), action->imagesInfo[i]->maxColorShift, &similarity);
						//				}
						//				else
						//				{
						//					glm::vec2 Position = COMPUTE_SHADER_COMPARE.FindSubImage(TestScreenShoot, action->imagesInfo[i]->partialImage, float(action->imagesInfo[i]->correctnessThreshold), action->imagesInfo[i]->maxColorShift);
						//					found = Position.x != -1 && Position.y != -1;
						//					x = Position.x;
						//					y = Position.y;
						//				}

						//				if (found)
						//				{
						//					INPUT_SYSTEM.mouseMoveTo(x + action->imagesInfo[i]->screenSearch->getXShiftFromFound(),
						//						y + action->imagesInfo[i]->screenSearch->getYShiftFromFound());
						//					action->imagesInfo[i]->lastRunResult = true;
						//					return true;
						//				}
						//			}
						//			else
						//			{
						//				similarity = SCREEN_SYSTEM.compare(image->getWidth(), image->getHeight(), tempScreenshoot.data(), tempRawData, tempDifferenceData.data(), action->imagesInfo[i]->maxColorShift);
						//			}
						//			delete[] tempRawData;

						//			if (similarity > action->imagesInfo[i]->correctnessThreshold)
						//			{
						//				action->imagesInfo[i]->lastRunResult = true;
						//				return true;
						//			}

						//			if (GetTickCount() - beginTime > DWORD(action->imagesInfo[i]->severalAttemptsTimeout))
						//			{
						//				currentTestResult->failReason = FE_TEST_FAIL_SCREENSHOOT_COMPARE;
						//				currentTestResult->failedAction = action;

						//				FETPImage* currentScreenshoot = new FETPImage(tempScreenshoot.data(), image->getWidth(), image->getHeight());
						//				FETPImage* diffMap = new FETPImage(tempDifferenceData.data(), image->getWidth(), image->getHeight());
						//				currentTestResult->setScreenshootCompareResult(new FETestScreenshootCompareResult(image, currentScreenshoot, diffMap, similarity));
						//				break;
						//			}
						//		}
						//	}
						//}
						//else
						//{
						//	action->imagesInfo[i]->lastRunResult = true;
						//	return true;
						//}



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