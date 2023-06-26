#pragma once

#include "beginNode.h"

class regionNode;
VISUAL_NODE_CHILD_PRIVATE_PART(endNode)
	friend regionNode;
	float Data = 0.0f;
	
	bool CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser);
	void SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, VISUAL_NODE_SOCKET_EVENT EventType);

	FETPImage* Icon = nullptr;
	VisualNode* NextNode = nullptr;
public:
	endNode();
	endNode(const endNode& Src);

	void Draw();
	float GetData();

	VisualNode* GetNextNode();
	VisualNode* GetLogicallyNextNode();
};

VISUAL_NODE_CHILD_AFTER_CLASS(endNode)