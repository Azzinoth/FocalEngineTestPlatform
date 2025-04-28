#pragma once
#include "BasicLogicNode.h"

class BranchNode : public BasicLogicNode
{
	friend class NodeFactory;
	static bool bIsRegistered;

	bool CanConnect(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* CandidateSocket, char** MsgToUser);
	void SocketEvent(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* ConnectedSocket, VisNodeSys::NODE_SOCKET_EVENT EventType);
public:
	BranchNode();
	BranchNode(const BranchNode& Other);

	void Draw();

	BasicLogicNode* GetNextNode();
};