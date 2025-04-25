#include "FEPTActionSystem.h"
using namespace VisNodeSys;

FEPTActionSystem::FEPTActionSystem()
{
}

FEPTActionSystem::~FEPTActionSystem() {}

void FEPTActionSystem::takeScreenshoot()
{
	if (recording)
	{
		if (GetTickCount() - lastTimeScreenshootWasTaken > 500)
		{
			lastTimeScreenshootWasTaken = GetTickCount();
			unsigned char* tempScreenshoot = SCREEN_SYSTEM.getScreenData();

			size_t screenshootSize = SCREEN_SYSTEM.getScreenWidth() * SCREEN_SYSTEM.getScreenHeight() * 4;
			unsigned char* newScreenshoot = new unsigned char[screenshootSize];
			memcpy_s(newScreenshoot, screenshootSize, tempScreenshoot, screenshootSize);

			addAction(new ScreenshootCompareAction(newScreenshoot, lastTimeScreenshootWasTaken));
		}
	}
}

VisNodeSys::Node* FEPTActionSystem::getNextNode(VisNodeSys::Node* currentNode)
{
	if (currentNode->GetType() == "beginNode")
	{
		beginNode* node = reinterpret_cast<beginNode*>(currentNode);
		return node->GetNextNode();
	}
	else if (currentNode->GetType() == "globalActionNode")
	{
		globalActionNode* node = reinterpret_cast<globalActionNode*>(currentNode);
		return node->GetNextNode();
	}
	else if (currentNode->GetType() == "combinedActionNode")
	{
		combinedActionNode* node = reinterpret_cast<combinedActionNode*>(currentNode);
		return node->GetNextNode();
	}

	return nullptr;
}

std::vector<FETPAction*> FEPTActionSystem::getActionsFromNode(VisNodeSys::Node* currentNode)
{
	std::vector<FETPAction*> result;

	if (currentNode->GetType() == "beginNode")
	{
		return result;
	}
	else if (currentNode->GetType() == "globalActionNode")
	{
		globalActionNode* node = reinterpret_cast<globalActionNode*>(currentNode);
		result.push_back(node->GetData());
	}
	else if (currentNode->GetType() == "combinedActionNode")
	{
		combinedActionNode* node = reinterpret_cast<combinedActionNode*>(currentNode);
		return node->GetData();
	}

	return result;
}

FETPImage* FEPTActionSystem::imageToUse(compareImageInfo* imageInfo)
{
	if (imageInfo->image == nullptr)
		return nullptr;
	
	//if (size_t(imageInfo->image->getHeight()) != TEST_PLATFORM.getScreenHeight() ||
	//	size_t(imageInfo->image->getWidth()) != TEST_PLATFORM.getScreenWidth())
	//	return nullptr;

	if (imageInfo->partialImage != nullptr)
	{
		if (size_t(imageInfo->partialImageTop + imageInfo->partialImage->GetHeight()) > TEST_PLATFORM.getScreenHeight() ||
			size_t(imageInfo->partialImageLeft + imageInfo->partialImage->GetWidth()) > TEST_PLATFORM.getScreenWidth())
			return nullptr;

		return imageInfo->partialImage;
	}
	else
	{
		return imageInfo->image;
	}

	return nullptr;
}

