#pragma once

#include "../FEVisualNodeSystem/FEVisualNodeSystem.h"
#include "../../FETPImage.h"

VISUAL_NODE_CHILD_PRIVATE_PART(beginNode)
	float Data = 0.0f;

	bool CanConnect(FEVisualNodeSocket* OwnSocket, FEVisualNodeSocket* CandidateSocket, char** MsgToUser);
	void SocketEvent(FEVisualNodeSocket* OwnSocket, FEVisualNodeSocket* ConnectedSocket, FE_VISUAL_NODE_SOCKET_EVENT EventType);

	FETPImage* Icon = nullptr;
public:
	beginNode();
	beginNode(const beginNode& Src);

	void Draw();
	float GetData();

	FEVisualNode* GetNextNode();
};

VISUAL_NODE_CHILD_AFTER_CLASS(beginNode)