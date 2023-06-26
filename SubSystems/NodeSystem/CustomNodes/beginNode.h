#pragma once

#include "../VisualNodeSystem/VisualNodeSystem.h"
#include "../../FETPImage.h"

VISUAL_NODE_CHILD_PRIVATE_PART(beginNode)
	float Data = 0.0f;

	bool CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser);
	void SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, VISUAL_NODE_SOCKET_EVENT EventType);

	FETPImage* Icon = nullptr;
public:
	beginNode();
	beginNode(const beginNode& Src);

	void Draw();
	float GetData();

	VisualNode* GetNextNode();
};

VISUAL_NODE_CHILD_AFTER_CLASS(beginNode)