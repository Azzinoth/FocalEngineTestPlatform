#include "FETestPlatform.h"
using namespace FocalEngine;

#include "VersionInfo/FETestPlatform_Version.h"
#include "VersionInfo/FEVersionInfo.h"
#include "Windows/NodeArea/NodeAreaWindowManager.h"
#include "SubSystems/TestManager.h"
#include "SubSystems/NodeSystem/CustomNodes/Images/ImageLiteralNode.h"

FE_DEFINE_VERSION_INFO(FETestPlatform_)

FETestPlatform::FETestPlatform() {}

std::string FETestPlatform::GetVersion()
{
	return GetFETestPlatform_VersionInfo().GetVersion();
}

int FETestPlatform::GetBuildNumber()
{
	return GetFETestPlatform_VersionInfo().BuildNumber;
}

std::string FETestPlatform::GetBuildTimestamp()
{
	return GetFETestPlatform_VersionInfo().BuildTimestamp;
}

std::string FETestPlatform::GetBuildInfo()
{
	return GetFETestPlatform_VersionInfo().GetBuildInfo();
}

std::string FETestPlatform::GetFullVersion()
{
	return "Test Platform " + GetFETestPlatform_VersionInfo().GetFullVersionString();
}

void FETestPlatform::ScreenDataInitialization()
{
	HWND HWND;
	RECT Rect;

	HWND = GetDesktopWindow();
	GetClientRect(HWND, &Rect);
	GetWindowRect(HWND, &Rect);

	ScreenWidth = Rect.right;
	ScreenHeight = Rect.bottom;
}

void FETestPlatform::SetImguiStyle()
{
	ImGuiStyle* Style = &ImGui::GetStyle();
	ImVec4* Colors = Style->Colors;

	Style->WindowRounding = 2.0f;
	Style->ScrollbarRounding = 3.0f;
	Style->GrabRounding = 2.0f;
	Style->AntiAliasedLines = true;
	Style->AntiAliasedFill = true;
	Style->WindowRounding = 2;
	Style->ChildRounding = 2;
	Style->ScrollbarSize = 16;
	Style->ScrollbarRounding = 3;
	Style->GrabRounding = 2;
	Style->ItemSpacing.x = 10;
	Style->ItemSpacing.y = 4;
	Style->IndentSpacing = 22;
	Style->FramePadding.x = 6;
	Style->FramePadding.y = 4;
	Style->Alpha = 1.0f;
	Style->FrameRounding = 3.0f;

	Colors[ImGuiCol_Text] = ImVec4(255.0f / 255.0f, 243.0f / 255.0f, 255.0f / 255.0f, 1.00f);
	Colors[ImGuiCol_TextDisabled] = ImVec4(158.0f / 255.0f, 158.0f / 255.0f, 158.0f / 255.0f, 1.00f);
	Colors[ImGuiCol_WindowBg] = ImVec4(43.0f / 255.0f, 43.0f / 255.0f, 43.0f / 255.0f, 1.00f);
	Colors[ImGuiCol_PopupBg] = ImVec4(60.0f / 255.0f, 60.0f / 255.0f, 60.0f / 255.0f, 0.98f);

	Colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	Colors[ImGuiCol_Border] = ImVec4(0.71f, 0.71f, 0.71f, 0.08f);
	Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.04f);
	Colors[ImGuiCol_FrameBg] = ImVec4(0.71f, 0.71f, 0.71f, 0.55f);
	Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.94f, 0.94f, 0.94f, 0.55f);
	Colors[ImGuiCol_FrameBgActive] = ImVec4(0.71f, 0.78f, 0.69f, 0.98f);
	Colors[ImGuiCol_TitleBg] = ImVec4(0.41f, 0.68f, 0.89f, 1.00f);
	Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.41f, 0.68f, 0.89f, 1.00f);
	Colors[ImGuiCol_TitleBgActive] = ImVec4(0.0f, 0.47f, 0.83f, 1.00f);

	Colors[ImGuiCol_MenuBarBg] = ImVec4(92.0f / 255.0f, 92.0f / 255.0f, 92.0f / 255.0f, 1.00f);
	Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.20f, 0.25f, 0.30f, 0.61f);
	Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.90f, 0.90f, 0.90f, 0.30f);
	Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.92f, 0.92f, 0.92f, 0.78f);
	Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	Colors[ImGuiCol_CheckMark] = ImVec4(0.184f, 0.407f, 0.193f, 1.00f);
	Colors[ImGuiCol_SliderGrab] = ImVec4(0.26f, 0.59f, 0.98f, 0.78f);
	Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	Colors[ImGuiCol_Button] = ImVec4(0.71f, 0.78f, 0.69f, 0.40f);
	Colors[ImGuiCol_ButtonHovered] = ImVec4(0.725f, 0.805f, 0.702f, 1.00f);
	Colors[ImGuiCol_ButtonActive] = ImVec4(0.793f, 0.900f, 0.836f, 1.00f);
	Colors[ImGuiCol_Header] = ImVec4(0.71f, 0.78f, 0.69f, 0.31f);
	Colors[ImGuiCol_HeaderHovered] = ImVec4(0.71f, 0.78f, 0.69f, 0.80f);
	Colors[ImGuiCol_HeaderActive] = ImVec4(0.71f, 0.78f, 0.69f, 1.00f);
	Colors[ImGuiCol_Separator] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
	Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.14f, 0.44f, 0.80f, 0.78f);
	Colors[ImGuiCol_SeparatorActive] = ImVec4(0.14f, 0.44f, 0.80f, 1.00f);
	Colors[ImGuiCol_ResizeGrip] = ImVec4(1.00f, 1.00f, 1.00f, 0.00f);
	Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.45f);
	Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.78f);
	Colors[ImGuiCol_PlotLines] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
	Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
	Colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
	Colors[ImGuiCol_DragDropTarget] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
	Colors[ImGuiCol_NavHighlight] = Colors[ImGuiCol_HeaderHovered];
	Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(0.70f, 0.70f, 0.70f, 0.70f);
}

