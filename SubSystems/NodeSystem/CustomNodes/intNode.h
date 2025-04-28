#pragma once

#include "BasicLogicNode.h"

class IntNode : public BasicLogicNode
{
	friend class NodeFactory;
	static bool bIsRegistered;

	bool CanConnect(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* CandidateSocket, char** MsgToUser);
	void SocketEvent(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* ConnectedSocket, VisNodeSys::NODE_SOCKET_EVENT EventType);

	int Data = 0;

	std::function<void* ()> IntDataGetter = [this]() -> void* {
		return &Data;
	};

public:
	IntNode();
	IntNode(const IntNode& Other);

	Json::Value ToJson();
	bool FromJson(Json::Value Json);

	void Draw();

	BasicLogicNode* GetNextNode();
};