bool FEPTActionSystem::execute(ScreenshootCompareAction* action)
{
	for (size_t i = 0; i < action->imagesInfo.size(); i++)
		action->imagesInfo[i]->lastRunResult = false;

	for (size_t i = 0; i < action->imagesInfo.size(); i++)
	{
		FETPImage* image = imageToUse(action->imagesInfo[i]);
		if (image == nullptr)
		{
			currentTestResult->failReason = FE_TEST_FAIL_SCREENSHOOT_COMPARE;
			currentTestResult->failedAction = action;
			continue;
		}

		std::vector<unsigned char> tempScreenshoot;
		tempScreenshoot.resize(image->GetWidth() * image->GetHeight() * 4);

		std::vector<unsigned char> tempDifferenceData;
		tempDifferenceData.resize(tempScreenshoot.size());

		FETPImage* TestScreenShoot = nullptr;

		if (action->imagesInfo[i]->partialImage != nullptr)
		{
			if (!action->bUseGPU)
			{
				SCREEN_SYSTEM.getScreenRegion(tempScreenshoot.data(), action->imagesInfo[i]->partialImageLeft, action->imagesInfo[i]->partialImageTop, image->GetWidth(), image->GetHeight());
			}
			else
			{
				TestScreenShoot = SCREEN_SYSTEM.GetScreenDataAsImage();
			}
		}
		else
		{
			SCREEN_SYSTEM.getScreenRegion(tempScreenshoot.data(), 0, 0, image->GetWidth(), image->GetHeight());
		}

		
		
		int similarity = 0;
		unsigned char* tempRawData = image->GetRawData();
		if (action->imagesInfo[i]->screenSearch != nullptr && action->imagesInfo[i]->partialImage != nullptr)
		{
			size_t x = 0;
			size_t y = 0;

			FocalEngine::TIME.BeginTimeStamp("M");
			bool found = false;
			if (!action->bUseGPU)
			{
				found = SCREEN_SYSTEM.searchOnScreen(image->GetWidth(), image->GetHeight(), tempRawData, x, y, float(action->imagesInfo[i]->correctnessThreshold), action->imagesInfo[i]->maxColorShift);
			}
			else
			{
				glm::vec2 Position = COMPUTE_SHADER_COMPARE.FindSubImage(TestScreenShoot, action->imagesInfo[i]->partialImage, float(action->imagesInfo[i]->correctnessThreshold), action->imagesInfo[i]->maxColorShift);
				found = Position.x != -1 && Position.y != -1;
				x = static_cast<size_t>(Position.x);
				y = static_cast<size_t>(Position.y);
			}
			auto time = FocalEngine::TIME.EndTimeStamp("M");

			if (found)
			{
				INPUT_SYSTEM.mouseMoveTo(static_cast<int>(x + action->imagesInfo[i]->screenSearch->getXShiftFromFound()),
										 static_cast<int>(y + action->imagesInfo[i]->screenSearch->getYShiftFromFound()));
				action->imagesInfo[i]->lastRunResult = true;
				return true;
			}
		}
		else
		{
			similarity = SCREEN_SYSTEM.compare(image->GetWidth(), image->GetHeight(), tempScreenshoot.data(), tempRawData, tempDifferenceData.data(), action->imagesInfo[i]->maxColorShift);
		}
		delete[] tempRawData;

		if (similarity < action->imagesInfo[i]->correctnessThreshold)
		{
			if (!action->imagesInfo[i]->severalAttempts)
			{
				currentTestResult->failReason = FE_TEST_FAIL_SCREENSHOOT_COMPARE;
				currentTestResult->failedAction = action;

				FETPImage* currentScreenshoot = new FETPImage(tempScreenshoot.data(), image->GetWidth(), image->GetHeight());
				FETPImage* diffMap = new FETPImage(tempDifferenceData.data(), image->GetWidth(), image->GetHeight());
				currentTestResult->setScreenshootCompareResult(new FETestScreenshootCompareResult(image, currentScreenshoot, diffMap, similarity));

				continue;
			}
			else
			{
				DWORD beginTime = GetTickCount();
				while (similarity < action->imagesInfo[i]->correctnessThreshold)
				{
					Sleep(10);
					if (action->imagesInfo[i]->partialImage != nullptr)
					{
						if (!action->bUseGPU)
						{
							SCREEN_SYSTEM.getScreenRegion(tempScreenshoot.data(), action->imagesInfo[i]->partialImageLeft, action->imagesInfo[i]->partialImageTop, image->GetWidth(), image->GetHeight());
						}
						else
						{
							delete TestScreenShoot;
							TestScreenShoot = SCREEN_SYSTEM.GetScreenDataAsImage();
						}
					}
					else
					{
						SCREEN_SYSTEM.getScreenRegion(tempScreenshoot.data(), 0, 0, image->GetWidth(), image->GetHeight());
					}

					//int similarity = SCREEN_SYSTEM.compare(image->getWidth(), image->getHeight(), tempScreenshoot.data(), tempRawData, tempDifferenceData.data());
					int similarity = 0;
					unsigned char* tempRawData = image->GetRawData();
					if (action->imagesInfo[i]->screenSearch != nullptr && action->imagesInfo[i]->partialImage != nullptr)
					{
						size_t x = 0;
						size_t y = 0;

						bool found = false;
						if (!action->bUseGPU)
						{
							found = SCREEN_SYSTEM.searchOnScreen(image->GetWidth(), image->GetHeight(), tempRawData, x, y, float(action->imagesInfo[i]->correctnessThreshold), action->imagesInfo[i]->maxColorShift, &similarity);
						}
						else
						{
							glm::vec2 Position = COMPUTE_SHADER_COMPARE.FindSubImage(TestScreenShoot, action->imagesInfo[i]->partialImage, float(action->imagesInfo[i]->correctnessThreshold), action->imagesInfo[i]->maxColorShift);
							found = Position.x != -1 && Position.y != -1;
							x = static_cast<size_t>(Position.x);
							y = static_cast<size_t>(Position.y);
						}

						if (found)
						{
							INPUT_SYSTEM.mouseMoveTo(static_cast<int>(x + action->imagesInfo[i]->screenSearch->getXShiftFromFound()),
													 static_cast<int>(y + action->imagesInfo[i]->screenSearch->getYShiftFromFound()));
							action->imagesInfo[i]->lastRunResult = true;
							return true;
						}
					}
					else
					{
						similarity = SCREEN_SYSTEM.compare(image->GetWidth(), image->GetHeight(), tempScreenshoot.data(), tempRawData, tempDifferenceData.data(), action->imagesInfo[i]->maxColorShift);
					}
					delete[] tempRawData;

					if (similarity > action->imagesInfo[i]->correctnessThreshold)
					{
						action->imagesInfo[i]->lastRunResult = true;
						return true;
					}

					if (GetTickCount() - beginTime > DWORD(action->imagesInfo[i]->severalAttemptsTimeout))
					{
						currentTestResult->failReason = FE_TEST_FAIL_SCREENSHOOT_COMPARE;
						currentTestResult->failedAction = action;

						FETPImage* currentScreenshoot = new FETPImage(tempScreenshoot.data(), image->GetWidth(), image->GetHeight());
						FETPImage* diffMap = new FETPImage(tempDifferenceData.data(), image->GetWidth(), image->GetHeight());
						currentTestResult->setScreenshootCompareResult(new FETestScreenshootCompareResult(image, currentScreenshoot, diffMap, similarity));
						break;
					}
				}
			}
		}
		else
		{
			action->imagesInfo[i]->lastRunResult = true;
			return true;
		}
	}

	return false;
}

