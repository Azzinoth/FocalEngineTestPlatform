#include "FETPAction.h"
using namespace FocalEngine;

FETPAction::FETPAction(FETP_ACTION_TYPE Type)
{
	InternalType = Type;
	Time = 0;
	ID = APPLICATION.GetUniqueHexID();
}

FETPAction::FETPAction(const FETPAction& Other)
{
	Time = Other.Time;
	InternalType = Other.InternalType;
	ID = Other.ID;
}

FETPAction::~FETPAction()
{
}

FETP_ACTION_TYPE FETPAction::GetType()
{
	return InternalType;
}

DWORD FETPAction::GetTimeStamp()
{
	return Time;
}

Json::Value FETPAction::ToJson()
{
	Json::Value result;

	result["ID"] = ID;
	result["internalType"] = InternalType;
	result["time"] = unsigned int(Time);

	return result;
}

std::string FETPAction::GetID()
{
	return ID;
}

void FETPAction::FromJson(Json::Value JsonData)
{
	ID = JsonData["ID"].asCString();

	InternalType = FETP_ACTION_TYPE(JsonData["internalType"].asInt());
	Time = JsonData["time"].asUInt();
}

void FETPAction::SetID(std::string NewID)
{
	ID = NewID;
}