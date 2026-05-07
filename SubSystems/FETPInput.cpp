#include "FETPInput.h"

HHOOK FETPInput::KeyboardHookHandle = nullptr;
std::vector<KeyboardAction> FETPInput::KeyboardActionBuffer;
std::mutex FETPInput::KeyboardBufferMutex;

HHOOK FETPInput::MouseHookHandle = nullptr;
std::vector<MouseAction> FETPInput::MouseActionBuffer;
std::mutex FETPInput::MouseBufferMutex;

FETPInput::FETPInput() {};

void FETPInput::Initialize()
{
	static int InitializationCounter = 0;
	InitializationCounter++;
	if (InitializationCounter > 1)
		return;

	/*std::thread([=]()
	{
		KeyboardHookHandle = SetWindowsHookExA(WH_KEYBOARD_LL, FETPInput::ProcessKeyboardHookEvent, GetModuleHandle(0), 0);
		MouseHookHandle = SetWindowsHookExA(WH_MOUSE_LL, FETPInput::ProcessMouseHookEvent, GetModuleHandle(0), 0);

		MSG Message;
		while (GetMessageA(&Message, NULL, 0, 0))
		{
			TranslateMessage(&Message);
			DispatchMessage(&Message);
		}
	}).detach();*/
}

void FETPInput::SimulateMouseMoveTo(int X, int Y, int MonitorIndex)
{
	std::vector<FocalEngine::MonitorInfo> Monitors = FocalEngine::APPLICATION.GetMonitors();
	if (Monitors.size() == 0)
		return;

	if (MonitorIndex < 0)
		MonitorIndex = 0;

	if (MonitorIndex >= Monitors.size())
		MonitorIndex = 0;

	SetCursorPos(Monitors[MonitorIndex].VirtualX + X, Monitors[MonitorIndex].VirtualY + Y);
}

void FETPInput::SimulateMouseDown(bool bLeftButton)
{
	INPUT Inputs;
	ZeroMemory(&Inputs, sizeof(Inputs));

	Inputs.type = INPUT_MOUSE;
	Inputs.mi.dx = 0;
	Inputs.mi.dy = 0;
	bLeftButton ? Inputs.mi.dwFlags = MOUSEEVENTF_LEFTDOWN : Inputs.mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;

	SendInput(1, &Inputs, sizeof(INPUT));
}

void FETPInput::SimulateMouseUp(bool bLeftButton)
{
	INPUT Inputs;
	ZeroMemory(&Inputs, sizeof(Inputs));

	Inputs.type = INPUT_MOUSE;
	Inputs.mi.dx = 0;
	Inputs.mi.dy = 0;
	bLeftButton ? Inputs.mi.dwFlags = MOUSEEVENTF_LEFTUP : Inputs.mi.dwFlags = MOUSEEVENTF_RIGHTUP;

	SendInput(1, &Inputs, sizeof(INPUT));
}

void FETPInput::SimulateMouseWheel(short WheelRotationDelta)
{
	INPUT Inputs[1] = { 0 };

	Inputs[0].type = INPUT_MOUSE;
	Inputs[0].mi.mouseData = WheelRotationDelta;
	Inputs[0].mi.dwFlags = MOUSEEVENTF_WHEEL;

	SendInput(1, Inputs, sizeof(INPUT));
}

LRESULT CALLBACK FETPInput::ProcessKeyboardHookEvent(int HookCode, WPARAM EventType, LPARAM EventDataPointer)
{
	if (HookCode != HC_ACTION)
		return CallNextHookEx(KeyboardHookHandle, HookCode, EventType, EventDataPointer);

	bool bShiftPressed = HIWORD(GetKeyState(VK_SHIFT)) || HIWORD(GetKeyState(VK_LSHIFT)) || HIWORD(GetKeyState(VK_RSHIFT));
	bool bCapsLockActivated = LOWORD(GetKeyState(VK_CAPITAL));
	GetKeyState(VK_CAPITAL);

	KeyboardBufferMutex.lock();
	KeyboardActionBuffer.push_back(KeyboardAction(HookCode, EventType, EventDataPointer, bShiftPressed, bCapsLockActivated));
	KeyboardBufferMutex.unlock();

	return CallNextHookEx(KeyboardHookHandle, HookCode, EventType, EventDataPointer);
}

LRESULT CALLBACK FETPInput::ProcessMouseHookEvent(int HookCode, WPARAM EventType, LPARAM EventDataPointer)
{
	if (HookCode != HC_ACTION)
		return CallNextHookEx(MouseHookHandle, HookCode, EventType, EventDataPointer);

	MouseBufferMutex.lock();
	MouseActionBuffer.push_back(MouseAction(HookCode, EventType, EventDataPointer));
	MouseBufferMutex.unlock();

	return CallNextHookEx(MouseHookHandle, HookCode, EventType, EventDataPointer);
}

