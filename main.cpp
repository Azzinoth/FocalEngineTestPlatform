#include "Windows/TestsOverviewWindow.h"
#include "Windows/TextInputPopup.h"
#include "Windows/ActionEditPopup.h"
#include "Windows/LinkAreaSelectionPopup.h"
using namespace FocalEngine;

#include "tesseract/baseapi.h"
#include "leptonica/allheaders.h"

void OnFinishRecordingCallback(std::vector<FETPAction*>& RecordedActions)
{
	if (RecordedActions.size() != 0)
	{
		PreviewWindow::GetInstance().Show();
		RecordedActions.clear();
	}
}

// That callback is painfuly slow, so need to find a better solution.
//CurrentNodeArea->AddNodeEventCallback(NodeCallback);
void NodeCallback(VisNodeSys::Node* Node, VisNodeSys::NODE_EVENT CurrentNodeEvent)
{
	if (Node == nullptr || Node->GetParentArea() == nullptr)
		return;

	if (CurrentNodeEvent == VisNodeSys::BEFORE_CONNECTED || CurrentNodeEvent == VisNodeSys::BEFORE_DISCONNECTED)
		return;

	// Change style of all connections to default.
	Node->GetParentArea()->RunOnEachNode([](VisNodeSys::Node* Node) {
		size_t OutSocketCount = Node->GetOutputSocketCount();
		for (size_t i = 0; i < OutSocketCount; i++)
		{
			VisNodeSys::ConnectionStyle TemporaryStyle;
			Node->GetParentArea()->GetConnectionStyle(Node, true, i, TemporaryStyle);
			TemporaryStyle.bMarchingAntsEffect = false;
			Node->GetParentArea()->SetConnectionStyle(Node, true, i, TemporaryStyle);
		}
	});

	if (Node->GetParentArea()->GetExecutionEntryNode() == nullptr)
		return;
}

bool bShouldOpenAboutWindow = false;
void ShowAboutDialog()
{
	bShouldOpenAboutWindow = true;
}

void RenderAboutWindow()
{
	if (bShouldOpenAboutWindow)
	{
		ImGui::OpenPopup("About");
		bShouldOpenAboutWindow = false;
	}

	ImGui::SetNextWindowSizeConstraints(ImVec2(400.0f, 0.0f), ImVec2(FLT_MAX, FLT_MAX));
	if (ImGui::BeginPopupModal("About", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove))
	{
		int WindowW = 0;
		int WindowH = 0;
		APPLICATION.GetMainWindow()->GetSize(&WindowW, &WindowH);

		ImGui::SetWindowPos(ImVec2(WindowW / 2.0f - ImGui::GetWindowWidth() / 2.0f, WindowH / 2.0f - ImGui::GetWindowHeight() / 2.0f));

		float ContentW = ImGui::GetWindowContentRegionMax().x - ImGui::GetWindowContentRegionMin().x;
		auto CenteredText = [ContentW](const std::string& Text) {
			ImVec2 TextSize = ImGui::CalcTextSize(Text.c_str());
			ImGui::SetCursorPosX((ContentW - TextSize.x) / 2.0f + ImGui::GetWindowContentRegionMin().x);
			ImGui::Text("%s", Text.c_str());
		};
		CenteredText(TEST_PLATFORM.GetFullVersion());

		ImGui::Separator();
		ImGui::Text("Modules:");

		CenteredText(SceneGraphUI::TreeView::GetFullVersion());
		CenteredText(NODE_SYSTEM.GetFullVersion());
		CenteredText(APPLICATION.GetFullVersion());

		ImGui::Separator();

		CenteredText("To submit a bug report or provide feedback,");
		CenteredText("please email me at ");

		ImGui::Separator();

		float ButtonW = 210.0f;
		ImGui::SetCursorPosX((ContentW - ButtonW) / 2.0f + ImGui::GetWindowContentRegionMin().x);
		if (ImGui::Button("Close", ImVec2(ButtonW, 25.0f)))
			ImGui::CloseCurrentPopup();

		ImGui::EndPopup();
	}
}

