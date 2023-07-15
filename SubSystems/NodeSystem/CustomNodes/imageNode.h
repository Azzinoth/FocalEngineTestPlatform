#pragma once

#include "../VisualNodeSystem/VisualNodeSystem.h"
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

	bool CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser);
	void SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, VISUAL_NODE_SOCKET_EVENT EventType);

	FETPImage* Data = nullptr;

	std::function<void* ()> ImageDataGetter = [this]() -> void* {
		return Data;
	};

	/*class ImageSocket : public NodeSocket
	{
	public:
		ImageSocket(VisualNode* Parent, std::string Type, std::string Name, bool bOutput = false) : NodeSocket(Parent, Type, Name, bOutput) {};

		void* GetData() override
		{
			return reinterpret_cast<imageNode*>(Parent)->Data;
		}
	};*/

public:
	imageNode();
	imageNode(const imageNode& Src);

	Json::Value ToJson();
	void FromJson(Json::Value Json);

	void Draw();

	basicLogicNode* GetNextNode();
};