void FETPInput::ProcessBufferedActions()
{
	KeyboardBufferMutex.lock();
	if (ClientGlobalKeyboardCallback != nullptr)
	{
		for (size_t i = 0; i < KeyboardActionBuffer.size(); i++)
		{
			ClientGlobalKeyboardCallback(KeyboardActionBuffer[i]);
		}
	}
	KeyboardActionBuffer.clear();
	KeyboardBufferMutex.unlock();

	MouseBufferMutex.lock();
	if (ClientGlobalMouseCallback != nullptr)
	{
		for (size_t i = 0; i < MouseActionBuffer.size(); i++)
		{
			ClientGlobalMouseCallback(MouseActionBuffer[i]);
		}
	}
	MouseActionBuffer.clear();
	MouseBufferMutex.unlock();
}

void FETPInput::Update()
{
	ProcessBufferedActions();
}

void FETPInput::SetGlobalKeyboardCallback(std::function<void(KeyboardAction keyAction)> Function)
{
	ClientGlobalKeyboardCallback = Function;
}

void FETPInput::SetGlobalMouseCallback(std::function<void(MouseAction mouseAction)> Function)
{
	ClientGlobalMouseCallback = Function;
}

void FETPInput::SimulateKeyEvent(WPARAM Type, DWORD VirtualKeyCode)
{
	INPUT Inputs;
	ZeroMemory(&Inputs, sizeof(Inputs));

	Inputs.type = INPUT_KEYBOARD;

	Inputs.ki.wScan = MapVirtualKeyEx(VirtualKeyCode, 0, GetKeyboardLayout(0));
	Inputs.ki.time = 0;
	Inputs.ki.dwExtraInfo = 0;
	Inputs.ki.wVk = WORD(VirtualKeyCode);

	Inputs.ki.dwFlags = (Type == WM_KEYUP || Type == WM_SYSKEYUP) ? KEYEVENTF_KEYUP : 0;

	//https://stackoverflow.com/questions/21197257/keybd-event-keyeventf-extendedkey-explanation-required
	if (VirtualKeyCode == 46)
		Inputs.ki.dwFlags |= KEYEVENTF_EXTENDEDKEY;

	unsigned int Result = SendInput(1, &Inputs, sizeof(INPUT));
}

char FETPInput::GetCharFromAction(KeyboardAction* Action)
{
	if (Action->EventType == WM_KEYDOWN || Action->EventType == WM_SYSKEYDOWN ||
		Action->EventType == WM_KEYUP || Action->EventType == WM_SYSKEYUP)
	{
		WORD TemporaryWord = 0;
		BYTE KeyboardState[256];

		for (size_t i = 0; i < 256; i++)
		{
			KeyboardState[i] = 0;
		}

		if (Action->bShiftPressed)
			KeyboardState[VK_SHIFT] = 0x80;

		if (Action->bCapsLockActivated)
			KeyboardState[VK_CAPITAL] = -127;

		ToAsciiEx(Action->HookInfo.vkCode, MapVirtualKeyA(Action->HookInfo.vkCode, 0), KeyboardState, &TemporaryWord, 0, GetKeyboardLayout(0));
		return char(TemporaryWord);
	}

	return 0;
}

void FETPInput::SimulateTextInput(std::string Text, int AverageDelay)
{
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
			//KeyboardAction* NewAction = new KeyboardAction();
			//NewAction->HookInfo.vkCode = 0x10;
			//NewAction->EventType = WM_KEYDOWN;
			//NewAction->bShiftPressed = false;
			//Result.push_back(NewAction);

			SimulateKeyEvent(WM_KEYDOWN, 0x10);
		}

		/*KeyboardAction* NewAction = new KeyboardAction();
		NewAction->HookInfo.vkCode = VirtualKeyCode;
		NewAction->EventType = WM_KEYDOWN;
		NewAction->bShiftPressed = KeysState & 1;
		Result.push_back(NewAction);*/

		SimulateKeyEvent(WM_KEYDOWN, VirtualKeyCode);

		/*SleepAction* NewSleepAction = new SleepAction(AverageDelay);
		Result.push_back(NewSleepAction);*/
		Sleep(AverageDelay);

		/*NewAction = new KeyboardAction();
		NewAction->HookInfo.vkCode = VirtualKeyCode;
		NewAction->EventType = WM_KEYUP;
		NewAction->bShiftPressed = KeysState & 1;
		Result.push_back(NewAction);*/
		SimulateKeyEvent(WM_KEYUP, VirtualKeyCode);

		if (KeysState & 1)
		{
			/*KeyboardAction* NewAction = new KeyboardAction();
			NewAction->HookInfo.vkCode = 0x10;
			NewAction->EventType = WM_KEYUP;
			NewAction->bShiftPressed = false;
			Result.push_back(NewAction);*/
			SimulateKeyEvent(WM_KEYUP, 0x10);
		}
	}
}