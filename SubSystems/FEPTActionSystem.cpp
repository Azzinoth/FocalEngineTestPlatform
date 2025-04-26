#include "FEPTActionSystem.h"
using namespace VisNodeSys;

FEPTActionSystem::FEPTActionSystem()
{
}

FEPTActionSystem::~FEPTActionSystem() {}

VisNodeSys::Node* FEPTActionSystem::GetNextNode(VisNodeSys::Node* CurrentNode)
{
	if (CurrentNode->GetType() == "beginNode")
	{
		beginNode* Node = reinterpret_cast<beginNode*>(CurrentNode);
		return Node->GetNextNode();
	}
	else if (CurrentNode->GetType() == "globalActionNode")
	{
		globalActionNode* Node = reinterpret_cast<globalActionNode*>(CurrentNode);
		return Node->GetNextNode();
	}
	else if (CurrentNode->GetType() == "combinedActionNode")
	{
		combinedActionNode* Node = reinterpret_cast<combinedActionNode*>(CurrentNode);
		return Node->GetNextNode();
	}

	return nullptr;
}

std::vector<FETPAction*> FEPTActionSystem::GetActionsFromNode(VisNodeSys::Node* CurrentNode)
{
	std::vector<FETPAction*> Result;

	if (CurrentNode->GetType() == "beginNode")
	{
		return Result;
	}
	else if (CurrentNode->GetType() == "globalActionNode")
	{
		globalActionNode* Node = reinterpret_cast<globalActionNode*>(CurrentNode);
		Result.push_back(Node->GetData());
	}
	else if (CurrentNode->GetType() == "combinedActionNode")
	{
		combinedActionNode* Node = reinterpret_cast<combinedActionNode*>(CurrentNode);
		return Node->GetData();
	}

	return Result;
}

bool FEPTActionSystem::Run(FETest* TestToRun)
{
	if (TestToRun == nullptr || TestToRun->GetBeginNode() == nullptr)
	{
		CurrentlyRunning = nullptr;
		return false;
	}

	FocalEngine::APPLICATION.GetMainWindow()->Minimize();
	Sleep(10);
	CurrentlyRunning = TestToRun;
	CurrentTestResult = new FETestResult();
	CurrentTestResult->Parent = CurrentlyRunning;
	CurrentTestResult->StartTime = GetTickCount();

	CurrentlyRunning->BeforeBegin();

	basicLogicNode* CurrentNode = CurrentlyRunning->GetBeginNode();
	CurrentlyRunning->NodeArea->TriggerOrphanSocketEvent(CurrentNode, EXECUTE);
	CurrentTestResult->bIsSuccessful = true;
	CurrentTestResult->EndTime = GetTickCount();
	CurrentlyRunning->AddResult(CurrentTestResult);

	if (CurrentlyRunning->GetLoopCount() <= 1)
	{
		CurrentlyRunning = nullptr;
		FocalEngine::APPLICATION.GetMainWindow()->Restore();
	}
	
	return true;
}

void FEPTActionSystem::PlaceStructuredNodes(std::vector<FETPAction*> actions, NodeArea* NodeArea, bool copyActions)
{
	static int LeftPadding = 15;
	static int MaxNodesPerWidth = 4;
	static int MaxNodesPerHeight = 4;
	static int DistanceBetweenNodesHorizontally = 40;
	static int DistanceBetweenNodesVertically = 150;

	FETPAction* TestAction = new FETPAction();
	globalActionNode* TestNode = new globalActionNode(TestAction);

	if (TestNode->GetStyle() == CIRCLE)
	{
		MaxNodesPerWidth = 6;
		MaxNodesPerHeight = 6;
		DistanceBetweenNodesHorizontally = 15;
		DistanceBetweenNodesVertically = int(NODE_DIAMETER + 15.0f);
	}
	else
	{
		MaxNodesPerWidth = 4;
		DistanceBetweenNodesHorizontally = 40;
		DistanceBetweenNodesVertically = 150;
	}

	delete TestNode;

	int ShowedIndex = 0;
	VisNodeSys::Node* PreviousNode = nullptr;
	for (size_t i = 0; i < actions.size(); i++)
	{
		VisNodeSys::Node* NewNode = nullptr;

		if (!copyActions)
		{
			if (actions[i]->GetType() != FETP_SLEEP_ACTION)
				NewNode = TryToPackActions(i);

			if (NewNode == nullptr)
				NewNode = new globalActionNode(actions[i]);
		}
		else
		{
			NewNode = new globalActionNode(CopyAction(actions[i]));
		}

		int Width, Height;
		FocalEngine::APPLICATION.GetMainWindow()->GetSize(&Width, &Height);

		int XPosition = LeftPadding;
		XPosition += (ShowedIndex % MaxNodesPerWidth * int(NewNode->GetSize().x + DistanceBetweenNodesHorizontally));
		XPosition %= Width;

		int YPosition = ShowedIndex / MaxNodesPerHeight;
		YPosition *= DistanceBetweenNodesVertically;

		NewNode->SetPosition(ImVec2(float(XPosition), float(YPosition)));
		NodeArea->AddNode(NewNode);

		if (PreviousNode != nullptr)
			NodeArea->TryToConnect(PreviousNode, 0, NewNode, 0);
		PreviousNode = NewNode;
		ShowedIndex++;
	}
}

