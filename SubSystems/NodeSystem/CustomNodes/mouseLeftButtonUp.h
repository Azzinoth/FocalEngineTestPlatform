#pragma once

#include "../VisualNodeSystem/VisualNodeSystem.h"
#include "basicLogicNode.h"

class mouseLeftButtonUp : public basicLogicNode
{
	friend class NodeFactory;
	static bool isRegistered;

	bool CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser);
	void SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, VISUAL_NODE_SOCKET_EVENT EventType);
public:
	mouseLeftButtonUp();
	mouseLeftButtonUp(const mouseLeftButtonUp& Src);

	void Draw();

	basicLogicNode* GetNextNode();
};