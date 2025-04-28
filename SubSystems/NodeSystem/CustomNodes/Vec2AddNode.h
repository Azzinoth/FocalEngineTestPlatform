#pragma once

#include "BasicLogicNode.h"

class Vec2AddNode : public BasicLogicNode
{
	friend class VisNodeSys::NodeFactory;
	static bool bIsRegistered;

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
	Vec2AddNode();
	Vec2AddNode(const Vec2AddNode& Other);

	void Draw();

	BasicLogicNode* GetNextNode();
};