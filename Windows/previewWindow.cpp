#include "previewWindow.h"
using namespace VisNodeSys;

ImVec2 previewWindow::nodeGridRelativePosition = ImVec2(0, 0);
ImVec2 previewWindow::windowPosition = ImVec2(0, 0);
ImVec2 previewWindow::mousePositionWhenContextMenuWasOpened = ImVec2(0, 0);
NodeArea* previewWindow::currentNodeArea = nullptr;
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
		currentNodeArea->AddNodeEventCallback(nodeCallback);

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
		int Width, Height;
		FocalEngine::APPLICATION.GetMainWindow()->GetSize(&Width, &Height);

		ImGui::SetWindowPos(ImVec2(Width / 2 - popupSize.x / 2.0f, Height / 2 - popupSize.y / 2.0f));

		currentNodeArea->SetPosition(nodeGridRelativePosition);
		currentNodeArea->SetSize(ImVec2(ImGui::GetWindowWidth(), ImGui::GetWindowHeight() - 35.0f));

		if (firstFrame)
		{
			firstFrame = false;
			positionNodesInCenter();
		}

		currentNodeArea->Update();

		if (ImGui::GetIO().MouseReleased[1])
			mousePositionWhenContextMenuWasOpened = ImVec2(ImGui::GetMousePos().x, ImGui::GetMousePos().y) - currentNodeArea->GetRenderOffset();

		if (!readOnly)
		{
			if (TEST_MANAGER.getSelectedTest() == nullptr)
				ImGui::BeginDisabled();

			addButton->render();
			if (addButton->getWasClicked() && TEST_MANAGER.getSelectedTest() != nullptr)
			{
				// Shift all nodes in order to place them in view center of selected test node area.
				positionNodesInTargetCenter();

				NODE_SYSTEM.MoveNodesTo(currentNodeArea, TEST_MANAGER.getSelectedTest()->NodeArea, true);
				close();
			}

			if (TEST_MANAGER.getSelectedTest() == nullptr)
				ImGui::EndDisabled();

			newTestButton->render();
			if (newTestButton->getWasClicked())
			{
				TEST_MANAGER.AddTest();

				NODE_SYSTEM.MoveNodesTo(currentNodeArea, TEST_MANAGER.list.back()->NodeArea);
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
			SleepAction* NewAction = new SleepAction(10);
			globalActionNode* newNode = new globalActionNode(NewAction);
			newNode->SetPosition(mousePositionWhenContextMenuWasOpened);

			currentNodeArea->AddNode(newNode);
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

					globalActionNode* newNode = new globalActionNode(NewAction);
					newNode->SetPosition(mousePositionWhenContextMenuWasOpened);

					currentNodeArea->AddNode(newNode);
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
				globalActionNode* newNode = new globalActionNode(NewAction);
				newNode->SetPosition(mousePositionWhenContextMenuWasOpened);

				currentNodeArea->AddNode(newNode);
			}
		}

		if (ImGui::BeginMenu("Mouse"))
		{
			if (ImGui::MenuItem("Move"))
			{
				MouseAction* NewAction = new MouseAction();
				globalActionNode* newNode = new globalActionNode(NewAction);
				newNode->SetPosition(mousePositionWhenContextMenuWasOpened);

				currentNodeArea->AddNode(newNode);
			}

			if (ImGui::MenuItem("LeftButtonDown"))
			{
				MouseAction* NewAction = new MouseAction();
				NewAction->EventType = WM_LBUTTONDOWN;
				globalActionNode* newNode = new globalActionNode(NewAction);
				newNode->SetPosition(mousePositionWhenContextMenuWasOpened);

				currentNodeArea->AddNode(newNode);
			}

			if (ImGui::MenuItem("LeftButtonUp"))
			{
				MouseAction* NewAction = new MouseAction();
				NewAction->EventType = WM_LBUTTONUP;
				globalActionNode* newNode = new globalActionNode(NewAction);
				newNode->SetPosition(mousePositionWhenContextMenuWasOpened);

				currentNodeArea->AddNode(newNode);
			}

			if (ImGui::MenuItem("RightButtonDown"))
			{
				MouseAction* NewAction = new MouseAction();
				NewAction->EventType = WM_RBUTTONDOWN;
				globalActionNode* newNode = new globalActionNode(NewAction);
				newNode->SetPosition(mousePositionWhenContextMenuWasOpened);

				currentNodeArea->AddNode(newNode);
			}

			if (ImGui::MenuItem("RightButtonUp"))
			{
				MouseAction* NewAction = new MouseAction();
				NewAction->EventType = WM_RBUTTONUP;
				globalActionNode* newNode = new globalActionNode(NewAction);
				newNode->SetPosition(mousePositionWhenContextMenuWasOpened);

				currentNodeArea->AddNode(newNode);
			}

			if (ImGui::MenuItem("WheelRotation"))
			{
				MouseAction* NewAction = new MouseAction();
				NewAction->EventType = WM_MOUSEWHEEL;
				globalActionNode* newNode = new globalActionNode(NewAction);
				newNode->SetPosition(mousePositionWhenContextMenuWasOpened);

				currentNodeArea->AddNode(newNode);
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Keyboard"))
		{
			if (ImGui::MenuItem("KeyDown"))
			{
				KeyboardAction* NewAction = new KeyboardAction();
				NewAction->EventType = WM_KEYDOWN;
				globalActionNode* newNode = new globalActionNode(NewAction);
				newNode->SetPosition(mousePositionWhenContextMenuWasOpened);

				currentNodeArea->AddNode(newNode);
			}

			if (ImGui::MenuItem("KeyUp"))
			{
				KeyboardAction* NewAction = new KeyboardAction();
				NewAction->EventType = WM_KEYUP;
				globalActionNode* newNode = new globalActionNode(NewAction);
				newNode->SetPosition(mousePositionWhenContextMenuWasOpened);

				currentNodeArea->AddNode(newNode);
			}

			/*if (ImGui::MenuItem("Combined text input..."))
			{
				textInputPopup::GetInstance().show(textInputCallback);
			}*/

			ImGui::EndMenu();
		}

		ImGui::EndMenu();
	}
}

void previewWindow::nodeCallback(VisNodeSys::Node* node, VisNodeSys::NODE_EVENT eventWithNode)
{

}

void previewWindow::positionNodesInCenter()
{
	ImVec2 viewCenter = currentNodeArea->GetRenderedViewCenter();
	ImVec2 nodesAABBCenter = currentNodeArea->GetAllElementsAABBCenter();

	neededShift = viewCenter - nodesAABBCenter;

	currentNodeArea->RunOnEachNode([](VisNodeSys::Node* node) {
		size_t outSocketCount = node->GetOutputSocketCount();
		node->SetPosition(node->GetPosition() + neededShift);
	});

	neededShift = ImVec2(0, 0);
}

void previewWindow::positionNodesInTargetCenter()
{
	if (TEST_MANAGER.getSelectedTest() == nullptr)
		return;

	ImVec2 viewCenter = TEST_MANAGER.getSelectedTest()->NodeArea->GetRenderedViewCenter();
	ImVec2 nodesAABBCenter = currentNodeArea->GetAllElementsAABBCenter();
	nodesAABBCenter -= currentNodeArea->GetRenderOffset();

	neededShift = viewCenter - nodesAABBCenter;

	currentNodeArea->RunOnEachNode([](VisNodeSys::Node* node) {
		size_t outSocketCount = node->GetOutputSocketCount();
		node->SetPosition(node->GetPosition() + neededShift);
	});

	neededShift = ImVec2(0, 0);
}