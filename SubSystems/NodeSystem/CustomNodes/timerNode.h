#pragma once

#include "../VisualNodeSystem/VisualNodeSystem.h"
#include "basicLogicNode.h"

class timerNode : public basicLogicNode
{
	friend class NodeFactory;
	static bool isRegistered;

	bool CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser);
	void SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, VISUAL_NODE_SOCKET_EVENT EventType);

	int Data = 0;

	std::function<void* ()> IntDataGetter = [this]() -> void* {
		return &Data;
	};

	//class TimerLeftSocket : public NodeSocket
	//{
	//public:
	//	TimerLeftSocket(VisualNode* Parent, std::string Type, std::string Name, bool bOutput = false) : NodeSocket(Parent, Type, Name, bOutput) {};

	//	void* GetData() override
	//	{
	//		// To update timer.
	//		reinterpret_cast<timerNode*>(Parent)->GetTimeLeft();
	//		return reinterpret_cast<int*>(&reinterpret_cast<timerNode*>(Parent)->Data);
	//	}
	//};

public:
	timerNode();
	timerNode(const timerNode& Src);

	Json::Value ToJson();
	void FromJson(Json::Value Json);

	void Draw();

	int GetTimeLeft();
	void SetTimeLeft(int TimeInMS);

	basicLogicNode* GetNextNode();
};