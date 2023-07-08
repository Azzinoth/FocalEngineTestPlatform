#include "Windows/testsOverviewWindow.h"

void keyButtonCallback(int key, int scancode, int action, int mods)
{
	if (key == 84 && action == GLFW_RELEASE && TEST_MANAGER.getSelectedTest() != nullptr)
	{
		if (textInputPopup::getInstance().isOpened())
			return;

		ACTION_SYSTEM.run(TEST_MANAGER.getSelectedTest());

		while (TEST_MANAGER.getSelectedTest()->getLoopCount() > 1)
		{
			TEST_MANAGER.getSelectedTest()->setLoopCount(TEST_MANAGER.getSelectedTest()->getLoopCount() - 1);
			ACTION_SYSTEM.run(TEST_MANAGER.getSelectedTest());
		}
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

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	TEST_PLATFORM.createWindow();
	TEST_PLATFORM.setKeyboardCallback(keyButtonCallback);

	INPUT_SYSTEM.initialize();
	INPUT_SYSTEM.setGlobalKeyboardCallback(globalKeyButtonsCallback);
	INPUT_SYSTEM.setGlobalMouseCallback(globalMouseCallback);

	testsOverviewWindow::getInstance().show();
	testEditorWinow::getInstance().show();
	testPropertiesWindow::getInstance().show();

	//TestShaderLoading();

	//FETPImage* ScreenImage = new FETPImage("test.png");

	/*TIME.BeginTimeStamp("M");
	RunComputeShader(ScreenImage, SubImage);
	auto time = TIME.EndTimeStamp("M");*/
	//int FrameCount = 0;

	NODE_SYSTEM.AssociateSocketTypeToColor("FLOAT", ImColor(200, 0, 0));
	NODE_SYSTEM.AssociateSocketTypeToColor("BOOL", ImColor(25, 25, 255));

	while (TEST_PLATFORM.isWindowOpened())
	{
		TEST_PLATFORM.beginFrame();
		INPUT_SYSTEM.update();
		
		testsOverviewWindow::getInstance().render();
		testEditorWinow::getInstance().render();
		testPropertiesWindow::getInstance().render();
		testStartPreparationsWindow::getInstance().render();
		nodeRegionWindow::getInstance().render();

		previewWindow::getInstance().render();
		failedTestWindow::getInstance().render();
		screenshootEditor::getInstance().render();

		textInputPopup::getInstance().render();
		actionEditPopup::getInstance().render();
		ACTION_SYSTEM.update();



		/*TIME.BeginTimeStamp("M");
		FETPImage* TestScreenShoot = SCREEN_SYSTEM.GetScreenDataAsImage();
		delete TestScreenShoot;
		auto time = TIME.EndTimeStamp("M");
		TEST_PLATFORM.setWindowTitle(std::to_string(time));*/

		//RunComputeShader(TestScreenShoot, SubImage);


		/*auto Temp = TestScreenShoot->getRawData();
		lodepng::encode("test.png", Temp, TestScreenShoot->getWidth(), TestScreenShoot->getHeight());
		delete TestScreenShoot;
		delete[] Temp;*/







		/*TIME.BeginTimeStamp("M");
		unsigned char* TestScreenShoot = SCREEN_SYSTEM.getScreenData();
		auto time = TIME.EndTimeStamp("M");
		TEST_PLATFORM.setWindowTitle(std::to_string(time));*/



		/*FrameCount++;
		if (FrameCount % 2 == 0)
		{
			delete SubImage;
			SubImage = new FETPImage("testSub2.png");
		}
		else
		{
			delete SubImage;
			SubImage = new FETPImage("testSub1.png");
		}

		TIME.BeginTimeStamp("M");
		Sleep(20);
		RunComputeShader(ScreenImage, SubImage);
		auto time = TIME.EndTimeStamp("M");
		TEST_PLATFORM.setWindowTitle(std::to_string(time));*/





		
		

		/*std::vector<unsigned char> tempScreenshoot;
		tempScreenshoot.resize(512 * 512 * 4);
		Sleep(500);
		SCREEN_SYSTEM.getScreenRegion(tempScreenshoot.data(), 0, 0, 512, 512);

		int similarity = SCREEN_SYSTEM.compare(512, 512, tempScreenshoot.data(), image->getRawData(), tempDifferenceData.data());*/
		//SCREEN_SYSTEM.updateScreenData();

		//textLOG.clear();
		//for (size_t i = 0; i < recordedActions.size(); i++)
		//{
		//	if (recordedActions[i].getActionType() != 0)
		//		continue;

		//	if (recordedActions[i].keyboard.wParam == WM_KEYDOWN || recordedActions[i].keyboard.wParam == WM_SYSKEYDOWN)
		//	{
		//		textLOG += "key down:" + std::to_string(recordedActions[i].keyboard.additionalInfo.vkCode) + "\n";
		//	}
		//	else
		//	{
		//		textLOG += "key up:" + std::to_string(recordedActions[i].keyboard.additionalInfo.vkCode) + "\n";
		//	}
		//}

		//
		//strcpy_s(tempText, textLOG.size() + 1, textLOG.c_str());
		//ImGui::InputTextMultiline("LOG", tempText, strlen(tempText));

		//ImGui::ShowDemoWindow();

		//bool shiftPressed = HIWORD(GetKeyState(VK_SHIFT)) || HIWORD(GetKeyState(VK_LSHIFT)) || HIWORD(GetKeyState(VK_RSHIFT));
		//bool capsLockActivated = LOWORD(GetKeyState(VK_CAPITAL));

		//if (shiftPressed || capsLockActivated)
		//{
		//	int y = 0;
		//	y++;
		//}

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Exit"))
				{
					return 0;
				}

				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();
		}

		ImGui::PopStyleVar();

		//TEST_PLATFORM.setWindowTitle();
		TEST_PLATFORM.endFrame();
	}

	return 0;
}