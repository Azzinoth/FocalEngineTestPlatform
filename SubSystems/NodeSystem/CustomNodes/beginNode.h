#pragma once

#include "../FEVisualNodeSystem/FEVisualNodeSystem.h"
#include "../../FETPImage.h"

VISUAL_NODE_CHILD_PRIVATE_PART(beginNode)
	float data = 0.0f;

	bool canConnect(FEVisualNodeSocket* ownSocket, FEVisualNodeSocket* candidateSocket, char** msgToUser);
	void socketEvent(FEVisualNodeSocket* ownSocket, FEVisualNodeSocket* connectedSocket, FE_VISUAL_NODE_SOCKET_EVENT eventType);

	FETPImage* icon = nullptr;
public:
	beginNode();
	beginNode(const beginNode& src);

	void draw();
	float getData();

	FEVisualNode* getNextNode();
};

VISUAL_NODE_CHILD_AFTER_CLASS(beginNode)