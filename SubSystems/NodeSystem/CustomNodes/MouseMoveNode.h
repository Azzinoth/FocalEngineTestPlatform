#pragma once
#include "../../NodeSystem/VisualNodeSystem/VisualNodeSystem.h"
#include "../FETPInput.h"

class MouseMoveNode : public BaseExecutionFlowNode
{
	friend class NodeFactory;
	static bool bIsRegistered;

	bool CanConnect(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* CandidateSocket, char** MsgToUser);
	void SocketEvent(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* ConnectedSocket, VisNodeSys::NODE_SOCKET_EVENT EventType);

	glm::vec2 Data = glm::vec2(0.0f);
	int Monitor = -1;
public:
	MouseMoveNode();
	MouseMoveNode(const MouseMoveNode& Other);

	Json::Value ToJson();
	bool FromJson(Json::Value Json);

	void Draw();

	glm::vec2 GetMouseTargetPosition() const;
	void SetMouseTargetPosition(glm::vec2 NewValue);
};