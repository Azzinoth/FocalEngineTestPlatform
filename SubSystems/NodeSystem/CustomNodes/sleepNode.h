#pragma once

#include "../VisualNodeSystem/VisualNodeSystem.h"
#include "basicLogicNode.h"

class sleepNode : public basicLogicNode
{
	friend class NodeFactory;
	static bool isRegistered;

	int SleepFor = 0;

	bool CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser);
	void SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, VISUAL_NODE_SOCKET_EVENT EventType);
public:
	sleepNode();
	sleepNode(const sleepNode& Src);

	void Draw();

	basicLogicNode* GetNextNode();
};