#include "Windows/TestsOverviewWindow.h"
#include "Windows/TextInputPopup.h"
#include "Windows/ActionEditPopup.h"
using namespace FocalEngine;

GLFWcursor* MouseCursor = nullptr;
void MainWindowRender()
 {
	ImGui::DockSpaceOverViewport(0U, ImGui::GetMainViewport());
	
	INPUT_SYSTEM.Update();
	
	TestsOverviewWindow::GetInstance().Render();
	TestEditorWindow::GetInstance().Render();
	TestPropertiesWindow::GetInstance().Render();
	TestStartPreparationsWindow::GetInstance().Render();
	NodeRegionWindow::GetInstance().Render();
	
	PreviewWindow::GetInstance().Render();
	if (TEST_MANAGER.GetSelectedTest() != nullptr && TEST_MANAGER.GetSelectedTest()->DebugNodeAreaWindow != nullptr)
		TEST_MANAGER.GetSelectedTest()->DebugNodeAreaWindow->Render();
	FailedTestWindow::GetInstance().Render();
	
	TextInputPopup::GetInstance().Render();
	ActionEditPopup::GetInstance().Render();
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
				TEST_MANAGER.GetSelectedTest()->NodeArea->AddNode(NewNode);
			}

			FILE_SYSTEM.DeleteFile("Temporary.png");
		}
	}
	
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Exit"))
			{
				//return 0;
			}
			
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Debug"))
		{
			if (ImGui::MenuItem("Select Nodes executed during last run"))
			{
				if (TEST_MANAGER.GetSelectedTest() != nullptr)
				{
					std::vector<VisNodeSys::Node*> SelectedNodes = TEST_MANAGER.GetSelectedTest()->NodeArea->GetLastExecutedNodes();
					TEST_MANAGER.GetSelectedTest()->NodeArea->UnSelectAll();
					for (size_t i = 0; i < SelectedNodes.size(); i++)
					{
						TEST_MANAGER.GetSelectedTest()->NodeArea->AddSelected(SelectedNodes[i]);
					}

					//TEST_MANAGER.GetSelectedTest()->NodeArea->Select(TEST_MANAGER.GetSelectedTest()->GetLastExecutedNodes());
					//TEST_MANAGER.GetSelectedTest()->NodeArea->SetRenderOffset(ImVec2(0, 0));
				}
			}

			ImGui::EndMenu();
		}
		
		ImGui::EndMainMenuBar();
	}

	ImGui::PopStyleVar();
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

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	TEST_PLATFORM.CreateMainWindow();
	APPLICATION.GetMainWindow()->AddOnKeyCallback(KeyButtonCallback);

	APPLICATION.GetMainWindow()->SetRenderFunction(MainWindowRender);

	INPUT_SYSTEM.Initialize();
	INPUT_SYSTEM.SetGlobalKeyboardCallback(OnGlobalKeyboardAction);
	INPUT_SYSTEM.SetGlobalMouseCallback(OnGlobalMouseAction);

	TestsOverviewWindow::GetInstance().Show();
	TestEditorWindow::GetInstance().Show();
	TestPropertiesWindow::GetInstance().Show();

	NODE_SYSTEM.Initialize();

	auto SecondWindow = APPLICATION.AddWindow(800, 600, "Test Window");
	SecondWindow->AddOnKeyCallback(SpecialWindowKeyCallback);

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