#include "testEditorWindow.h"

testEditorWinow* testEditorWinow::Instance = nullptr;
ImVec2 testEditorWinow::mousePositionWhenContextMenuWasOpened = ImVec2(0, 0);

testEditorWinow::testEditorWinow()
{
	std::string tempCaption = "main area";
	strcpy_s(caption, tempCaption.size() + 1, tempCaption.c_str());
	
	ACTION_SYSTEM.setFinishRecordingCallback(finishRecordingCallback);
}

testEditorWinow::~testEditorWinow()
{
}

void testEditorWinow::show()
{
}

void testEditorWinow::render()
{
	ImGui::Begin("Test Editor", nullptr, ImGuiWindowFlags_None | ImGuiWindowFlags_NoScrollbar);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

	renderMainMenu();

	if (TEST_MANAGER.getSelectedTest() != nullptr)
	{
		VisualNodeArea* currentNodeArea = TEST_MANAGER.getSelectedTest()->nodeArea;

		currentNodeArea->SetMainContextMenuFunc(mainContextMenu);
		currentNodeArea->SetNodeEventCallback(nodeCallback);

		currentNodeArea->SetAreaPosition(ImVec2(0.0f, 0.0f));
		currentNodeArea->SetAreaSize(ImVec2(ImGui::GetWindowWidth(), ImGui::GetWindowHeight()));
		currentNodeArea->Update();
	}

	if (ImGui::GetIO().MouseReleased[1] && TEST_MANAGER.getSelectedTest() != nullptr)
	{
		mousePositionWhenContextMenuWasOpened = ImVec2(ImGui::GetMousePos().x - ImGui::GetWindowPos().x, ImGui::GetMousePos().y - ImGui::GetWindowPos().y) - TEST_MANAGER.getSelectedTest()->nodeArea->GetAreaRenderOffset();
		mousePositionWhenContextMenuWasOpened /= TEST_MANAGER.getSelectedTest()->nodeArea->GetZoomFactor();
	}	

	ImGui::PopStyleVar();
	ImGui::End();
}

