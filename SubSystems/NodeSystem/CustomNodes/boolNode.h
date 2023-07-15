#pragma once

#include "../VisualNodeSystem/VisualNodeSystem.h"
#include "basicLogicNode.h"

class boolNode : public basicLogicNode
{
	friend class NodeFactory;
	static bool isRegistered;

	bool CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser);
	void SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, VISUAL_NODE_SOCKET_EVENT EventType);

	bool Data = false;

	std::function<void* ()> BoolDataGetter = [this]() -> void* {
		return &Data;
	};

public:
	boolNode();
	boolNode(const boolNode& Src);

	Json::Value ToJson();
	void FromJson(Json::Value Json);

	void Draw();

	basicLogicNode* GetNextNode();
};