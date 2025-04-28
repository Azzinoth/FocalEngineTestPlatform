#pragma once

#include "BasicLogicNode.h"

class TimerNode : public BasicLogicNode
{
	friend class NodeFactory;
	static bool bIsRegistered;

	bool CanConnect(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* CandidateSocket, char** MsgToUser);
	void SocketEvent(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* ConnectedSocket, VisNodeSys::NODE_SOCKET_EVENT EventType);

	int Data = 0;

	std::function<void* ()> IntDataGetter = [this]() -> void* {
		return &Data;
	};

public:
	TimerNode();
	TimerNode(const TimerNode& Other);

	Json::Value ToJson();
	bool FromJson(Json::Value Json);

	void Draw();

	int GetTimeLeft();
	void SetTimeLeft(int TimeInMS);

	BasicLogicNode* GetNextNode();
};