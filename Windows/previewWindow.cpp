#include "previewWindow.h"

previewWindow* previewWindow::Instance = nullptr;
ImVec2 previewWindow::nodeGridRelativePosition = ImVec2(0, 0);
ImVec2 previewWindow::windowPosition = ImVec2(0, 0);
ImVec2 previewWindow::mousePositionWhenContextMenuWasOpened = ImVec2(0, 0);
FEVisualNodeArea* previewWindow::currentNodeArea = nullptr;
ImVec2 previewWindow::neededShift = ImVec2(0, 0);
bool previewWindow::readOnly = false;

previewWindow::previewWindow()
{
	popupCaption = "Preview of nodes to add";
}

previewWindow::~previewWindow()
{
	if (cancelButton != nullptr)
		delete cancelButton;

	if (addButton != nullptr)
		delete addButton;

	if (newTestButton != nullptr)
		delete newTestButton;
}

void previewWindow::show(bool isReadOnly)
{
	readOnly = isReadOnly;
	shouldOpen = true;
	firstFrame = true;

	if (currentNodeArea == nullptr)
	{
		currentNodeArea = NODE_SYSTEM.CreateNodeArea();
		currentNodeArea->SetMainContextMenuFunc(mainContextMenu);
		currentNodeArea->SetNodeEventCallback(nodeCallback);

		cancelButton = new ImGuiButton("Cancel");
		cancelButton->setPosition(ImVec2(popupSize.x - popupSize.x / 6.0f - cancelButton->getSize().x / 2.0f, popupSize.y - 30.0f));
		cancelButton->setSize(ImVec2(80.0f, 25.0f));
		cancelButton->setDefaultColor(ImVec4(0.7f, 0.5f, 0.5f, 1.0f));
		cancelButton->setHoveredColor(ImVec4(0.95f, 0.5f, 0.0f, 1.0f));
		cancelButton->setActiveColor(ImVec4(0.1f, 1.0f, 0.1f, 1.0f));

		addButton = new ImGuiButton("Add to current test");
		addButton->setPosition(ImVec2(popupSize.x * 0.33f - popupSize.x / 6.0f - addButton->getSize().x / 2.0f, popupSize.y - 30.0f));
		addButton->setSize(ImVec2(180.0f, 25.0f));

		newTestButton = new ImGuiButton("Add as new test");
		newTestButton->setPosition(ImVec2(popupSize.x * 0.66f - popupSize.x / 6.0f - newTestButton->getSize().x / 2.0f, popupSize.y - 30.0f));
		newTestButton->setSize(ImVec2(150.0f, 25.0f));
	}

	if (readOnly)
		cancelButton->setPosition(ImVec2(popupSize.x / 2.0f - cancelButton->getSize().x / 2.0f, popupSize.y - 30.0f));
}

void previewWindow::close()
{
	ImGuiModalPopup::close();
	NODE_SYSTEM.DeleteNodeArea(currentNodeArea);
	currentNodeArea = nullptr;
}

void previewWindow::render()
{
	if (currentNodeArea == nullptr)
		return;

	ImGuiModalPopup::render();

	ImGui::SetNextWindowSize(popupSize);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	if (ImGui::BeginPopupModal(popupCaption.c_str(), NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar))
	{
		ImGui::SetWindowPos(ImVec2(TEST_PLATFORM.getWindowWidth() / 2 - popupSize.x / 2.0f, TEST_PLATFORM.getWindowHeight() / 2 - popupSize.y / 2.0f));

		currentNodeArea->SetAreaPosition(nodeGridRelativePosition);
		currentNodeArea->SetAreaSize(ImVec2(ImGui::GetWindowWidth(), ImGui::GetWindowHeight() - 35.0f));

		if (firstFrame)
		{
			firstFrame = false;
			positionNodesInCenter();
		}

		currentNodeArea->Update();

		if (ImGui::GetIO().MouseReleased[1])
			mousePositionWhenContextMenuWasOpened = ImVec2(ImGui::GetMousePos().x, ImGui::GetMousePos().y) - currentNodeArea->GetAreaRenderOffset();

		if (!readOnly)
		{
			if (TEST_MANAGER.getSelectedTest() == nullptr)
				ImGui::BeginDisabled();

			addButton->render();
			if (addButton->getWasClicked() && TEST_MANAGER.getSelectedTest() != nullptr)
			{
				// Shift all nodes in order to place them in view center of selected test node area.
				positionNodesInTargetCenter();

				NODE_SYSTEM.MoveNodesTo(currentNodeArea, TEST_MANAGER.getSelectedTest()->nodeArea, true);
				close();
			}

			if (TEST_MANAGER.getSelectedTest() == nullptr)
				ImGui::EndDisabled();

			newTestButton->render();
			if (newTestButton->getWasClicked())
			{
				TEST_MANAGER.addTest();

				NODE_SYSTEM.MoveNodesTo(currentNodeArea, TEST_MANAGER.list.back()->nodeArea);
				close();
			}
		}

		ImGui::SetItemDefaultFocus();
		cancelButton->render();
		if (cancelButton->getWasClicked())
		{
			currentNodeArea->Clear();
			close();
		}

		ImGui::PopStyleVar();
		ImGui::EndPopup();
	}
	else
	{
		ImGui::PopStyleVar();
	}
}

