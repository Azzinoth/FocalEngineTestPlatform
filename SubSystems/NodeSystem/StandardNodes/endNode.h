#pragma once

#include "../FEEditorNodeSystem.h"

class regionNode;
EDITOR_NODE_CHILD_PRIVATE_PART(endNode)
	friend regionNode;
	float data = 0.0f;
	
	bool canConnect(FEEditorNodeSocket* ownSocket, FEEditorNodeSocket* candidateSocket, char** msgToUser);
	void socketEvent(FEEditorNodeSocket* ownSocket, FEEditorNodeSocket* connectedSocket, FE_EDITOR_NODE_SOCKET_EVENT eventType);

	FETPImage* icon = nullptr;
	FEEditorNode* nextNode = nullptr;
public:
	endNode();
	endNode(const endNode& src);

	void draw();
	float getData();

	FEEditorNode* getNextNode();
	FEEditorNode* getLogicallyNextNode();
};

EDITOR_NODE_CHILD_AFTER_CLASS(endNode)