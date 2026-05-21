#pragma once

#include "FEFileSystem.h"

#include "FEDearImguiWrapper/FEDearImguiWrapper.h"
#include "NodeSystem/CustomNodes/BeginNode.h"
#include "NodeSystem/CustomNodes/Timing/TimerNode.h"
#include "NodeSystem/CustomNodes/Timing/SleepNode.h"
#include "NodeSystem/CustomNodes/InputSimulation/MouseMoveNode.h"
#include "NodeSystem/CustomNodes/InputSimulation/MouseLeftButtonDownNode.h"
#include "NodeSystem/CustomNodes/InputSimulation/MouseLeftButtonUpNode.h"

#include "NodeSystem/CustomNodes/InputSimulation/MouseRightButtonDownNode.h"
#include "NodeSystem/CustomNodes/InputSimulation/MouseRightButtonUpNode.h"
#include "NodeSystem/CustomNodes/Images/ImageLiteralNode.h"
#include "NodeSystem/CustomNodes/Images/ImageVariableNode.h"
#include "NodeSystem/CustomNodes/Images/ImageLoadNode.h"
#include "NodeSystem/CustomNodes/Images/ImageSaveNode.h"
#include "NodeSystem/CustomNodes/Images/ImageSearchNode.h"
#include "NodeSystem/CustomNodes/Images/ScreenshotNode.h"
#include "NodeSystem/CustomNodes/Images/RecognizeTextNode.h"
#include "NodeSystem/CustomNodes/Strings/StringLiteralNode.h"
#include "NodeSystem/CustomNodes/Strings/StringVariableNode.h"
#include "NodeSystem/CustomNodes/Strings/IsStringNumberNode.h"
#include "NodeSystem/CustomNodes/Strings/IsStringEmptyNode.h"
#include "NodeSystem/CustomNodes/Strings/IsStringIntegerNode.h"
#include "NodeSystem/CustomNodes/Strings/StringEqualsNode.h"
#include "NodeSystem/CustomNodes/Strings/StringContainsNode.h"
#include "NodeSystem/CustomNodes/Strings/StringStartsWithNode.h"
#include "NodeSystem/CustomNodes/Strings/StringEndsWithNode.h"
#include "NodeSystem/CustomNodes/Strings/StringLengthNode.h"
#include "NodeSystem/CustomNodes/Strings/StringToIntNode.h"
#include "NodeSystem/CustomNodes/Strings/StringToFloatNode.h"
#include "NodeSystem/CustomNodes/Strings/StringConcatenateNode.h"
#include "NodeSystem/CustomNodes/Strings/StringSubstringNode.h"
#include "NodeSystem/CustomNodes/Strings/StringReplaceNode.h"
#include "NodeSystem/CustomNodes/Strings/StringToUpperNode.h"
#include "NodeSystem/CustomNodes/Strings/StringToLowerNode.h"
#include "NodeSystem/CustomNodes/Strings/StringTrimNode.h"
#include "NodeSystem/CustomNodes/FileSystem/ExtractFileNameNode.h"
#include "NodeSystem/CustomNodes/FileSystem/ExtractFileExtensionNode.h"
#include "NodeSystem/CustomNodes/FileSystem/ExtractDirectoryPathNode.h"
#include "NodeSystem/CustomNodes/FileSystem/GetAbsolutePathNode.h"
#include "NodeSystem/CustomNodes/FileSystem/DoesFileExistNode.h"
#include "NodeSystem/CustomNodes/FileSystem/DoesDirectoryExistNode.h"
#include "NodeSystem/CustomNodes/FileSystem/GetFileSizeNode.h"
#include "NodeSystem/CustomNodes/FileSystem/GetCurrentWorkingPathNode.h"
#include "NodeSystem/CustomNodes/FileSystem/GetTestWorkingPathNode.h"
#include "NodeSystem/CustomNodes/FileSystem/CopyFileNode.h"
#include "NodeSystem/CustomNodes/FileSystem/RenameFileNode.h"
#include "NodeSystem/CustomNodes/FileSystem/DeleteFileNode.h"
#include "NodeSystem/CustomNodes/FileSystem/CreateDirectoryNode.h"
#include "NodeSystem/CustomNodes/FileSystem/CopyDirectoryNode.h"
#include "NodeSystem/CustomNodes/FileSystem/RenameDirectoryNode.h"
#include "NodeSystem/CustomNodes/FileSystem/DeleteDirectoryNode.h"
#include "NodeSystem/CustomNodes/InputSimulation/KeyboardKeyDownNode.h"
#include "NodeSystem/CustomNodes/InputSimulation/KeyboardKeyUpNode.h"
#include "NodeSystem/CustomNodes/InputSimulation/TextInputNode.h"
#include "NodeSystem/CustomNodes/System/LaunchApplicationNode.h"
#include "../Windows/NodeArea/NodeAreaWindowManager.h"

enum FE_TEST_FAIL_REASON
{
	FE_TEST_NO_FAIL = 0,
	FE_TEST_FAIL_INTERNAL_ERROR = 1,
	FE_TEST_FAIL_SCREENSHOOT_COMPARE = 2,
	FE_TEST_FAIL_CANT_FIND_FILE = 3
};

struct FETestScreenshotCompareResult
{
	FETPImage* Expected = nullptr;
	FETPImage* Screenshot = nullptr;
	FETPImage* Difference = nullptr;
	int Similarity = 0;

	FETestScreenshotCompareResult(FETPImage* Expected, FETPImage* Screenshot, FETPImage* Difference, int Similarity)
	{
		this->Expected = Expected;
		this->Screenshot = Screenshot;
		this->Difference = Difference;
		this->Similarity = Similarity;
	}

	~FETestScreenshotCompareResult()
	{
		delete Screenshot;
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
	FETestScreenshotCompareResult* ScreenshotCompare = nullptr;
public:
	~FETestResult()
	{
		delete ScreenshotCompare;
	}

	FETest* Parent = nullptr;
	DWORD StartTime = 0;
	DWORD EndTime = 0;

	bool bIsSuccessful = true;
	FETPAction* FailedAction = nullptr;
	FE_TEST_FAIL_REASON FailReason = FE_TEST_NO_FAIL;

	FETestScreenshotCompareResult* GetScreenshotCompareResult()
	{
		return ScreenshotCompare;
	}

	void setScreenshotCompareResult(FETestScreenshotCompareResult* NewValue)
	{
		delete ScreenshotCompare;
		ScreenshotCompare = NewValue;
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

class TestEditorWindow;
class TestsOverviewWindow;
class TestPropertiesWindow;
class FETest
{
	friend TestEditorWindow;
	friend TestsOverviewWindow;
	friend TestPropertiesWindow;

	std::string Name;
	int LoopCount = 1;
	float SpeedFactor = 1.0f;
	
	VisNodeSys::NodeArea* DummyRootNodeArea = nullptr;
	BeginNode* Begin = nullptr;
	std::vector<FETestResult*> Results;

	std::unordered_map<std::string, std::string> MacrosToReplace;
public:
	FETest();
	~FETest();

	static ImColor* DefaultConnectionColor;
	static ImColor* MainPathConnectionColor;

	std::vector<FETestBeforeAction*> BeforeStart;
	
	std::string FilePath;
	VisNodeSys::NodeArea* EntryPointNodeArea = nullptr;

	VisNodeSys::NodeArea* GetDummyRootNodeArea() const;
	BeginNode* GetBeginNode();
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

	static FE_BEFORE_TEST_ACTION_TYPE StringToFEBeforeTestActionType(std::string Text)
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