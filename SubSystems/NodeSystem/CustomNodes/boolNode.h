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

	class BoolSocket : public NodeSocket
	{
	public:
		BoolSocket(VisualNode* Parent, std::string Type, std::string Name, bool bOutput = false) : NodeSocket(Parent, Type, Name, bOutput) {};

		void* GetData() override
		{
			/*bool TempBool = reinterpret_cast<boolNode*>(Parent)->Data;
			bool* NewBool = new bool;
			*NewBool = TempBool;
			return reinterpret_cast<void*>(NewBool);*/
			//return reinterpret_cast<void*>(&(reinterpret_cast<boolNode*>(Parent)->Data));
			return &reinterpret_cast<boolNode*>(Parent)->Data;
		}
	};

public:
	boolNode();
	boolNode(const boolNode& Src);

	void Draw();

	basicLogicNode* GetNextNode();
};