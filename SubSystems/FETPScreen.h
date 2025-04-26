#pragma once

#include "FETPAction.h"
#include "FETPComputeShaderCompare.h"

class FETPScreen
{
public:
	SINGLETON_PUBLIC_PART(FETPScreen)

	unsigned char* GetScreenData();
	FETPImage* GetScreenDataAsImage(unsigned int MonitorIndex);
	void GetScreenRegion(unsigned char* Data, int Left, int Top, int Width, int Height, bool UpdateScreenDataFlag = true);

	size_t GetScreenWidth();
	size_t GetScreenHeight();

	int Compare(size_t Width, size_t Height, unsigned char* FirstData, unsigned char* SecondData, unsigned char* DifferenceData, int MaxColorShift);
	bool bSearchOnScreen(size_t Width, size_t Height, unsigned char* Data, size_t& X, size_t& Y, float CorrectnessThreshold, int MaxColorShift, int* MaxSimilarity = nullptr);

	glm::vec2 ImageSizeInRegion(size_t ImageWidth, size_t ImageHeight, size_t RegionWidth, size_t RegionHeight);

	void UpdateScreenData();
private:
	SINGLETON_PRIVATE_PART(FETPScreen)

	unsigned char* ScreenData = nullptr;
	int SimpleCompare(size_t Width, size_t Height, unsigned char* FirstData, unsigned char* SecondData, int MaxColorShift);
};

#define SCREEN_SYSTEM FETPScreen::GetInstance()
 
// FIX ME! Do I need it?
struct ScreenSearchInfo
{
private:
	ImVec2 ScreenRegionMin = ImVec2(0, 0);
	ImVec2 ScreenRegionMax = ImVec2(0, 0);
	bool bSearchOnScreen = false;

	// FIX ME! I think I do not need it anymore
	int XShiftFromFound = 50;
	int YShiftFromFound = 50;
public:
	ScreenSearchInfo()
	{
	}

	ScreenSearchInfo(ScreenSearchInfo& Other)
	{
		ScreenRegionMin = Other.ScreenRegionMin;
		ScreenRegionMax = Other.ScreenRegionMax;
		bSearchOnScreen = Other.bSearchOnScreen;

		XShiftFromFound = Other.XShiftFromFound;
		YShiftFromFound = Other.YShiftFromFound;
	}

	bool GetSearchOnScreenMode();
	void SetSearchOnScreenMode(bool NewValue);

	ImVec2 GetScreenMinRegion();
	ImVec2 GetScreenMaxRegion();

	void SetScreenRegion(ImVec2 ScreenRegionMin, ImVec2 ScreenRegionMax);

	int GetXShiftFromFound();
	void SetXShiftFromFound(int NewValue);

	int GetYShiftFromFound();
	void SetYShiftFromFound(int NewValue);
};

// FIX ME! Do I need it?
struct CompareImageInfo
{
public:
	FETPImage* Image = nullptr;
	bool lastRunResult = false;
	int correctnessThreshold = 95;
	int maxColorShift = 4;

	FETPImage* partialImage = nullptr;
	int partialImageLeft = 0;
	int partialImageTop = 0;
	
	bool severalAttempts = false;
	int severalAttemptsTimeout = 10000;

	// Search for image on screen
	ScreenSearchInfo* screenSearch = nullptr;

	CompareImageInfo()
	{
		Image = nullptr;
		partialImage = nullptr;

		partialImageLeft = 0;
		partialImageTop = 0;
	}

	CompareImageInfo(CompareImageInfo& src)
	{
		Image = new FETPImage(*src.Image);
		if (src.partialImage != nullptr)
			partialImage = new FETPImage(*src.partialImage);

		if (src.screenSearch != nullptr)
			screenSearch = new ScreenSearchInfo(*src.screenSearch);

		lastRunResult = src.lastRunResult;
		correctnessThreshold = src.correctnessThreshold;
		maxColorShift = src.maxColorShift;

		partialImageLeft = src.partialImageLeft;
		partialImageTop = src.partialImageTop;

		severalAttempts = src.severalAttempts;
		severalAttemptsTimeout = src.severalAttemptsTimeout;
	}

	CompareImageInfo(FETPImage* Image)
	{
		this->Image = Image;

		partialImageLeft = 0;
		partialImageTop = 0;
	}

	~CompareImageInfo()
	{
		delete Image;
		delete partialImage;
		delete screenSearch;
	}
};

struct ScreenshootCompareAction : public FETPAction
{
private:

