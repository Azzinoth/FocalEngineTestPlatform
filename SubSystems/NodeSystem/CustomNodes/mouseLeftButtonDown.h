#pragma once

#include "basicLogicNode.h"

class mouseLeftButtonDown : public basicLogicNode
{
	friend class NodeFactory;
	static bool isRegistered;

	bool CanConnect(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* CandidateSocket, char** MsgToUser);
	void SocketEvent(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* ConnectedSocket, VisNodeSys::NODE_SOCKET_EVENT EventType);
public:
	mouseLeftButtonDown();
	mouseLeftButtonDown(const mouseLeftButtonDown& Src);

	void Draw();

	basicLogicNode* GetNextNode();
};