void FETestPlatform::Initialize()
{
	CreateMainWindow();

	InfoIconWhite = new FETPImage("Resources//information-button_W.png");
	InfoIconBlue = new FETPImage("Resources//information-button_B.png");
	InfoIconGreen = new FETPImage("Resources//information-button_G.png");
	InfoIconYellow = new FETPImage("Resources//information-button_Y.png");
	InfoIconRed = new FETPImage("Resources//information-button_R.png");

	NODE_AREAS_GRAPH_WINDOW.InitializeResources();
}

void FETestPlatform::CreateMainWindow()
{
	APPLICATION.AddWindow(1500, 1000, "FETestPlatform");
	
	ScreenDataInitialization();

	ImGuiIO& IO = ImGui::GetIO();

	size_t PathLength = strlen("Resources//imgui.ini") + 1;
	char* ImguiIniFile = new char[PathLength];
	strcpy_s(ImguiIniFile, PathLength, "Resources//imgui.ini");
	IO.IniFilename = ImguiIniFile;
	bHadImGuiIniFileAtStartup = FILE_SYSTEM.DoesFileExist(ImguiIniFile);
	IO.Fonts->AddFontFromFileTTF("Resources//Cousine-Regular.ttf", 16);
	IO.Fonts->AddFontFromFileTTF("Resources//Cousine-Regular.ttf", 32);
	IO.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	IO.DisplaySize = ImVec2(float(ScreenWidth), float(ScreenHeight));
	ImGui::StyleColorsDark();

	SetImguiStyle();
}