	void loadImageInfo(Json::Value json, std::string directoryPath, size_t index)
	{
		std::string fileName = directoryPath + json["compareImageInfos"][std::to_string(index)]["screenshot_fileName"].asCString();
		if (json["compareImageInfos"][std::to_string(index)].isMember("screenshot_fullPath"))
			fileName = json["compareImageInfos"][std::to_string(index)]["screenshot_fullPath"].asCString();

		imagesInfo.push_back(new CompareImageInfo(new FETPImage(fileName)));

		imagesInfo.back()->correctnessThreshold = json["compareImageInfos"][std::to_string(index)]["correctnessThreshold"].asInt();
		if (json["compareImageInfos"][std::to_string(index)].isMember("maxColorShift"))
			imagesInfo.back()->maxColorShift = json["compareImageInfos"][std::to_string(index)]["maxColorShift"].asInt();

		imagesInfo.back()->partialImageLeft = json["compareImageInfos"][std::to_string(index)]["partialImageLeft"].asInt();
		imagesInfo.back()->partialImageTop = json["compareImageInfos"][std::to_string(index)]["partialImageTop"].asInt();

		imagesInfo.back()->severalAttempts = json["compareImageInfos"][std::to_string(index)]["severalAttempts"].asBool();
		imagesInfo.back()->severalAttemptsTimeout = json["compareImageInfos"][std::to_string(index)]["severalAttemptsTimeout"].asInt();

		if (json["compareImageInfos"][std::to_string(index)].isMember("partial_fileName"))
		{
			std::string fileName = directoryPath + json["compareImageInfos"][std::to_string(index)]["partial_fileName"].asCString();
			if (json["compareImageInfos"][std::to_string(index)].isMember("partial_fullPath"))
				fileName = json["compareImageInfos"][std::to_string(index)]["partial_fullPath"].asCString();

			imagesInfo.back()->partialImage = new FETPImage(fileName);

			if (json["compareImageInfos"][std::to_string(index)].isMember("screenSearch"))
			{
				if (json["compareImageInfos"][std::to_string(index)]["screenSearch"]["isActive"].asBool())
					imagesInfo.back()->screenSearch = new ScreenSearchInfo();

				if (json["compareImageInfos"][std::to_string(index)]["screenSearch"].isMember("xShiftFromFound"))
					imagesInfo.back()->screenSearch->SetXShiftFromFound(json["compareImageInfos"][std::to_string(index)]["screenSearch"]["xShiftFromFound"].asInt());

				if (json["compareImageInfos"][std::to_string(index)]["screenSearch"].isMember("yShiftFromFound"))
					imagesInfo.back()->screenSearch->SetYShiftFromFound(json["compareImageInfos"][std::to_string(index)]["screenSearch"]["yShiftFromFound"].asInt());
			}
		}
	}

public:
	std::vector<CompareImageInfo*> imagesInfo;
	static bool bUseGPU;

	ScreenshootCompareAction() : FETPAction(FETP_SCREENSHOOT_COMPARE_ACTION)
	{
		Time = 0;
	}

	//ScreenshootCompareAction(const ScreenshootCompareAction& src);
	ScreenshootCompareAction(const ScreenshootCompareAction& src) : FETPAction(src)
	{
		Time = src.Time;

		for (size_t i = 0; i < src.imagesInfo.size(); i++)
		{
			CompareImageInfo* newImageInfo = new CompareImageInfo(*src.imagesInfo[i]);
			imagesInfo.push_back(newImageInfo);
		}
	}

	ScreenshootCompareAction(unsigned char* Screenshoot, DWORD actionTime, int width = -1, int height = -1) : FETPAction(FETP_SCREENSHOOT_COMPARE_ACTION)
	{
		Time = actionTime;

		if (width == -1 || height == -1)
		{
			width = static_cast<int>(SCREEN_SYSTEM.GetScreenWidth());
			height = static_cast<int>(SCREEN_SYSTEM.GetScreenHeight());
		}

		CompareImageInfo* newImageInfo = new CompareImageInfo(new FETPImage(Screenshoot, width, height));
		imagesInfo.push_back(newImageInfo);
	}

	~ScreenshootCompareAction()
	{
		for (size_t i = 0; i < imagesInfo.size(); i++)
			delete imagesInfo[i];
	}

	Json::Value ToJson()
	{
		std::string DirectoryPath = "";
		Json::Value Result = FETPAction::ToJson();

		Json::Value CompareImageInfos;
		for (size_t i = 0; i < imagesInfo.size(); i++)
		{
			std::string Index = std::to_string(i);

			CompareImageInfos[Index]["correctnessThreshold"] = imagesInfo[i]->correctnessThreshold;
			CompareImageInfos[Index]["maxColorShift"] = imagesInfo[i]->maxColorShift;

			std::string FileName = DirectoryPath + "screenshot_";
			FileName += ID;
			FileName += "_" + Index + "_";
			FileName += ".png";
			CompareImageInfos[Index]["screenshot_fileName"] = FileName;
			CompareImageInfos[Index]["screenshot_fullPath"] = imagesInfo[i]->Image->GetFullPath();

			CompareImageInfos[Index]["partialImageLeft"] = imagesInfo[i]->partialImageLeft;
			CompareImageInfos[Index]["partialImageTop"] = imagesInfo[i]->partialImageTop;

			CompareImageInfos[Index]["severalAttempts"] = imagesInfo[i]->severalAttempts;
			CompareImageInfos[Index]["severalAttemptsTimeout"] = imagesInfo[i]->severalAttemptsTimeout;

			if (imagesInfo[i]->partialImage != nullptr)
			{
				FileName = DirectoryPath + "partial_";
				FileName += ID;
				FileName += "_" + Index + "_";
				FileName += ".png";
				CompareImageInfos[Index]["partial_fileName"] = FileName;
				CompareImageInfos[Index]["partial_fullPath"] = imagesInfo[i]->partialImage->GetFullPath();

				CompareImageInfos[Index]["screenSearch"]["isActive"] = imagesInfo[i]->screenSearch != nullptr;
				if (imagesInfo[i]->screenSearch != nullptr)
				{
					CompareImageInfos[Index]["screenSearch"]["xShiftFromFound"] = imagesInfo[i]->screenSearch->GetXShiftFromFound();
					CompareImageInfos[Index]["screenSearch"]["yShiftFromFound"] = imagesInfo[i]->screenSearch->GetYShiftFromFound();
				}
			}
		}
		Result["compareImageInfos"] = CompareImageInfos;

		return Result;
	}

