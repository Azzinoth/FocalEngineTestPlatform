#pragma once

#include "FETPAction.h"
#include "FETPComputeShaderCompare.h"

class FETPScreen
{
public:
	SINGLETON_PUBLIC_PART(FETPScreen)

	unsigned char* getScreenData();
	FETPImage* GetScreenDataAsImage();
	void getScreenRegion(unsigned char* data, int left, int top, int width, int height, bool updateScreenDataFlag = true);

	size_t getScreenWidth();
	size_t getScreenHeight();

	int compare(size_t width, size_t height, unsigned char* firstData, unsigned char* secondData, unsigned char* diffData, int maxColorShift);
	bool searchOnScreen(size_t width, size_t height, unsigned char* data, size_t& x, size_t& y, float correctnessThreshold, int maxColorShift, int* maxSimilarity = nullptr);

	glm::vec2 imageSizeInRegion(size_t imageW, size_t imageH, size_t regionW, size_t regionH);

	void updateScreenData();
private:
	SINGLETON_PRIVATE_PART(FETPScreen)

	unsigned char* screenData = nullptr;
	int simpleCompare(size_t width, size_t height, unsigned char* firstData, unsigned char* secondData, int maxColorShift);
};

#define SCREEN_SYSTEM FETPScreen::GetInstance()
 
struct screenSearchInfo
{
private:
	ImVec2 screenRegionMin = ImVec2(0, 0);
	ImVec2 screenRegionMax = ImVec2(0, 0);
	bool searchOnScreen = false;

	int xShiftFromFound = 50;
	int yShiftFromFound = 50;
public:
	screenSearchInfo()
	{
	}

	screenSearchInfo(screenSearchInfo& src)
	{
		screenRegionMin = src.screenRegionMin;
		screenRegionMax = src.screenRegionMax;
		searchOnScreen = src.searchOnScreen;

		xShiftFromFound = src.xShiftFromFound;
		yShiftFromFound = src.yShiftFromFound;
	}

	bool getSearchOnScreenMode();
	void setSearchOnScreenMode(bool newValue);

	ImVec2 getScreenMinRegion();
	ImVec2 getScreenMaxRegion();

	void setScreenRegion(ImVec2 ScreenRegionMin, ImVec2 ScreenRegionMax);

	int getXShiftFromFound();
	void setXShiftFromFound(int newValue);

	int getYShiftFromFound();
	void setYShiftFromFound(int newValue);
};

struct compareImageInfo
{
public:
	FETPImage* image = nullptr;
	bool lastRunResult = false;
	int correctnessThreshold = 95;
	int maxColorShift = 4;

	FETPImage* partialImage = nullptr;
	int partialImageLeft = 0;
	int partialImageTop = 0;
	
	bool severalAttempts = false;
	int severalAttemptsTimeout = 10000;

	// Search for image on screen
	screenSearchInfo* screenSearch = nullptr;

	compareImageInfo()
	{
		image = nullptr;
		partialImage = nullptr;

		partialImageLeft = 0;
		partialImageTop = 0;
	}

	compareImageInfo(compareImageInfo& src)
	{
		image = new FETPImage(*src.image);
		if (src.partialImage != nullptr)
			partialImage = new FETPImage(*src.partialImage);

		if (src.screenSearch != nullptr)
			screenSearch = new screenSearchInfo(*src.screenSearch);

		lastRunResult = src.lastRunResult;
		correctnessThreshold = src.correctnessThreshold;
		maxColorShift = src.maxColorShift;

		partialImageLeft = src.partialImageLeft;
		partialImageTop = src.partialImageTop;

		severalAttempts = src.severalAttempts;
		severalAttemptsTimeout = src.severalAttemptsTimeout;
	}

	compareImageInfo(FETPImage* image)
	{
		this->image = image;

		partialImageLeft = 0;
		partialImageTop = 0;
	}

	~compareImageInfo()
	{
		delete image;
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

		imagesInfo.push_back(new compareImageInfo(new FETPImage(fileName)));

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
					imagesInfo.back()->screenSearch = new screenSearchInfo();

				if (json["compareImageInfos"][std::to_string(index)]["screenSearch"].isMember("xShiftFromFound"))
					imagesInfo.back()->screenSearch->setXShiftFromFound(json["compareImageInfos"][std::to_string(index)]["screenSearch"]["xShiftFromFound"].asInt());

				if (json["compareImageInfos"][std::to_string(index)]["screenSearch"].isMember("yShiftFromFound"))
					imagesInfo.back()->screenSearch->setYShiftFromFound(json["compareImageInfos"][std::to_string(index)]["screenSearch"]["yShiftFromFound"].asInt());
			}
		}
	}

