#pragma once
#include "../../../NodeSystem/VisualNodeSystem/VisualNodeSystem.h"

class StringLiteralNode : public VisNodeSys::Node
{
	friend class NodeFactory;
	static bool bIsRegistered;

	bool CanConnect(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* CandidateSocket, char** MsgToUser);
	void SocketEvent(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* ConnectedSocket, VisNodeSys::NODE_SOCKET_EVENT EventType);

	std::string Data = "";

	std::function<void* ()> StringDataGetter = [this]() -> void* {
		return &Data;
	};

public:
	StringLiteralNode();
	StringLiteralNode(const StringLiteralNode& Other);

	Json::Value ToJson();
	bool FromJson(Json::Value Json);

	std::string GetData() const;
	void SetData(std::string NewValue);

	void Draw();
};