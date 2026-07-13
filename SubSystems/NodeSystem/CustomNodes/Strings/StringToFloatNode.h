#pragma once
#include "../../../NodeSystem/VisualNodeSystem/VisualNodeSystem.h"

class StringToFloatNode : public BaseExecutionFlowNode
{
	friend class NodeFactory;
	static bool bIsRegistered;

	bool CanConnect(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* CandidateSocket, char** MsgToUser);
	void SocketEvent(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* ConnectedSocket, VisNodeSys::NODE_SOCKET_EVENT EventType);

	float Result = 0.0f;

	std::function<void* ()> ResultDataGetter = [this]() -> void* {
		return &Result;
	};

public:
	StringToFloatNode();
	StringToFloatNode(const StringToFloatNode& Other);

	Json::Value ToJson();
	bool FromJson(Json::Value Json);

	void Draw();
};