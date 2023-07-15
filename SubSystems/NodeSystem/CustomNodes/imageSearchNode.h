#pragma once

#include "../VisualNodeSystem/VisualNodeSystem.h"
#include "basicLogicNode.h"

class imageSearchNode : public basicLogicNode
{
	friend class NodeFactory;
	static bool isRegistered;

	bool CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser);
	void SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, VISUAL_NODE_SOCKET_EVENT EventType);

	glm::vec2 FoundPosition = glm::vec2(-1.0f);
	bool bFound = false;

	std::function<void* ()> Vec2DataGetter = [this]() -> void* {
		return &FoundPosition;
	};

	/*class Vec2Socket : public NodeSocket
	{
	public:
		Vec2Socket(VisualNode* Parent, std::string Type, std::string Name, bool bOutput = false) : NodeSocket(Parent, Type, Name, bOutput) {};

		void* GetData() override
		{
			return reinterpret_cast<glm::vec2*>(&reinterpret_cast<imageSearchNode*>(Parent)->FoundPosition);
		}
	};*/

	std::function<void* ()> BoolDataGetter = [this]() -> void* {
		return &bFound;
	};

	/*class BoolSocket : public NodeSocket
	{
	public:
		BoolSocket(VisualNode* Parent, std::string Type, std::string Name, bool bOutput = false) : NodeSocket(Parent, Type, Name, bOutput) {};

		void* GetData() override
		{
			return &reinterpret_cast<imageSearchNode*>(Parent)->bFound;
		}
	};*/

public:
	imageSearchNode();
	imageSearchNode(const imageSearchNode& Src);

	Json::Value ToJson();
	void FromJson(Json::Value Json);

	void Draw();

	basicLogicNode* GetNextNode();
};