bool FEPTActionSystem::execute(std::vector<FETPAction*> actions)
{
	for (size_t i = 0; i < actions.size(); i++)
	{
		if (actions[i]->getType() == FETP_KEYBOARD_ACTION)
		{
			KeyboardAction* action = reinterpret_cast<KeyboardAction*>(actions[i]);
			if (action->wParam == WM_KEYDOWN || action->wParam == WM_SYSKEYDOWN)
			{
				INPUT_SYSTEM.keyEvent(WM_KEYDOWN, action->additionalInfo.vkCode);
			}
			else if (action->wParam == WM_KEYUP || action->wParam == WM_SYSKEYUP)
			{
				INPUT_SYSTEM.keyEvent(WM_KEYUP, action->additionalInfo.vkCode);
			}
		}
		else if (actions[i]->getType() == FETP_MOUSE_ACTION)
		{
			MouseAction* action = reinterpret_cast<MouseAction*>(actions[i]);

			if (action->wParam == WM_MOUSEMOVE)
			{
				//INPUT_SYSTEM.mouseMoveTo(action->additionalInfo.pt.x, action->additionalInfo.pt.y);
			}
			else if (action->wParam == WM_LBUTTONUP)
			{
				//INPUT_SYSTEM.mouseUp();
			}
			else if (action->wParam == WM_RBUTTONUP)
			{
				INPUT_SYSTEM.mouseUp(false);
			}
			else if (action->wParam == WM_LBUTTONDOWN)
			{
				//INPUT_SYSTEM.mouseDown();
			}
			else if (action->wParam == WM_RBUTTONDOWN)
			{
				INPUT_SYSTEM.mouseDown(false);
			}
			else if (action->wParam == WM_MOUSEWHEEL)
			{
				INPUT_SYSTEM.mouseWheel((short)HIWORD(action->additionalInfo.mouseData));
			}
		}
		else if (actions[i]->getType() == FETP_SCREENSHOOT_COMPARE_ACTION)
		{
			ScreenshootCompareAction* action = reinterpret_cast<ScreenshootCompareAction*>(actions[i]);
			if (!execute(action))
				return false;
		}
		else if (actions[i]->getType() == FETP_LUNCH_APPLICATION_ACTION)
		{
			LunchApplicationAction* action = reinterpret_cast<LunchApplicationAction*>(actions[i]);
			if (!FocalEngine::FILE_SYSTEM.DoesFileExist(action->applicationPath.c_str()))
			{
				currentTestResult->failReason = FE_TEST_FAIL_CANT_FIND_FILE;
				currentTestResult->failedAction = action;
				return false;
			}

			ShellExecuteA(NULL, NULL, action->applicationPath.c_str(), NULL, FocalEngine::FILE_SYSTEM.GetDirectoryPath(action->applicationPath).c_str(), SW_NORMAL);
		}
		/*else if (actions[i]->getType() == FETP_SLEEP_ACTION)
		{
			SleepAction* action = reinterpret_cast<SleepAction*>(actions[i]);
			Sleep(DWORD(action->sleepFor * currentlyRunning->getSpeedFactor()));
		}*/
	}

	return true;
}

