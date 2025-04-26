#include "nodeRegionWindow.h"

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

	if (currentRegion->GetData() != nullptr)
	{
		currentRegion->GetData()->SetMainContextMenuFunc(mainContextMenu);
		currentRegion->GetData()->SetSize(ImVec2(ImGui::GetWindowWidth(), ImGui::GetWindowHeight() - 35.0f));
		currentRegion->GetData()->Update();
	}

	ImGui::SetItemDefaultFocus();
	cancelButton->setPosition(ImVec2(size.x / 2.0f - cancelButton->getSize().x / 2, size.y - 30));
	cancelButton->render();
	if (cancelButton->getWasClicked())
		FEImGuiWindow::close();

	if (ImGui::GetIO().MouseReleased[1])
		mousePositionWhenContextMenuWasOpened = ImVec2(ImGui::GetMousePos().x - ImGui::GetWindowPos().x, ImGui::GetMousePos().y - ImGui::GetWindowPos().y) - currentRegion->GetData()->GetRenderOffset();

	FEImGuiWindow::onRenderEnd();
}

void nodeRegionWindow::mainContextMenu()
{
	if (currentRegion == nullptr)
		return;

	if (currentRegion->GetData()->GetHovered() == nullptr && currentRegion->GetData()->GetSelected().size() == 0)
	{
		if (ImGui::BeginMenu("Add"))
		{
			globalActionNode* newNode = nullptr;

			if (ImGui::MenuItem("Sleep node"))
			{
				SleepAction* NewAction = new SleepAction(10);
				newNode = new globalActionNode(NewAction);
			}

			if (ImGui::MenuItem("Screen compare node..."))
			{
				std::string Path;
				FocalEngine::FILE_SYSTEM.ShowFileOpenDialog(Path, pngLoadFilter, 1);

				if (Path != "")
				{
					std::vector<unsigned char> rawData;
					unsigned uWidth, uHeight;
					int error = lodepng::decode(rawData, uWidth, uHeight, Path);

					if (error == 0)
					{
						unsigned char* tempData = new unsigned char[uWidth * uHeight * 4];
						memcpy_s(tempData, uWidth * uHeight * 4, rawData.data(), uWidth * uHeight * 4);
						ScreenshootCompareAction* NewAction = new ScreenshootCompareAction(tempData, 0, uWidth, uHeight);
						delete[] tempData;

						newNode = new globalActionNode(NewAction);
					}
				}
			}

			if (ImGui::MenuItem("Application lunch node..."))
			{
				std::string Path;
				FocalEngine::FILE_SYSTEM.ShowFileOpenDialog(Path, ApplicationLoadFilter, 1);

				if (Path != "")
				{
					LunchApplicationAction* NewAction = new LunchApplicationAction(Path);
					newNode = new globalActionNode(NewAction);
				}
			}

			if (ImGui::BeginMenu("Mouse"))
			{
				if (ImGui::MenuItem("Move"))
				{
					MouseAction* NewAction = new MouseAction();
					newNode = new globalActionNode(NewAction);
				}

				if (ImGui::MenuItem("LeftButtonDown"))
				{
					MouseAction* NewAction = new MouseAction();
					NewAction->EventType = WM_LBUTTONDOWN;
					newNode = new globalActionNode(NewAction);
				}

				if (ImGui::MenuItem("LeftButtonUp"))
				{
					MouseAction* NewAction = new MouseAction();
					NewAction->EventType = WM_LBUTTONUP;
					newNode = new globalActionNode(NewAction);
				}

				if (ImGui::MenuItem("RightButtonDown"))
				{
					MouseAction* NewAction = new MouseAction();
					NewAction->EventType = WM_RBUTTONDOWN;
					newNode = new globalActionNode(NewAction);
				}

				if (ImGui::MenuItem("RightButtonUp"))
				{
					MouseAction* NewAction = new MouseAction();
					NewAction->EventType = WM_RBUTTONUP;
					newNode = new globalActionNode(NewAction);
				}

				if (ImGui::MenuItem("WheelRotation"))
				{
					MouseAction* NewAction = new MouseAction();
					NewAction->EventType = WM_MOUSEWHEEL;
					newNode = new globalActionNode(NewAction);
				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Keyboard"))
			{
				if (ImGui::MenuItem("KeyDown"))
				{
					KeyboardAction* NewAction = new KeyboardAction();
					NewAction->EventType = WM_KEYDOWN;
					newNode = new globalActionNode(NewAction);
				}

				if (ImGui::MenuItem("KeyUp"))
				{
					KeyboardAction* NewAction = new KeyboardAction();
					NewAction->EventType = WM_KEYUP;
					newNode = new globalActionNode(NewAction);
				}

				if (ImGui::MenuItem("Combined text input..."))
				{
					textInputPopup::GetInstance().show(textInputCallback);
				}

				ImGui::EndMenu();
			}

			if (newNode != nullptr)
			{
				newNode->SetPosition(mousePositionWhenContextMenuWasOpened);
				currentRegion->GetData()->AddNode(newNode);
			}

			ImGui::EndMenu();
		}
	}
	else if (currentRegion->GetData()->GetSelected().size() == 1 &&
			 currentRegion->GetData()->GetSelected()[0]->GetType() == "combinedActionNode")
	{
		combinedActionNode* currentNode = reinterpret_cast<combinedActionNode*>(currentRegion->GetData()->GetSelected()[0]);
		if (currentNode->GetCombinedActionType() == FETP_COMBINED_TEXT_INPUT_ACTION)
		{
			if (ImGui::MenuItem(std::string("Change input text").c_str()))
			{
				combinedActionNode::NodeForCallback = currentNode;
				textInputPopup::GetInstance().show(combinedActionNode::ChangeTextCallback, currentNode->Text);
			}
		}

		if (ImGui::MenuItem(std::string("Remove").c_str()))
		{
			currentNode->Remove();
		}

		if (ImGui::MenuItem(std::string("Look inside").c_str()))
		{
			previewWindow::GetInstance().show(true);
			previewWindow::GetInstance().currentNodeArea->Clear();
			ACTION_SYSTEM.PlaceStructuredNodes(currentNode->Data, previewWindow::GetInstance().currentNodeArea, true);
		}
	}
}

void nodeRegionWindow::textInputCallback(std::string text)
{
	if (text != "")
	{
		combinedActionNode* newNode = new combinedActionNode(std::vector<FETPAction*>(), FETP_COMBINED_TEXT_INPUT_ACTION);
		newNode->ChangeText(text);

		newNode->SetPosition(mousePositionWhenContextMenuWasOpened);
		currentRegion->GetData()->AddNode(newNode);
	}
}