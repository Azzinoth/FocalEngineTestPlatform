#include "FETPAction.h"
using namespace FocalEngine;

FETPAction::FETPAction(FETP_ACTION_TYPE type)
{
	internalType = type;
	time = 0;
	ID = APPLICATION.GetUniqueHexID();
}

FETPAction::FETPAction(const FETPAction& src)
{
	time = src.time;
	internalType = src.internalType;
	ID = src.ID;
}

FETPAction::~FETPAction()
{
}

FETP_ACTION_TYPE FETPAction::getType()
{
	return internalType;
}

DWORD FETPAction::getTimeStamp()
{
	return time;
}

Json::Value FETPAction::toJson()
{
	Json::Value result;

	result["ID"] = ID;
	result["internalType"] = internalType;
	result["time"] = unsigned int(time);

	return result;
}

std::string FETPAction::getID()
{
	return ID;
}

void FETPAction::fromJson(Json::Value json)
{
	ID = json["ID"].asCString();
	//FETPAction::seenIDs[ID] = true;

	internalType = FETP_ACTION_TYPE(json["internalType"].asInt());
	time = json["time"].asUInt();
}

void FETPAction::setID(std::string newID)
{
	ID = newID;
	//FETPAction::seenIDs[ID] = true;
}

//bool FETPAction::wasIDseen(std::string ID)
//{
//	if (seenIDs.find(ID) != seenIDs.end())
//		return true;
//
//	return false;
//}