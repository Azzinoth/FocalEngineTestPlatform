#pragma once

#include "../SubSystems/TestManager.h"

const COMDLG_FILTERSPEC ApplicationLoadFilter[] =
{
	{ L"exe files (*.exe)", L"*.exe" }
};

const COMDLG_FILTERSPEC SaveFileFilter[] =
{
	{ L"fetp files (*.fetp)", L"*.fetp" }
};

const COMDLG_FILTERSPEC OpenFETPFileFilter[] =
{
	{ L"fetp files (*.fetp)", L"*.fetp" }
};

const COMDLG_FILTERSPEC FETestsFileFilter[] =
{
	{ L"FETest set files (*.fetests)", L"*.fetests" }
};

struct FETestResult;
class FEPTActionSystem
{
	SINGLETON_PRIVATE_PART(FEPTActionSystem)

	friend class TestEditorWindow;
	friend class FETest;

	bool bIsRecording = false;

	std::vector<FETPAction*> RecordedActions;
	std::vector<KeyboardAction> LastKeyboardActions;

	bool bWasRecordingLastFrame = false;
	DWORD LastRecordModeChangeTimestamp = 0;

	std::vector<KeyboardAction> KeysToDelete;
	std::unordered_map<DWORD, bool> PressedKeysMap;

	DWORD LastScreenshotTimestamp = 0;

	KeyboardAction altTempStorage;
	KeyboardAction LastLeftAltUp;

	void FindAndDeleteKeys();
	void AddAction(FETPAction* NewAction);

	VisNodeSys::Node* GetNextNode(VisNodeSys::Node* CurrentNode);
	std::vector<FETPAction*> GetActionsFromNode(VisNodeSys::Node* currentNode);

	std::function<void(std::vector<FETPAction*>&)> OnFinishRecordingCallback = nullptr;

	FETest* CurrentlyRunning = nullptr;
	FETestResult* CurrentTestResult = nullptr;

	FETPAction* CopyAction(FETPAction* Other);
public:
	SINGLETON_PUBLIC_PART(FEPTActionSystem)

	void SwitchRecordMode();
	bool Run(FETest* TestToRun);

	void NewKeyboardAction(KeyboardAction KeyAction);
	void NewMouseAction(MouseAction NewMouseAction);
	void NewAction(FETPAction* NewAction);

	//void PlaceStructuredNodes(std::vector<FETPAction*> actions, VisNodeSys::NodeArea* NodeArea, bool copyActions = false);

	void Update();

	FETPAction* GetAction(size_t Index);

	void FilterActions(size_t StartIndex, std::function<bool (FETPAction*, int)> FilterFunction, std::vector<FETPAction*>& Output, bool StopOnFirstNonMatch = true);
	static bool MouseMoveActionFilter(FETPAction* Action, int OutputCount);
	static bool MouseLeftButtonActionFilter(FETPAction* Action, int OutputCount);
	static bool MouseRightButtonActionFilter(FETPAction* Action, int OutputCount);
	static bool MouseWheelActionFilter(FETPAction* Action, int OutputCount);
	static bool KeyboardTextActionFilter(FETPAction* Action, int OutputCount);
	static bool KeyboardPressActionFilter(FETPAction* Action, int OutputCount);

	void SetOnFinishRecordingCallback(std::function<void(std::vector<FETPAction*>&)> Callback);

	std::string ExtractText(std::vector<FETPAction*> Actions);
	std::vector<FETPAction*> GenerateInputTextActions(std::string Text, int AverageDelay);
};

#define ACTION_SYSTEM FEPTActionSystem::GetInstance()