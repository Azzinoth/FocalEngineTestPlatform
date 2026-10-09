#include "FETPAction.h"
using namespace FocalEngine;

FETPAction::FETPAction(FETP_ACTION_TYPE Type)
{
	InternalType = Type;
	Time = 0;
	ID = UNIQUE_ID.GenerateID();
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
	Json::Value Result;

	Result["ID"] = UNIQUE_ID.ToString(ID);
	Result["internalType"] = InternalType;
	Result["time"] = unsigned int(Time);

	return Result;
}

FEUUID FETPAction::GetID()
{
	return ID;
}

void FETPAction::FromJson(Json::Value JsonData)
{
	ID = UNIQUE_ID.FromStringLegacyCompatible(JsonData["ID"].asString());

	InternalType = FETP_ACTION_TYPE(JsonData["internalType"].asInt());
	Time = JsonData["time"].asUInt();
}

void FETPAction::SetID(const FEUUID& NewID)
{
	ID = NewID;
}