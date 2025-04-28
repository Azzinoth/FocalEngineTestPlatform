#pragma once

#include "BasicLogicNode.h"

class SleepNode : public BasicLogicNode
{
	friend class NodeFactory;
	static bool bIsRegistered;

	int SleepFor = 0;

	bool CanConnect(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* CandidateSocket, char** MsgToUser);
	void SocketEvent(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* ConnectedSocket, VisNodeSys::NODE_SOCKET_EVENT EventType);
public:
	SleepNode();
	SleepNode(const SleepNode& Other);

	Json::Value ToJson();
	bool FromJson(Json::Value Json);

	void Draw();

	BasicLogicNode* GetNextNode();
};