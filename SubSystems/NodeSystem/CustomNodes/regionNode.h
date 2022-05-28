#pragma once

#include "beginNode.h"
#include "endNode.h"
#include "../Windows/textInputPopup.h"
#include "../Windows/actionEditPopup.h"

VISUAL_NODE_CHILD_PRIVATE_PART(regionNode)
	FEVisualNodeArea* data;

	bool canConnect(FEVisualNodeSocket* ownSocket, FEVisualNodeSocket* candidateSocket, char** msgToUser);
	void socketEvent(FEVisualNodeSocket* ownSocket, FEVisualNodeSocket* connectedSocket, FE_VISUAL_NODE_SOCKET_EVENT eventType);

	static FETPImage* regionIcon;

	void showTooltip();
	void checkIcons();
	void renderIcon();

	beginNode* begin;
	endNode* end;
public:
	regionNode();
	regionNode(const regionNode& src);

	void draw();
	FEVisualNodeArea* getData();

	FEVisualNode* getNextNode();
	FEVisualNode* getLogicallyNextNode();

	Json::Value toJson();
	void fromJson(Json::Value json);
};

VISUAL_NODE_CHILD_AFTER_CLASS(regionNode)