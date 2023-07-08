#pragma once

#include "../VisualNodeSystem/VisualNodeSystem.h"
#include "basicLogicNode.h"

class mouseMoveNode : public basicLogicNode
{
	friend class NodeFactory;
	static bool isRegistered;

	bool CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser);
	void SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, VISUAL_NODE_SOCKET_EVENT EventType);

	glm::vec2 Data = glm::vec2(0.0f);

	/*class BoolSocket : public NodeSocket
	{
	public:
		BoolSocket(VisualNode* Parent, std::string Type, std::string Name, bool bOutput = false) : NodeSocket(Parent, Type, Name, bOutput) {};

		void* GetData() override
		{
			return reinterpret_cast<bool*>(&reinterpret_cast<boolNode*>(Parent)->Data);
		}
	};*/

public:
	mouseMoveNode();
	mouseMoveNode(const mouseMoveNode& Src);

	void Draw();

	basicLogicNode* GetNextNode();
};