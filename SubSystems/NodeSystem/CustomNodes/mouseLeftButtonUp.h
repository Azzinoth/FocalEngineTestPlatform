#pragma once

#include "BasicLogicNode.h"

class MouseLeftButtonUp : public BasicLogicNode
{
	friend class NodeFactory;
	static bool bIsRegistered;

	bool CanConnect(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* CandidateSocket, char** MsgToUser);
	void SocketEvent(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* ConnectedSocket, VisNodeSys::NODE_SOCKET_EVENT EventType);
public:
	MouseLeftButtonUp();
	MouseLeftButtonUp(const MouseLeftButtonUp& Other);

	void Draw();

	BasicLogicNode* GetNextNode();
};