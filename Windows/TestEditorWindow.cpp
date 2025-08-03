#include "TestEditorWindow.h"
#include "TextInputPopup.h"
#include "ActionEditPopup.h"
using namespace VisNodeSys;

ImVec2 TestEditorWindow::MousePositionWhenContextMenuWasOpened = ImVec2(0, 0);

TestEditorWindow::TestEditorWindow()
{
	std::string TemporaryCaption = "main area";
	strcpy_s(Caption, TemporaryCaption.size() + 1, TemporaryCaption.c_str());
	
	ACTION_SYSTEM.SetOnFinishRecordingCallback(OnFinishRecordingCallback);
}

TestEditorWindow::~TestEditorWindow()
{
}

void TestEditorWindow::Show()
{
}

void TestEditorWindow::Render()
{
	ImGui::Begin("Test Editor", nullptr, ImGuiWindowFlags_None | ImGuiWindowFlags_NoScrollbar);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

	RenderMainMenu();

	if (TEST_MANAGER.GetSelectedTest() != nullptr)
	{
		NodeArea* CurrentNodeArea = TEST_MANAGER.GetSelectedTest()->NodeArea;

		CurrentNodeArea->SetMainContextMenuFunction(RenderMainContextMenu);
		// That callback is painfuly slow, so need to find a better solution.
		//CurrentNodeArea->AddNodeEventCallback(NodeCallback);

		CurrentNodeArea->SetPosition(ImVec2(0.0f, 0.0f));
		CurrentNodeArea->SetSize(ImVec2(ImGui::GetWindowWidth(), ImGui::GetWindowHeight()));
		CurrentNodeArea->Update();
	}

	if (ImGui::GetIO().MouseReleased[1] && TEST_MANAGER.GetSelectedTest() != nullptr)
	{
		MousePositionWhenContextMenuWasOpened = ImVec2(ImGui::GetMousePos().x - ImGui::GetWindowPos().x, ImGui::GetMousePos().y - ImGui::GetWindowPos().y) - TEST_MANAGER.GetSelectedTest()->NodeArea->GetRenderOffset();
		MousePositionWhenContextMenuWasOpened /= TEST_MANAGER.GetSelectedTest()->NodeArea->GetZoomFactor();
	}

	ImGui::PopStyleVar();
	ImGui::End();
}

