#pragma once

#include "FECoreIncludes.h"

enum FETP_ACTION_TYPE
{
	FETP_BASE_ACTION = 0,
	FETP_KEYBOARD_ACTION = 1,
	FETP_MOUSE_ACTION = 2,
	FETP_SCREENSHOOT_COMPARE_ACTION = 3,
	FETP_LUNCH_APPLICATION_ACTION = 4,
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

class globalActionNode;
class FETest;

class FETPAction
{
	friend class Node;
	friend globalActionNode;
	friend FETest;

	//static std::unordered_map<std::string, bool> seenIDs;
	void setID(std::string newID);
public:
	FETPAction(FETP_ACTION_TYPE type = FETP_BASE_ACTION);
	FETPAction(const FETPAction& src);

	virtual ~FETPAction();

	std::string getID();

	FETP_ACTION_TYPE getType();
	DWORD getTimeStamp();
	DWORD time;

	virtual Json::Value toJson();
	virtual void fromJson(Json::Value json);

	//static bool wasIDseen(std::string ID);

	static std::string FETPActionTypeToString(FETP_ACTION_TYPE type)
	{
		if (type == FETP_BASE_ACTION)
		{
			return "base";
		}
		else if (type == FETP_KEYBOARD_ACTION)
		{
			return "Keyboard";
		}
		else if (type == FETP_MOUSE_ACTION)
		{
			return "Mouse";
		}
		else if (type == FETP_SCREENSHOOT_COMPARE_ACTION)
		{
			return "Screenshoot";
		}
		else if (type == FETP_LUNCH_APPLICATION_ACTION)
		{
			return "Lunch application";
		}
		else if (type == FETP_SLEEP_ACTION)
		{
			return "Sleep";
		}

		return "None";
	}
protected:
	FETP_ACTION_TYPE internalType;
	std::string ID;
};