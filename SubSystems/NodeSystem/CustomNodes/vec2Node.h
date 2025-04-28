#pragma once

#include "BasicLogicNode.h"

class Vec2Node : public BasicLogicNode
{
	friend class NodeFactory;
	static bool bIsRegistered;

	bool CanConnect(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* CandidateSocket, char** MsgToUser);
	void SocketEvent(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* ConnectedSocket, VisNodeSys::NODE_SOCKET_EVENT EventType);

	glm::vec2 Data = glm::vec2(0.0f);

	std::function<void* ()> Vec2DataGetter = [this]() -> void* {
		return &Data;
	};

public:
	Vec2Node();
	Vec2Node(const Vec2Node& Other);

	Json::Value ToJson();
	bool FromJson(Json::Value Json);

	void Draw();

	BasicLogicNode* GetNextNode();
};