void FEPTActionSystem::SwitchRecordMode()
{
	if (GetTickCount() - LastRecordModeChangeTimestamp > 500)
	{
		LastRecordModeChangeTimestamp = GetTickCount();

		bIsRecording ? FocalEngine::APPLICATION.GetMainWindow()->Restore() : FocalEngine::APPLICATION.GetMainWindow()->Minimize();
		bIsRecording = !bIsRecording;

		if (bIsRecording)
		{
			RecordedActions.clear();
		}
		else
		{
			if (OnFinishRecordingCallback != nullptr)
				OnFinishRecordingCallback(RecordedActions);
		}
	}
}

void FEPTActionSystem::NewKeyboardAction(KeyboardAction KeyAction)
{
	if (KeyAction.EventType == WM_KEYDOWN || KeyAction.EventType == WM_SYSKEYDOWN)
	{
		if (PressedKeysMap.find(KeyAction.HookInfo.vkCode) != PressedKeysMap.end())
			return;

		PressedKeysMap[KeyAction.HookInfo.vkCode] = true;

		if (PressedKeysMap.find(164) != PressedKeysMap.end())
		{
			altTempStorage = KeyAction;
			return;
		}
	}
	else if (KeyAction.EventType == WM_KEYUP || KeyAction.EventType == WM_SYSKEYUP)
	{
		if (PressedKeysMap.find(164) != PressedKeysMap.end() && KeyAction.HookInfo.vkCode == 164)
		{
			LastLeftAltUp = KeyAction;

			PressedKeysMap.erase(KeyAction.HookInfo.vkCode);
			return;
		}

		DWORD TimeFromLastLeftAltUp = 1000;
		if (LastLeftAltUp.EventType != 0)
			TimeFromLastLeftAltUp = GetTickCount() - LastLeftAltUp.HookInfo.time;

		if (KeyAction.HookInfo.vkCode == 77 &&
		   (PressedKeysMap.find(164) != PressedKeysMap.end() || (TimeFromLastLeftAltUp < 100)))
		{
			LastLeftAltUp = KeyboardAction();
			SwitchRecordMode();

			PressedKeysMap.erase(KeyAction.HookInfo.vkCode);
			return;
		}
		else if (KeyAction.HookInfo.vkCode == 67 &&
				(PressedKeysMap.find(164) != PressedKeysMap.end() || (TimeFromLastLeftAltUp < 100)))
		{
			PressedKeysMap.erase(KeyAction.HookInfo.vkCode);
			return;
		}
		else
		{

		}

		PressedKeysMap.erase(KeyAction.HookInfo.vkCode);
	}

	if (bIsRecording)
	{
		AddAction(new KeyboardAction(KeyAction));
	}
}

void FEPTActionSystem::NewMouseAction(MouseAction NewMouseAction)
{
	if (bIsRecording)
	{
		AddAction(new MouseAction(NewMouseAction));
	}
}

