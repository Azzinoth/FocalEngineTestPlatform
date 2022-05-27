#pragma once

#include "../FEVisualNodeSystem/FEEditorNodeSystem.h"
#include "../../FETPImage.h"

EDITOR_NODE_CHILD_PRIVATE_PART(beginNode)
	float data = 0.0f;

	bool canConnect(FEEditorNodeSocket* ownSocket, FEEditorNodeSocket* candidateSocket, char** msgToUser);
	void socketEvent(FEEditorNodeSocket* ownSocket, FEEditorNodeSocket* connectedSocket, FE_EDITOR_NODE_SOCKET_EVENT eventType);

	FETPImage* icon = nullptr;
public:
	beginNode();
	beginNode(const beginNode& src);

	void draw();
	float getData();

	FEEditorNode* getNextNode();
};

EDITOR_NODE_CHILD_AFTER_CLASS(beginNode)