#pragma once

#include "../../../NodeSystem/VisualNodeSystem/VisualNodeSystem.h"
#include "../../FETPInput.h"

class ImageVariableNode : public BaseExecutionFlowNode
{
	friend class NodeFactory;
	static bool bIsRegistered;

	bool CanConnect(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* CandidateSocket, char** MsgToUser);
	void SocketEvent(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* ConnectedSocket, VisNodeSys::NODE_SOCKET_EVENT EventType);

	FETPImage* Data = nullptr;
	glm::vec2 ImageSize = glm::vec2(0.0f, 0.0f);

	std::function<void* ()> ImageDataGetter = [this]() -> void* {
		return Data;
	};

	std::function<void* ()> ImageSizeGetter = [this]() -> void* {
		return &ImageSize;
	};

public:
	ImageVariableNode();
	ImageVariableNode(const ImageVariableNode& Other);

	Json::Value ToJson();
	bool FromJson(Json::Value Json);

	void Draw();
};