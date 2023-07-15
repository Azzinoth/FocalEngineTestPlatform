#pragma once

#include "../VisualNodeSystem/VisualNodeSystem.h"
#include "basicLogicNode.h"

class intNode : public basicLogicNode
{
	friend class NodeFactory;
	static bool isRegistered;

	bool CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser);
	void SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, VISUAL_NODE_SOCKET_EVENT EventType);

	int Data = 0;

	std::function<void* ()> IntDataGetter = [this]() -> void* {
		return &Data;
	};

public:
	intNode();
	intNode(const intNode& Src);

	Json::Value ToJson();
	void FromJson(Json::Value Json);

	void Draw();

	basicLogicNode* GetNextNode();
};