public:
	std::vector<compareImageInfo*> imagesInfo;
	static bool bUseGPU;

	ScreenshootCompareAction() : FETPAction(FETP_SCREENSHOOT_COMPARE_ACTION)
	{
		time = 0;
	}

	//ScreenshootCompareAction(const ScreenshootCompareAction& src);
	ScreenshootCompareAction(const ScreenshootCompareAction& src) : FETPAction(src)
	{
		time = src.time;

		for (size_t i = 0; i < src.imagesInfo.size(); i++)
		{
			compareImageInfo* newImageInfo = new compareImageInfo(*src.imagesInfo[i]);
			imagesInfo.push_back(newImageInfo);
		}
	}

	ScreenshootCompareAction(unsigned char* screenshoot, DWORD actionTime, int width = -1, int height = -1) : FETPAction(FETP_SCREENSHOOT_COMPARE_ACTION)
	{
		time = actionTime;

		if (width == -1 || height == -1)
		{
			width = static_cast<int>(SCREEN_SYSTEM.getScreenWidth());
			height = static_cast<int>(SCREEN_SYSTEM.getScreenHeight());
		}

		compareImageInfo* newImageInfo = new compareImageInfo(new FETPImage(screenshoot, width, height));
		imagesInfo.push_back(newImageInfo);
	}

	~ScreenshootCompareAction()
	{
		for (size_t i = 0; i < imagesInfo.size(); i++)
			delete imagesInfo[i];
	}

	Json::Value toJson()
	{
		std::string DirectoryPath = "";
		Json::Value Result = FETPAction::toJson();

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
			CompareImageInfos[Index]["screenshot_fullPath"] = imagesInfo[i]->image->GetFullPath();

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
					CompareImageInfos[Index]["screenSearch"]["xShiftFromFound"] = imagesInfo[i]->screenSearch->getXShiftFromFound();
					CompareImageInfos[Index]["screenSearch"]["yShiftFromFound"] = imagesInfo[i]->screenSearch->getYShiftFromFound();
				}
			}
		}
		Result["compareImageInfos"] = CompareImageInfos;

		return Result;
	}

	void fromJson(Json::Value json)
	{
		std::string directoryPath = "";
		FETPAction::fromJson(json);

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
			imagesInfo.push_back(new compareImageInfo(new FETPImage(tempData, uWidth, uHeight)));
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
			if (imagesInfo[i]->image != nullptr)
			{
				unsigned char* tempRawData = imagesInfo[i]->image->GetRawData();
				// One possibility why it is empty is that user copy node to clipboard.
				if (imagesInfo[i]->image->GetFullPath() == "")
				{
					// So we need to save it to temp location.
					std::string tempDirectory = FocalEngine::FILE_SYSTEM.getDirectoryPath(FocalEngine::FILE_SYSTEM.getApplicationPath().c_str());
					std::string fileName = tempDirectory + "screenshot_";
					fileName += getID();
					fileName += "_" + std::to_string(i) + "_";
					fileName += ".png";
					imagesInfo[i]->image->SetFullPath(fileName);
				}
				lodepng::encode(imagesInfo[i]->image->GetFullPath(), tempRawData, imagesInfo[i]->image->GetWidth(), imagesInfo[i]->image->GetHeight());
				delete[] tempRawData;
			}

			if (imagesInfo[i]->partialImage != nullptr)
			{
				unsigned char* tempRawData = imagesInfo[i]->partialImage->GetRawData();
				// One possibility why it is empty is that user copy node to clipboard.
				if (imagesInfo[i]->partialImage->GetFullPath() == "")
				{
					// So we need to save it to temp location.
					std::string tempDirectory = FocalEngine::FILE_SYSTEM.getDirectoryPath(FocalEngine::FILE_SYSTEM.getApplicationPath().c_str());
					std::string fileName = tempDirectory + "partial_";
					fileName += getID();
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