void FEPTActionSystem::FindAndDeleteKeys()
{
	if (KeysToDelete.size() != 0)
	{
		for (int i = static_cast<int>(RecordedActions.size() - 1); i >= 0; i--)
		{
			if (RecordedActions[i]->GetType() != FETP_KEYBOARD_ACTION)
				continue;

			KeyboardAction* Action = reinterpret_cast<KeyboardAction*>(RecordedActions[i]);
			for (int j = 0; j < int(KeysToDelete.size()); j++)
			{
				if (KeysToDelete.size() == 0 || RecordedActions.size() == 0)
					break;

				if (Action->HookInfo.vkCode == KeysToDelete[j].HookInfo.vkCode)
				{
					if ((KeysToDelete[j].EventType == WM_KEYUP && (Action->EventType == WM_KEYUP || Action->EventType == WM_SYSKEYUP)) ||
						(KeysToDelete[j].EventType == WM_KEYDOWN && (Action->EventType == WM_KEYDOWN || Action->EventType == WM_SYSKEYDOWN)))
					{
						delete Action;
						RecordedActions.erase(RecordedActions.begin() + i, RecordedActions.begin() + i + 1);
						KeysToDelete.erase(KeysToDelete.begin() + j, KeysToDelete.begin() + j + 1);

						j--;

						if (i != 0)
							i--;
					}
				}
			}
		}
	}
}

void FEPTActionSystem::Update()
{
	FindAndDeleteKeys();
}

FETPAction* FEPTActionSystem::GetAction(size_t Index)
{
	if (Index >= RecordedActions.size())
		return nullptr;

	return RecordedActions[Index];
}

void FEPTActionSystem::AddAction(FETPAction* NewAction)
{
	RecordedActions.push_back(NewAction);
	if (RecordedActions.size() > 1)
	{
		int DelayBetweenActions = 0;
		DelayBetweenActions = NewAction->GetTimeStamp() - RecordedActions[RecordedActions.size() - 2]->GetTimeStamp();
		if (DelayBetweenActions < 0)
			DelayBetweenActions = 0;

		if (DelayBetweenActions != 0)
			RecordedActions.insert(RecordedActions.end() - 1, new SleepAction(DelayBetweenActions));
	}
}

void FEPTActionSystem::FilterActions(size_t StartIndex, std::function<bool(FETPAction*, int)> FilerFunction, std::vector<FETPAction*>& Output, bool StopOnFirstNonMatch)
{
	if (StartIndex >= RecordedActions.size())
		return;

	Output.clear();

	while (true)
	{
		if (StartIndex >= RecordedActions.size())
			break;

		if (FilerFunction(RecordedActions[StartIndex], static_cast<int>(Output.size())))
		{
			Output.push_back(RecordedActions[StartIndex]);
		}
		else if (StopOnFirstNonMatch)
		{
			return;
		}

		StartIndex++;
	}
}

bool FEPTActionSystem::MouseMoveActionFilter(FETPAction* Action, int OutputCount)
{
	if (Action == nullptr)
		return false;

	if (Action->GetType() == FETP_SLEEP_ACTION)
		return true;

	if (Action->GetType() == FETP_MOUSE_ACTION)
	{
		if (reinterpret_cast<MouseAction*>(Action)->EventType == WM_MOUSEMOVE)
			return true;
	}

	return false;
}

bool FEPTActionSystem::MouseLeftButtonActionFilter(FETPAction* Action, int OutputCount)
{
	if (Action == nullptr)
		return false;

	if (OutputCount >= 3)
		return false;

	if (Action->GetType() == FETP_SLEEP_ACTION)
		return true;

	if (Action->GetType() == FETP_MOUSE_ACTION)
	{
		if (reinterpret_cast<MouseAction*>(Action)->EventType == WM_LBUTTONDOWN || reinterpret_cast<MouseAction*>(Action)->EventType == WM_LBUTTONUP)
			return true;
	}

	return false;
}

bool FEPTActionSystem::MouseRightButtonActionFilter(FETPAction* Action, int OutputCount)
{
	if (Action == nullptr)
		return false;

	if (OutputCount >= 3)
		return false;

	if (Action->GetType() == FETP_SLEEP_ACTION)
		return true;

	if (Action->GetType() == FETP_MOUSE_ACTION)
	{
		if (reinterpret_cast<MouseAction*>(Action)->EventType == WM_RBUTTONDOWN || reinterpret_cast<MouseAction*>(Action)->EventType == WM_RBUTTONUP)
			return true;
	}

	return false;
}

