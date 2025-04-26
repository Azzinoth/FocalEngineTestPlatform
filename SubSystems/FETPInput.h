#pragma once

#include "FETPScreen.h"

struct KeyboardAction : public FETPAction
{
	int nCode = 0;
	WPARAM wParam;
	KBDLLHOOKSTRUCT additionalInfo;
	bool shiftPressed = false;
	bool capsLockActivated = false;
	//BYTE keyboardState[256];

	KeyboardAction() : FETPAction(FETP_KEYBOARD_ACTION)
	{
		this->wParam = 0;
		ZeroMemory(&additionalInfo, sizeof(KBDLLHOOKSTRUCT));

		/*for (size_t i = 0; i < 256; i++)
		{
			keyboardState[i] = 0;
		}*/
		//GetKeyboardState
		// https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-getkeyboardstate
	}

	KeyboardAction(int nCode, WPARAM wParam, LPARAM lParam, bool shiftPressed, bool capsLockActivated) : FETPAction(FETP_KEYBOARD_ACTION)
	{
		ZeroMemory(&additionalInfo, sizeof(KBDLLHOOKSTRUCT));

		this->nCode = nCode;
		this->wParam = wParam;
		this->shiftPressed = shiftPressed;
		this->capsLockActivated = capsLockActivated;

		if (lParam != 0)
		{
			this->additionalInfo = *reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);
			Time = this->additionalInfo.time;
		}
	}

	KeyboardAction(const KeyboardAction& ref) : FETPAction(FETP_KEYBOARD_ACTION)
	{
		ZeroMemory(&additionalInfo, sizeof(KBDLLHOOKSTRUCT));

		this->nCode = ref.nCode;
		this->wParam = ref.wParam;
		this->shiftPressed = ref.shiftPressed;
		this->capsLockActivated = ref.capsLockActivated;

		this->additionalInfo = ref.additionalInfo;
		Time = this->additionalInfo.time;
	}

	Json::Value ToJson()
	{
		Json::Value Result = FETPAction::ToJson();

		Result["nCode"] = nCode;
		Result["wParam"] = wParam;

		Result["shiftPressed"] = shiftPressed;
		Result["capsLockActivated"] = capsLockActivated;

		Result["additionalInfo"]["dwExtraInfo"] = long long(additionalInfo.dwExtraInfo);
		Result["additionalInfo"]["flags"] = unsigned int(additionalInfo.flags);
		Result["additionalInfo"]["vkCode"] = unsigned int(additionalInfo.vkCode);
		Result["additionalInfo"]["scanCode"] = unsigned int(additionalInfo.scanCode);

		return Result;
	}

	void FromJson(Json::Value Json)
	{
		FETPAction::FromJson(Json);

		nCode = Json["nCode"].asInt();
		wParam = Json["wParam"].asInt();

		shiftPressed = Json["shiftPressed"].asBool();
		capsLockActivated = Json["capsLockActivated"].asBool();
		
		additionalInfo.dwExtraInfo = Json["additionalInfo"]["dwExtraInfo"].asUInt();
		additionalInfo.flags = Json["additionalInfo"]["flags"].asUInt();
		additionalInfo.vkCode = Json["additionalInfo"]["vkCode"].asUInt();
		additionalInfo.scanCode = Json["additionalInfo"]["scanCode"].asUInt();
	}
};

struct MouseAction : public FETPAction
{
	int nCode;
	WPARAM wParam;
	MSLLHOOKSTRUCT additionalInfo;
	size_t screenWidth;
	size_t screenHeight;

	MouseAction() : FETPAction(FETP_MOUSE_ACTION)
	{
		Time = 0;
		nCode = 0;
		wParam = WM_MOUSEMOVE;

		MSLLHOOKSTRUCT temp;
		ZeroMemory(&temp, sizeof(MSLLHOOKSTRUCT));
		this->additionalInfo = temp;

		screenWidth = SCREEN_SYSTEM.GetScreenWidth();
		screenHeight = SCREEN_SYSTEM.GetScreenHeight();
	}

	MouseAction(int nCode, WPARAM wParam, LPARAM lParam) : FETPAction(FETP_MOUSE_ACTION)
	{
		this->nCode = nCode;
		this->wParam = wParam;

		if (lParam != 0)
		{
			this->additionalInfo = *reinterpret_cast<MSLLHOOKSTRUCT*>(lParam);
			Time = this->additionalInfo.time;
		}

		screenWidth = SCREEN_SYSTEM.GetScreenWidth();
		screenHeight = SCREEN_SYSTEM.GetScreenHeight();
	}