bool FEPTActionSystem::run(FETest* testToRun)
{
	if (testToRun == nullptr || testToRun->getBeginNode() == nullptr)
	{
		currentlyRunning = nullptr;
		return false;
	}

	TEST_PLATFORM.minimizeWindow();
	Sleep(10);
	currentlyRunning = testToRun;
	currentTestResult = new FETestResult();
	currentTestResult->parent = currentlyRunning;
	currentTestResult->startTime = GetTickCount();

	currentlyRunning->beforeBegin();

	basicLogicNode* currentNode = currentlyRunning->getBeginNode();
	currentlyRunning->nodeArea->TriggerOrphanSocketEvent(currentNode, EXECUTE);
	
	/*while (currentNode != nullptr)
	{
		std::string originalText = "";
		std::vector<FETPAction*> actions = getActionsFromNode(currentNode);
		if (currentNode->GetType() == "combinedActionNode")
		{
			std::string originalText = ACTION_SYSTEM.extractText(actions);
			if (currentlyRunning->replaceMacro(originalText))
				actions = generateInputTextActions(originalText, 20);
		}

		if (!execute(actions))
		{
			currentTestResult->success = false;
			currentTestResult->endTime = GetTickCount();
			currentlyRunning->addResult(currentTestResult);
			currentlyRunning = nullptr;
			TEST_PLATFORM.restoreWindow();
			return false;
		}

		currentNode = currentNode->GetNextNode();
	}*/


	currentTestResult->success = true;
	currentTestResult->endTime = GetTickCount();
	currentlyRunning->addResult(currentTestResult);

	if (currentlyRunning->getLoopCount() <= 1)
	{
		currentlyRunning = nullptr;
		TEST_PLATFORM.restoreWindow();
	}
	
	return true;
}

void FEPTActionSystem::placeStructuredNodes(std::vector<FETPAction*> actions, NodeArea* nodeArea, bool copyActions)
{
	static int leftPadding = 15;
	static int nodesPerW = 4;
	static int nodesPerH = 4;
	static int disBetweenOnW = 40;
	static int disBetweenOnH = 150;

	FETPAction* testAction = new FETPAction();
	globalActionNode* testNode = new globalActionNode(testAction);

	if (testNode->GetStyle() == CIRCLE)
	{
		nodesPerW = 6;
		nodesPerH = 6;
		disBetweenOnW = 15;
		disBetweenOnH = int(NODE_DIAMETER + 15.0f);
	}
	else
	{
		nodesPerW = 4;
		disBetweenOnW = 40;
		disBetweenOnH = 150;
	}

	delete testNode;

	int showedIndex = 0;
	VisNodeSys::Node* prevNode = nullptr;
	for (size_t i = 0; i < actions.size(); i++)
	{
		VisNodeSys::Node* newNode = nullptr;

		if (!copyActions)
		{
			if (actions[i]->getType() != FETP_SLEEP_ACTION)
				newNode = tryToPackActions(i);

			if (newNode == nullptr)
				newNode = new globalActionNode(actions[i]);
		}
		else
		{
			newNode = new globalActionNode(copyAction(actions[i]));
		}

		int xPosition = leftPadding;
		xPosition += (showedIndex % nodesPerW * int(newNode->GetSize().x + disBetweenOnW));
		xPosition %= TEST_PLATFORM.getWindowWidth();

		int yPosition = showedIndex / nodesPerH;
		yPosition *= disBetweenOnH;

		newNode->SetPosition(ImVec2(float(xPosition), float(yPosition)));
		nodeArea->AddNode(newNode);

		if (prevNode != nullptr)
			nodeArea->TryToConnect(prevNode, 0, newNode, 0);
		prevNode = newNode;
		showedIndex++;
	}
}

