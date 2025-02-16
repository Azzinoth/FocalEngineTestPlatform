#include "Windows/testsOverviewWindow.h"
using namespace FocalEngine;

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

	APPLICATION.GetMainWindow()->SetRenderFunction(MainWindowRender);

	INPUT_SYSTEM.initialize();
	INPUT_SYSTEM.setGlobalKeyboardCallback(globalKeyButtonsCallback);
	INPUT_SYSTEM.setGlobalMouseCallback(globalMouseCallback);

	testsOverviewWindow::GetInstance().show();
	testEditorWinow::GetInstance().show();
	testPropertiesWindow::GetInstance().show();

	NODE_SYSTEM.Initialize();
	NODE_SYSTEM.AssociateSocketTypeToColor("BOOL", ImColor(25, 25, 255));

	while (TEST_PLATFORM.IsNotTerminated())
	{
		TEST_PLATFORM.beginFrame();

		APPLICATION.RenderWindows();

		TEST_PLATFORM.endFrame();
	}

	return 0;
}