#pragma once
#include "../../../NodeSystem/VisualNodeSystem/VisualNodeSystem.h"

class ExtractFileNameNode : public BaseExecutionFlowNode
{
	friend class NodeFactory;
	static bool bIsRegistered;

	bool CanConnect(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* CandidateSocket, char** MsgToUser);
	void SocketEvent(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* ConnectedSocket, VisNodeSys::NODE_SOCKET_EVENT EventType);

	std::string ExtractedName = "";

	std::function<void* ()> NameDataGetter = [this]() -> void* {
		return &ExtractedName;
	};

public:
	ExtractFileNameNode();
	ExtractFileNameNode(const ExtractFileNameNode& Other);

	Json::Value ToJson();
	bool FromJson(Json::Value Json);
};