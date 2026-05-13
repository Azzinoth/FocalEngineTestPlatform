#pragma once
#include "../../../NodeSystem/VisualNodeSystem/VisualNodeSystem.h"

class ExtractDirectoryPathNode : public BaseExecutionFlowNode
{
	friend class NodeFactory;
	static bool bIsRegistered;

	bool CanConnect(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* CandidateSocket, char** MsgToUser);
	void SocketEvent(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* ConnectedSocket, VisNodeSys::NODE_SOCKET_EVENT EventType);

	std::string ExtractedDirectory = "";

	std::function<void* ()> DirectoryDataGetter = [this]() -> void* {
		return &ExtractedDirectory;
	};

public:
	ExtractDirectoryPathNode();
	ExtractDirectoryPathNode(const ExtractDirectoryPathNode& Other);

	Json::Value ToJson();
	bool FromJson(Json::Value Json);
};