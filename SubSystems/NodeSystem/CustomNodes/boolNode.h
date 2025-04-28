#pragma once

#include "BasicLogicNode.h"

class BoolNode : public BasicLogicNode
{
	friend class NodeFactory;
	static bool bIsRegistered;

	bool CanConnect(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* CandidateSocket, char** MsgToUser);
	void SocketEvent(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* ConnectedSocket, VisNodeSys::NODE_SOCKET_EVENT EventType);

	bool bData = false;

	std::function<void* ()> BoolDataGetter = [this]() -> void* {
		return &bData;
	};

public:
	BoolNode();
	BoolNode(const BoolNode& Other);

	Json::Value ToJson();
	bool FromJson(Json::Value Json);

	void Draw();

	BasicLogicNode* GetNextNode();
};