void testEditorWinow::mainContextMenu()
{
	if (TEST_MANAGER.getSelectedTest() == nullptr || TEST_MANAGER.getSelectedTest()->nodeArea == nullptr)
		return;

	if (TEST_MANAGER.getSelectedTest()->nodeArea->GetHovered() == nullptr && TEST_MANAGER.getSelectedTest()->nodeArea->GetSelected().size() == 0)
	{
		if (ImGui::BeginMenu("Add"))
		{
			globalActionNode* newNode = nullptr;

			if (ImGui::MenuItem("Int"))
			{
				intNode* newNode = new intNode();
				newNode->SetPosition(mousePositionWhenContextMenuWasOpened);
				TEST_MANAGER.getSelectedTest()->nodeArea->AddNode(newNode);
			}

			if (ImGui::MenuItem("Float"))
			{
				floatNode* newNode = new floatNode();
				newNode->SetPosition(mousePositionWhenContextMenuWasOpened);
				TEST_MANAGER.getSelectedTest()->nodeArea->AddNode(newNode);
			}

			if (ImGui::MenuItem("Vec2"))
			{
				vec2Node* newNode = new vec2Node();
				newNode->SetPosition(mousePositionWhenContextMenuWasOpened);
				TEST_MANAGER.getSelectedTest()->nodeArea->AddNode(newNode);
			}

			if (ImGui::MenuItem("Vec2 add"))
			{
				vec2AddNode* newNode = new vec2AddNode();
				newNode->SetPosition(mousePositionWhenContextMenuWasOpened);
				TEST_MANAGER.getSelectedTest()->nodeArea->AddNode(newNode);
			}

			if (ImGui::BeginMenu("Flow control nodes"))
			{
				if (ImGui::MenuItem("Bool"))
				{
					boolNode* newNode = new boolNode();
					newNode->SetPosition(mousePositionWhenContextMenuWasOpened);
					TEST_MANAGER.getSelectedTest()->nodeArea->AddNode(newNode);
				}

				if (ImGui::MenuItem("Branch"))
				{
					branchNode* newNode = new branchNode();
					newNode->SetPosition(mousePositionWhenContextMenuWasOpened);
					TEST_MANAGER.getSelectedTest()->nodeArea->AddNode(newNode);
				}

				if (ImGui::MenuItem("Sequence"))
				{
					sequenceNode* neNode = new sequenceNode();
					neNode->SetPosition(mousePositionWhenContextMenuWasOpened);
					TEST_MANAGER.getSelectedTest()->nodeArea->AddNode(neNode);
				}

				if (ImGui::MenuItem("ForLoop"))
				{
					//branchNode* newBranchNode = new branchNode();
					//newBranchNode->SetPosition(mousePositionWhenContextMenuWasOpened);
					//TEST_MANAGER.getSelectedTest()->nodeArea->AddNode(newBranchNode);
				}

				ImGui::EndMenu();
			}

			if (ImGui::MenuItem("Region node"))
			{
				regionNode* newRegionNode = new regionNode();
				newRegionNode->SetPosition(mousePositionWhenContextMenuWasOpened);
				TEST_MANAGER.getSelectedTest()->nodeArea->AddNode(newRegionNode);
			}

			if (ImGui::MenuItem("Sleep node"))
			{
				sleepNode* newNode = new sleepNode();
				newNode->SetPosition(mousePositionWhenContextMenuWasOpened);
				TEST_MANAGER.getSelectedTest()->nodeArea->AddNode(newNode);
				//SleepAction* newAction = new SleepAction(10);
				//newNode = new globalActionNode(newAction);
			}

			if (ImGui::MenuItem("Timer node"))
			{
				timerNode* newNode = new timerNode();
				newNode->SetPosition(mousePositionWhenContextMenuWasOpened);
				TEST_MANAGER.getSelectedTest()->nodeArea->AddNode(newNode);
			}

			if (ImGui::MenuItem("Image node"))
			{
				imageNode* newNode = new imageNode();
				newNode->SetPosition(mousePositionWhenContextMenuWasOpened);
				TEST_MANAGER.getSelectedTest()->nodeArea->AddNode(newNode);
			}

			if (ImGui::MenuItem("Image search node"))
			{
				imageSearchNode* newNode = new imageSearchNode();
				newNode->SetPosition(mousePositionWhenContextMenuWasOpened);
				TEST_MANAGER.getSelectedTest()->nodeArea->AddNode(newNode);
			}
			
			if (ImGui::MenuItem("Screen compare node..."))
			{
				//std::string path;
				//FocalEngine::FILE_SYSTEM.showFileOpenDialog(path, pngLoadFilter, 1);

				//if (path != "")
				//{
				//	std::vector<unsigned char> rawData;
				//	unsigned uWidth, uHeight;
				//	int error = lodepng::decode(rawData, uWidth, uHeight, path);

				//	if (error == 0)
				//	{
				//		unsigned char* tempData = new unsigned char[uWidth * uHeight * 4];
				//		memcpy_s(tempData, uWidth * uHeight * 4, rawData.data(), uWidth * uHeight * 4);
				//		ScreenshootCompareAction* newAction = new ScreenshootCompareAction(tempData, 0, uWidth, uHeight);
				//		delete[] tempData;

				//		newNode = new globalActionNode(newAction);
				//	}
				//}
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
					mouseMoveNode* newNode = new mouseMoveNode();
					newNode->SetPosition(mousePositionWhenContextMenuWasOpened);
					TEST_MANAGER.getSelectedTest()->nodeArea->AddNode(newNode);
				}

				if (ImGui::MenuItem("LeftButtonDown"))
				{
					mouseLeftButtonDown* newNode = new mouseLeftButtonDown();
					newNode->SetPosition(mousePositionWhenContextMenuWasOpened);
					TEST_MANAGER.getSelectedTest()->nodeArea->AddNode(newNode);
					/*MouseAction* newAction = new MouseAction();
					newAction->wParam = WM_LBUTTONDOWN;
					newNode = new globalActionNode(newAction);*/
				}

				if (ImGui::MenuItem("LeftButtonUp"))
				{
					mouseLeftButtonUp* newNode = new mouseLeftButtonUp();
					newNode->SetPosition(mousePositionWhenContextMenuWasOpened);
					TEST_MANAGER.getSelectedTest()->nodeArea->AddNode(newNode);
					/*MouseAction* newAction = new MouseAction();
					newAction->wParam = WM_LBUTTONUP;
					newNode = new globalActionNode(newAction);*/
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
				newNode->SetPosition(mousePositionWhenContextMenuWasOpened);
				TEST_MANAGER.getSelectedTest()->nodeArea->AddNode(newNode);
			}

			ImGui::EndMenu();
		}
	}
	else if (TEST_MANAGER.getSelectedTest()->nodeArea->GetSelected().size() == 1 &&
			 TEST_MANAGER.getSelectedTest()->nodeArea->GetSelected()[0]->GetType() == "regionNode")
	{
		if (ImGui::MenuItem("Open region"))
		{
			nodeRegionWindow::getInstance().show(reinterpret_cast<regionNode*>(TEST_MANAGER.getSelectedTest()->nodeArea->GetSelected()[0]));
		}

		if (ImGui::MenuItem("Rename"))
		{
			textInputPopup::getInstance().show(textInputChangeNameCallback, TEST_MANAGER.getSelectedTest()->nodeArea->GetSelected()[0]->GetName());
		}

		if (ImGui::MenuItem("Remove"))
		{
			TEST_MANAGER.getSelectedTest()->nodeArea->DeleteNode(TEST_MANAGER.getSelectedTest()->nodeArea->GetSelected()[0]);
		}
	}
	else if (TEST_MANAGER.getSelectedTest()->nodeArea->GetSelected().size() == 1 &&
			 TEST_MANAGER.getSelectedTest()->nodeArea->GetSelected()[0]->GetType() == "combinedActionNode")
	{
		combinedActionNode* currentNode = reinterpret_cast<combinedActionNode*>(TEST_MANAGER.getSelectedTest()->nodeArea->GetSelected()[0]);
		if (currentNode->GetCombinedActionType() == FETP_COMBINED_TEXT_INPUT_ACTION)
		{
			if (ImGui::MenuItem(std::string("Change input text").c_str()))
			{
				combinedActionNode::NodeForCallback = currentNode;
				textInputPopup::getInstance().show(combinedActionNode::ChangeTextCallback, currentNode->Text);
			}
		}

		if (ImGui::MenuItem(std::string("Remove").c_str()))
		{
			currentNode->Remove();
		}

		if (ImGui::MenuItem(std::string("Look inside").c_str()))
		{
			previewWindow::getInstance().show(true);
			previewWindow::getInstance().currentNodeArea->Clear();
			ACTION_SYSTEM.placeStructuredNodes(currentNode->Data, previewWindow::getInstance().currentNodeArea, true);
		}
	}
	else if (TEST_MANAGER.getSelectedTest()->nodeArea->GetSelected().size() > 1)
	{
		auto selectedList = TEST_MANAGER.getSelectedTest()->nodeArea->GetSelected();
		if (ImGui::MenuItem("Delete selected nodes"))
		{
			for (size_t i = 0; i < selectedList.size(); i++)
			{
				TEST_MANAGER.getSelectedTest()->nodeArea->DeleteNode(selectedList[i]);
			}

			TEST_MANAGER.getSelectedTest()->nodeArea->ClearSelection();
		}

		if (ImGui::MenuItem("Save selected nodes..."))
		{
			std::string path;
			FocalEngine::FILE_SYSTEM.showFileSaveDialog(path, NULL/*saveSnippetFileFilter*/, 0/*1*/);

			if (path != "")
			{
				TEST_MANAGER.getSelectedTest()->validateImagePathes(TEST_MANAGER.getSelectedTest()->nodeArea, path);
				TEST_MANAGER.getSelectedTest()->nodeArea->SaveNodesToFile(path.c_str(), selectedList);
			}
		}

		/*if (ImGui::MenuItem("Place in a grid"))
		{
			int nodesPerW = 4;
			int nodesPerH = 4;
			int disBetweenOnW = 40;
			int disBetweenOnH = 150;

			auto selectedList = TEST_MANAGER.getSelectedTest()->nodeArea->GetSelected();
			for (size_t i = 0; i < selectedList.size(); i++)
			{
				int xPosition = selectedList[0]->getPosition().x;
				xPosition += (i % nodesPerW * int(selectedList[i]->getSize().x + disBetweenOnW));
				xPosition %= TEST_PLATFORM.getWindowWidth();

				int yPosition = i / nodesPerH;
				yPosition *= disBetweenOnH;

				selectedList[i]->SetPosition(ImVec2(float(xPosition), float(yPosition)));
			}
		}*/
	}
}