void FEPTActionSystem::recordModeSwitch()
{
	if (GetTickCount() - lastTimeRecordModeWasChanged > 500)
	{
		lastTimeRecordModeWasChanged = GetTickCount();

		recording ? TEST_PLATFORM.restoreWindow() : TEST_PLATFORM.minimizeWindow();
		recording = !recording;

		if (recording)
		{
			recordedActions.clear();
		}
		else
		{
			if (finishRecordingCallback != nullptr)
				finishRecordingCallback(recordedActions);

			//placeStructuredNodes(recordedActions, nodeArea);
			//recordedActions.clear();
		}
	}
}

void FEPTActionSystem::newKeyboardAction(KeyboardAction keyAction)
{
	if (keyAction.wParam == WM_KEYDOWN || keyAction.wParam == WM_SYSKEYDOWN)
	{
		if (pressedKeys.find(keyAction.additionalInfo.vkCode) != pressedKeys.end())
			return;

		pressedKeys[keyAction.additionalInfo.vkCode] = true;

		if (pressedKeys.find(164) != pressedKeys.end())
		{
			altTempStorage = keyAction;
			return;
		}
	}
	else if (keyAction.wParam == WM_KEYUP || keyAction.wParam == WM_SYSKEYUP)
	{
		if (pressedKeys.find(164) != pressedKeys.end() && keyAction.additionalInfo.vkCode == 164)
		{
			lastLeftAltUp = keyAction;

			pressedKeys.erase(keyAction.additionalInfo.vkCode);
			return;
		}

		DWORD timeFromLastLeftAltUp = 1000;
		if (lastLeftAltUp.wParam != 0)
			timeFromLastLeftAltUp = GetTickCount() - lastLeftAltUp.additionalInfo.time;

		if (keyAction.additionalInfo.vkCode == 77 &&
			(pressedKeys.find(164) != pressedKeys.end() || (timeFromLastLeftAltUp < 100)))
		{
			lastLeftAltUp = KeyboardAction();
			recordModeSwitch();

			pressedKeys.erase(keyAction.additionalInfo.vkCode);
			return;
		}
		else if (keyAction.additionalInfo.vkCode == 67 &&
			(pressedKeys.find(164) != pressedKeys.end() || (timeFromLastLeftAltUp < 100)))
		{
			ACTION_SYSTEM.takeScreenshoot();

			pressedKeys.erase(keyAction.additionalInfo.vkCode);
			return;
		}
		else
		{
			//if (lastLeftAltUp.wParam != 0 && actionsRecord && GetTickCount() - lastLeftAltUp.additionalInfo.time > 100)
			//{
			//	// We need to insert action according to it's time stamp.
			//	for (size_t i = 0; i < recordedActions.size(); i++)
			//	{
			//		if (recordedActions[i].time > lastLeftAltUp.additionalInfo.time)
			//		{
			//			recordedActions.insert(recordedActions.begin() + i, globalActionWrapper(globalKeyboardActionInfo(lastLeftAltUp), globalMouseActionInfo(), screenshootActionInfo()));
			//			break;
			//		}

			//		if (i == recordedActions.size() - 1)
			//		{
			//			recordedActions.push_back(globalActionWrapper(globalKeyboardActionInfo(lastLeftAltUp), globalMouseActionInfo(), screenshootActionInfo()));
			//			break;
			//		}
			//	}
			//	
			//	lastLeftAltUp = globalKeyboardActionInfo();
			//}

			//if (altTempStorage.wParam != 0 && actionsRecord)
			//{
			//	// We need to insert action according to it's time stamp.
			//	for (size_t i = 0; i < recordedActions.size(); i++)
			//	{
			//		if (recordedActions[i].time > altTempStorage.additionalInfo.time)
			//		{
			//			recordedActions.insert(recordedActions.begin() + i, globalActionWrapper(globalKeyboardActionInfo(altTempStorage), globalMouseActionInfo(), screenshootActionInfo()));
			//			break;
			//		}

			//		if (i == recordedActions.size() - 1)
			//		{
			//			recordedActions.push_back(globalActionWrapper(globalKeyboardActionInfo(altTempStorage), globalMouseActionInfo(), screenshootActionInfo()));
			//			break;
			//		}
			//	}

			//	//recordedActions.push_back(globalActionWrapper(globalKeyboardActionInfo(altTempStorage), globalMouseActionInfo(), screenshootActionInfo()));
			//	altTempStorage = globalKeyboardActionInfo();
			//}
		}

		pressedKeys.erase(keyAction.additionalInfo.vkCode);
	}

	if (recording)
	{
		addAction(new KeyboardAction(keyAction));
	}
}

