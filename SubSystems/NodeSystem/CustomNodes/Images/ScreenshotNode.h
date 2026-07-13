#pragma once

#include "../../../NodeSystem/VisualNodeSystem/VisualNodeSystem.h"
#include "../../FETPInput.h"

class ScreenshotNode : public BaseExecutionFlowNode
{
	friend class NodeFactory;
	static bool bIsRegistered;

	bool CanConnect(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* CandidateSocket, char** MsgToUser);
	void SocketEvent(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* ConnectedSocket, VisNodeSys::NODE_SOCKET_EVENT EventType);

	int MonitorIndex = 0;
	FETPImage* StoredImage = nullptr;

	std::function<void* ()> ImageDataGetter = [this]() -> void* {
		return StoredImage;
	};

public:
	ScreenshotNode();
	ScreenshotNode(const ScreenshotNode& Other);
	~ScreenshotNode();

	Json::Value ToJson();
	bool FromJson(Json::Value Json);

	void Draw();
};
