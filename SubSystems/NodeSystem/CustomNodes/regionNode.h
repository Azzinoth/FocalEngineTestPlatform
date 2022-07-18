#pragma once

#include "beginNode.h"
#include "endNode.h"
#include "../Windows/textInputPopup.h"
#include "../Windows/actionEditPopup.h"

VISUAL_NODE_CHILD_PRIVATE_PART(regionNode)
	FEVisualNodeArea* Data;

	bool CanConnect(FEVisualNodeSocket* OwnSocket, FEVisualNodeSocket* CandidateSocket, char** MsgToUser);
	void SocketEvent(FEVisualNodeSocket* OwnSocket, FEVisualNodeSocket* ConnectedSocket, FE_VISUAL_NODE_SOCKET_EVENT EventType);

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
	FEVisualNodeArea* GetData();

	FEVisualNode* GetNextNode();
	FEVisualNode* GetLogicallyNextNode();

	Json::Value ToJson();
	void FromJson(Json::Value Json);
};

VISUAL_NODE_CHILD_AFTER_CLASS(regionNode)