void FEPTActionSystem::newMouseAction(MouseAction mouseAction)
{
	if (recording)
	{
		addAction(new MouseAction(mouseAction));
	}
}

void FEPTActionSystem::findAndDeleteKeys()
{
	if (keysToDelete.size() != 0)
	{
		for (int i = static_cast<int>(recordedActions.size() - 1); i >= 0; i--)
		{
			if (recordedActions[i]->getType() != FETP_KEYBOARD_ACTION)
				continue;

			KeyboardAction* action = reinterpret_cast<KeyboardAction*>(recordedActions[i]);

			for (int j = 0; j < int(keysToDelete.size()); j++)
			{
				if (keysToDelete.size() == 0 || recordedActions.size() == 0)
					break;
				if (action->additionalInfo.vkCode == keysToDelete[j].additionalInfo.vkCode)
				{
					if ((keysToDelete[j].wParam == WM_KEYUP && (action->wParam == WM_KEYUP || action->wParam == WM_SYSKEYUP)) ||
						(keysToDelete[j].wParam == WM_KEYDOWN && (action->wParam == WM_KEYDOWN || action->wParam == WM_SYSKEYDOWN)))
					{
						delete action;
						recordedActions.erase(recordedActions.begin() + i, recordedActions.begin() + i + 1);
						keysToDelete.erase(keysToDelete.begin() + j, keysToDelete.begin() + j + 1);

						j--;

						if (i != 0)
							i--;
					}
				}
			}
		}
	}
}

void FEPTActionSystem::update()
{
	findAndDeleteKeys();
}

FETPAction* FEPTActionSystem::getAction(size_t index)
{
	if (index >= recordedActions.size())
		return nullptr;

	return recordedActions[index];
}

void FEPTActionSystem::addAction(FETPAction* newAction)
{
	recordedActions.push_back(newAction);
	if (recordedActions.size() > 1)
	{
		int sleepAfter = 0;
		sleepAfter = newAction->getTimeStamp() - recordedActions[recordedActions.size() - 2]->getTimeStamp();
		if (sleepAfter < 0)
			sleepAfter = 0;

		if (sleepAfter != 0)
			recordedActions.insert(recordedActions.end() - 1, new SleepAction(sleepAfter));
	}
}

void FEPTActionSystem::filterActions(size_t startIndex, std::function<bool(FETPAction*, int)> filerFunction, std::vector<FETPAction*>& output, bool stopOnFirstNonMatch)
{
	if (startIndex >= recordedActions.size())
		return;

	output.clear();

	while (true)
	{
		if (startIndex >= recordedActions.size())
			break;

		if (filerFunction(recordedActions[startIndex], static_cast<int>(output.size())))
		{
			output.push_back(recordedActions[startIndex]);
		}
		else if (stopOnFirstNonMatch)
		{
			return;
		}

		startIndex++;
	}
}

bool FEPTActionSystem::mouseMoveActionFilter(FETPAction* action, int outputCount)
{
	if (action == nullptr)
		return false;

	if (action->getType() == FETP_SLEEP_ACTION)
		return true;

	if (action->getType() == FETP_MOUSE_ACTION)
	{
		if (reinterpret_cast<MouseAction*>(action)->wParam == WM_MOUSEMOVE)
			return true;
	}

	return false;
}

bool FEPTActionSystem::mouseLeftButtonActionFilter(FETPAction* action, int outputCount)
{
	if (action == nullptr)
		return false;

	if (outputCount >= 3)
		return false;

	if (action->getType() == FETP_SLEEP_ACTION)
		return true;

	if (action->getType() == FETP_MOUSE_ACTION)
	{
		if (reinterpret_cast<MouseAction*>(action)->wParam == WM_LBUTTONDOWN || reinterpret_cast<MouseAction*>(action)->wParam == WM_LBUTTONUP)
			return true;
	}

	return false;
}

