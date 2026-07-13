#pragma once

#include "FETPAction.h"
#include "FETPComputeShaderCompare.h"

class FETPScreen
{
public:
	SINGLETON_PUBLIC_PART(FETPScreen)

	unsigned char* GetScreenData();
	FETPImage* GetScreenDataAsImage(unsigned int MonitorIndex);
	void GetScreenRegion(unsigned char* Data, int Left, int Top, int Width, int Height, bool bUpdateScreenDataFlag = true);

	size_t GetScreenWidth();
	size_t GetScreenHeight();

	int Compare(size_t Width, size_t Height, unsigned char* FirstData, unsigned char* SecondData, unsigned char* DifferenceData, int MaxColorShift);
	bool SearchOnScreen(size_t Width, size_t Height, unsigned char* Data, size_t& X, size_t& Y, float CorrectnessThreshold, int MaxColorShift, int* MaxSimilarity = nullptr);

	glm::vec2 ImageSizeInRegion(size_t ImageWidth, size_t ImageHeight, size_t RegionWidth, size_t RegionHeight);

	void UpdateScreenData();
private:
	SINGLETON_PRIVATE_PART(FETPScreen)

	unsigned char* ScreenData = nullptr;
	int SimpleCompare(size_t Width, size_t Height, unsigned char* FirstData, unsigned char* SecondData, int MaxColorShift);
};

#define SCREEN_SYSTEM FETPScreen::GetInstance()