void TestEditorWindow::RenderMainContextMenu()
{
	if (TEST_MANAGER.GetSelectedTest() == nullptr || TEST_MANAGER.GetSelectedTest()->NodeArea == nullptr)
		return;

	if (TEST_MANAGER.GetSelectedTest()->NodeArea->GetHovered() == nullptr && TEST_MANAGER.GetSelectedTest()->NodeArea->GetSelected().size() == 0)
	{
		if (ImGui::BeginMenu("Add Node"))
		{
			VisNodeSys::Node* NewNode = nullptr;

			if (ImGui::MenuItem("Sleep"))
			{
				NewNode = new SleepNode();
			}

			if (ImGui::MenuItem("Timer"))
			{
				NewNode = new TimerNode();
			}

			if (ImGui::MenuItem("Image"))
			{
				NewNode = new ImageNode();
			}

			if (ImGui::MenuItem("Image Search"))
			{
				NewNode = new ImageSearchNode();
			}
			
			if (ImGui::MenuItem("Application Launch..."))
			{
				std::string Path;
				FocalEngine::FILE_SYSTEM.ShowFileOpenDialog(Path, ApplicationLoadFilter, 1);

				if (!Path.empty())
				{
					LunchApplicationNode* TemporaryNewNode = new LunchApplicationNode();
					TemporaryNewNode->SetPath(Path);
					NewNode = TemporaryNewNode;
				}
			}

			if (ImGui::BeginMenu("Input Simulation"))
			{
				if (ImGui::BeginMenu("Mouse"))
				{
					if (ImGui::MenuItem("Move"))
					{
						MouseMoveNode* NewNode = new MouseMoveNode();
						NewNode->SetPosition(MousePositionWhenContextMenuWasOpened);
						TEST_MANAGER.GetSelectedTest()->NodeArea->AddNode(NewNode);
					}

					if (ImGui::MenuItem("LeftButtonDown"))
					{
						MouseLeftButtonDownNode* NewNode = new MouseLeftButtonDownNode();
						NewNode->SetPosition(MousePositionWhenContextMenuWasOpened);
						TEST_MANAGER.GetSelectedTest()->NodeArea->AddNode(NewNode);
					}

					if (ImGui::MenuItem("LeftButtonUp"))
					{
						MouseLeftButtonUpNode* NewNode = new MouseLeftButtonUpNode();
						NewNode->SetPosition(MousePositionWhenContextMenuWasOpened);
						TEST_MANAGER.GetSelectedTest()->NodeArea->AddNode(NewNode);
					}

					if (ImGui::MenuItem("RightButtonDown"))
					{
						MouseRightButtonDownNode* NewNode = new MouseRightButtonDownNode();
						NewNode->SetPosition(MousePositionWhenContextMenuWasOpened);
						TEST_MANAGER.GetSelectedTest()->NodeArea->AddNode(NewNode);
					}

					if (ImGui::MenuItem("RightButtonUp"))
					{
						MouseRightButtonUpNode* NewNode = new MouseRightButtonUpNode();
						NewNode->SetPosition(MousePositionWhenContextMenuWasOpened);
						TEST_MANAGER.GetSelectedTest()->NodeArea->AddNode(NewNode);
					}

					if (ImGui::MenuItem("WheelRotation"))
					{

					}

					ImGui::EndMenu();
				}

				if (ImGui::BeginMenu("Keyboard"))
				{
					if (ImGui::MenuItem("KeyDown"))
					{
						KeyboardKeyDownNode* NewNode = new KeyboardKeyDownNode();
						NewNode->SetPosition(MousePositionWhenContextMenuWasOpened);
						TEST_MANAGER.GetSelectedTest()->NodeArea->AddNode(NewNode);
					}

					if (ImGui::MenuItem("KeyUp"))
					{

						KeyboardKeyUpNode* NewNode = new KeyboardKeyUpNode();
						NewNode->SetPosition(MousePositionWhenContextMenuWasOpened);
						TEST_MANAGER.GetSelectedTest()->NodeArea->AddNode(NewNode);
					}

					if (ImGui::MenuItem("KeyPress"))
					{
					}

					if (ImGui::MenuItem("Text Input"))
					{
						TextInputNode* NewNode = new TextInputNode();
						NewNode->SetPosition(MousePositionWhenContextMenuWasOpened);
						NewNode->SetText("Text Input");
						TEST_MANAGER.GetSelectedTest()->NodeArea->AddNode(NewNode);
					}

					ImGui::EndMenu();
				}

				ImGui::EndMenu();
			}

			if (NewNode != nullptr)
			{
				NewNode->SetPosition(MousePositionWhenContextMenuWasOpened);
				TEST_MANAGER.GetSelectedTest()->NodeArea->AddNode(NewNode);
			}

			ImGui::EndMenu();
		}
	}
	//else if (TEST_MANAGER.GetSelectedTest()->NodeArea->GetSelected().size() == 1 &&
	//		 TEST_MANAGER.GetSelectedTest()->NodeArea->GetSelected()[0]->GetType() == "RegionNode")
	//{
	//	if (ImGui::MenuItem("Open region"))
	//	{
	//		NodeRegionWindow::GetInstance().Show(/*reinterpret_cast<RegionNode*>(TEST_MANAGER.GetSelectedTest()->NodeArea->GetSelected()[0])*/);
	//	}

	//	if (ImGui::MenuItem("Rename"))
	//	{
	//		TextInputPopup::GetInstance().Show(TextInputChangeNameCallback, TEST_MANAGER.GetSelectedTest()->NodeArea->GetSelected()[0]->GetName());
	//	}

	//	if (ImGui::MenuItem("Remove"))
	//	{
	//		TEST_MANAGER.GetSelectedTest()->NodeArea->DeleteNode(TEST_MANAGER.GetSelectedTest()->NodeArea->GetSelected()[0]);
	//	}
	//}
	else if (TEST_MANAGER.GetSelectedTest()->NodeArea->GetHovered() != nullptr)
	{
		if (ImGui::MenuItem("Copy NodeID to clipboard"))
		{
			ImGui::SetClipboardText(TEST_MANAGER.GetSelectedTest()->NodeArea->GetHovered()->GetID().c_str());
		}
	}
	else if (TEST_MANAGER.GetSelectedTest()->NodeArea->GetSelected().size() > 1)
	{
		auto SelectedList = TEST_MANAGER.GetSelectedTest()->NodeArea->GetSelected();
		if (ImGui::MenuItem("Delete Selected Nodes"))
		{
			for (size_t i = 0; i < SelectedList.size(); i++)
			{
				TEST_MANAGER.GetSelectedTest()->NodeArea->DeleteNode(SelectedList[i]);
			}

			TEST_MANAGER.GetSelectedTest()->NodeArea->UnSelectAll();
		}

		if (ImGui::MenuItem("Save Selected Nodes..."))
		{
			std::string Path;
			FocalEngine::FILE_SYSTEM.ShowFileSaveDialog(Path, NULL, 0);

			if (Path != "")
			{
				TEST_MANAGER.GetSelectedTest()->NodeArea->SaveNodesToFile(Path.c_str(), SelectedList);
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
				int XPosition = selectedList[0]->getPosition().x;
				XPosition += (i % nodesPerW * int(selectedList[i]->getSize().x + disBetweenOnW));
				XPosition %= TEST_PLATFORM.getWindowWidth();

				int YPosition = i / nodesPerH;
				YPosition *= disBetweenOnH;

				selectedList[i]->SetPosition(ImVec2(float(XPosition), float(YPosition)));
			}
		}*/
	}
}

