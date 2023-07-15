#pragma once

#include "../VisualNodeSystem/VisualNodeSystem.h"
#include "basicLogicNode.h"

class sequenceNode : public basicLogicNode
{
	friend class NodeFactory;
	static bool isRegistered;

	bool CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser);
	void SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, VISUAL_NODE_SOCKET_EVENT EventType);

	int Data = 0;

	void AddSequenceOutput();
public:
	sequenceNode();
	sequenceNode(const sequenceNode& Src);

	Json::Value ToJson();
	void FromJson(Json::Value Json);

	void Draw();

	basicLogicNode* GetNextNode();
};