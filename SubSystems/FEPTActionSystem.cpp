#include "FEPTActionSystem.h"
using namespace VisNodeSys;
#include "../Windows/NodeArea/NodeAreaWindowManager.h"

FEPTActionSystem::FEPTActionSystem()
{
}

FEPTActionSystem::~FEPTActionSystem() {}

std::vector<FETPAction*> FEPTActionSystem::GetActionsFromNode(VisNodeSys::Node* CurrentNode)
{
	std::vector<FETPAction*> Result;

	if (CurrentNode->GetType() == "BeginNode")
	{
		return Result;
	}

	return Result;
}

bool FEPTActionSystem::Run(FETest* TestToRun)
{
	if (TestToRun == nullptr || TestToRun->GetBeginNode() == nullptr || TestToRun->EntryPointNodeArea == nullptr)
	{
		CurrentlyRunning = nullptr;
		return false;
	}

	NodeAreaIDToHadProblematicAction.clear();
	std::vector<std::string> NodeAreaIDs = NODE_SYSTEM.GetNodeAreaIDList();
	for (size_t i = 0; i < NodeAreaIDs.size(); i++)
	{
		NodeArea* CurrentNodeArea = NODE_SYSTEM.GetNodeAreaByID(NodeAreaIDs[i]);
		if (CurrentNodeArea == nullptr)
			continue;

		CurrentNodeArea->SetSaveExecutedNodes(true);

		std::vector<ImageSearchNode*> ImageSearchNodes = CurrentNodeArea->GetNodesByType<ImageSearchNode>();
		for (size_t j = 0; j < ImageSearchNodes.size(); j++)
			ImageSearchNodes[j]->ResetToDefaultStatus();
	}

	FocalEngine::APPLICATION.GetMainWindow()->Minimize();
	Sleep(10);
	CurrentlyRunning = TestToRun;
	CurrentTestResult = new FETestResult();
	CurrentTestResult->Parent = CurrentlyRunning;
	CurrentTestResult->StartTime = GetTickCount();

	CurrentlyRunning->BeforeBegin();

	CurrentlyRunning->EntryPointNodeArea->SetExecutionEntryNode(CurrentlyRunning->GetBeginNode());
	CurrentlyRunning->EntryPointNodeArea->ExecuteNodeNetwork();
	CurrentTestResult->bIsSuccessful = true;
	CurrentTestResult->EndTime = GetTickCount();
	CurrentlyRunning->AddResult(CurrentTestResult);

	if (CurrentlyRunning->GetLoopCount() <= 1)
	{
		CurrentlyRunning = nullptr;
		FocalEngine::APPLICATION.GetMainWindow()->Restore();
	}

	NodeAreaIDs = NODE_SYSTEM.GetNodeAreaIDList();
	for (size_t i = 0; i < NodeAreaIDs.size(); i++)
	{
		NodeArea* CurrentNodeArea = NODE_SYSTEM.GetNodeAreaByID(NodeAreaIDs[i]);
		if (CurrentNodeArea == nullptr)
			continue;

		std::vector<ImageSearchNode*> ImageSearchNodes = CurrentNodeArea->GetNodesByType<ImageSearchNode>();
		for (size_t j = 0; j < ImageSearchNodes.size(); j++)
		{
			if (ImageSearchNodes[j]->GetStatus() == ACTION_NODE_STATUS::Failure)
			{
				CurrentTestResult->bIsSuccessful = false;
				CurrentTestResult->FailReason = FE_TEST_FAIL_SCREENSHOOT_COMPARE;
				NodeAreaIDToHadProblematicAction[CurrentNodeArea->GetID()] = true;
				break;
			}
		}
	}
	
	return true;
}

static void LayoutNodesInSnakingGrid(const std::vector<VisNodeSys::Node*>& Nodes,
									 ImVec2 Origin, int MaxColumns, float HorizontalPadding, float VerticalPadding)
{
	if (Nodes.empty() || MaxColumns < 1)
		return;

	float CurrentY = Origin.y;
	size_t RowStart = 0;

	while (RowStart < Nodes.size())
	{
		size_t RowEnd = std::min(RowStart + size_t(MaxColumns), Nodes.size());

		float RowMaxHeight = 0.0f;
		for (size_t i = RowStart; i < RowEnd; i++)
			RowMaxHeight = std::max(RowMaxHeight, Nodes[i]->GetSize().y);

		float CurrentX = Origin.x;
		for (size_t i = RowStart; i < RowEnd; i++)
		{
			Nodes[i]->SetPosition(ImVec2(CurrentX, CurrentY));
			CurrentX += Nodes[i]->GetSize().x + HorizontalPadding;
		}

		CurrentY += RowMaxHeight + VerticalPadding;
		RowStart = RowEnd;
	}
}

