#pragma once

#include "BasicLogicNode.h"

class SequenceNode : public BasicLogicNode
{
	friend class NodeFactory;
	static bool bIsRegistered;

	bool CanConnect(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* CandidateSocket, char** MsgToUser);
	void SocketEvent(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* ConnectedSocket, VisNodeSys::NODE_SOCKET_EVENT EventType);

	int Data = 0;

	void AddSequenceOutput();
public:
	SequenceNode();
	SequenceNode(const SequenceNode& Other);

	Json::Value ToJson();
	bool FromJson(Json::Value Json);

	void Draw();

	BasicLogicNode* GetNextNode();
};