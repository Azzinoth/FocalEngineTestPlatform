#include "Windows/testsOverviewWindow.h"
using namespace FocalEngine;

GLFWcursor* cursor = nullptr;
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

	if (cursor == nullptr)
		cursor = glfwCreateStandardCursor(GLFW_CROSSHAIR_CURSOR);

	//glfwSetCursor(APPLICATION.GetMainWindow()->GetGlfwWindow(), cursor);


	if (TEST_MANAGER.getSelectedTest() != nullptr)
	{
		if (FILE_SYSTEM.checkFile("Temporary.png"))
		{
			std::vector<unsigned char> rawData;
			unsigned uWidth, uHeight;
			int error = lodepng::decode(rawData, uWidth, uHeight, "Temporary.png");

			imageNode* newNode = nullptr;
			if (error == 0)
			{
				newNode = new imageNode();
				unsigned char* tempData = new unsigned char[uWidth * uHeight * 4];
				memcpy_s(tempData, uWidth * uHeight * 4, rawData.data(), uWidth * uHeight * 4);
				newNode->SetImage(new FETPImage(tempData, uWidth, uHeight));
				delete[] tempData;

				newNode->SetPosition(ImVec2(200, 200));
				TEST_MANAGER.getSelectedTest()->nodeArea->AddNode(newNode);
			}

			FILE_SYSTEM.deleteFile("Temporary.png");
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

bool NeedToCreateWindow = false;
void keyButtonCallback(int key, int scancode, int action, int mods)
{
	if (key == 84 && action == GLFW_RELEASE && TEST_MANAGER.getSelectedTest() != nullptr)
	{
		if (textInputPopup::GetInstance().isOpened())
			return;

		ACTION_SYSTEM.run(TEST_MANAGER.getSelectedTest());

		while (TEST_MANAGER.getSelectedTest()->getLoopCount() > 1)
		{
			TEST_MANAGER.getSelectedTest()->setLoopCount(TEST_MANAGER.getSelectedTest()->getLoopCount() - 1);
			ACTION_SYSTEM.run(TEST_MANAGER.getSelectedTest());
		}
	}

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		NeedToCreateWindow = true;
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
//double MouseUpX, MouseDownY;
FEWindow* FirstFullScreenWindow = nullptr;
int MonitorIndex = 0;
FETPImage* BackGround = nullptr;
void FirstMonitorScreenShotWindowRender()
{
	if (FirstFullScreenWindow == nullptr)
		return;

	if (BackGround == nullptr)
	{
		BackGround = FETPScreenCapture::GetInstance().GetScreenImage(MonitorIndex);

		BackGround->ModifyPixels([](unsigned char& R, unsigned char& G, unsigned char& B) {
			R /= 2.5;
			G /= 2.5;
			B /= 2.5;
		});
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glfwSetCursor(FirstFullScreenWindow->GetGlfwWindow(), cursor);

	int Width, Height;
	FirstFullScreenWindow->GetSize(&Width, &Height);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(ImVec2(Width, Height));
	ImGui::Begin("FullScreenWindow", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar /*| ImGuiWindowFlags_NoInputs*/ | ImGuiWindowFlags_NoDecoration);

	//ImGui::Text("Mouse Position: X: %f Y: %f", MouseX, MouseY);
	//ImGui::Text("Mouse Down: X: %f Y: %f", MouseDownX, MouseDownY);

	ImVec2 uv0 = ImVec2(0, 0);
	ImVec2 uv1 = ImVec2(1, 1);
	ImVec2 size = ImVec2(Width, Height);
	if (BackGround != nullptr)
		ImGui::Image((void*)(intptr_t)BackGround->GetTextureID(), size, uv0, uv1);


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
		/*TIME.BeginTimeStamp("ScreenCapture");
		BackGround = FETPScreenCapture::getInstance().GetScreenImage(0);

		BackGround->ModifyPixels([](unsigned char& R, unsigned char& G, unsigned char& B) {
			R /= 2.5;
			G /= 2.5;
			B /= 2.5;
			});


		auto TimePassed = TIME.EndTimeStamp("ScreenCapture");
		MessageBoxA(NULL, ("Screen Capture Time: " + std::to_string(TimePassed) + "ms").c_str(), "Screen Capture Time", MB_OK);*/

		//glfwSetWindowShouldClose(fullscreenWindow, GLFW_TRUE);
		//return 0;

		APPLICATION.CloseWindow(FirstFullScreenWindow);
		FirstFullScreenWindow = nullptr;
		delete BackGround;
		BackGround = nullptr;
	}
}


void SpecialWindowKeyCallback(int Key, int Scancode, int Action, int Mods)
{
	if (Key == GLFW_KEY_ESCAPE && Action == GLFW_PRESS)
	{
		NeedToCreateWindow = true;
	}
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	TEST_PLATFORM.createWindow();
	TEST_PLATFORM.setKeyboardCallback(keyButtonCallback);

	APPLICATION.GetMainWindow()->SetRenderFunction(MainWindowRender);

	INPUT_SYSTEM.initialize();
	INPUT_SYSTEM.setGlobalKeyboardCallback(globalKeyButtonsCallback);
	INPUT_SYSTEM.setGlobalMouseCallback(globalMouseCallback);

	testsOverviewWindow::GetInstance().show();
	testEditorWinow::GetInstance().show();
	testPropertiesWindow::GetInstance().show();

	NODE_SYSTEM.Initialize();
	NODE_SYSTEM.AssociateSocketTypeToColor("BOOL", ImColor(25, 25, 255));

	auto SecondW = APPLICATION.AddWindow(800, 600, "Test Window");
	SecondW->AddOnKeyCallback(SpecialWindowKeyCallback);

	while (TEST_PLATFORM.IsNotTerminated())
	{
		if (NeedToCreateWindow)
		{
			NeedToCreateWindow = false;

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
					double MouseUpX = MouseX;
					double MouseUpY = MouseY;

					FETPImage* Image = BackGround->GetRegion(MouseDownX, MouseDownY, abs(MouseDownX - MouseUpX)/*100*//*MouseUpX*/, abs(MouseDownY - MouseUpY)/*100*//*MouseUpY*/);
					lodepng::encode("Temporary.png", Image->GetRawData(), Image->GetWidth(), Image->GetHeight());

					APPLICATION.CloseWindow(FirstFullScreenWindow);
					FirstFullScreenWindow = nullptr;
					delete BackGround;
					BackGround = nullptr;
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

		TEST_PLATFORM.beginFrame();

		APPLICATION.RenderWindows();

		TEST_PLATFORM.endFrame();
	}

	return 0;
}