#pragma once

#include "FEFileSystem.h"

#include "FEDearImguiWrapper/FEDearImguiWrapper.h"
#include "NodeSystem/CustomNodes/intNode.h"
#include "NodeSystem/CustomNodes/vec2Node.h"
#include "NodeSystem/CustomNodes/boolNode.h"
#include "NodeSystem/CustomNodes/branchNode.h"
#include "NodeSystem/CustomNodes/timerNode.h"
#include "NodeSystem/CustomNodes/sleepNode.h"
#include "NodeSystem/CustomNodes/beginNode.h"
#include "NodeSystem/CustomNodes/mouseMoveNode.h"
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
	FETPImage* expected = nullptr;
	FETPImage* screenshoot = nullptr;
	FETPImage* difference = nullptr;
	int similarity = 0;

	FETestScreenshootCompareResult(FETPImage* Expected, FETPImage* Screenshoot, FETPImage* Difference, int Similarity)
	{
		expected = Expected;
		screenshoot = Screenshoot;
		difference = Difference;
		similarity = Similarity;
	}

	~FETestScreenshootCompareResult()
	{
		delete screenshoot;
		delete difference;
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
	FE_BEFORE_TEST_ACTION_TYPE type;

	std::string path;
	std::string newObjectName;
};

class FETest;
struct FETestResult
{
private:
	FETestScreenshootCompareResult* screenshootCompare = nullptr;
public:
	~FETestResult()
	{
		delete screenshootCompare;
	}

	FETest* parent = nullptr;
	DWORD startTime = 0;
	DWORD endTime = 0;

	bool success = true;
	FETPAction* failedAction = nullptr;
	FE_TEST_FAIL_REASON failReason = FE_TEST_NO_FAIL;

	FETestScreenshootCompareResult* getScreenshootCompareResult()
	{
		return screenshootCompare;
	}

	void setScreenshootCompareResult(FETestScreenshootCompareResult* newValue)
	{
		delete screenshootCompare;
		screenshootCompare = newValue;
	}

	static std::string FETestFailReasonToString(FE_TEST_FAIL_REASON reasonType)
	{
		switch (reasonType)
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

	std::string name;
	int loopCount = 1;
	float speedFactor = 1.0f;

	beginNode* begin = nullptr;
	std::vector<FETestResult*> results;

	void validateImagePathesInFile(std::string filePath);
	Json::Value validateImagePathesInNodeArea(std::string nodeAreaText);
	void validateImagePathes(VisualNodeArea* nodeArea = nullptr, std::string filePath = "");

	std::unordered_map<std::string, std::string> macrosToReplace;
public:
	FETest();
	~FETest();

	static ImColor* defaultConnectionColor;
	static ImColor* mainPathConnectionColor;

	std::vector<FETestBeforeAction*> beforeStart;
	
	std::string filePath;
	VisualNodeArea* nodeArea;

	beginNode* getBeginNode();
	void reColorMainTestPath();

	void save(const char* fileName);
	void load();

	void addResult(FETestResult* newResult);
	FETestResult* getLastTestResult();

	std::string getName();
	void setName(std::string newValue);

	float getSpeedFactor();
	void setSpeedFactor(float newValue);

	int getLoopCount();
	void setLoopCount(int newValue);

	void addBeforeStartAction(FETestBeforeAction* action);
	void beforeBegin();

	void addMacro(std::string macro, std::string replaceWith);
	bool replaceMacro(std::string& text);
	//void getReplaceWith(std::string macro);

	static std::string FEBeforeTestActionTypeToString(FE_BEFORE_TEST_ACTION_TYPE action)
	{
		switch (action)
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

	static FE_BEFORE_TEST_ACTION_TYPE stringToFEBeforeTestActionType(std::string text)
	{
		if (text == "CREATE_FILE")
		{
			return FE_BEFORE_TEST_ACTION_CREATE_FILE;
		}
		else if (text == "CREATE_DIRECTORY")
		{
			return FE_BEFORE_TEST_ACTION_CREATE_DIRECTORY;
		}
		else if (text == "COPY_FILE")
		{
			return FE_BEFORE_TEST_ACTION_COPY_FILE;
		}
		else if (text == "COPY_DIRECTORY")
		{
			return FE_BEFORE_TEST_ACTION_COPY_DIRECTORY;
		}
		else if (text == "DELETE_FILE")
		{
			return FE_BEFORE_TEST_ACTION_DELETE_FILE;
		}
		else if (text == "DELETE_DIRECTORY")
		{
			return FE_BEFORE_TEST_ACTION_DELETE_DIRECTORY;
		}

		return FE_BEFORE_TEST_ACTION_NONE;
	}
};