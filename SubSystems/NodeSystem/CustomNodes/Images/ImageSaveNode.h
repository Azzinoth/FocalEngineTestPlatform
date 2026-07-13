#pragma once

#include "../../../NodeSystem/VisualNodeSystem/VisualNodeSystem.h"
#include "../../FETPInput.h"

class ImageSaveNode : public BaseExecutionFlowNode
{
	friend class NodeFactory;
	static bool bIsRegistered;

	bool CanConnect(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* CandidateSocket, char** MsgToUser);
	void SocketEvent(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* ConnectedSocket, VisNodeSys::NODE_SOCKET_EVENT EventType);

	// Local file path (editable in the node). Overridden by Input[2] (STRING "Path") if connected.
	std::string Data = "";

	bool bHasRun = false;
	bool Result = false;

	bool TrySaveImage(FETPImage* Image);

	std::function<void* ()> ResultDataGetter = [this]() -> void* {
		return &Result;
	};

public:
	ImageSaveNode();
	ImageSaveNode(const ImageSaveNode& Other);

	Json::Value ToJson();
	bool FromJson(Json::Value Json);

	void Draw();

	bool SetPath(std::string Path);
};