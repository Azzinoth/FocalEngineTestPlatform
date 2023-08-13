#pragma once

#include "basicLogicNode.h"

class timerNode : public basicLogicNode
{
	friend class NodeFactory;
	static bool isRegistered;

	bool CanConnect(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* CandidateSocket, char** MsgToUser);
	void SocketEvent(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* ConnectedSocket, VisNodeSys::NODE_SOCKET_EVENT EventType);

	int Data = 0;

	std::function<void* ()> IntDataGetter = [this]() -> void* {
		return &Data;
	};

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