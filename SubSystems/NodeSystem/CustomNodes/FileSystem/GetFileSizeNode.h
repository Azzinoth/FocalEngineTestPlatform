#pragma once
#include "../../../NodeSystem/VisualNodeSystem/VisualNodeSystem.h"

class GetFileSizeNode : public BaseExecutionFlowNode
{
	friend class NodeFactory;
	static bool bIsRegistered;

	bool CanConnect(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* CandidateSocket, char** MsgToUser);
	void SocketEvent(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* ConnectedSocket, VisNodeSys::NODE_SOCKET_EVENT EventType);

	int SizeInBytes = 0;

	std::function<void* ()> SizeDataGetter = [this]() -> void* {
		return &SizeInBytes;
	};

public:
	GetFileSizeNode();
	GetFileSizeNode(const GetFileSizeNode& Other);

	Json::Value ToJson();
	bool FromJson(Json::Value Json);

	void Draw();
};