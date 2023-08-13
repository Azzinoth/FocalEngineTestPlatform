#pragma once

#include "basicLogicNode.h"

class beginNode : public basicLogicNode
{
	friend class NodeFactory;
	static bool isRegistered;

	float Data = 0.0f;

	bool CanConnect(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* CandidateSocket, char** MsgToUser);
	void SocketEvent(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* ConnectedSocket, VisNodeSys::NODE_SOCKET_EVENT EventType);

	FETPImage* Icon = nullptr;
public:
	beginNode();
	beginNode(const beginNode& Src);

	void Draw();
	float GetData();

	basicLogicNode* GetNextNode();
};