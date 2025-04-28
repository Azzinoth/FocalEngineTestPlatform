#pragma once

#include "BasicLogicNode.h"

class BeginNode : public BasicLogicNode
{
	friend class NodeFactory;
	static bool bIsRegistered;

	float Data = 0.0f;

	bool CanConnect(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* CandidateSocket, char** MsgToUser);
	void SocketEvent(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* ConnectedSocket, VisNodeSys::NODE_SOCKET_EVENT EventType);

	FETPImage* Icon = nullptr;
public:
	BeginNode();
	BeginNode(const BeginNode& Other);

	void Draw();
	float GetData();

	BasicLogicNode* GetNextNode();
};