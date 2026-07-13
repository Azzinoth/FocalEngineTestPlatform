#pragma once
#include "../../../NodeSystem/VisualNodeSystem/VisualNodeSystem.h"

class StringToUpperNode : public BaseExecutionFlowNode
{
	friend class NodeFactory;
	static bool bIsRegistered;

	bool CanConnect(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* CandidateSocket, char** MsgToUser);
	void SocketEvent(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* ConnectedSocket, VisNodeSys::NODE_SOCKET_EVENT EventType);

	std::string Result = "";

	std::function<void* ()> ResultDataGetter = [this]() -> void* {
		return &Result;
	};

public:
	StringToUpperNode();
	StringToUpperNode(const StringToUpperNode& Other);

	Json::Value ToJson();
	bool FromJson(Json::Value Json);
};