void testEditorWinow::nodeCallback(VisualNode* node, VISUAL_NODE_EVENT eventWithNode)
{
	if (node == nullptr)
		return;

	if (eventWithNode == VISUAL_NODE_BEFORE_CONNECTED || eventWithNode == VISUAL_NODE_BEFORE_DISCONNECTED)
		return;

	// Change style of all connections to default.
	TEST_MANAGER.getSelectedTest()->nodeArea->RunOnEachNode([](VisualNode* node) {
		size_t OutSocketCount = node->OutSocketCount();
		for (size_t i = 0; i < OutSocketCount; i++)
		{
			VisualNodeConnectionStyle TempStyle;
			node->GetSocketStyle(true, i, TempStyle);
			TempStyle.bMarchingAntsEffect = false;
			node->SetSocketStyle(true, i, TempStyle);
		}
	});

	// Start at begin node at change each connection color from it.
	if (TEST_MANAGER.getSelectedTest()->getBeginNode() == nullptr)
		return;

	TEST_MANAGER.getSelectedTest()->nodeArea->RunOnEachConnectedNode(TEST_MANAGER.getSelectedTest()->getBeginNode(),
		[](VisualNode* node) {
			size_t OutSocketCount = node->OutSocketCount();
			for (size_t i = 0; i < OutSocketCount; i++)
			{
				VisualNodeConnectionStyle TempStyle;
				node->GetSocketStyle(true, i, TempStyle);
				TempStyle.bMarchingAntsEffect = true;
				node->SetSocketStyle(true, i, TempStyle);
			}
		}
	);
}