bool FEPTActionSystem::MouseWheelActionFilter(FETPAction* Action, int OutputCount)
{
	if (Action == nullptr)
		return false;

	if (Action->GetType() == FETP_SLEEP_ACTION)
		return true;

	if (Action->GetType() == FETP_MOUSE_ACTION)
	{
		if (reinterpret_cast<MouseAction*>(Action)->EventType == WM_MOUSEWHEEL)
			return true;
	}

	return false;
}

bool FEPTActionSystem::KeyboardTextActionFilter(FETPAction* Action, int OutputCount)
{
	if (Action == nullptr)
		return false;

	if (Action->GetType() == FETP_SLEEP_ACTION)
		return true;

	if (Action->GetType() == FETP_KEYBOARD_ACTION)
	{
		KeyboardAction* keyboardAction = reinterpret_cast<KeyboardAction*>(Action);
		if (keyboardAction->EventType == WM_KEYDOWN || keyboardAction->EventType == WM_SYSKEYDOWN ||
			keyboardAction->EventType == WM_KEYUP || keyboardAction->EventType == WM_SYSKEYUP)
		{
			if (INPUT_SYSTEM.GetCharFromAction(keyboardAction) != 0 || keyboardAction->HookInfo.vkCode == VK_RSHIFT || keyboardAction->HookInfo.vkCode == VK_CAPITAL)
				return true;
		}
	}

	return false;
}

bool FEPTActionSystem::KeyboardPressActionFilter(FETPAction* Action, int OutputCount)
{
	if (Action == nullptr)
		return false;

	if (OutputCount >= 3)
		return false;

	if (Action->GetType() == FETP_SLEEP_ACTION)
		return true;

	if (Action->GetType() == FETP_KEYBOARD_ACTION)
	{
		KeyboardAction* keyboardAction = reinterpret_cast<KeyboardAction*>(Action);
		if (keyboardAction->EventType == WM_KEYDOWN || keyboardAction->EventType == WM_SYSKEYDOWN ||
			keyboardAction->EventType == WM_KEYUP || keyboardAction->EventType == WM_SYSKEYUP)
		{
			if (INPUT_SYSTEM.GetCharFromAction(keyboardAction) == 0 && keyboardAction->HookInfo.vkCode != VK_RSHIFT && keyboardAction->HookInfo.vkCode != VK_CAPITAL)
				return true;
		}
	}

	return false;
}

VisNodeSys::Node* FEPTActionSystem::TryToPackActions(size_t& Index)
{
	std::vector<FETPAction*> ActionsToCombine;

	FilterActions(Index, MouseMoveActionFilter, ActionsToCombine);
	if (ActionsToCombine.size() > 1)
	{
		Index += ActionsToCombine.size() - 1;
		return new combinedActionNode(ActionsToCombine, FETP_COMBINED_MOUSE_MOVE_ACTION);
	}

	FilterActions(Index, MouseLeftButtonActionFilter, ActionsToCombine);
	if (ActionsToCombine.size() > 1)
	{
		Index += ActionsToCombine.size() - 1;
		return new combinedActionNode(ActionsToCombine, FETP_COMBINED_LEFT_MOUSE_ACTION);
	}

	FilterActions(Index, MouseRightButtonActionFilter, ActionsToCombine);
	if (ActionsToCombine.size() > 1)
	{
		Index += ActionsToCombine.size() - 1;
		return new combinedActionNode(ActionsToCombine, FETP_COMBINED_RIGHT_MOUSE_ACTION);
	}

	FilterActions(Index, MouseWheelActionFilter, ActionsToCombine);
	if (ActionsToCombine.size() > 1)
	{
		Index += ActionsToCombine.size() - 1;
		return new combinedActionNode(ActionsToCombine, FETP_COMBINED_WHEEL_MOUSE_ACTION);
	}

	FilterActions(Index, KeyboardPressActionFilter, ActionsToCombine);
	if (ActionsToCombine.size() > 1)
	{
		Index += ActionsToCombine.size() - 1;
		return new combinedActionNode(ActionsToCombine, FETP_COMBINED_KEY_PRESS_ACTION);
	}

	FilterActions(Index, KeyboardTextActionFilter, ActionsToCombine);
	if (ActionsToCombine.size() > 1)
	{
		Index += ActionsToCombine.size() - 1;
		return new combinedActionNode(ActionsToCombine, FETP_COMBINED_TEXT_INPUT_ACTION);
	}

	return nullptr;
}

