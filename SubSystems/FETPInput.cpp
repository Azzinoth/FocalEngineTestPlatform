#include "FETPInput.h"

HHOOK FETPInput::keyboardHookPTR = nullptr;
std::vector<KeyboardAction> FETPInput::tempKeyboarActions;
std::mutex FETPInput::keyboarActionsMutex;

HHOOK FETPInput::mouseHookPTR = nullptr;
std::vector<MouseAction> FETPInput::tempMouseActions;
std::mutex FETPInput::mouseActionsMutex;

FETPInput::FETPInput() {};

void FETPInput::initialize()
{
	static int counter = 0;
	counter++;
	if (counter > 1)
		return;

	/*std::thread([=]()
	{
		keyboardHookPTR = SetWindowsHookExA(WH_KEYBOARD_LL, FETPInput::keyboardHook, GetModuleHandle(0), 0);
		mouseHookPTR = SetWindowsHookExA(WH_MOUSE_LL, FETPInput::mouseHook, GetModuleHandle(0), 0);

		MSG msg;
		while (GetMessageA(&msg, NULL, 0, 0))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}).detach();*/
}

void FETPInput::mouseMoveTo(int x, int y)
{
	SetCursorPos(x, y);
}

void FETPInput::mouseDown(bool leftbutton)
{
	INPUT Inputs;
	ZeroMemory(&Inputs, sizeof(Inputs));

	Inputs.type = INPUT_MOUSE;
	Inputs.mi.dx = 0;
	Inputs.mi.dy = 0;
	leftbutton ? Inputs.mi.dwFlags = MOUSEEVENTF_LEFTDOWN : Inputs.mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;

	SendInput(1, &Inputs, sizeof(INPUT));
}

void FETPInput::mouseUp(bool leftbutton)
{
	INPUT Inputs;
	ZeroMemory(&Inputs, sizeof(Inputs));

	Inputs.type = INPUT_MOUSE;
	Inputs.mi.dx = 0;
	Inputs.mi.dy = 0;
	leftbutton ? Inputs.mi.dwFlags = MOUSEEVENTF_LEFTUP : Inputs.mi.dwFlags = MOUSEEVENTF_RIGHTUP;

	SendInput(1, &Inputs, sizeof(INPUT));
}

void FETPInput::mouseWheel(short wheelRotation)
{
	INPUT Inputs[1] = { 0 };

	Inputs[0].type = INPUT_MOUSE;
	Inputs[0].mi.mouseData = wheelRotation;
	Inputs[0].mi.dwFlags = MOUSEEVENTF_WHEEL;

	SendInput(1, Inputs, sizeof(INPUT));
}

LRESULT CALLBACK FETPInput::keyboardHook(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode != HC_ACTION)
		return CallNextHookEx(keyboardHookPTR, nCode, wParam, lParam);

	bool shiftPressed = HIWORD(GetKeyState(VK_SHIFT)) || HIWORD(GetKeyState(VK_LSHIFT)) || HIWORD(GetKeyState(VK_RSHIFT));
	bool capsLockActivated = LOWORD(GetKeyState(VK_CAPITAL));

	auto test = GetKeyState(VK_CAPITAL);
	if (capsLockActivated)
	{
		int y = 0;
		y++;
	}

	keyboarActionsMutex.lock();
	tempKeyboarActions.push_back(KeyboardAction(nCode, wParam, lParam, shiftPressed, capsLockActivated));
	keyboarActionsMutex.unlock();

	return CallNextHookEx(keyboardHookPTR, nCode, wParam, lParam);
}


LRESULT CALLBACK FETPInput::mouseHook(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode != HC_ACTION)
		return CallNextHookEx(mouseHookPTR, nCode, wParam, lParam);

	mouseActionsMutex.lock();
	tempMouseActions.push_back(MouseAction(nCode, wParam, lParam));
	mouseActionsMutex.unlock();

	return CallNextHookEx(mouseHookPTR, nCode, wParam, lParam);
}

