#pragma once

#include "beginNode.h"
#include "endNode.h"
#include "../Windows/textInputPopup.h"
#include "../Windows/actionEditPopup.h"

class regionNode : public basicLogicNode
{
	friend class NodeFactory;
	static bool isRegistered;

	VisNodeSys::NodeArea* Data;

	bool CanConnect(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* CandidateSocket, char** MsgToUser);
	void SocketEvent(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* ConnectedSocket, VisNodeSys::NODE_SOCKET_EVENT EventType);

	static FETPImage* RegionIcon;

	void ShowTooltip();
	void CheckIcons();
	void RenderIcon();

	beginNode* Begin;
	endNode* End;
public:
	regionNode();
	regionNode(const regionNode& Src);

	void Draw();
	VisNodeSys::NodeArea* GetData();

	basicLogicNode* GetNextNode();

	Json::Value ToJson();
	void FromJson(Json::Value Json);
};