	MouseAction(const MouseAction& ref) : FETPAction(FETP_MOUSE_ACTION)
	{
		nCode = ref.nCode;
		wParam = ref.wParam;
		screenWidth = ref.screenWidth;
		screenHeight = ref.screenHeight;

		additionalInfo = ref.additionalInfo;
		Time = additionalInfo.time;
	}

	Json::Value ToJson()
	{
		Json::Value result = FETPAction::ToJson();

		result["nCode"] = nCode;
		result["wParam"] = wParam;

		result["screenWidht"] = screenWidth;
		result["screenHeight"] = screenHeight;

		result["additionalInfo"]["dwExtraInfo"] = long long(additionalInfo.dwExtraInfo);
		result["additionalInfo"]["flags"] = unsigned int(additionalInfo.flags);
		result["additionalInfo"]["mouseData"] = unsigned int(additionalInfo.mouseData);
		result["additionalInfo"]["pt"]["x"] = unsigned int(additionalInfo.pt.x);
		result["additionalInfo"]["pt"]["y"] = unsigned int(additionalInfo.pt.y);

		return result;
	}

	void FromJson(Json::Value json)
	{
		FETPAction::FromJson(json);

		nCode = json["nCode"].asInt();
		wParam = json["wParam"].asInt();

		screenWidth = json["screenWidth"].asInt();
		screenHeight = json["screenHeight"].asInt();

		additionalInfo.dwExtraInfo = json["additionalInfo"]["dwExtraInfo"].asUInt();
		additionalInfo.flags = json["additionalInfo"]["flags"].asUInt();
		additionalInfo.mouseData = json["additionalInfo"]["mouseData"].asUInt();
		additionalInfo.pt.x = json["additionalInfo"]["pt"]["x"].asUInt();
		additionalInfo.pt.y = json["additionalInfo"]["pt"]["y"].asUInt();
	}
};

struct LunchApplicationAction : public FETPAction
{
	std::string applicationPath;

	LunchApplicationAction() : FETPAction(FETP_LUNCH_APPLICATION_ACTION)
	{
		applicationPath = "";
	}

	LunchApplicationAction(const LunchApplicationAction& src) : FETPAction(src)
	{
		applicationPath = src.applicationPath;
	}

	LunchApplicationAction(std::string applicationPath) : FETPAction(FETP_LUNCH_APPLICATION_ACTION)
	{
		this->applicationPath = applicationPath;
	}

	Json::Value ToJson()
	{
		Json::Value result = FETPAction::ToJson();
		result["applicationPath"] = applicationPath;
		return result;
	}

	void FromJson(Json::Value json)
	{
		FETPAction::FromJson(json);
		applicationPath = json["applicationPath"].asCString();
	}
};

struct SleepAction : public FETPAction
{
	int sleepFor;

	SleepAction() : FETPAction(FETP_SLEEP_ACTION)
	{
		sleepFor = 10;
	}

	SleepAction(int timeToSleep) : FETPAction(FETP_SLEEP_ACTION)
	{
		this->sleepFor = timeToSleep;
	}

	Json::Value ToJson()
	{
		Json::Value result = FETPAction::ToJson();
		result["sleepFor"] = sleepFor;
		return result;
	}

	void FromJson(Json::Value json)
	{
		FETPAction::FromJson(json);
		sleepFor = json["sleepFor"].asInt();
	}
};

class FETPInput
{
public:
	SINGLETON_PUBLIC_PART(FETPInput)

	void initialize();

	void mouseMoveTo(int x, int y);
	void mouseDown(bool leftbutton = true);
	void mouseUp(bool leftbutton = true);

	void mouseWheel(short wheelRotation);

	void keyEvent(WPARAM Type, DWORD vkCode);

	void setGlobalKeyboardCallback(std::function<void(KeyboardAction keyAction)> func);
	void setGlobalMouseCallback(std::function<void(MouseAction mouseAction)> func);

	void update();

	char getChar(KeyboardAction* action);
private:
	SINGLETON_PRIVATE_PART(FETPInput)

	static HHOOK keyboardHookPTR;
	static std::vector<KeyboardAction> tempKeyboarActions;
	static std::mutex keyboarActionsMutex;

	static HHOOK mouseHookPTR;
	static std::vector<MouseAction> tempMouseActions;
	static std::mutex mouseActionsMutex;

	static LRESULT CALLBACK keyboardHook(int nCode, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK mouseHook(int nCode, WPARAM wParam, LPARAM lParam);

	void updateHooks();

	std::function<void(KeyboardAction keyAction)> clientGlobalKeyboardCallback = nullptr;
	std::function<void(MouseAction keyAction)> clientGlobalMouseCallback = nullptr;
};

#define INPUT_SYSTEM FETPInput::GetInstance()