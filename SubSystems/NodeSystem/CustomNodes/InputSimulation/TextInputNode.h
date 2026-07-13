#pragma once

#include "../../../NodeSystem/VisualNodeSystem/VisualNodeSystem.h"
#include "../../FETPInput.h"

class TextInputNode : public BaseExecutionFlowNode
{
	friend class NodeFactory;
	static bool bIsRegistered;

	friend class FETest;

	bool CanConnect(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* CandidateSocket, char** MsgToUser);
	void SocketEvent(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* ConnectedSocket, VisNodeSys::NODE_SOCKET_EVENT EventType);

	std::string Data = "";

	std::function<void* ()> DataGetter = [this]() -> void* {
		return &Data;
	};

public:
	TextInputNode();
	TextInputNode(const TextInputNode& Other);

	Json::Value ToJson();
	bool FromJson(Json::Value Json);

	void Draw();

	bool SetText(std::string Text);
};