void FEPTActionSystem::NewAction(FETPAction* NewAction)
{
	if (NewAction != nullptr)
		AddAction(NewAction);
}

void FEPTActionSystem::SetOnFinishRecordingCallback(std::function<void(std::vector<FETPAction*>&)> Callback)
{
	OnFinishRecordingCallback = Callback;
}

FETPAction* FEPTActionSystem::CopyAction(FETPAction* Other)
{
	if (Other->GetType() == FETP_KEYBOARD_ACTION)
	{
		return new KeyboardAction(*reinterpret_cast<KeyboardAction*>(Other));
	}
	else if (Other->GetType() == FETP_MOUSE_ACTION)
	{
		return new MouseAction(*reinterpret_cast<MouseAction*>(Other));
	}
	else if (Other->GetType() == FETP_SCREENSHOOT_COMPARE_ACTION)
	{
		return new ScreenshootCompareAction(*reinterpret_cast<ScreenshootCompareAction*>(Other));
	}
	else if (Other->GetType() == FETP_SLEEP_ACTION)
	{
		return new SleepAction(*reinterpret_cast<SleepAction*>(Other));
	}
	else if (Other->GetType() == FETP_LUNCH_APPLICATION_ACTION)
	{
		return new LunchApplicationAction(*reinterpret_cast<LunchApplicationAction*>(Other));
	}
	else
	{
		return new FETPAction(*Other);
	}
}

std::string FEPTActionSystem::ExtractText(std::vector<FETPAction*> Actions)
{
	std::string Result = "";
	for (size_t i = 0; i < Actions.size(); i++)
	{
		if (Actions[i]->GetType() != FETP_KEYBOARD_ACTION)
			continue;

		KeyboardAction* Action = reinterpret_cast<KeyboardAction*>(Actions[i]);
		if (Action->EventType == WM_KEYUP)
		{
			char TemporaryChar = INPUT_SYSTEM.GetCharFromAction(Action);
			if (TemporaryChar != 0)
				Result += TemporaryChar;
		}
	}

	return Result;
}

std::vector<FETPAction*> FEPTActionSystem::GenerateInputTextActions(std::string Text, int AverageDelay)
{
	std::vector<FETPAction*> Result;
	for (size_t i = 0; i < Text.size(); i++)
	{
		int ConvertedKey = VkKeyScanExA(char(Text[i]), GetKeyboardLayout(0));
		int VirtualKeyCode = ConvertedKey & 0xff;
		int KeysState = (ConvertedKey & 0xff00) >> 8;

		// Can't find appropriate key for that char.
		if (VirtualKeyCode == -1)
			continue;

		if (KeysState & 1)
		{
			KeyboardAction* NewAction = new KeyboardAction();
			NewAction->HookInfo.vkCode = 0x10;
			NewAction->EventType = WM_KEYDOWN;
			NewAction->bShiftPressed = false;
			Result.push_back(NewAction);
		}

		KeyboardAction* NewAction = new KeyboardAction();
		NewAction->HookInfo.vkCode = VirtualKeyCode;
		NewAction->EventType = WM_KEYDOWN;
		NewAction->bShiftPressed = KeysState & 1;
		Result.push_back(NewAction);

		SleepAction* NewSleepAction = new SleepAction(AverageDelay);
		Result.push_back(NewSleepAction);

		NewAction = new KeyboardAction();
		NewAction->HookInfo.vkCode = VirtualKeyCode;
		NewAction->EventType = WM_KEYUP;
		NewAction->bShiftPressed = KeysState & 1;
		Result.push_back(NewAction);

		if (KeysState & 1)
		{
			KeyboardAction* NewAction = new KeyboardAction();
			NewAction->HookInfo.vkCode = 0x10;
			NewAction->EventType = WM_KEYUP;
			NewAction->bShiftPressed = false;
			Result.push_back(NewAction);
		}
	}

	return Result;
}