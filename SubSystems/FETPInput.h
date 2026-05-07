#pragma once

#include "FETPScreen.h"

struct KeyboardAction : public FETPAction
{
	int HookCode = 0;
	WPARAM EventType;
	KBDLLHOOKSTRUCT HookInfo;
	bool bShiftPressed = false;
	bool bCapsLockActivated = false;

	KeyboardAction() : FETPAction(FETP_KEYBOARD_ACTION)
	{
		this->EventType = 0;
		ZeroMemory(&HookInfo, sizeof(KBDLLHOOKSTRUCT));
		//GetKeyboardState
		// https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-getkeyboardstate
	}

	KeyboardAction(int HookCode, WPARAM EventType, LPARAM EventDataPointer, bool bShiftPressed, bool bCapsLockActivated) : FETPAction(FETP_KEYBOARD_ACTION)
	{
		ZeroMemory(&HookInfo, sizeof(KBDLLHOOKSTRUCT));

		this->HookCode = HookCode;
		this->EventType = EventType;
		this->bShiftPressed = bShiftPressed;
		this->bCapsLockActivated = bCapsLockActivated;

		if (EventDataPointer != 0)
		{
			this->HookInfo = *reinterpret_cast<KBDLLHOOKSTRUCT*>(EventDataPointer);
			Time = this->HookInfo.time;
		}
	}

	KeyboardAction(const KeyboardAction& Other) : FETPAction(FETP_KEYBOARD_ACTION)
	{
		ZeroMemory(&HookInfo, sizeof(KBDLLHOOKSTRUCT));

		this->HookCode = Other.HookCode;
		this->EventType = Other.EventType;
		this->bShiftPressed = Other.bShiftPressed;
		this->bCapsLockActivated = Other.bCapsLockActivated;

		this->HookInfo = Other.HookInfo;
		Time = this->HookInfo.time;
	}

	Json::Value ToJson()
	{
		Json::Value Result = FETPAction::ToJson();

		Result["nCode"] = HookCode;
		Result["wParam"] = EventType;

		Result["shiftPressed"] = bShiftPressed;
		Result["capsLockActivated"] = bCapsLockActivated;

		Result["additionalInfo"]["dwExtraInfo"] = long long(HookInfo.dwExtraInfo);
		Result["additionalInfo"]["flags"] = unsigned int(HookInfo.flags);
		Result["additionalInfo"]["vkCode"] = unsigned int(HookInfo.vkCode);
		Result["additionalInfo"]["scanCode"] = unsigned int(HookInfo.scanCode);

		return Result;
	}

	void FromJson(Json::Value Json)
	{
		FETPAction::FromJson(Json);

		HookCode = Json["nCode"].asInt();
		EventType = Json["wParam"].asInt();

		bShiftPressed = Json["shiftPressed"].asBool();
		bCapsLockActivated = Json["capsLockActivated"].asBool();
		
		HookInfo.dwExtraInfo = Json["additionalInfo"]["dwExtraInfo"].asUInt();
		HookInfo.flags = Json["additionalInfo"]["flags"].asUInt();
		HookInfo.vkCode = Json["additionalInfo"]["vkCode"].asUInt();
		HookInfo.scanCode = Json["additionalInfo"]["scanCode"].asUInt();
	}
};

struct MouseAction : public FETPAction
{
	int HookCode;
	WPARAM EventType;
	MSLLHOOKSTRUCT HookInfo;
	size_t ScreenWidth;
	size_t ScreenHeight;

	MouseAction() : FETPAction(FETP_MOUSE_ACTION)
	{
		Time = 0;
		HookCode = 0;
		EventType = WM_MOUSEMOVE;

		ZeroMemory(&this->HookInfo, sizeof(MSLLHOOKSTRUCT));

		ScreenWidth = SCREEN_SYSTEM.GetScreenWidth();
		ScreenHeight = SCREEN_SYSTEM.GetScreenHeight();
	}

