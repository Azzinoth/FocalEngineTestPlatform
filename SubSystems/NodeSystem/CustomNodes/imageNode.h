#pragma once

#include "basicLogicNode.h"

const COMDLG_FILTERSPEC pngLoadFilter[] =
{
	{ L"png files (*.png)", L"*.png" }
};

class imageNode : public basicLogicNode
{
	friend class NodeFactory;
	static bool isRegistered;

	friend class FETest;

	bool CanConnect(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* CandidateSocket, char** MsgToUser);
	void SocketEvent(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* ConnectedSocket, VisNodeSys::NODE_SOCKET_EVENT EventType);

	FETPImage* Data = nullptr;

	std::function<void* ()> ImageDataGetter = [this]() -> void* {
		return Data;
	};

public:
	imageNode();
	imageNode(const imageNode& Src);

	Json::Value ToJson();
	void FromJson(Json::Value Json);

	void Draw();

	bool SetImage(FETPImage* Image);

	basicLogicNode* GetNextNode();
};