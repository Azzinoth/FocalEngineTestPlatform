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
			time = this->additionalInfo.time;
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
		time = this->additionalInfo.time;
	}

	Json::Value toJson()
	{
		Json::Value result = FETPAction::toJson();

		result["nCode"] = nCode;
		result["wParam"] = wParam;

		result["shiftPressed"] = shiftPressed;
		result["capsLockActivated"] = capsLockActivated;

		result["additionalInfo"]["dwExtraInfo"] = long long(additionalInfo.dwExtraInfo);
		result["additionalInfo"]["flags"] = unsigned int(additionalInfo.flags);
		result["additionalInfo"]["vkCode"] = unsigned int(additionalInfo.vkCode);
		result["additionalInfo"]["scanCode"] = unsigned int(additionalInfo.scanCode);

		return result;
	}

	void fromJson(Json::Value json)
	{
		FETPAction::fromJson(json);

		nCode = json["nCode"].asInt();
		wParam = json["wParam"].asInt();

		shiftPressed = json["shiftPressed"].asBool();
		capsLockActivated = json["capsLockActivated"].asBool();
		
		additionalInfo.dwExtraInfo = json["additionalInfo"]["dwExtraInfo"].asUInt();
		additionalInfo.flags = json["additionalInfo"]["flags"].asUInt();
		additionalInfo.vkCode = json["additionalInfo"]["vkCode"].asUInt();
		additionalInfo.scanCode = json["additionalInfo"]["scanCode"].asUInt();
	}
};

struct MouseAction : public FETPAction
{
	int nCode;
	WPARAM wParam;
	MSLLHOOKSTRUCT additionalInfo;
	int screenWidth;
	int screenHeight;

	MouseAction() : FETPAction(FETP_MOUSE_ACTION)
	{
		time = 0;
		nCode = 0;
		wParam = WM_MOUSEMOVE;

		MSLLHOOKSTRUCT temp;
		ZeroMemory(&temp, sizeof(MSLLHOOKSTRUCT));
		this->additionalInfo = temp;

		screenWidth = SCREEN_SYSTEM.getScreenWidth();
		screenHeight = SCREEN_SYSTEM.getScreenHeight();
	}

	MouseAction(int nCode, WPARAM wParam, LPARAM lParam) : FETPAction(FETP_MOUSE_ACTION)
	{
		this->nCode = nCode;
		this->wParam = wParam;

		if (lParam != 0)
		{
			this->additionalInfo = *reinterpret_cast<MSLLHOOKSTRUCT*>(lParam);
			time = this->additionalInfo.time;
		}

		screenWidth = SCREEN_SYSTEM.getScreenWidth();
		screenHeight = SCREEN_SYSTEM.getScreenHeight();
	}

	MouseAction(const MouseAction& ref) : FETPAction(FETP_MOUSE_ACTION)
	{
		nCode = ref.nCode;
		wParam = ref.wParam;
		screenWidth = ref.screenWidth;
		screenHeight = ref.screenHeight;

		additionalInfo = ref.additionalInfo;
		time = additionalInfo.time;
	}

	Json::Value toJson()
	{
		Json::Value result = FETPAction::toJson();

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

	void fromJson(Json::Value json)
	{
		FETPAction::fromJson(json);

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

	Json::Value toJson()
	{
		Json::Value result = FETPAction::toJson();
		result["applicationPath"] = applicationPath;
		return result;
	}

	void fromJson(Json::Value json)
	{
		FETPAction::fromJson(json);
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

	Json::Value toJson()
	{
		Json::Value result = FETPAction::toJson();
		result["sleepFor"] = sleepFor;
		return result;
	}

	void fromJson(Json::Value json)
	{
		FETPAction::fromJson(json);
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

	void keyEvent(WPARAM type, DWORD vkCode);

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

#define INPUT_SYSTEM FETPInput::getInstance()