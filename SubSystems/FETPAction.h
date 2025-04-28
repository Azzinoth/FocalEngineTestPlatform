#pragma once

#include "FECoreIncludes.h"

enum FETP_ACTION_TYPE
{
	FETP_BASE_ACTION = 0,
	FETP_KEYBOARD_ACTION = 1,
	FETP_MOUSE_ACTION = 2,
	FETP_SCREENSHOT_COMPARE_ACTION = 3,
	FETP_LAUNCH_APPLICATION_ACTION = 4,
	FETP_SLEEP_ACTION = 5
};

enum FETP_COMBINED_ACTION_TYPE
{
	FETP_COMBINED_MOUSE_MOVE_ACTION = 0,
	FETP_COMBINED_LEFT_MOUSE_ACTION = 1,
	FETP_COMBINED_RIGHT_MOUSE_ACTION = 2,
	FETP_COMBINED_WHEEL_MOUSE_ACTION = 3,
	FETP_COMBINED_KEY_PRESS_ACTION = 4,
	FETP_COMBINED_TEXT_INPUT_ACTION = 5
};

class GlobalActionNode;
class FETest;

class FETPAction
{
	friend class Node;
	friend FETest;

	void SetID(std::string NewID);
public:
	FETPAction(FETP_ACTION_TYPE Type = FETP_BASE_ACTION);
	FETPAction(const FETPAction& Other);

	virtual ~FETPAction();

	std::string GetID();

	FETP_ACTION_TYPE GetType();
	// FIX ME! Change it to use uint64_t and FETime.
	DWORD GetTimeStamp();
	DWORD Time;

	virtual Json::Value ToJson();
	virtual void FromJson(Json::Value JsonData);

	static std::string FETPActionTypeToString(FETP_ACTION_TYPE Type)
	{
		if (Type == FETP_BASE_ACTION)
		{
			return "base";
		}
		else if (Type == FETP_KEYBOARD_ACTION)
		{
			return "Keyboard";
		}
		else if (Type == FETP_MOUSE_ACTION)
		{
			return "Mouse";
		}
		else if (Type == FETP_SCREENSHOT_COMPARE_ACTION)
		{
			return "Screenshot";
		}
		else if (Type == FETP_LAUNCH_APPLICATION_ACTION)
		{
			return "Launch application";
		}
		else if (Type == FETP_SLEEP_ACTION)
		{
			return "Sleep";
		}

		return "None";
	}
protected:
	FETP_ACTION_TYPE InternalType;
	std::string ID;
};