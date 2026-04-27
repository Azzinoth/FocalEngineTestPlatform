#pragma once

#include "SubSystems/FEFileSystem.h"

class FETPInput;

class FETestPlatform
{
	friend FETPInput;
public:
	SINGLETON_PUBLIC_PART(FETestPlatform)

	void CreateMainWindow();

	size_t GetScreenWidth();
	size_t GetScreenHeight();

	void SetWindowTitle(std::string NewTitle);

	std::string GetFullVersion();
private:
	SINGLETON_PRIVATE_PART(FETestPlatform)

	size_t ScreenWidth = 0;
	size_t ScreenHeight = 0;
	void SetImguiStyle();
	void ScreenDataInitialization();

	std::string GetVersion();
	int GetBuildNumber();
	std::string GetBuildTimestamp();
	std::string GetBuildInfo();
};

#define TEST_PLATFORM FETestPlatform::GetInstance()