	void FromJson(Json::Value json)
	{
		std::string directoryPath = "";
		FETPAction::FromJson(json);

		// New version
		if (json.isMember("compareImageInfos"))
		{
			std::vector<Json::String> ImageInfosList = json["compareImageInfos"].getMemberNames();
			for (size_t i = 0; i < ImageInfosList.size(); i++)
			{
				loadImageInfo(json, directoryPath, i);
			}
		}
		else
		{
			std::string fileName = directoryPath + json["screenshot_fileName"].asCString();

			std::vector<unsigned char> rawData;
			unsigned uWidth, uHeight;
			lodepng::decode(rawData, uWidth, uHeight, fileName);

			unsigned char* tempData = new unsigned char[uWidth * uHeight * 4];
			memcpy_s(tempData, uWidth * uHeight * 4, rawData.data(), uWidth * uHeight * 4);
			imagesInfo.push_back(new CompareImageInfo(new FETPImage(tempData, uWidth, uHeight)));
			delete[] tempData;

			imagesInfo.back()->partialImageLeft = json["partialImageLeft"].asInt();
			imagesInfo.back()->partialImageTop = json["partialImageTop"].asInt();

			if (json.isMember("severalAttempts"))
				imagesInfo.back()->severalAttempts = json["severalAttempts"].asBool();

			if (json.isMember("severalAttemptsTimeout"))
				imagesInfo.back()->severalAttemptsTimeout = json["severalAttemptsTimeout"].asInt();

			if (json.isMember("partial_fileName"))
			{
				std::string fileName = directoryPath + json["partial_fileName"].asCString();

				std::vector<unsigned char> rawData;
				unsigned uWidth, uHeight;
				lodepng::decode(rawData, uWidth, uHeight, fileName);

				unsigned char* tempData = new unsigned char[uWidth * uHeight * 4];
				memcpy_s(tempData, uWidth * uHeight * 4, rawData.data(), uWidth * uHeight * 4);
				imagesInfo.back()->partialImage = new FETPImage(tempData, uWidth, uHeight);
				delete[] tempData;
			}
		}
	}

	void saveImagesToDisk()
	{
		for (size_t i = 0; i < imagesInfo.size(); i++)
		{
			if (imagesInfo[i]->Image != nullptr)
			{
				unsigned char* tempRawData = imagesInfo[i]->Image->GetRawData();
				// One possibility why it is empty is that user copy node to clipboard.
				if (imagesInfo[i]->Image->GetFullPath() == "")
				{
					// So we need to save it to temp location.
					std::string tempDirectory = FocalEngine::FILE_SYSTEM.GetDirectoryPath(FocalEngine::FILE_SYSTEM.GetCurrentWorkingPath().c_str());
					std::string fileName = tempDirectory + "screenshot_";
					fileName += GetID();
					fileName += "_" + std::to_string(i) + "_";
					fileName += ".png";
					imagesInfo[i]->Image->SetFullPath(fileName);
				}
				lodepng::encode(imagesInfo[i]->Image->GetFullPath(), tempRawData, imagesInfo[i]->Image->GetWidth(), imagesInfo[i]->Image->GetHeight());
				delete[] tempRawData;
			}

			if (imagesInfo[i]->partialImage != nullptr)
			{
				unsigned char* tempRawData = imagesInfo[i]->partialImage->GetRawData();
				// One possibility why it is empty is that user copy node to clipboard.
				if (imagesInfo[i]->partialImage->GetFullPath() == "")
				{
					// So we need to save it to temp location.
					std::string tempDirectory = FocalEngine::FILE_SYSTEM.GetDirectoryPath(FocalEngine::FILE_SYSTEM.GetCurrentWorkingPath().c_str());
					std::string fileName = tempDirectory + "partial_";
					fileName += GetID();
					fileName += "_" + std::to_string(i) + "_";
					fileName += ".png";
					imagesInfo[i]->partialImage->SetFullPath(fileName);
				}
				lodepng::encode(imagesInfo[i]->partialImage->GetFullPath(), tempRawData, imagesInfo[i]->partialImage->GetWidth(), imagesInfo[i]->partialImage->GetHeight());
				delete[] tempRawData;
			}
		}
	}
};