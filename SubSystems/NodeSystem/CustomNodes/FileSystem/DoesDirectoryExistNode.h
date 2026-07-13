#pragma once
#include "../../../NodeSystem/VisualNodeSystem/VisualNodeSystem.h"

class DoesDirectoryExistNode : public BaseExecutionFlowNode
{
	friend class NodeFactory;
	static bool bIsRegistered;

	bool CanConnect(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* CandidateSocket, char** MsgToUser);
	void SocketEvent(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* ConnectedSocket, VisNodeSys::NODE_SOCKET_EVENT EventType);

	bool Result = false;

	std::function<void* ()> ResultDataGetter = [this]() -> void* {
		return &Result;
	};

public:
	DoesDirectoryExistNode();
	DoesDirectoryExistNode(const DoesDirectoryExistNode& Other);

	Json::Value ToJson();
	bool FromJson(Json::Value Json);

	void Draw();
};