void testEditorWinow::finishRecordingCallback(std::vector<FETPAction*>& recordedActions)
{
	if (recordedActions.size() != 0)
	{
		previewWindow::getInstance().show();
		ACTION_SYSTEM.placeStructuredNodes(recordedActions, previewWindow::getInstance().currentNodeArea);
		recordedActions.clear();
	}
}

void testEditorWinow::renderMainMenu()
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("New test"))
			{
				TEST_MANAGER.addTest();
				TEST_MANAGER.setSelelectedTestIndex(TEST_MANAGER.list.size() - 1);
			}

			if (TEST_MANAGER.getSelectedTest() == nullptr)
				ImGui::BeginDisabled();

			if (ImGui::MenuItem("Save test...") && TEST_MANAGER.getSelectedTest() != nullptr)
			{
				std::string path;
				FocalEngine::FILE_SYSTEM.showFileSaveDialog(path, saveFileFilter, 1);

				if (path != "")
				{
					if (path.find(".fetp") == std::string::npos)
						path += ".fetp";

					TEST_MANAGER.getSelectedTest()->save(path.c_str());
				}
			}

			if (TEST_MANAGER.getSelectedTest() == nullptr)
				ImGui::EndDisabled();

			if (ImGui::MenuItem("Load test..."))
			{
				std::string path;
				FocalEngine::FILE_SYSTEM.showFileOpenDialog(path, openFETPFileFilter, 2);

				if (path != "")
				{
					TEST_MANAGER.addTest(path);
					TEST_MANAGER.setSelelectedTestIndex(TEST_MANAGER.list.size() - 1);
				}
			}

			if (ImGui::MenuItem("Load nodes..."))
			{
				std::string path;
				FocalEngine::FILE_SYSTEM.showFileOpenDialog(path, NULL, 0);

				if (path != "")
				{
					previewWindow::getInstance().show();
					previewWindow::getInstance().currentNodeArea->LoadFromFile(path.c_str());
				}
			}

			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}

	ImGui::PopStyleVar();
}

void testEditorWinow::textInputCallback(std::string text)
{
	if (text != "")
	{
		combinedActionNode* newNode = new combinedActionNode(std::vector<FETPAction*>(), FETP_COMBINED_TEXT_INPUT_ACTION);
		newNode->ChangeText(text);

		newNode->SetPosition(mousePositionWhenContextMenuWasOpened);
		TEST_MANAGER.getSelectedTest()->nodeArea->AddNode(newNode);
	}
}

void testEditorWinow::textInputChangeNameCallback(std::string text)
{
	if (text != "")
	{
		if (TEST_MANAGER.getSelectedTest()->nodeArea->GetSelected().size() == 1 &&
			TEST_MANAGER.getSelectedTest()->nodeArea->GetSelected()[0]->GetType() == "regionNode")
		{
			TEST_MANAGER.getSelectedTest()->nodeArea->GetSelected()[0]->SetName(text);
		}
	}
}