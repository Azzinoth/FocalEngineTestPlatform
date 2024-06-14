#pragma once

#include "beginNode.h"

class regionNode;
class endNode : public basicLogicNode
{
	friend class NodeFactory;
	static bool isRegistered;

	friend class regionNode;

	float Data = 0.0f;
	
	bool CanConnect(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* CandidateSocket, char** MsgToUser);
	void SocketEvent(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* ConnectedSocket, VisNodeSys::NODE_SOCKET_EVENT EventType);

	FETPImage* Icon = nullptr;
	basicLogicNode* NextNode = nullptr;
public:
	endNode();
	endNode(const endNode& Src);

	void Draw();
	float GetData();

	basicLogicNode* GetNextNode();
};