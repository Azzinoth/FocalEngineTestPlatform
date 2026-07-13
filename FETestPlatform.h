#pragma once

#include "SubSystems/FEFileSystem.h"

class FETPInput;
class FETPImage;

class FETestPlatform
{
	friend FETPInput;
public:
	SINGLETON_PUBLIC_PART(FETestPlatform)

	void Initialize();
	void CreateMainWindow();
	void SetUpDocking();

	size_t GetScreenWidth();
	size_t GetScreenHeight();

	void SetWindowTitle(std::string NewTitle);

	std::string GetFullVersion();

	void Update();

	FETPImage* GetInfoIconWhite() const { return InfoIconWhite; }
	FETPImage* GetInfoIconBlue() const { return InfoIconBlue; }
	FETPImage* GetInfoIconGreen() const { return InfoIconGreen; }
	FETPImage* GetInfoIconYellow() const { return InfoIconYellow; }
	FETPImage* GetInfoIconRed() const { return InfoIconRed; }

private:
	SINGLETON_PRIVATE_PART(FETestPlatform)

	size_t ScreenWidth = 0;
	size_t ScreenHeight = 0;

	bool bHadImGuiIniFileAtStartup = false;

	FETPImage* InfoIconWhite = nullptr;
	FETPImage* InfoIconBlue = nullptr;
	FETPImage* InfoIconGreen = nullptr;
	FETPImage* InfoIconYellow = nullptr;
	FETPImage* InfoIconRed = nullptr;

	void SetImguiStyle();
	void ScreenDataInitialization();

	std::string GetVersion();
	int GetBuildNumber();
	std::string GetBuildTimestamp();
	std::string GetBuildInfo();

	void KeyboardInputUpdate();
};

#define TEST_PLATFORM FETestPlatform::GetInstance()