void FETPInput::updateHooks()
{
	keyboarActionsMutex.lock();
	if (clientGlobalKeyboardCallback != nullptr)
	{
		for (size_t i = 0; i < tempKeyboarActions.size(); i++)
		{
			clientGlobalKeyboardCallback(tempKeyboarActions[i]);
		}
	}
	tempKeyboarActions.clear();
	keyboarActionsMutex.unlock();


	mouseActionsMutex.lock();
	if (clientGlobalMouseCallback != nullptr)
	{
		for (size_t i = 0; i < tempMouseActions.size(); i++)
		{
			clientGlobalMouseCallback(tempMouseActions[i]);
		}
	}
	tempMouseActions.clear();
	mouseActionsMutex.unlock();
}

void FETPInput::update()
{
	updateHooks();
}

void FETPInput::setGlobalKeyboardCallback(std::function<void(KeyboardAction keyAction)> func)
{
	clientGlobalKeyboardCallback = func;
}

void FETPInput::setGlobalMouseCallback(std::function<void(MouseAction mouseAction)> func)
{
	clientGlobalMouseCallback = func;
}

void FETPInput::keyEvent(WPARAM Type, DWORD vkCode)
{
	INPUT Inputs;
	ZeroMemory(&Inputs, sizeof(Inputs));

	Inputs.type = INPUT_KEYBOARD;

	/*Inputs.ki.wScan = 0;
	Inputs.ki.time = 0;
	Inputs.ki.dwExtraInfo = 0;
	Inputs.ki.wVk = VkKeyScanEx(vkCode, GetKeyboardLayout(0));
	Inputs.mi.dwFlags = type == WM_KEYUP ? KEYEVENTF_KEYUP : 0;*/

	Inputs.ki.wScan = MapVirtualKeyEx(vkCode, 0, GetKeyboardLayout(0));
	Inputs.ki.time = 0;
	Inputs.ki.dwExtraInfo = 0;
	Inputs.ki.wVk = WORD(vkCode);

	//if (type == WM_KEYUP || type == WM_SYSKEYUP)
	//{
	//	/*INPUT Inputs_;
	//	ZeroMemory(&Inputs_, sizeof(Inputs));

	//	Inputs_.type = INPUT_KEYBOARD;
	//	Inputs_.ki.wVk = 0x44;
	//	Inputs_.ki.dwFlags = KEYEVENTF_KEYUP;
	//	SendInput(1, &Inputs_, sizeof(INPUT));*/
	//	Inputs.ki.dwFlags = KEYEVENTF_KEYUP;
	//}
	//else 
	//{
	//	Inputs.ki.dwFlags = 0;
	//}

	Inputs.ki.dwFlags = (Type == WM_KEYUP || Type == WM_SYSKEYUP) ? KEYEVENTF_KEYUP : 0;

	//https://stackoverflow.com/questions/21197257/keybd-event-keyeventf-extendedkey-explanation-required
	if (vkCode == 46)
	{
		Inputs.ki.dwFlags |= KEYEVENTF_EXTENDEDKEY;
	}

	UINT result = SendInput(1, &Inputs, sizeof(INPUT));
}

char FETPInput::getChar(KeyboardAction* action)
{
	if (action->wParam == WM_KEYDOWN || action->wParam == WM_SYSKEYDOWN ||
		action->wParam == WM_KEYUP || action->wParam == WM_SYSKEYUP)
	{
		WORD tempWord = 0;
		BYTE keyboard_state[256];

		for (size_t i = 0; i < 256; i++)
		{
			keyboard_state[i] = 0;
		}

		if (action->shiftPressed)
			keyboard_state[VK_SHIFT] = 0x80;

		if (action->capsLockActivated)
			keyboard_state[VK_CAPITAL] = -127;

		ToAsciiEx(action->additionalInfo.vkCode, MapVirtualKeyA(action->additionalInfo.vkCode, 0), keyboard_state, &tempWord, 0, GetKeyboardLayout(0));
		return char(tempWord);
	}

	return 0;
}