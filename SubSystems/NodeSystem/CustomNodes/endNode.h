#pragma once

#include "beginNode.h"

class regionNode;
class endNode : public basicLogicNode
{
	friend class NodeFactory;
	static bool isRegistered;

	friend class regionNode;

	float Data = 0.0f;
	
	bool CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser);
	void SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, VISUAL_NODE_SOCKET_EVENT EventType);

	FETPImage* Icon = nullptr;
	basicLogicNode* NextNode = nullptr;
public:
	endNode();
	endNode(const endNode& Src);

	void Draw();
	float GetData();

	basicLogicNode* GetNextNode();
};