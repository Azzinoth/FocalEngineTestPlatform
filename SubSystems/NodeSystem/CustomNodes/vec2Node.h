#pragma once

#include "../VisualNodeSystem/VisualNodeSystem.h"
#include "basicLogicNode.h"

class vec2Node : public basicLogicNode
{
	friend class NodeFactory;
	static bool isRegistered;

	bool CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser);
	void SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, VISUAL_NODE_SOCKET_EVENT EventType);

	glm::vec2 Data = glm::vec2(0.0f);

	std::function<void* ()> Vec2DataGetter = [this]() -> void* {
		return &Data;
	};

	//class Vec2Socket : public NodeSocket
	//{
	//public:
	//	Vec2Socket(VisualNode* Parent, std::string Type, std::string Name, bool bOutput = false) : NodeSocket(Parent, Type, Name, bOutput) {};

	//	void* GetData() override
	//	{
	//		return reinterpret_cast<glm::vec2*>(&reinterpret_cast<vec2Node*>(Parent)->Data);
	//	}
	//};

public:
	vec2Node();
	vec2Node(const vec2Node& Src);

	Json::Value ToJson();
	void FromJson(Json::Value Json);

	void Draw();

	basicLogicNode* GetNextNode();
};