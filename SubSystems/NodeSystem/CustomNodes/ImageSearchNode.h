#pragma once

#include "../../NodeSystem/VisualNodeSystem/VisualNodeSystem.h"
#include "../FETPInput.h"

class ImageSearchNode : public BaseExecutionFlowNode
{
	friend class NodeFactory;
	static bool bIsRegistered;

	bool CanConnect(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* CandidateSocket, char** MsgToUser);
	void SocketEvent(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* ConnectedSocket, VisNodeSys::NODE_SOCKET_EVENT EventType);

	float Similarity = 95.0f;
	int MaxColorShift = 4;
	glm::vec2 FoundPosition = glm::vec2(-1.0f);
	bool bFound = false;
	int MonitorIndex = -1;
	int FoundMonitorIndex = -1;

	std::function<void* ()> MonitorIndexDataGetter = [this]() -> void* {
		return &FoundMonitorIndex;
	};

	std::function<void* ()> Vec2DataGetter = [this]() -> void* {
		return &FoundPosition;
	};

	std::function<void* ()> BoolDataGetter = [this]() -> void* {
		return &bFound;
	};

public:
	ImageSearchNode();
	ImageSearchNode(const ImageSearchNode& Other);

	Json::Value ToJson();
	bool FromJson(Json::Value Json);

	void Draw();
};