#pragma once

#include "../SubSystems/TestManager.h"

const COMDLG_FILTERSPEC applicationLoadFilter[] =
{
	{ L"exe files (*.exe)", L"*.exe" }
};

const COMDLG_FILTERSPEC saveFileFilter[] =
{
	{ L"fetp files (*.fetp)", L"*.fetp" }
};

const COMDLG_FILTERSPEC openFETPFileFilter[] =
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

	friend class testEditorWinow;
	friend class FETest;

	bool recording = false;

	std::vector<FETPAction*> recordedActions;
	std::vector<KeyboardAction> lastKeyboarActions;

	bool actionsRecordLastFrame = false;
	DWORD lastTimeRecordModeWasChanged = 0;

	std::vector<KeyboardAction> keysToDelete;
	std::unordered_map<DWORD, bool> pressedKeys;

	DWORD lastTimeScreenshootWasTaken = 0;

	KeyboardAction altTempStorage;
	KeyboardAction lastLeftAltUp;

	void findAndDeleteKeys();
	void addAction(FETPAction* newAction);

	VisNodeSys::Node* getNextNode(VisNodeSys::Node* currentNode);
	std::vector<FETPAction*> getActionsFromNode(VisNodeSys::Node* currentNode);
	bool execute(std::vector<FETPAction*> action);

	FETPImage* imageToUse(compareImageInfo* imageInfo);
	bool execute(ScreenshootCompareAction* action);

	std::function<void(std::vector<FETPAction*>&)> finishRecordingCallback = nullptr;

	FETest* currentlyRunning = nullptr;
	FETestResult* currentTestResult = nullptr;

	FETPAction* copyAction(FETPAction* src);
public:
	SINGLETON_PUBLIC_PART(FEPTActionSystem)

	void takeScreenshoot();
	void recordModeSwitch();
	bool run(FETest* testToRun);

	void newKeyboardAction(KeyboardAction keyAction);
	void newMouseAction(MouseAction mouseAction);
	void newAction(FETPAction* newAction);

	void placeStructuredNodes(std::vector<FETPAction*> actions, VisNodeSys::NodeArea* NodeArea, bool copyActions = false);

	void update();

	FETPAction* getAction(size_t index);

	void filterActions(size_t startIndex, std::function<bool (FETPAction*, int)> filerFunction, std::vector<FETPAction*>& output, bool stopOnFirstNonMatch = true);
	static bool mouseMoveActionFilter(FETPAction* action, int outputCount);
	static bool mouseLeftButtonActionFilter(FETPAction* action, int outputCount);
	static bool mouseRightButtonActionFilter(FETPAction* action, int outputCount);
	static bool mouseWheelActionFilter(FETPAction* action, int outputCount);
	static bool keyboardTextActionFilter(FETPAction* action, int outputCount);
	static bool keyboardPressActionFilter(FETPAction* action, int outputCount);

	VisNodeSys::Node* tryToPackActions(size_t& index);

	void setFinishRecordingCallback(std::function<void(std::vector<FETPAction*>&)> callback);

	std::string extractText(std::vector<FETPAction*> actions);
	std::vector<FETPAction*> generateInputTextActions(std::string text, int avarageDelay);
};

#define ACTION_SYSTEM FEPTActionSystem::GetInstance()