void TestEditorWindow::NodeCallback(VisNodeSys::Node* Node, NODE_EVENT CurrentNodeEvent)
{
	if (Node == nullptr)
		return;

	if (CurrentNodeEvent == BEFORE_CONNECTED || CurrentNodeEvent == BEFORE_DISCONNECTED)
		return;

	// Change style of all connections to default.
	TEST_MANAGER.GetSelectedTest()->NodeArea->RunOnEachNode([](VisNodeSys::Node* Node) {
		size_t OutSocketCount = Node->GetOutputSocketCount();
		for (size_t i = 0; i < OutSocketCount; i++)
		{
			ConnectionStyle TempStyle;
			Node->GetParentArea()->GetConnectionStyle(Node, true, i, TempStyle);
			TempStyle.bMarchingAntsEffect = false;
			Node->GetParentArea()->SetConnectionStyle(Node, true, i, TempStyle);
		}
	});

	if (TEST_MANAGER.GetSelectedTest()->NodeArea->GetExecutionEntryNode() == nullptr)
		return;
}

void TestEditorWindow::OnFinishRecordingCallback(std::vector<FETPAction*>& RecordedActions)
{
	if (RecordedActions.size() != 0)
	{
		PreviewWindow::GetInstance().Show();
		//ACTION_SYSTEM.PlaceStructuredNodes(RecordedActions, PreviewWindow::GetInstance().CurrentNodeArea);
		RecordedActions.clear();
	}
}

void TestEditorWindow::RenderMainMenu()
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("New test"))
			{
				TEST_MANAGER.AddTest();
				TEST_MANAGER.SetSelectedTestIndex(TEST_MANAGER.Tests.size() - 1);
			}

			if (TEST_MANAGER.GetSelectedTest() == nullptr)
				ImGui::BeginDisabled();

			if (ImGui::MenuItem("Save test...") && TEST_MANAGER.GetSelectedTest() != nullptr)
			{
				std::string Path;
				FocalEngine::FILE_SYSTEM.ShowFileSaveDialog(Path, SaveFileFilter, 1);

				if (Path != "")
				{
					if (Path.find(".fetp") == std::string::npos)
						Path += ".fetp";

					TEST_MANAGER.GetSelectedTest()->Save(Path.c_str());
				}
			}

			if (TEST_MANAGER.GetSelectedTest() == nullptr)
				ImGui::EndDisabled();

			if (ImGui::MenuItem("Load test..."))
			{
				std::string Path;
				FocalEngine::FILE_SYSTEM.ShowFileOpenDialog(Path, OpenFETPFileFilter, 1);

				if (Path != "")
				{
					TEST_MANAGER.AddTest(Path);
					TEST_MANAGER.SetSelectedTestIndex(TEST_MANAGER.Tests.size() - 1);
				}
			}

			if (ImGui::MenuItem("Load nodes..."))
			{
				std::string Path;
				FocalEngine::FILE_SYSTEM.ShowFileOpenDialog(Path, NULL, 0);

				if (Path != "")
				{
					PreviewWindow::GetInstance().Show();
					PreviewWindow::GetInstance().CurrentNodeArea->LoadFromFile(Path.c_str());
				}
			}

			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}

	ImGui::PopStyleVar();
}