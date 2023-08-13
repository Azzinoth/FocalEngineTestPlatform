#pragma once

#include "basicLogicNode.h"

class mouseLeftButtonUp : public basicLogicNode
{
	friend class NodeFactory;
	static bool isRegistered;

	bool CanConnect(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* CandidateSocket, char** MsgToUser);
	void SocketEvent(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* ConnectedSocket, VisNodeSys::NODE_SOCKET_EVENT EventType);
public:
	mouseLeftButtonUp();
	mouseLeftButtonUp(const mouseLeftButtonUp& Src);

	void Draw();

	basicLogicNode* GetNextNode();
};