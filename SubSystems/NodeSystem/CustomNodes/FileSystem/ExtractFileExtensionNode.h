#pragma once
#include "../../../NodeSystem/VisualNodeSystem/VisualNodeSystem.h"

class ExtractFileExtensionNode : public BaseExecutionFlowNode
{
	friend class NodeFactory;
	static bool bIsRegistered;

	bool CanConnect(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* CandidateSocket, char** MsgToUser);
	void SocketEvent(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* ConnectedSocket, VisNodeSys::NODE_SOCKET_EVENT EventType);

	std::string ExtractedExtension = "";

	std::function<void* ()> ExtensionDataGetter = [this]() -> void* {
		return &ExtractedExtension;
	};

public:
	ExtractFileExtensionNode();
	ExtractFileExtensionNode(const ExtractFileExtensionNode& Other);

	Json::Value ToJson();
	bool FromJson(Json::Value Json);
};