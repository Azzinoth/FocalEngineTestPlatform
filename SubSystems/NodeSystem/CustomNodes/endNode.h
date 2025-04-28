#pragma once

#include "BeginNode.h"

class RegionNode;
class EndNode : public BasicLogicNode
{
	friend class NodeFactory;
	static bool bIsRegistered;

	friend class RegionNode;

	float Data = 0.0f;
	
	bool CanConnect(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* CandidateSocket, char** MsgToUser);
	void SocketEvent(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* ConnectedSocket, VisNodeSys::NODE_SOCKET_EVENT EventType);

	FETPImage* Icon = nullptr;
	BasicLogicNode* NextNode = nullptr;
public:
	EndNode();
	EndNode(const EndNode& Other);

	void Draw();
	float GetData();

	BasicLogicNode* GetNextNode();
};