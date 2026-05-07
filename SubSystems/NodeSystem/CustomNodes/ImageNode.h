#pragma once

#include "../../NodeSystem/VisualNodeSystem/VisualNodeSystem.h"
#include "../FETPInput.h"

const COMDLG_FILTERSPEC PNGLoadFilter[] =
{
	{ L"png files (*.png)", L"*.png" }
};

class ImageNode : public BaseExecutionFlowNode
{
	friend class NodeFactory;
	static bool bIsRegistered;

	friend class FETest;

	bool CanConnect(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* CandidateSocket, char** MsgToUser);
	void SocketEvent(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* ConnectedSocket, VisNodeSys::NODE_SOCKET_EVENT EventType);

	FETPImage* Data = nullptr;
	glm::vec2 ImageSize = glm::vec2(0.0f, 0.0f);

	std::function<void* ()> ImageDataGetter = [this]() -> void* {
		return Data;
	};

	std::function<void* ()> ImageSizeGetter = [this]() -> void* {
		return (void*)&ImageSize;
	};

public:
	ImageNode();
	ImageNode(const ImageNode& Other);

	Json::Value ToJson();
	bool FromJson(Json::Value Json);

	void Draw();

	bool SetImage(FETPImage* Image);
};