bool FEPTActionSystem::mouseRightButtonActionFilter(FETPAction* action, int outputCount)
{
	if (action == nullptr)
		return false;

	if (outputCount >= 3)
		return false;

	if (action->getType() == FETP_SLEEP_ACTION)
		return true;

	if (action->getType() == FETP_MOUSE_ACTION)
	{
		if (reinterpret_cast<MouseAction*>(action)->wParam == WM_RBUTTONDOWN || reinterpret_cast<MouseAction*>(action)->wParam == WM_RBUTTONUP)
			return true;
	}

	return false;
}

bool FEPTActionSystem::mouseWheelActionFilter(FETPAction* action, int outputCount)
{
	if (action == nullptr)
		return false;

	if (action->getType() == FETP_SLEEP_ACTION)
		return true;

	if (action->getType() == FETP_MOUSE_ACTION)
	{
		if (reinterpret_cast<MouseAction*>(action)->wParam == WM_MOUSEWHEEL)
			return true;
	}

	return false;
}

bool FEPTActionSystem::keyboardTextActionFilter(FETPAction* action, int outputCount)
{
	if (action == nullptr)
		return false;

	if (action->getType() == FETP_SLEEP_ACTION)
		return true;

	if (action->getType() == FETP_KEYBOARD_ACTION)
	{
		KeyboardAction* keyboardAction = reinterpret_cast<KeyboardAction*>(action);
		if (keyboardAction->wParam == WM_KEYDOWN || keyboardAction->wParam == WM_SYSKEYDOWN ||
			keyboardAction->wParam == WM_KEYUP || keyboardAction->wParam == WM_SYSKEYUP)
		{
			if (INPUT_SYSTEM.getChar(keyboardAction) != 0 || keyboardAction->additionalInfo.vkCode == VK_RSHIFT || keyboardAction->additionalInfo.vkCode == VK_CAPITAL)
				return true;
		}
	}

	return false;
}

bool FEPTActionSystem::keyboardPressActionFilter(FETPAction* action, int outputCount)
{
	if (action == nullptr)
		return false;

	if (outputCount >= 3)
		return false;

	if (action->getType() == FETP_SLEEP_ACTION)
		return true;

	if (action->getType() == FETP_KEYBOARD_ACTION)
	{
		KeyboardAction* keyboardAction = reinterpret_cast<KeyboardAction*>(action);
		if (keyboardAction->wParam == WM_KEYDOWN || keyboardAction->wParam == WM_SYSKEYDOWN ||
			keyboardAction->wParam == WM_KEYUP || keyboardAction->wParam == WM_SYSKEYUP)
		{
			if (INPUT_SYSTEM.getChar(keyboardAction) == 0 && keyboardAction->additionalInfo.vkCode != VK_RSHIFT && keyboardAction->additionalInfo.vkCode != VK_CAPITAL)
				return true;
		}
	}

	return false;
}

VisNodeSys::Node* FEPTActionSystem::tryToPackActions(size_t& index)
{
	std::vector<FETPAction*> actionsToCombine;

	filterActions(index, mouseMoveActionFilter, actionsToCombine);
	if (actionsToCombine.size() > 1)
	{
		index += actionsToCombine.size() - 1;
		return new combinedActionNode(actionsToCombine, FETP_COMBINED_MOUSE_MOVE_ACTION);
	}

	filterActions(index, mouseLeftButtonActionFilter, actionsToCombine);
	if (actionsToCombine.size() > 1)
	{
		index += actionsToCombine.size() - 1;
		return new combinedActionNode(actionsToCombine, FETP_COMBINED_LEFT_MOUSE_ACTION);
	}

	filterActions(index, mouseRightButtonActionFilter, actionsToCombine);
	if (actionsToCombine.size() > 1)
	{
		index += actionsToCombine.size() - 1;
		return new combinedActionNode(actionsToCombine, FETP_COMBINED_RIGHT_MOUSE_ACTION);
	}

	filterActions(index, mouseWheelActionFilter, actionsToCombine);
	if (actionsToCombine.size() > 1)
	{
		index += actionsToCombine.size() - 1;
		return new combinedActionNode(actionsToCombine, FETP_COMBINED_WHEEL_MOUSE_ACTION);
	}

	filterActions(index, keyboardPressActionFilter, actionsToCombine);
	if (actionsToCombine.size() > 1)
	{
		index += actionsToCombine.size() - 1;
		return new combinedActionNode(actionsToCombine, FETP_COMBINED_KEY_PRESS_ACTION);
	}

	filterActions(index, keyboardTextActionFilter, actionsToCombine);
	if (actionsToCombine.size() > 1)
	{
		index += actionsToCombine.size() - 1;
		return new combinedActionNode(actionsToCombine, FETP_COMBINED_TEXT_INPUT_ACTION);
	}

	return nullptr;
}

