#pragma once

#include "../../../NodeSystem/VisualNodeSystem/VisualNodeSystem.h"
#include "../../FETPInput.h"

class ImageLoadNode : public BaseExecutionFlowNode
{
	friend class NodeFactory;
	static bool bIsRegistered;

	bool CanConnect(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* CandidateSocket, char** MsgToUser);
	void SocketEvent(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* ConnectedSocket, VisNodeSys::NODE_SOCKET_EVENT EventType);

	// File path.
	std::string Data = "";
	FETPImage* LoadedImage = nullptr;

	void LoadImageFromCurrentPath();

	std::function<void* ()> ImageDataGetter = [this]() -> void* {
		return LoadedImage;
	};

public:
	ImageLoadNode();
	ImageLoadNode(const ImageLoadNode& Other);
	~ImageLoadNode();

	Json::Value ToJson();
	bool FromJson(Json::Value Json);

	void Draw();

	bool SetPath(std::string Path);
};