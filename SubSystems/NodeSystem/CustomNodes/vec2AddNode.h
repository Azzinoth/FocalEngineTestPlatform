#pragma once

#include "basicLogicNode.h"

class vec2AddNode : public basicLogicNode
{
	friend class VisNodeSys::NodeFactory;
	static bool isRegistered;

	bool CanConnect(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* CandidateSocket, char** MsgToUser);
	void SocketEvent(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* ConnectedSocket, VisNodeSys::NODE_SOCKET_EVENT EventType);

	glm::vec2 LocalData = glm::vec2(0.0f);

	std::function<void* ()> Vec2AddDataGetter = [this]() -> void* {
		
		LocalData = glm::vec2(0.0f);
		if (!Input[0]->GetConnectedSockets().empty())
		{
			void* TempData = Input[0]->GetConnectedSockets()[0]->GetData();
			if (TempData != nullptr)
				LocalData = *reinterpret_cast<glm::vec2*>(TempData);

			if (!Input[1]->GetConnectedSockets().empty())
			{
				void* TempData = Input[1]->GetConnectedSockets()[0]->GetData();
				if (TempData != nullptr)
					LocalData += *reinterpret_cast<glm::vec2*>(TempData);
			}
		}

		return &LocalData;
	};

public:
	vec2AddNode();
	vec2AddNode(const vec2AddNode& Src);

	void Draw();

	basicLogicNode* GetNextNode();
};