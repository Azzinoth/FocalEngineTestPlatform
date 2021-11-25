#include "FETestPlatform.h"

FETestPlatform* FETestPlatform::_instance = nullptr;
std::function<void(int key, int scancode, int action, int mods)> FETestPlatform::clientKeyboardCallback = nullptr;

FETestPlatform::FETestPlatform() {};

void FETestPlatform::screenDataInitialization()
{
	HWND hwnd;
	RECT rect;

	hwnd = GetDesktopWindow();
	GetClientRect(hwnd, &rect);
	GetWindowRect(hwnd, &rect);

	int temp = GetSystemMetrics(SM_CYSCREEN);

	screenW = rect.right;
	screenH = rect.bottom;
}

void FETestPlatform::setImguiStyle()
{
	ImGuiStyle* style = &ImGui::GetStyle();
	ImVec4* colors = style->Colors;

	style->WindowRounding = 2.0f;
	style->ScrollbarRounding = 3.0f;
	style->GrabRounding = 2.0f;
	style->AntiAliasedLines = true;
	style->AntiAliasedFill = true;
	style->WindowRounding = 2;
	style->ChildRounding = 2;
	style->ScrollbarSize = 16;
	style->ScrollbarRounding = 3;
	style->GrabRounding = 2;
	style->ItemSpacing.x = 10;
	style->ItemSpacing.y = 4;
	style->IndentSpacing = 22;
	style->FramePadding.x = 6;
	style->FramePadding.y = 4;
	style->Alpha = 1.0f;
	style->FrameRounding = 3.0f;

	colors[ImGuiCol_Text] = ImVec4(255.0f / 255.0f, 243.0f / 255.0f, 255.0f / 255.0f, 1.00f);
	colors[ImGuiCol_TextDisabled] = ImVec4(158.0f / 255.0f, 158.0f / 255.0f, 158.0f / 255.0f, 1.00f);
	colors[ImGuiCol_WindowBg] = ImVec4(43.0f / 255.0f, 43.0f / 255.0f, 43.0f / 255.0f, 1.00f);
	colors[ImGuiCol_PopupBg] = ImVec4(60.0f / 255.0f, 60.0f / 255.0f, 60.0f / 255.0f, 0.98f);

	colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_Border] = ImVec4(0.71f, 0.71f, 0.71f, 0.08f);
	colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.04f);
	colors[ImGuiCol_FrameBg] = ImVec4(0.71f, 0.71f, 0.71f, 0.55f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.94f, 0.94f, 0.94f, 0.55f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.71f, 0.78f, 0.69f, 0.98f);
	colors[ImGuiCol_TitleBg] = ImVec4(0.41f, 0.68f, 0.89f, 1.00f);
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.41f, 0.68f, 0.89f, 1.00f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.0f, 0.47f, 0.83f, 1.00f);

	colors[ImGuiCol_MenuBarBg] = ImVec4(92.0f / 255.0f, 92.0f / 255.0f, 92.0f / 255.0f, 1.00f);
	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.20f, 0.25f, 0.30f, 0.61f);
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.90f, 0.90f, 0.90f, 0.30f);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.92f, 0.92f, 0.92f, 0.78f);
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	colors[ImGuiCol_CheckMark] = ImVec4(0.184f, 0.407f, 0.193f, 1.00f);
	colors[ImGuiCol_SliderGrab] = ImVec4(0.26f, 0.59f, 0.98f, 0.78f);
	colors[ImGuiCol_SliderGrabActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_Button] = ImVec4(0.71f, 0.78f, 0.69f, 0.40f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.725f, 0.805f, 0.702f, 1.00f);
	colors[ImGuiCol_ButtonActive] = ImVec4(0.793f, 0.900f, 0.836f, 1.00f);
	colors[ImGuiCol_Header] = ImVec4(0.71f, 0.78f, 0.69f, 0.31f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.71f, 0.78f, 0.69f, 0.80f);
	colors[ImGuiCol_HeaderActive] = ImVec4(0.71f, 0.78f, 0.69f, 1.00f);
	colors[ImGuiCol_Separator] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
	colors[ImGuiCol_SeparatorHovered] = ImVec4(0.14f, 0.44f, 0.80f, 0.78f);
	colors[ImGuiCol_SeparatorActive] = ImVec4(0.14f, 0.44f, 0.80f, 1.00f);
	colors[ImGuiCol_ResizeGrip] = ImVec4(1.00f, 1.00f, 1.00f, 0.00f);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.45f);
	colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.78f);
	colors[ImGuiCol_PlotLines] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
	colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
	colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
	colors[ImGuiCol_DragDropTarget] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
	colors[ImGuiCol_NavHighlight] = colors[ImGuiCol_HeaderHovered];
	colors[ImGuiCol_NavWindowingHighlight] = ImVec4(0.70f, 0.70f, 0.70f, 0.70f);
}

void FETestPlatform::createWindow()
{
	glfwInit();

	window = glfwCreateWindow(1500, 1000, "FETestPlatform", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return;
	}

	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, &FETestPlatform::keyboardCallback);
	glewInit();

	screenDataInitialization();

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();

	size_t pathLen = strlen("Resources//imgui.ini") + 1;
	char* imguiIniFile = new char[pathLen];
	strcpy_s(imguiIniFile, pathLen, "Resources//imgui.ini");
	io.IniFilename = imguiIniFile;
	io.Fonts->AddFontFromFileTTF("Resources//Cousine-Regular.ttf", 16);
	io.Fonts->AddFontFromFileTTF("Resources//Cousine-Regular.ttf", 32);
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	unsigned char* tex_pixels = NULL;
	int tex_w, tex_h;
	io.Fonts->GetTexDataAsRGBA32(&tex_pixels, &tex_w, &tex_h);

	io.DisplaySize = ImVec2(float(screenW), float(screenH));
	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 410");

	setImguiStyle();
}

bool FETestPlatform::isWindowOpened()
{
	return !glfwWindowShouldClose(window);
}

void FETestPlatform::beginFrame()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	ImGui::GetIO().DeltaTime = 1.0f / 60.0f;
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
}

void FETestPlatform::endFrame()
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	glfwSwapBuffers(window);
	glfwPollEvents();
}

size_t FETestPlatform::getScreenWidth()
{
	return screenW;
}

size_t FETestPlatform::getScreenHeight()
{
	return screenH;
}

void FETestPlatform::setWindowTitle(std::string newTitle)
{
	glfwSetWindowTitle(window, newTitle.c_str());
}

void FETestPlatform::minimizeWindow()
{
	glfwIconifyWindow(window);
}

void FETestPlatform::restoreWindow()
{
	glfwRestoreWindow(window);
}

void FETestPlatform::setKeyboardCallback(std::function<void(int key, int scancode, int action, int mods)> func)
{
	clientKeyboardCallback = func;
}

void FETestPlatform::keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (clientKeyboardCallback != nullptr)
		clientKeyboardCallback(key, scancode, action, mods);
}

size_t FETestPlatform::getWindowWidth()
{
	int width, height;
	glfwGetWindowSize(window, &width, &height);
	return width;
}

size_t FETestPlatform::getWindowHeight()
{
	int width, height;
	glfwGetWindowSize(window, &width, &height);
	return height;
}