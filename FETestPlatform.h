#pragma once

#include "SubSystems/FEFileSystem.h"

class FETPInput;

class FETestPlatform
{
	friend FETPInput;
public:
	SINGLETON_PUBLIC_PART(FETestPlatform)

	void createWindow();
	bool isWindowOpened();

	void beginFrame();
	void endFrame();

	size_t getScreenWidth();
	size_t getScreenHeight();

	size_t getWindowWidth();
	size_t getWindowHeight();

	void setWindowTitle(std::string newTitle);
	void minimizeWindow();
	void restoreWindow();

	void setKeyboardCallback(std::function<void(int key, int scancode, int action, int mods)> func);
private:
	SINGLETON_PRIVATE_PART(FETestPlatform)

	GLFWwindow* window;
	size_t screenW = 0;
	size_t screenH = 0;
	void setImguiStyle();
	void screenDataInitialization();

	static void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static std::function<void(int key, int scancode, int action, int mods)> clientKeyboardCallback;
};

#define TEST_PLATFORM FETestPlatform::getInstance()