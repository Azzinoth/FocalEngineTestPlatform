#pragma once

#include "beginNode.h"

class regionNode;
VISUAL_NODE_CHILD_PRIVATE_PART(endNode)
	friend regionNode;
	float data = 0.0f;
	
	bool canConnect(FEVisualNodeSocket* ownSocket, FEVisualNodeSocket* candidateSocket, char** msgToUser);
	void socketEvent(FEVisualNodeSocket* ownSocket, FEVisualNodeSocket* connectedSocket, FE_VISUAL_NODE_SOCKET_EVENT eventType);

	FETPImage* icon = nullptr;
	FEVisualNode* nextNode = nullptr;
public:
	endNode();
	endNode(const endNode& src);

	void draw();
	float getData();

	FEVisualNode* getNextNode();
	FEVisualNode* getLogicallyNextNode();
};

VISUAL_NODE_CHILD_AFTER_CLASS(endNode)