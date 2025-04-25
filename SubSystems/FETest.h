#pragma once

#include "FEFileSystem.h"

#include "FEDearImguiWrapper/FEDearImguiWrapper.h"
#include "NodeSystem/CustomNodes/intNode.h"
#include "NodeSystem/CustomNodes/floatNode.h"
#include "NodeSystem/CustomNodes/vec2Node.h"
#include "NodeSystem/CustomNodes/vec2AddNode.h"
#include "NodeSystem/CustomNodes/boolNode.h"
#include "NodeSystem/CustomNodes/branchNode.h"
#include "NodeSystem/CustomNodes/sequenceNode.h"
#include "NodeSystem/CustomNodes/timerNode.h"
#include "NodeSystem/CustomNodes/sleepNode.h"
#include "NodeSystem/CustomNodes/beginNode.h"
#include "NodeSystem/CustomNodes/mouseMoveNode.h"
#include "NodeSystem/CustomNodes/mouseLeftButtonDown.h"
#include "NodeSystem/CustomNodes/mouseLeftButtonUp.h"
#include "NodeSystem/CustomNodes/imageNode.h"
#include "NodeSystem/CustomNodes/imageSearchNode.h"
#include "NodeSystem/CustomNodes/combinedActionNode.h"
#include "NodeSystem/CustomNodes/regionNode.h"

enum FE_TEST_FAIL_REASON
{
	FE_TEST_NO_FAIL = 0,
	FE_TEST_FAIL_INTERNAL_ERROR = 1,
	FE_TEST_FAIL_SCREENSHOOT_COMPARE = 2,
	FE_TEST_FAIL_CANT_FIND_FILE = 3
};

struct FETestScreenshootCompareResult
{
	FETPImage* Expected = nullptr;
	FETPImage* Screenshoot = nullptr;
	FETPImage* Difference = nullptr;
	int Similarity = 0;

	FETestScreenshootCompareResult(FETPImage* Expected, FETPImage* Screenshoot, FETPImage* Difference, int Similarity)
	{
		this->Expected = Expected;
		this->Screenshoot = Screenshoot;
		this->Difference = Difference;
		this->Similarity = Similarity;
	}

	~FETestScreenshootCompareResult()
	{
		delete Screenshoot;
		delete Difference;
	}
};

enum FE_BEFORE_TEST_ACTION_TYPE
{
	FE_BEFORE_TEST_ACTION_NONE = 0,
	FE_BEFORE_TEST_ACTION_CREATE_FILE = 1,
	FE_BEFORE_TEST_ACTION_CREATE_DIRECTORY = 2,
	FE_BEFORE_TEST_ACTION_COPY_FILE = 3,
	FE_BEFORE_TEST_ACTION_COPY_DIRECTORY = 4,
	FE_BEFORE_TEST_ACTION_DELETE_FILE = 5,
	FE_BEFORE_TEST_ACTION_DELETE_DIRECTORY = 6
};

struct FETestBeforeAction
{
	FE_BEFORE_TEST_ACTION_TYPE Type;

	std::string Path;
	std::string NewObjectName;
};

class FETest;
struct FETestResult
{
private:
	FETestScreenshootCompareResult* ScreenshootCompare = nullptr;
public:
	~FETestResult()
	{
		delete ScreenshootCompare;
	}

	FETest* Parent = nullptr;
	DWORD StartTime = 0;
	DWORD EndTime = 0;

	bool bIsSuccessful = true;
	FETPAction* FailedAction = nullptr;
	FE_TEST_FAIL_REASON FailReason = FE_TEST_NO_FAIL;

	FETestScreenshootCompareResult* getScreenshootCompareResult()
	{
		return ScreenshootCompare;
	}

	void setScreenshootCompareResult(FETestScreenshootCompareResult* NewValue)
	{
		delete ScreenshootCompare;
		ScreenshootCompare = NewValue;
	}

	static std::string FETestFailReasonToString(FE_TEST_FAIL_REASON ReasonType)
	{
		switch (ReasonType)
		{
			case FE_TEST_NO_FAIL:
			{
				return "NO_FAIL";
				break;
			}

			case FE_TEST_FAIL_INTERNAL_ERROR:
			{
				return "INTERNAL_ERROR";
				break;
			}

			case FE_TEST_FAIL_SCREENSHOOT_COMPARE:
			{
				return "FAIL_SCREENSHOOT_COMPARE";
				break;
			}

			case FE_TEST_FAIL_CANT_FIND_FILE:
			{
				return "CANT_FIND_FILE";
				break;
			}
		
			default:
				break;
		}

		return "FE_NULL";
	}
};

