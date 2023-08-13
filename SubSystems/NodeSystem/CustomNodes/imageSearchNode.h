#pragma once

#include "basicLogicNode.h"

class imageSearchNode : public basicLogicNode
{
	friend class NodeFactory;
	static bool isRegistered;

	bool CanConnect(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* CandidateSocket, char** MsgToUser);
	void SocketEvent(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* ConnectedSocket, VisNodeSys::NODE_SOCKET_EVENT EventType);

	float Simularity = 95.0f;
	int MaxColorShift = 4;
	glm::vec2 FoundPosition = glm::vec2(-1.0f);
	bool bFound = false;

	std::function<void* ()> Vec2DataGetter = [this]() -> void* {
		return &FoundPosition;
	};

	std::function<void* ()> BoolDataGetter = [this]() -> void* {
		return &bFound;
	};

public:
	imageSearchNode();
	imageSearchNode(const imageSearchNode& Src);

	Json::Value ToJson();
	void FromJson(Json::Value Json);

	void Draw();

	basicLogicNode* GetNextNode();
};