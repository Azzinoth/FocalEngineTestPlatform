#include "Windows/TestsOverviewWindow.h"
#include "Windows/TextInputPopup.h"
#include "Windows/ActionEditPopup.h"
using namespace FocalEngine;

#include "tesseract/baseapi.h"
#include "leptonica/allheaders.h"

void OnFinishRecordingCallback(std::vector<FETPAction*>& RecordedActions)
{
	if (RecordedActions.size() != 0)
	{
		PreviewWindow::GetInstance().Show();
		//ACTION_SYSTEM.PlaceStructuredNodes(RecordedActions, PreviewWindow::GetInstance().CurrentNodeArea);
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
			VisNodeSys::ConnectionStyle TempStyle;
			Node->GetParentArea()->GetConnectionStyle(Node, true, i, TempStyle);
			TempStyle.bMarchingAntsEffect = false;
			Node->GetParentArea()->SetConnectionStyle(Node, true, i, TempStyle);
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

					//TEST_MANAGER.GetSelectedTest()->Save(Path.c_str());
					NODE_SYSTEM.SaveToFile(Path);
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
					//TEST_MANAGER.AddTest(Path);
					//TEST_MANAGER.SetSelectedTestIndex(TEST_MANAGER.Tests.size() - 1);

					NODE_SYSTEM.LoadFromFile(Path);

					std::vector<std::string> NodeAreas = NODE_SYSTEM.GetNodeAreaIDList();
					for (size_t i = 0; i < NodeAreas.size(); i++)
					{
						VisNodeSys::NodeArea* CurrentNodeArea = NODE_SYSTEM.GetNodeAreaByID(NodeAreas[i]);
						if (CurrentNodeArea != nullptr)
							NODE_AREA_WINDOW_MANAGER.CreateNodeAreaWindow(CurrentNodeArea);
					}
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
	ImGui::DockSpaceOverViewport(0U, ImGui::GetMainViewport());
	
	INPUT_SYSTEM.Update();

	TestsOverviewWindow::GetInstance().Render();
	TestPropertiesWindow::GetInstance().Render();
	TestStartPreparationsWindow::GetInstance().Render();
	
	PreviewWindow::GetInstance().Render();
	FailedTestWindow::GetInstance().Render();
	
	TextInputPopup::GetInstance().Render();
	ActionEditPopup::GetInstance().Render();

	NODE_AREA_WINDOW_MANAGER.Render();

	ACTION_SYSTEM.Update();

	if (TEST_MANAGER.GetSelectedTest() != nullptr)
	{
		if (FILE_SYSTEM.DoesFileExist("Temporary.png"))
		{
			std::vector<unsigned char> RawData;
			unsigned ImageWidth, ImageHeight;
			int Error = lodepng::decode(RawData, ImageWidth, ImageHeight, "Temporary.png");

			ImageNode* NewNode = nullptr;
			if (Error == 0)
			{
				NewNode = new ImageNode();
				unsigned char* tempData = new unsigned char[ImageWidth * ImageHeight * 4];
				memcpy_s(tempData, ImageWidth * ImageHeight * 4, RawData.data(), ImageWidth * ImageHeight * 4);
				NewNode->SetImage(new FETPImage(tempData, ImageWidth, ImageHeight));
				delete[] tempData;

				NewNode->SetPosition(ImVec2(200, 200));
				NodeAreaWindow* FocusedNodeAreaWindow = NODE_AREA_WINDOW_MANAGER.GetInFocusNodeAreaWindow();
				if (FocusedNodeAreaWindow != nullptr && FocusedNodeAreaWindow->GetNodeArea() != nullptr)
					FocusedNodeAreaWindow->GetNodeArea()->AddNode(NewNode);
			}

			FILE_SYSTEM.DeleteFile("Temporary.png");
		}
	}
	
	RenderMainMenu();
	RenderAboutWindow();
}

bool bNeedToCreateWindow = false;
void KeyButtonCallback(int Key, int ScanCode, int Action, int Mods)
{
	if (Key == 84 && Action == GLFW_RELEASE && TEST_MANAGER.GetSelectedTest() != nullptr)
	{
		if (TextInputPopup::GetInstance().IsOpened())
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
FEWindow* FirstFullScreenWindow = nullptr;
size_t MonitorIndex = 0;
FETPImage* CapturedScreenshot = nullptr;
FETPImage* DarkenedCapturedScreenshot = nullptr;
void FirstMonitorScreenshotWindowRender()
{
	if (FirstFullScreenWindow == nullptr)
		return;

	if (CapturedScreenshot == nullptr)
	{
		CapturedScreenshot = FETPScreenCapture::GetInstance().GetScreenImage(unsigned int(MonitorIndex));
		DarkenedCapturedScreenshot = new FETPImage(*CapturedScreenshot);

		DarkenedCapturedScreenshot->ModifyPixels([](unsigned char& R, unsigned char& G, unsigned char& B) {
			R /= 2;
			G /= 2;
			B /= 2;
		});
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (MouseCursor == nullptr)
		MouseCursor = glfwCreateStandardCursor(GLFW_CROSSHAIR_CURSOR);
	glfwSetCursor(FirstFullScreenWindow->GetGlfwWindow(), MouseCursor);

	int Width, Height;
	FirstFullScreenWindow->GetSize(&Width, &Height);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(ImVec2(float(Width), float(Height)));
	ImGui::Begin("FullScreenWindow", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar /*| ImGuiWindowFlags_NoInputs*/ | ImGuiWindowFlags_NoDecoration);

	ImVec2 UV0 = ImVec2(0, 0);
	ImVec2 UV1 = ImVec2(1, 1);
	ImVec2 Size = ImVec2(float(Width), float(Height));
	if (DarkenedCapturedScreenshot != nullptr)
		ImGui::Image(DarkenedCapturedScreenshot->GetTextureID(), Size, UV0, UV1);

	/*if (ImGui::Button("Cancel"))
	{
		APPLICATION.CloseWindow(FirstFullScreenWindow);
		FirstFullScreenWindow = nullptr;
		delete BackGround;
		BackGround = nullptr;
	}*/

	ImGui::End();

	ImGui::PopStyleVar();
	ImGui::PopStyleVar();

	//ImGui::ShowDemoWindow();
}

void FirstMonitorScreenshotWindowKeyCallback(int Key, int Scancode, int Action, int Mods)
{
	if (Key == GLFW_KEY_ESCAPE && Action == GLFW_PRESS)
	{
		APPLICATION.CloseWindow(FirstFullScreenWindow);
		FirstFullScreenWindow = nullptr;
		delete CapturedScreenshot;
		CapturedScreenshot = nullptr;
	}
}

void SpecialWindowKeyCallback(int Key, int Scancode, int Action, int Mods)
{
	if (Key == GLFW_KEY_ESCAPE && Action == GLFW_PRESS)
	{
		bNeedToCreateWindow = true;
	}
}

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
	TEST_PLATFORM.CreateMainWindow();
	APPLICATION.GetMainWindow()->AddOnKeyCallback(KeyButtonCallback);

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

	auto SecondWindow = APPLICATION.AddWindow(800, 600, "Test Window");
	SecondWindow->AddOnKeyCallback(SpecialWindowKeyCallback);

	// FE_FIX_ME: Temporary check of tesseract OCR functionality. Remove it later.
	//tesseract::TessBaseAPI ocr;
	//int res = ocr.Init("ThirdParty/tesseract/traineddata", "eng"); // path to tessdata folder
	////ocr.SetPageSegMode(tesseract::PSM_SINGLE_LINE);
	//ocr.SetPageSegMode(tesseract::PSM_AUTO_OSD);

	//// From screenshot/image file
	//Pix* image = pixRead("Untitled2.png");
	//ocr.SetImage(image);
	//std::string text = ocr.GetUTF8Text();
	//pixDestroy(&image);

	while (APPLICATION.IsNotTerminated())
	{
		if (bNeedToCreateWindow)
		{
			bNeedToCreateWindow = false;

			auto Monitor = APPLICATION.GetWindow(0)->DetermineCurrentMonitor();
			MonitorIndex = APPLICATION.MonitorInfoToMonitorIndex(&Monitor);

			FirstFullScreenWindow = APPLICATION.AddFullScreenWindow(&Monitor);
			FirstFullScreenWindow->SetRenderFunction(FirstMonitorScreenshotWindowRender);
			FirstFullScreenWindow->AddOnKeyCallback(FirstMonitorScreenshotWindowKeyCallback);

			FirstFullScreenWindow->AddOnMouseMoveCallback([&](double X, double Y) {
				MouseX = X;
				MouseY = Y;
				//MessageBoxA(NULL, ("X: " + std::to_string(X) + " Y: " + std::to_string(Y)).c_str(), "Mouse Position", MB_OK);
			});

			FirstFullScreenWindow->AddOnMouseButtonCallback([&](int Button, int Action, int Mods) {
				if (Button == GLFW_MOUSE_BUTTON_LEFT && Action == GLFW_PRESS)
				{
					MouseDownX = MouseX;
					MouseDownY = MouseY;
				}
				else if (Button == GLFW_MOUSE_BUTTON_LEFT && Action == GLFW_RELEASE)
				{
					if (TEST_MANAGER.GetSelectedTest() != nullptr)
					{
						double MouseUpX = MouseX;
						double MouseUpY = MouseY;

						FETPImage* Image = CapturedScreenshot->GetRegion(int(MouseDownX), int(MouseDownY), int(abs(MouseDownX - MouseUpX)), int(abs(MouseDownY - MouseUpY)));
						if (Image != nullptr)
							lodepng::encode("Temporary.png", Image->GetRawData(), Image->GetWidth(), Image->GetHeight());
					}

					APPLICATION.CloseWindow(FirstFullScreenWindow);
					FirstFullScreenWindow = nullptr;
					delete CapturedScreenshot;
					CapturedScreenshot = nullptr;
				}
			});

			FirstFullScreenWindow->AddOnMouseButtonCallback([&](int Button, int Action, int Mods) {
				if (Button == GLFW_MOUSE_BUTTON_LEFT && Action == GLFW_PRESS)
				{
					//MessageBoxA(NULL, ("X: " + std::to_string(MouseX) + " Y: " + std::to_string(MouseY)).c_str(), "Mouse Position", MB_OK);
					//NeedToCreateWindow = true;
				}
			});
		}

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		APPLICATION.BeginFrame();

		APPLICATION.RenderWindows();

		APPLICATION.EndFrame();
	}

	return 0;
}