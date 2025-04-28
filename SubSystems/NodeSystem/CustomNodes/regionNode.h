#pragma once

#include "BeginNode.h"
#include "EndNode.h"
#include "../Windows/TextInputPopup.h"
#include "../Windows/ActionEditPopup.h"

class RegionNode : public BasicLogicNode
{
	friend class NodeFactory;
	static bool bIsRegistered;

	VisNodeSys::NodeArea* Data;

	bool CanConnect(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* CandidateSocket, char** MsgToUser);
	void SocketEvent(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* ConnectedSocket, VisNodeSys::NODE_SOCKET_EVENT EventType);

	static FETPImage* RegionIcon;

	void ShowTooltip();
	void CheckIcons();
	void RenderIcon();

	BeginNode* Begin;
	EndNode* End;
public:
	RegionNode();
	RegionNode(const RegionNode& Other);

	void Draw();
	VisNodeSys::NodeArea* GetData();

	BasicLogicNode* GetNextNode();

	Json::Value ToJson();
	bool FromJson(Json::Value Json);
};