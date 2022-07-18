#pragma once

#include "beginNode.h"

class regionNode;
VISUAL_NODE_CHILD_PRIVATE_PART(endNode)
	friend regionNode;
	float Data = 0.0f;
	
	bool CanConnect(FEVisualNodeSocket* OwnSocket, FEVisualNodeSocket* CandidateSocket, char** MsgToUser);
	void SocketEvent(FEVisualNodeSocket* OwnSocket, FEVisualNodeSocket* ConnectedSocket, FE_VISUAL_NODE_SOCKET_EVENT EventType);

	FETPImage* Icon = nullptr;
	FEVisualNode* NextNode = nullptr;
public:
	endNode();
	endNode(const endNode& Src);

	void Draw();
	float GetData();

	FEVisualNode* GetNextNode();
	FEVisualNode* GetLogicallyNextNode();
};

VISUAL_NODE_CHILD_AFTER_CLASS(endNode)