	MouseAction(int HookCode, WPARAM EventType, LPARAM EventDataPointer) : FETPAction(FETP_MOUSE_ACTION)
	{
		this->HookCode = HookCode;
		this->EventType = EventType;

		if (EventDataPointer != 0)
		{
			this->HookInfo = *reinterpret_cast<MSLLHOOKSTRUCT*>(EventDataPointer);
			Time = this->HookInfo.time;
		}

		ScreenWidth = SCREEN_SYSTEM.GetScreenWidth();
		ScreenHeight = SCREEN_SYSTEM.GetScreenHeight();
	}

	MouseAction(const MouseAction& Other) : FETPAction(FETP_MOUSE_ACTION)
	{
		HookCode = Other.HookCode;
		EventType = Other.EventType;
		ScreenWidth = Other.ScreenWidth;
		ScreenHeight = Other.ScreenHeight;

		HookInfo = Other.HookInfo;
		Time = HookInfo.time;
	}

	Json::Value ToJson()
	{
		Json::Value Result = FETPAction::ToJson();

		Result["nCode"] = HookCode;
		Result["wParam"] = EventType;

		Result["screenWidht"] = ScreenWidth;
		Result["screenHeight"] = ScreenHeight;

		Result["additionalInfo"]["dwExtraInfo"] = long long(HookInfo.dwExtraInfo);
		Result["additionalInfo"]["flags"] = unsigned int(HookInfo.flags);
		Result["additionalInfo"]["mouseData"] = unsigned int(HookInfo.mouseData);
		Result["additionalInfo"]["pt"]["x"] = unsigned int(HookInfo.pt.x);
		Result["additionalInfo"]["pt"]["y"] = unsigned int(HookInfo.pt.y);

		return Result;
	}

	void FromJson(Json::Value Json)
	{
		FETPAction::FromJson(Json);

		HookCode = Json["nCode"].asInt();
		EventType = Json["wParam"].asInt();

		ScreenWidth = Json["screenWidth"].asInt();
		ScreenHeight = Json["screenHeight"].asInt();

		HookInfo.dwExtraInfo = Json["additionalInfo"]["dwExtraInfo"].asUInt();
		HookInfo.flags = Json["additionalInfo"]["flags"].asUInt();
		HookInfo.mouseData = Json["additionalInfo"]["mouseData"].asUInt();
		HookInfo.pt.x = Json["additionalInfo"]["pt"]["x"].asUInt();
		HookInfo.pt.y = Json["additionalInfo"]["pt"]["y"].asUInt();
	}
};

class FETPInput
{
public:
	SINGLETON_PUBLIC_PART(FETPInput)

	void Initialize();

	void SimulateMouseMoveTo(int X, int Y, int MonitorIndex = -1);
	void SimulateMouseDown(bool bLeftButton = true);
	void SimulateMouseUp(bool bLeftButton = true);

	void SimulateMouseWheel(short WheelRotationDelta);

	void SimulateKeyEvent(WPARAM Type, DWORD VirtualKeyCode);
	void SimulateTextInput(std::string Text, int AverageDelay = 10);

	void SetGlobalKeyboardCallback(std::function<void(KeyboardAction keyAction)> Function);
	void SetGlobalMouseCallback(std::function<void(MouseAction mouseAction)> Function);

	void Update();

	char GetCharFromAction(KeyboardAction* Action);
private:
	SINGLETON_PRIVATE_PART(FETPInput)

	static HHOOK KeyboardHookHandle;
	static std::vector<KeyboardAction> KeyboardActionBuffer;
	static std::mutex KeyboardBufferMutex;

	static HHOOK MouseHookHandle;
	static std::vector<MouseAction> MouseActionBuffer;
	static std::mutex MouseBufferMutex;

	static LRESULT CALLBACK ProcessKeyboardHookEvent(int HookCode, WPARAM EventType, LPARAM EventDataPointer);
	static LRESULT CALLBACK ProcessMouseHookEvent(int HookCode, WPARAM EventType, LPARAM EventDataPointer);

	void ProcessBufferedActions();

	std::function<void(KeyboardAction keyAction)> ClientGlobalKeyboardCallback = nullptr;
	std::function<void(MouseAction keyAction)> ClientGlobalMouseCallback = nullptr;
};

#define INPUT_SYSTEM FETPInput::GetInstance()