void previewWindow::mainContextMenu()
{
	if (readOnly)
		return;

	if (ImGui::BeginMenu("Add"))
	{
		if (ImGui::MenuItem("Sleep node"))
		{
			SleepAction* newAction = new SleepAction(10);
			globalActionNode* newNode = new globalActionNode(newAction);
			newNode->SetPosition(mousePositionWhenContextMenuWasOpened);

			currentNodeArea->AddNode(newNode);
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

					globalActionNode* newNode = new globalActionNode(newAction);
					newNode->SetPosition(mousePositionWhenContextMenuWasOpened);

					currentNodeArea->AddNode(newNode);
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
				globalActionNode* newNode = new globalActionNode(newAction);
				newNode->SetPosition(mousePositionWhenContextMenuWasOpened);

				currentNodeArea->AddNode(newNode);
			}
		}

		if (ImGui::BeginMenu("Mouse"))
		{
			if (ImGui::MenuItem("Move"))
			{
				MouseAction* newAction = new MouseAction();
				globalActionNode* newNode = new globalActionNode(newAction);
				newNode->SetPosition(mousePositionWhenContextMenuWasOpened);

				currentNodeArea->AddNode(newNode);
			}

			if (ImGui::MenuItem("LeftButtonDown"))
			{
				MouseAction* newAction = new MouseAction();
				newAction->wParam = WM_LBUTTONDOWN;
				globalActionNode* newNode = new globalActionNode(newAction);
				newNode->SetPosition(mousePositionWhenContextMenuWasOpened);

				currentNodeArea->AddNode(newNode);
			}

			if (ImGui::MenuItem("LeftButtonUp"))
			{
				MouseAction* newAction = new MouseAction();
				newAction->wParam = WM_LBUTTONUP;
				globalActionNode* newNode = new globalActionNode(newAction);
				newNode->SetPosition(mousePositionWhenContextMenuWasOpened);

				currentNodeArea->AddNode(newNode);
			}

			if (ImGui::MenuItem("RightButtonDown"))
			{
				MouseAction* newAction = new MouseAction();
				newAction->wParam = WM_RBUTTONDOWN;
				globalActionNode* newNode = new globalActionNode(newAction);
				newNode->SetPosition(mousePositionWhenContextMenuWasOpened);

				currentNodeArea->AddNode(newNode);
			}

			if (ImGui::MenuItem("RightButtonUp"))
			{
				MouseAction* newAction = new MouseAction();
				newAction->wParam = WM_RBUTTONUP;
				globalActionNode* newNode = new globalActionNode(newAction);
				newNode->SetPosition(mousePositionWhenContextMenuWasOpened);

				currentNodeArea->AddNode(newNode);
			}

			if (ImGui::MenuItem("WheelRotation"))
			{
				MouseAction* newAction = new MouseAction();
				newAction->wParam = WM_MOUSEWHEEL;
				globalActionNode* newNode = new globalActionNode(newAction);
				newNode->SetPosition(mousePositionWhenContextMenuWasOpened);

				currentNodeArea->AddNode(newNode);
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Keyboard"))
		{
			if (ImGui::MenuItem("KeyDown"))
			{
				KeyboardAction* newAction = new KeyboardAction();
				newAction->wParam = WM_KEYDOWN;
				globalActionNode* newNode = new globalActionNode(newAction);
				newNode->SetPosition(mousePositionWhenContextMenuWasOpened);

				currentNodeArea->AddNode(newNode);
			}

			if (ImGui::MenuItem("KeyUp"))
			{
				KeyboardAction* newAction = new KeyboardAction();
				newAction->wParam = WM_KEYUP;
				globalActionNode* newNode = new globalActionNode(newAction);
				newNode->SetPosition(mousePositionWhenContextMenuWasOpened);

				currentNodeArea->AddNode(newNode);
			}

			/*if (ImGui::MenuItem("Combined text input..."))
			{
				textInputPopup::getInstance().show(textInputCallback);
			}*/

			ImGui::EndMenu();
		}

		ImGui::EndMenu();
	}
}

void previewWindow::nodeCallback(FEVisualNode* node, FE_VISUAL_NODE_EVENT eventWithNode)
{

}

void previewWindow::positionNodesInCenter()
{
	ImVec2 viewCenter = currentNodeArea->GetRenderedViewCenter();
	ImVec2 nodesAABBCenter = currentNodeArea->GetAllNodesAABBCenter();

	neededShift = viewCenter - nodesAABBCenter;

	currentNodeArea->RunOnEachNode([](FEVisualNode* node) {
		size_t outSocketCount = node->OutSocketCount();
		node->SetPosition(node->GetPosition() + neededShift);
	});

	neededShift = ImVec2(0, 0);
}

void previewWindow::positionNodesInTargetCenter()
{
	if (TEST_MANAGER.getSelectedTest() == nullptr)
		return;

	ImVec2 viewCenter = TEST_MANAGER.getSelectedTest()->nodeArea->GetRenderedViewCenter();
	ImVec2 nodesAABBCenter = currentNodeArea->GetAllNodesAABBCenter();
	nodesAABBCenter -= currentNodeArea->GetAreaRenderOffset();

	neededShift = viewCenter - nodesAABBCenter;

	currentNodeArea->RunOnEachNode([](FEVisualNode* node) {
		size_t outSocketCount = node->OutSocketCount();
		node->SetPosition(node->GetPosition() + neededShift);
	});

	neededShift = ImVec2(0, 0);
}