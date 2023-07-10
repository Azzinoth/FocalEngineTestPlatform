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

	class Vec2Socket : public NodeSocket
	{
	public:
		Vec2Socket(VisualNode* Parent, std::string Type, std::string Name, bool bOutput = false) : NodeSocket(Parent, Type, Name, bOutput) {};

		void* GetData() override
		{
			return reinterpret_cast<glm::vec2*>(&reinterpret_cast<imageSearchNode*>(Parent)->FoundPosition);
		}
	};

	class BoolSocket : public NodeSocket
	{
	public:
		BoolSocket(VisualNode* Parent, std::string Type, std::string Name, bool bOutput = false) : NodeSocket(Parent, Type, Name, bOutput) {};

		void* GetData() override
		{
			return &reinterpret_cast<imageSearchNode*>(Parent)->bFound;
		}
	};

public:
	imageSearchNode();
	imageSearchNode(const imageSearchNode& Src);

	void Draw();

	basicLogicNode* GetNextNode();
};