void FEPTActionSystem::ConvertActionsToNodes(std::vector<FETPAction*> Actions, VisNodeSys::NodeArea* TargetNodeArea)
{
	if (TargetNodeArea == nullptr)
		return;

	SubAreaNode* SubArea = NODE_SYSTEM.CreateSubAreaNode(TargetNodeArea->GetID());
	NodeArea* SubAreaNodeArea = SubArea->GetOwnedArea();

	for (size_t i = 1; i < Actions.size(); i++)
	{
		FETPAction* PreviousAction = Actions[i - 1];
		FETPAction* CurrentAction = Actions[i];
		if (PreviousAction->GetType() == FETP_MOUSE_ACTION && CurrentAction->GetType() == FETP_MOUSE_ACTION)
		{
			MouseAction* PreviousMouseAction = dynamic_cast<MouseAction*>(PreviousAction);
			MouseAction* CurrentMouseAction = dynamic_cast<MouseAction*>(CurrentAction);
			if (PreviousMouseAction->EventType == CurrentMouseAction->EventType)
			{
				if (PreviousMouseAction->HookInfo.pt.x == CurrentMouseAction->HookInfo.pt.x && PreviousMouseAction->HookInfo.pt.y == CurrentMouseAction->HookInfo.pt.y)
				{
					Actions.erase(Actions.begin() + i);
					i--;
				}
			}
		}
	}

	SubAreaInputNode* SubAreaInput = SubAreaNodeArea->GetNodesByType<SubAreaInputNode>()[0];

	std::vector<VisNodeSys::Node*> CreatedNodes;
	CreatedNodes.reserve(Actions.size() * 2);

	VisNodeSys::Node* PreviousNode = SubAreaInput;
	for (size_t i = 0; i < Actions.size(); i++)
	{
		VisNodeSys::Node* NewNode = nullptr;
		if (Actions[i]->GetType() == FETP_MOUSE_ACTION)
		{
			MouseAction* MouseActionPointer = dynamic_cast<MouseAction*>(Actions[i]);

			if (MouseActionPointer->EventType == WM_MOUSEMOVE)
			{
				NewNode = new MouseMoveNode();
				MouseMoveNode* NewMouseMoveNode = dynamic_cast<MouseMoveNode*>(NewNode);
				NewMouseMoveNode->SetMouseTargetPosition(glm::vec2(float(MouseActionPointer->HookInfo.pt.x), float(MouseActionPointer->HookInfo.pt.y)));
				SubAreaNodeArea->AddNode(NewMouseMoveNode);
			}

			if (MouseActionPointer->EventType == WM_LBUTTONDOWN)
			{
				NewNode = new MouseLeftButtonDownNode();
				MouseLeftButtonDownNode* NewMouseLeftButtonDownNode = dynamic_cast<MouseLeftButtonDownNode*>(NewNode);
				SubAreaNodeArea->AddNode(NewMouseLeftButtonDownNode);
			}

			if (MouseActionPointer->EventType == WM_LBUTTONUP)
			{
				NewNode = new MouseLeftButtonUpNode();
				MouseLeftButtonUpNode* NewMouseLeftButtonUpNode = dynamic_cast<MouseLeftButtonUpNode*>(NewNode);
				SubAreaNodeArea->AddNode(NewMouseLeftButtonUpNode);
			}

			if (MouseActionPointer->EventType == WM_RBUTTONDOWN)
			{
				NewNode = new MouseRightButtonDownNode();
				MouseRightButtonDownNode* NewMouseRightButtonDownNode = dynamic_cast<MouseRightButtonDownNode*>(NewNode);
				SubAreaNodeArea->AddNode(NewMouseRightButtonDownNode);
			}

			if (MouseActionPointer->EventType == WM_RBUTTONUP)
			{
				NewNode = new MouseRightButtonUpNode();
				MouseRightButtonUpNode* NewMouseRightButtonUpNode = dynamic_cast<MouseRightButtonUpNode*>(NewNode);
				SubAreaNodeArea->AddNode(NewMouseRightButtonUpNode);
			}
		}

		if (Actions[i]->GetType() == FETP_KEYBOARD_ACTION)
		{
			KeyboardAction* KeyboardActionPointer = dynamic_cast<KeyboardAction*>(Actions[i]);

			if (KeyboardActionPointer->EventType == WM_KEYDOWN || KeyboardActionPointer->EventType == WM_SYSKEYDOWN)
			{
				NewNode = new KeyboardKeyDownNode();
				KeyboardKeyDownNode* NewKeyboardKeyDownNode = dynamic_cast<KeyboardKeyDownNode*>(NewNode);
				NewKeyboardKeyDownNode->SetVirtualKeyCode(KeyboardActionPointer->HookInfo.vkCode);
				SubAreaNodeArea->AddNode(NewKeyboardKeyDownNode);
			}

			if (KeyboardActionPointer->EventType == WM_KEYUP || KeyboardActionPointer->EventType == WM_SYSKEYUP)
			{
				NewNode = new KeyboardKeyUpNode();
				KeyboardKeyUpNode* NewKeyboardKeyUpNode = dynamic_cast<KeyboardKeyUpNode*>(NewNode);
				NewKeyboardKeyUpNode->SetVirtualKeyCode(KeyboardActionPointer->HookInfo.vkCode);
				SubAreaNodeArea->AddNode(NewKeyboardKeyUpNode);
			}
		}

		if (NewNode == nullptr)
			continue;

		if (PreviousNode != nullptr)
			SubAreaNodeArea->TryToConnect(PreviousNode, 0, NewNode, 0);
		PreviousNode = NewNode;
		CreatedNodes.push_back(NewNode);

		if (i > 0)
		{
			int DelayBetweenActions = int(Actions[i]->GetTimeStamp()) - int(Actions[i - 1]->GetTimeStamp());
			if (DelayBetweenActions > 0)
			{
				SleepNode* NewSleepNode = new SleepNode();
				NewSleepNode->SetSleepDuration(DelayBetweenActions);
				SubAreaNodeArea->AddNode(NewSleepNode);

				SubAreaNodeArea->TryToConnect(PreviousNode, 0, NewSleepNode, 0);
				PreviousNode = NewSleepNode;
				CreatedNodes.push_back(NewSleepNode);
			}
		}
	}

	LayoutNodesInSnakingGrid(CreatedNodes, ImVec2(15.0f, 0.0f), 6, 40.0f, 40.0f);

	SubAreaOutputNode* SubAreaOutput = SubAreaNodeArea->GetNodesByType<SubAreaOutputNode>()[0];
	if (PreviousNode != nullptr)
		SubAreaNodeArea->TryToConnect(PreviousNode, 0, SubAreaOutput, 0);
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
			NodeAreaWindow* WindowInFocus = NODE_AREA_WINDOW_MANAGER.GetInFocusNodeAreaWindow();
			if (WindowInFocus != nullptr)
				ConvertActionsToNodes(RecordedActions, WindowInFocus->GetNodeArea());

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
			AltTemporaryStorage = KeyAction;
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
}