void FETestPlatform::SetUpDocking()
{
	ImGuiID DockspaceID = APPLICATION.GetMainWindow()->GetDefaultDockspaceID();
	if (!bHadImGuiIniFileAtStartup && DockspaceID != 0)
	{
		bHadImGuiIniFileAtStartup = true;
		ImGui::DockBuilderRemoveNode(DockspaceID);
		ImGui::DockBuilderAddNode(DockspaceID, ImGuiDockNodeFlags_DockSpace);
		ImGui::DockBuilderSetNodeSize(DockspaceID, ImGui::GetMainViewport()->Size);

		ImGuiID BottomID;
		ImGuiID CenterID;
		ImGuiID LeftID;
		ImGuiID RightID;

		ImGui::DockBuilderSplitNode(DockspaceID, ImGuiDir_Down, 0.25f, &BottomID, &CenterID);
		ImGui::DockBuilderSplitNode(CenterID, ImGuiDir_Left, 0.20f, &LeftID, &CenterID);
		// 0.25f of the post-left-split center (0.80f of full) = 0.20f of full, matching LeftID's width.
		ImGui::DockBuilderSplitNode(CenterID, ImGuiDir_Right, 0.25f, &RightID, &CenterID);
		// Split the right column horizontally so "Tests overview" sits on top of "Test Properties".
		ImGuiID RightTopID;
		ImGuiID RightBottomID;
		ImGui::DockBuilderSplitNode(RightID, ImGuiDir_Down, 0.5f, &RightBottomID, &RightTopID);

		ImGui::DockBuilderDockWindow("Scene Graph", LeftID);
		ImGui::DockBuilderDockWindow("Tests overview", RightTopID);
		ImGui::DockBuilderDockWindow("Test Properties", RightBottomID);

		ImGui::DockBuilderFinish(DockspaceID);
	}
}

size_t FETestPlatform::GetScreenWidth()
{
	return ScreenWidth;
}

size_t FETestPlatform::GetScreenHeight()
{
	return ScreenHeight;
}

void FETestPlatform::SetWindowTitle(std::string NewTitle)
{
	APPLICATION.GetMainWindow()->SetTitle(NewTitle);
}

void FETestPlatform::Update()
{
	KeyboardInputUpdate();
}

void FETestPlatform::KeyboardInputUpdate()
{
	bool CtrlDown = ImGui::IsKeyDown(ImGuiKey_LeftCtrl) || ImGui::IsKeyDown(ImGuiKey_RightCtrl);
	if (!CtrlDown)
		return;

	if (!ImGui::IsKeyPressed(ImGuiKey_V, false))
		return;

	NodeAreaWindow* FocusedWindow = NODE_AREA_WINDOW_MANAGER.GetInFocusNodeAreaWindow();
	if (FocusedWindow == nullptr || FocusedWindow->GetNodeArea() == nullptr)
		return;

	FETPImage* Image = FETPImage::FromClipboard();
	if (Image == nullptr)
		return;

	VisNodeSys::NodeArea* TargetArea = FocusedWindow->GetNodeArea();
	ImVec2 MouseLocalPosition = TargetArea->GetRenderedViewCenter();

	NodeAreaWindow* AreaWindow = NODE_AREA_WINDOW_MANAGER.GetNodeAreaWindow(TargetArea);
	if (AreaWindow != nullptr && AreaWindow->IsVisible() && AreaWindow->GetWindow() != nullptr)
	{
		ImGuiWindow* WindowPointer = AreaWindow->GetWindow();
		ImVec2 WindowPosition = WindowPointer->Pos;
		ImVec2 WindowSize = WindowPointer->Size;
		ImVec2 MousePosition = ImGui::GetMousePos();

		bool bMouseInsideWindow = MousePosition.x >= WindowPosition.x && MousePosition.x <= WindowPosition.x + WindowSize.x &&
								 MousePosition.y >= WindowPosition.y && MousePosition.y <= WindowPosition.y + WindowSize.y;

		if (bMouseInsideWindow)
		{
			float Zoom = TargetArea->GetZoomFactor();
			if (Zoom <= 0.0f)
				Zoom = 1.0f;

			ImVec2 Local = (MousePosition - WindowPosition) - TargetArea->GetRenderOffset();
			Local.x /= Zoom;
			Local.y /= Zoom;
			MouseLocalPosition = Local;
		}
	}

	ImageLiteralNode* NewNode = new ImageLiteralNode();
	NewNode->SetImage(Image);
	MouseLocalPosition -= NewNode->GetSize() / 2.0f;
	NewNode->SetPosition(MouseLocalPosition);
	TargetArea->AddNode(NewNode);
}