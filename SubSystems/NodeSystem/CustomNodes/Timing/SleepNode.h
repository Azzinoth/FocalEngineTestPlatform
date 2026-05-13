#pragma once
#include "../../../NodeSystem/VisualNodeSystem/VisualNodeSystem.h"

class SleepNode : public BaseExecutionFlowNode
{
	friend class NodeFactory;
	static bool bIsRegistered;

	int SleepDurationMs = 0;

	bool CanConnect(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* CandidateSocket, char** MsgToUser);
	void SocketEvent(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* ConnectedSocket, VisNodeSys::NODE_SOCKET_EVENT EventType);
public:
	SleepNode();
	SleepNode(const SleepNode& Other);

	Json::Value ToJson();
	bool FromJson(Json::Value Json);

	void Draw();

	int GetSleepDuration() const;
	void SetSleepDuration(int NewValue);
};