void FEPTActionSystem::FilterActions(size_t StartIndex, std::function<bool(FETPAction*, int)> FilterFunction, std::vector<FETPAction*>& Output, bool StopOnFirstNonMatch)
{
	if (StartIndex >= RecordedActions.size())
		return;

	Output.clear();

	while (true)
	{
		if (StartIndex >= RecordedActions.size())
			break;

		if (FilterFunction(RecordedActions[StartIndex], static_cast<int>(Output.size())))
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
		KeyboardAction* TypedAction = reinterpret_cast<KeyboardAction*>(Action);
		if (TypedAction->EventType == WM_KEYDOWN || TypedAction->EventType == WM_SYSKEYDOWN ||
			TypedAction->EventType == WM_KEYUP || TypedAction->EventType == WM_SYSKEYUP)
		{
			if (INPUT_SYSTEM.GetCharFromAction(TypedAction) != 0 || TypedAction->HookInfo.vkCode == VK_RSHIFT || TypedAction->HookInfo.vkCode == VK_CAPITAL)
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
		KeyboardAction* TypedAction = reinterpret_cast<KeyboardAction*>(Action);
		if (TypedAction->EventType == WM_KEYDOWN || TypedAction->EventType == WM_SYSKEYDOWN ||
			TypedAction->EventType == WM_KEYUP || TypedAction->EventType == WM_SYSKEYUP)
		{
			if (INPUT_SYSTEM.GetCharFromAction(TypedAction) == 0 && TypedAction->HookInfo.vkCode != VK_RSHIFT && TypedAction->HookInfo.vkCode != VK_CAPITAL)
				return true;
		}
	}

	return false;
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
	//else if (Other->GetType() == FETP_SLEEP_ACTION)
	//{
	//	return new SleepAction(*reinterpret_cast<SleepAction*>(Other));
	//}
	/*else if (Other->GetType() == FETP_LAUNCH_APPLICATION_ACTION)
	{
		return new LaunchApplicationAction(*reinterpret_cast<LaunchApplicationAction*>(Other));
	}*/
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

//bool FEPTActionSystem::Execute(std::vector<FETPAction*> Actions)
//{
//	for (size_t i = 0; i < Actions.size(); i++)
//	{
//		if (Actions[i]->GetType() == FETP_KEYBOARD_ACTION)
//		{
//			KeyboardAction* CurrentAction = reinterpret_cast<KeyboardAction*>(Actions[i]);
//			if (CurrentAction->wParam == WM_KEYDOWN || CurrentAction->wParam == WM_SYSKEYDOWN)
//			{
//				INPUT_SYSTEM.keyEvent(WM_KEYDOWN, CurrentAction->additionalInfo.vkCode);
//			}
//			else if (CurrentAction->wParam == WM_KEYUP || CurrentAction->wParam == WM_SYSKEYUP)
//			{
//				INPUT_SYSTEM.keyEvent(WM_KEYUP, CurrentAction->additionalInfo.vkCode);
//			}
//		}
//		else if (Actions[i]->GetType() == FETP_MOUSE_ACTION)
//		{
//			MouseAction* CurrentAction = reinterpret_cast<MouseAction*>(Actions[i]);
//
//			if (CurrentAction->wParam == WM_MOUSEMOVE)
//			{
//				//INPUT_SYSTEM.mouseMoveTo(action->additionalInfo.pt.x, action->additionalInfo.pt.y);
//			}
//			else if (CurrentAction->wParam == WM_LBUTTONUP)
//			{
//				//INPUT_SYSTEM.mouseUp();
//			}
//			else if (CurrentAction->wParam == WM_RBUTTONUP)
//			{
//				INPUT_SYSTEM.mouseUp(false);
//			}
//			else if (CurrentAction->wParam == WM_LBUTTONDOWN)
//			{
//				//INPUT_SYSTEM.mouseDown();
//			}
//			else if (CurrentAction->wParam == WM_RBUTTONDOWN)
//			{
//				INPUT_SYSTEM.mouseDown(false);
//			}
//			else if (CurrentAction->wParam == WM_MOUSEWHEEL)
//			{
//				INPUT_SYSTEM.mouseWheel((short)HIWORD(CurrentAction->additionalInfo.mouseData));
//			}
//		}
//		/*else if (Actions[i]->GetType() == FETP_LUNCH_APPLICATION_ACTION)
//		{
//			LunchApplicationAction* CurrentAction = reinterpret_cast<LunchApplicationAction*>(Actions[i]);
//			if (!FocalEngine::FILE_SYSTEM.checkFile(CurrentAction->applicationPath.c_str()))
//			{
//				currentTestResult->failReason = FE_TEST_FAIL_CANT_FIND_FILE;
//				currentTestResult->failedAction = CurrentAction;
//				return false;
//			}
//
//			ShellExecuteA(NULL, NULL, CurrentAction->applicationPath.c_str(), NULL, FocalEngine::FILE_SYSTEM.getDirectoryPath(CurrentAction->applicationPath.c_str()), SW_NORMAL);
//		}*/
//		/*else if (actions[i]->GetType() == FETP_SLEEP_ACTION)
//		{
//			SleepAction* action = reinterpret_cast<SleepAction*>(actions[i]);
//			Sleep(DWORD(action->sleepFor * currentlyRunning->getSpeedFactor()));
//		}*/
//	}
//
//	return true;
//}

bool FEPTActionSystem::DoesNodeAreaHaveProblematicAction(std::string NodeAreaID)
{
	if (NodeAreaIDToHadProblematicAction.find(NodeAreaID) != NodeAreaIDToHadProblematicAction.end())
		return NodeAreaIDToHadProblematicAction[NodeAreaID];

	return false;
}