void FEPTActionSystem::newAction(FETPAction* newAction)
{
	if (newAction != nullptr)
		addAction(newAction);
}

void FEPTActionSystem::setFinishRecordingCallback(std::function<void(std::vector<FETPAction*>&)> callback)
{
	finishRecordingCallback = callback;
}

FETPAction* FEPTActionSystem::copyAction(FETPAction* src)
{
	if (src->getType() == FETP_KEYBOARD_ACTION)
	{
		return new KeyboardAction(*reinterpret_cast<KeyboardAction*>(src));
	}
	else if (src->getType() == FETP_MOUSE_ACTION)
	{
		return new MouseAction(*reinterpret_cast<MouseAction*>(src));
	}
	else if (src->getType() == FETP_SCREENSHOOT_COMPARE_ACTION)
	{
		return new ScreenshootCompareAction(*reinterpret_cast<ScreenshootCompareAction*>(src));
	}
	else if (src->getType() == FETP_SLEEP_ACTION)
	{
		return new SleepAction(*reinterpret_cast<SleepAction*>(src));
	}
	else if (src->getType() == FETP_LUNCH_APPLICATION_ACTION)
	{
		return new LunchApplicationAction(*reinterpret_cast<LunchApplicationAction*>(src));
	}
	else
	{
		return new FETPAction(*src);
	}
}

std::string FEPTActionSystem::extractText(std::vector<FETPAction*> actions)
{
	std::string result = "";
	for (size_t i = 0; i < actions.size(); i++)
	{
		if (actions[i]->getType() != FETP_KEYBOARD_ACTION)
			continue;

		KeyboardAction* action = reinterpret_cast<KeyboardAction*>(actions[i]);
		if (action->wParam == WM_KEYUP)
		{
			char tempChar = INPUT_SYSTEM.getChar(action);
			if (tempChar != 0)
				result += tempChar;
		}
	}

	return result;
}

std::vector<FETPAction*> FEPTActionSystem::generateInputTextActions(std::string text, int avarageDelay)
{
	std::vector<FETPAction*> result;
	for (size_t i = 0; i < text.size(); i++)
	{
		int convertedKey = VkKeyScanExA(char(text[i]), GetKeyboardLayout(0));
		int vkCode = convertedKey & 0xff;
		int keysState = (convertedKey & 0xff00) >> 8;

		// Can't find appropriate key for that char.
		if (vkCode == -1)
			continue;

		if (keysState & 1)
		{
			KeyboardAction* newAction = new KeyboardAction();
			newAction->additionalInfo.vkCode = 0x10;
			newAction->wParam = WM_KEYDOWN;
			newAction->shiftPressed = false;
			result.push_back(newAction);
		}

		KeyboardAction* newAction = new KeyboardAction();
		newAction->additionalInfo.vkCode = vkCode;
		newAction->wParam = WM_KEYDOWN;
		newAction->shiftPressed = keysState & 1;
		result.push_back(newAction);

		SleepAction* newSleepAction = new SleepAction(avarageDelay);
		result.push_back(newSleepAction);

		newAction = new KeyboardAction();
		newAction->additionalInfo.vkCode = vkCode;
		newAction->wParam = WM_KEYUP;
		newAction->shiftPressed = keysState & 1;
		result.push_back(newAction);

		if (keysState & 1)
		{
			KeyboardAction* newAction = new KeyboardAction();
			newAction->additionalInfo.vkCode = 0x10;
			newAction->wParam = WM_KEYUP;
			newAction->shiftPressed = false;
			result.push_back(newAction);
		}
	}

	return result;
}