class testEditorWinow;
class testsOverviewWindow;
class testPropertiesWindow;
class FETest
{
	friend testEditorWinow;
	friend testsOverviewWindow;
	friend testPropertiesWindow;

	std::string Name;
	int LoopCount = 1;
	float SpeedFactor = 1.0f;

	beginNode* Begin = nullptr;
	std::vector<FETestResult*> Results;

	void ValidateImagePathesInFile(std::string FilePath);
	Json::Value ValidateImagePathesInNodeArea(std::string nodeAreaText);
	void ValidateImagePathes(VisNodeSys::NodeArea* NodeArea = nullptr, std::string FilePath = "");

	std::unordered_map<std::string, std::string> MacrosToReplace;
public:
	FETest();
	~FETest();

	static ImColor* DefaultConnectionColor;
	static ImColor* MainPathConnectionColor;

	std::vector<FETestBeforeAction*> BeforeStart;
	
	std::string FilePath;
	VisNodeSys::NodeArea* NodeArea;

	beginNode* GetBeginNode();
	void ReColorMainTestPath();

	void Save(const char* FilePath);
	void Load();

	void AddResult(FETestResult* NewResult);
	FETestResult* GetLastTestResult();

	std::string GetName();
	void SetName(std::string NewValue);

	float GetSpeedFactor();
	void SetSpeedFactor(float NewValue);

	int GetLoopCount();
	void SetLoopCount(int NewValue);

	void AddBeforeStartAction(FETestBeforeAction* Action);
	void BeforeBegin();

	void AddMacro(std::string Macro, std::string ReplaceWith);
	bool ReplaceMacro(std::string& Text);
	//void getReplaceWith(std::string macro);

	static std::string FEBeforeTestActionTypeToString(FE_BEFORE_TEST_ACTION_TYPE Action)
	{
		switch (Action)
		{
			case FE_BEFORE_TEST_ACTION_NONE:
			{
				return "NONE";
				break;
			}

			case FE_BEFORE_TEST_ACTION_CREATE_FILE:
			{
				return "CREATE_FILE";
				break;
			}

			case FE_BEFORE_TEST_ACTION_CREATE_DIRECTORY:
			{
				return "CREATE_DIRECTORY";
				break;
			}

			case FE_BEFORE_TEST_ACTION_COPY_FILE:
			{
				return "COPY_FILE";
				break;
			}

			case FE_BEFORE_TEST_ACTION_COPY_DIRECTORY:
			{
				return "COPY_DIRECTORY";
				break;
			}

			case FE_BEFORE_TEST_ACTION_DELETE_FILE:
			{
				return "DELETE_FILE";
				break;
			}

			case FE_BEFORE_TEST_ACTION_DELETE_DIRECTORY:
			{
				return "DELETE_DIRECTORY";
				break;
			}

			default:
				break;
		}

		return "FE_NULL";
	}

	static FE_BEFORE_TEST_ACTION_TYPE stringToFEBeforeTestActionType(std::string Text)
	{
		if (Text == "CREATE_FILE")
		{
			return FE_BEFORE_TEST_ACTION_CREATE_FILE;
		}
		else if (Text == "CREATE_DIRECTORY")
		{
			return FE_BEFORE_TEST_ACTION_CREATE_DIRECTORY;
		}
		else if (Text == "COPY_FILE")
		{
			return FE_BEFORE_TEST_ACTION_COPY_FILE;
		}
		else if (Text == "COPY_DIRECTORY")
		{
			return FE_BEFORE_TEST_ACTION_COPY_DIRECTORY;
		}
		else if (Text == "DELETE_FILE")
		{
			return FE_BEFORE_TEST_ACTION_DELETE_FILE;
		}
		else if (Text == "DELETE_DIRECTORY")
		{
			return FE_BEFORE_TEST_ACTION_DELETE_DIRECTORY;
		}

		return FE_BEFORE_TEST_ACTION_NONE;
	}
};