#include "nodeRegionWindow.h"

nodeRegionWindow* nodeRegionWindow::_instance = nullptr;
ImVec2 nodeRegionWindow::mousePositionWhenContextMenuWasOpened = ImVec2(0, 0);
regionNode* nodeRegionWindow::currentRegion = nullptr;

nodeRegionWindow::nodeRegionWindow()
{
	std::string tempCaption = "node region area";
	strcpy_s(caption, tempCaption.size() + 1, tempCaption.c_str());

	size = ImVec2(1200, 800);
}

nodeRegionWindow::~nodeRegionWindow()
{
	if (cancelButton != nullptr)
		delete cancelButton;
}

void nodeRegionWindow::show(regionNode* region)
{
	if (region == nullptr)
		return;

	currentRegion = region;
	FEImGuiWindow::show();

	flags |= ImGuiWindowFlags_NoScrollbar;

	if (cancelButton == nullptr)
	{
		cancelButton = new ImGuiButton("Cancel");
		cancelButton->setPosition(ImVec2(size.x / 2.0f - cancelButton->getSize().x / 2, size.y - 30));
		cancelButton->setSize(ImVec2(80.0f, 25.0f));
		cancelButton->setDefaultColor(ImVec4(0.7f, 0.5f, 0.5f, 1.0f));
		cancelButton->setHoveredColor(ImVec4(0.95f, 0.5f, 0.0f, 1.0f));
		cancelButton->setActiveColor(ImVec4(0.1f, 1.0f, 0.1f, 1.0f));
	}
}

void nodeRegionWindow::render()
{
	FEImGuiWindow::render();

	if (!isVisible())
		return;

	if (currentRegion->getData() != nullptr)
	{
		currentRegion->getData()->setMainContextMenuFunc(mainContextMenu);
		//currentRegion->getData()->setNodeEventCallback(nodeCallback);
		//currentNodeArea->setAreaPosition(ImVec2(0.0f, 0.0f));
		currentRegion->getData()->setAreaSize(ImVec2(ImGui::GetWindowWidth(), ImGui::GetWindowHeight() - 35.0f));
		currentRegion->getData()->update();
	}

	ImGui::SetItemDefaultFocus();
	cancelButton->setPosition(ImVec2(size.x / 2.0f - cancelButton->getSize().x / 2, size.y - 30));
	cancelButton->render();
	if (cancelButton->getWasClicked())
		FEImGuiWindow::close();

	if (ImGui::GetIO().MouseReleased[1])
		mousePositionWhenContextMenuWasOpened = ImVec2(ImGui::GetMousePos().x - ImGui::GetWindowPos().x, ImGui::GetMousePos().y - ImGui::GetWindowPos().y) - currentRegion->getData()->getAreaRenderOffset();

	FEImGuiWindow::onRenderEnd();
}

void nodeRegionWindow::mainContextMenu()
{
	if (currentRegion == nullptr)
		return;

	if (currentRegion->getData()->getHovered() == nullptr && currentRegion->getData()->getSelected().size() == 0)
	{
		if (ImGui::BeginMenu("Add"))
		{
			globalActionNode* newNode = nullptr;

			if (ImGui::MenuItem("Sleep node"))
			{
				SleepAction* newAction = new SleepAction(10);
				newNode = new globalActionNode(newAction);
			}

			if (ImGui::MenuItem("Screen compare node..."))
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
						ScreenshootCompareAction* newAction = new ScreenshootCompareAction(tempData, 0, uWidth, uHeight);
						delete[] tempData;

						newNode = new globalActionNode(newAction);
					}
				}
			}

			if (ImGui::MenuItem("Application lunch node..."))
			{
				std::string path;
				FocalEngine::FILE_SYSTEM.showFileOpenDialog(path, applicationLoadFilter, 1);

				if (path != "")
				{
					LunchApplicationAction* newAction = new LunchApplicationAction(path);
					newNode = new globalActionNode(newAction);
				}
			}

			if (ImGui::BeginMenu("Mouse"))
			{
				if (ImGui::MenuItem("Move"))
				{
					MouseAction* newAction = new MouseAction();
					newNode = new globalActionNode(newAction);
				}

				if (ImGui::MenuItem("LeftButtonDown"))
				{
					MouseAction* newAction = new MouseAction();
					newAction->wParam = WM_LBUTTONDOWN;
					newNode = new globalActionNode(newAction);
				}

				if (ImGui::MenuItem("LeftButtonUp"))
				{
					MouseAction* newAction = new MouseAction();
					newAction->wParam = WM_LBUTTONUP;
					newNode = new globalActionNode(newAction);
				}

				if (ImGui::MenuItem("RightButtonDown"))
				{
					MouseAction* newAction = new MouseAction();
					newAction->wParam = WM_RBUTTONDOWN;
					newNode = new globalActionNode(newAction);
				}

				if (ImGui::MenuItem("RightButtonUp"))
				{
					MouseAction* newAction = new MouseAction();
					newAction->wParam = WM_RBUTTONUP;
					newNode = new globalActionNode(newAction);
				}

				if (ImGui::MenuItem("WheelRotation"))
				{
					MouseAction* newAction = new MouseAction();
					newAction->wParam = WM_MOUSEWHEEL;
					newNode = new globalActionNode(newAction);
				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Keyboard"))
			{
				if (ImGui::MenuItem("KeyDown"))
				{
					KeyboardAction* newAction = new KeyboardAction();
					newAction->wParam = WM_KEYDOWN;
					newNode = new globalActionNode(newAction);
				}

				if (ImGui::MenuItem("KeyUp"))
				{
					KeyboardAction* newAction = new KeyboardAction();
					newAction->wParam = WM_KEYUP;
					newNode = new globalActionNode(newAction);
				}

				if (ImGui::MenuItem("Combined text input..."))
				{
					textInputPopup::getInstance().show(textInputCallback);
				}

				ImGui::EndMenu();
			}

			if (newNode != nullptr)
			{
				newNode->setPosition(mousePositionWhenContextMenuWasOpened);
				currentRegion->getData()->addNode(newNode);
			}

			ImGui::EndMenu();
		}
	}
	else if (currentRegion->getData()->getSelected().size() == 1 &&
			 currentRegion->getData()->getSelected()[0]->getType() == "combinedActionNode")
	{
		combinedActionNode* currentNode = reinterpret_cast<combinedActionNode*>(currentRegion->getData()->getSelected()[0]);
		if (currentNode->getCombinedActionType() == FETP_COMBINED_TEXT_INPUT_ACTION)
		{
			if (ImGui::MenuItem(std::string("Change input text").c_str()))
			{
				combinedActionNode::nodeForCallback = currentNode;
				textInputPopup::getInstance().show(combinedActionNode::changeTextCallback, currentNode->text);
			}
		}

		if (ImGui::MenuItem(std::string("Remove").c_str()))
		{
			currentNode->remove();
		}

		if (ImGui::MenuItem(std::string("Look inside").c_str()))
		{
			previewWindow::getInstance().show(true);
			previewWindow::getInstance().currentNodeArea->clear();
			ACTION_SYSTEM.placeStructuredNodes(currentNode->data, previewWindow::getInstance().currentNodeArea, true);
		}
	}
}

void nodeRegionWindow::textInputCallback(std::string text)
{
	if (text != "")
	{
		combinedActionNode* newNode = new combinedActionNode(std::vector<FETPAction*>(), FETP_COMBINED_TEXT_INPUT_ACTION);
		newNode->changeText(text);

		newNode->setPosition(mousePositionWhenContextMenuWasOpened);
		currentRegion->getData()->addNode(newNode);
	}
}