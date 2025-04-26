#include "Windows/testsOverviewWindow.h"
using namespace FocalEngine;

GLFWcursor* MouseCursor = nullptr;
void MainWindowRender()
 {
	ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
	
	INPUT_SYSTEM.update();
	
	testsOverviewWindow::GetInstance().render();
	testEditorWinow::GetInstance().render();
	testPropertiesWindow::GetInstance().render();
	testStartPreparationsWindow::GetInstance().render();
	nodeRegionWindow::GetInstance().render();
	
	previewWindow::GetInstance().render();
	failedTestWindow::GetInstance().render();
	screenshootEditor::GetInstance().render();
	
	textInputPopup::GetInstance().render();
	actionEditPopup::GetInstance().render();
	ACTION_SYSTEM.update();

	if (TEST_MANAGER.getSelectedTest() != nullptr)
	{
		if (FILE_SYSTEM.DoesFileExist("Temporary.png"))
		{
			std::vector<unsigned char> RawData;
			unsigned ImageWidth, ImageHeight;
			int Error = lodepng::decode(RawData, ImageWidth, ImageHeight, "Temporary.png");

			imageNode* NewNode = nullptr;
			if (Error == 0)
			{
				NewNode = new imageNode();
				unsigned char* tempData = new unsigned char[ImageWidth * ImageHeight * 4];
				memcpy_s(tempData, ImageWidth * ImageHeight * 4, RawData.data(), ImageWidth * ImageHeight * 4);
				NewNode->SetImage(new FETPImage(tempData, ImageWidth, ImageHeight));
				delete[] tempData;

				NewNode->SetPosition(ImVec2(200, 200));
				TEST_MANAGER.getSelectedTest()->NodeArea->AddNode(NewNode);
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
		
		ImGui::EndMainMenuBar();
	}
	
	ImGui::PopStyleVar();
}

bool bNeedToCreateWindow = false;
void KeyButtonCallback(int key, int scancode, int action, int mods)
{
	if (key == 84 && action == GLFW_RELEASE && TEST_MANAGER.getSelectedTest() != nullptr)
	{
		if (textInputPopup::GetInstance().isOpened())
			return;

		ACTION_SYSTEM.run(TEST_MANAGER.getSelectedTest());

		while (TEST_MANAGER.getSelectedTest()->GetLoopCount() > 1)
		{
			TEST_MANAGER.getSelectedTest()->SetLoopCount(TEST_MANAGER.getSelectedTest()->GetLoopCount() - 1);
			ACTION_SYSTEM.run(TEST_MANAGER.getSelectedTest());
		}
	}

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		bNeedToCreateWindow = true;
	}
}

void globalKeyButtonsCallback(KeyboardAction keyAction)
{
	ACTION_SYSTEM.newKeyboardAction(keyAction);
}

void globalMouseCallback(MouseAction mouseAction)
{
	ACTION_SYSTEM.newMouseAction(mouseAction);
}

double MouseX, MouseY;
double MouseDownX, MouseDownY;
FEWindow* FirstFullScreenWindow = nullptr;
int MonitorIndex = 0;
FETPImage* CapturedScreenshot = nullptr;
FETPImage* DarkenedCapturedScreenshot = nullptr;
void FirstMonitorScreenShotWindowRender()
{
	if (FirstFullScreenWindow == nullptr)
		return;

	if (CapturedScreenshot == nullptr)
	{
		CapturedScreenshot = FETPScreenCapture::GetInstance().GetScreenImage(MonitorIndex);
		DarkenedCapturedScreenshot = new FETPImage(*CapturedScreenshot);

		DarkenedCapturedScreenshot->ModifyPixels([](unsigned char& R, unsigned char& G, unsigned char& B) {
			R /= 2.5;
			G /= 2.5;
			B /= 2.5;
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
	ImGui::SetNextWindowSize(ImVec2(Width, Height));
	ImGui::Begin("FullScreenWindow", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar /*| ImGuiWindowFlags_NoInputs*/ | ImGuiWindowFlags_NoDecoration);

	ImVec2 uv0 = ImVec2(0, 0);
	ImVec2 uv1 = ImVec2(1, 1);
	ImVec2 size = ImVec2(Width, Height);
	if (DarkenedCapturedScreenshot != nullptr)
		ImGui::Image((void*)(intptr_t)DarkenedCapturedScreenshot->GetTextureID(), size, uv0, uv1);


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

void FirstMonitorScreenShotWindowKeyCallback(int Key, int Scancode, int Action, int Mods)
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

	INPUT_SYSTEM.initialize();
	INPUT_SYSTEM.setGlobalKeyboardCallback(globalKeyButtonsCallback);
	INPUT_SYSTEM.setGlobalMouseCallback(globalMouseCallback);

	testsOverviewWindow::GetInstance().show();
	testEditorWinow::GetInstance().show();
	testPropertiesWindow::GetInstance().show();

	NODE_SYSTEM.Initialize();
	NODE_SYSTEM.AssociateSocketTypeToColor("BOOL", ImColor(25, 25, 255));

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
			FirstFullScreenWindow->SetRenderFunction(FirstMonitorScreenShotWindowRender);
			FirstFullScreenWindow->AddOnKeyCallback(FirstMonitorScreenShotWindowKeyCallback);

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
					if (TEST_MANAGER.getSelectedTest() != nullptr)
					{
						double MouseUpX = MouseX;
						double MouseUpY = MouseY;

						FETPImage* Image = CapturedScreenshot->GetRegion(MouseDownX, MouseDownY, abs(MouseDownX - MouseUpX), abs(MouseDownY - MouseUpY));
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