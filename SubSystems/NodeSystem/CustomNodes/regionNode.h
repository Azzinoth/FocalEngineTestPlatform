#pragma once

#include "beginNode.h"
#include "endNode.h"
#include "../Windows/textInputPopup.h"
#include "../Windows/actionEditPopup.h"

class regionNode : public basicLogicNode
{
	friend class NodeFactory;
	static bool isRegistered;

	VisualNodeArea* Data;

	bool CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser);
	void SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, VISUAL_NODE_SOCKET_EVENT EventType);

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
	VisualNodeArea* GetData();

	basicLogicNode* GetNextNode();

	Json::Value ToJson();
	void FromJson(Json::Value Json);
};