void RenderMainMenu()
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
				NODE_AREA_WINDOW_MANAGER.CreateNodeAreaWindow(TEST_MANAGER.Tests.back()->EntryPointNodeArea);
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
					//NODE_SYSTEM.SaveToFile(Path);
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

					//NODE_SYSTEM.LoadFromFile(Path);

					if (TEST_MANAGER.Tests.back()->EntryPointNodeArea != nullptr)
						NODE_AREA_WINDOW_MANAGER.CreateNodeAreaWindow(TEST_MANAGER.Tests.back()->EntryPointNodeArea);
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

			if (ImGui::MenuItem("Close All"))
			{
				TEST_MANAGER.Clear();
			}

			if (ImGui::MenuItem("Exit"))
			{
				APPLICATION.Close();
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Debug"))
		{
			if (ImGui::MenuItem("Select Nodes executed during last run"))
			{
				NodeAreaWindow* FocusedNodeAreaWindow = NODE_AREA_WINDOW_MANAGER.GetInFocusNodeAreaWindow();
				if (TEST_MANAGER.GetSelectedTest() != nullptr && FocusedNodeAreaWindow != nullptr && FocusedNodeAreaWindow->GetNodeArea() != nullptr)
				{
					std::vector<VisNodeSys::Node*> SelectedNodes = FocusedNodeAreaWindow->GetNodeArea()->GetLastExecutedNodes();
					FocusedNodeAreaWindow->GetNodeArea()->UnSelectAll();
					for (size_t i = 0; i < SelectedNodes.size(); i++)
					{
						FocusedNodeAreaWindow->GetNodeArea()->AddSelected(SelectedNodes[i]);
					}

					//TEST_MANAGER.GetSelectedTest()->NodeArea->Select(TEST_MANAGER.GetSelectedTest()->GetLastExecutedNodes());
					//TEST_MANAGER.GetSelectedTest()->NodeArea->SetRenderOffset(ImVec2(0, 0));
				}
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Info"))
		{
			if (ImGui::MenuItem("About..."))
				ShowAboutDialog();

			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}

	ImGui::PopStyleVar();
}

GLFWcursor* MouseCursor = nullptr;
void MainWindowRender()
 {
	TEST_PLATFORM.SetUpDocking();

	INPUT_SYSTEM.Update();

	TestsOverviewWindow::GetInstance().Render();
	TestPropertiesWindow::GetInstance().Render();
	TestStartPreparationsWindow::GetInstance().Render();
	
	PreviewWindow::GetInstance().Render();
	FailedTestWindow::GetInstance().Render();
	
	TextInputPopup::GetInstance().Render();
	ActionEditPopup::GetInstance().Render();
	LinkAreaSelectionPopup::GetInstance().Render();

	NODE_AREA_WINDOW_MANAGER.Render();

	ACTION_SYSTEM.Update();

	/*if (ImGui::Button("Load Old File"))
	{
		VisNodeSys::NodeArea* NewNodeArea = NODE_SYSTEM.CreateNodeArea();
		NewNodeArea->LoadFromFile("C:\\Users\\kberegovyi\\Downloads\\InstanceEntites\\InstanceEntites\\Add_instances\\Add_instances.fetp");
	}*/

	TEST_PLATFORM.Update();

	RenderMainMenu();
	RenderAboutWindow();
}

bool bNeedToCreateWindow = false;
void KeyButtonCallback(int Key, int ScanCode, int Action, int Mods)
{
	if (Key == GLFW_KEY_T && Action == GLFW_RELEASE && TEST_MANAGER.GetSelectedTest() != nullptr)
	{
		if (TextInputPopup::GetInstance().IsOpened())
			return;

		if (ImGui::GetIO().WantTextInput || ImGui::GetIO().WantCaptureKeyboard)
			return;

		ACTION_SYSTEM.Run(TEST_MANAGER.GetSelectedTest());

		while (TEST_MANAGER.GetSelectedTest()->GetLoopCount() > 1)
		{
			TEST_MANAGER.GetSelectedTest()->SetLoopCount(TEST_MANAGER.GetSelectedTest()->GetLoopCount() - 1);
			ACTION_SYSTEM.Run(TEST_MANAGER.GetSelectedTest());
		}
	}

	if (Key == GLFW_KEY_ESCAPE && Action == GLFW_PRESS)
	{
		bNeedToCreateWindow = true;
	}
}

void OnGlobalKeyboardAction(KeyboardAction KeyAction)
{
	ACTION_SYSTEM.NewKeyboardAction(KeyAction);
}

void OnGlobalMouseAction(MouseAction MouseAction)
{
	ACTION_SYSTEM.NewMouseAction(MouseAction);
}

double MouseX, MouseY;
double MouseDownX, MouseDownY;
std::string FileToBase64(const std::string& FilePath, size_t ChunkSize = 8000)
{
	std::ifstream File(FilePath, std::ios::binary);
	if (!File.is_open())
		return "";

	std::vector<unsigned char> Bytes(
		(std::istreambuf_iterator<char>(File)),
		std::istreambuf_iterator<char>());
	File.close();

	std::string Base64String = NODE_CORE.Base64Encode(Bytes.data(), static_cast<unsigned int>(Bytes.size()));

	std::string Result = "std::string Data = \n";
	for (size_t i = 0; i < Base64String.size(); i += ChunkSize)
	{
		Result += "    \"" + Base64String.substr(i, ChunkSize) + "\"\n";
	}
	Result += "    ;";

	return Result;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	TEST_PLATFORM.Initialize();
	APPLICATION.GetMainWindow()->AddOnKeyCallback(KeyButtonCallback);
	APPLICATION.GetMainWindow()->EnableDefaultDockspace();

	APPLICATION.GetMainWindow()->SetRenderFunction(MainWindowRender);

	INPUT_SYSTEM.Initialize();
	INPUT_SYSTEM.SetGlobalKeyboardCallback(OnGlobalKeyboardAction);
	INPUT_SYSTEM.SetGlobalMouseCallback(OnGlobalMouseAction);

	TestsOverviewWindow::GetInstance().Show();
	TestPropertiesWindow::GetInstance().Show();

	//std::string test = FileToBase64("C:/Users/Kindr/Downloads/structure_W_.png");

	NODE_CORE.SetTextureLoader([](const std::string& Path) -> unsigned int {
		std::vector<unsigned char> RawData;
		unsigned ImageWidth, ImageHeight;
		int Error = lodepng::decode(RawData, ImageWidth, ImageHeight, Path.c_str());
		if (Error != 0)
			return 0;

		unsigned char* TemporaryData = new unsigned char[ImageWidth * ImageHeight * 4];
		memcpy_s(TemporaryData, ImageWidth * ImageHeight * 4, RawData.data(), ImageWidth * ImageHeight * 4);
		FETPImage* ImageHoldingTexture = new FETPImage(TemporaryData, ImageWidth, ImageHeight);
		delete[] TemporaryData;

		return ImageHoldingTexture->GetTextureID();
	});
	NODE_SYSTEM.Initialize();
	NODE_SYSTEM.AssociateSocketTypeToColor("STRING", ImColor(241, 0, 165));
	NODE_SYSTEM.AssociateSocketTypeToColor("IMAGE", ImColor(0, 174, 239));

	while (APPLICATION.IsNotTerminated())
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		APPLICATION.BeginFrame();

		APPLICATION.RenderWindows();

		APPLICATION.EndFrame();
	}

	return 0;
}