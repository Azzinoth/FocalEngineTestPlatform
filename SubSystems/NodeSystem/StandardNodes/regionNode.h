#pragma once

#include "beginNode.h"
#include "endNode.h"
#include "../FEEditorNodeSystem.h"
#include "../Windows/textInputPopup.h"
#include "../Windows/actionEditPopup.h"

EDITOR_NODE_CHILD_PRIVATE_PART(regionNode)
	FEEditorNodeArea* data;

	bool canConnect(FEEditorNodeSocket* ownSocket, FEEditorNodeSocket* candidateSocket, char** msgToUser);
	void socketEvent(FEEditorNodeSocket* ownSocket, FEEditorNodeSocket* connectedSocket, FE_EDITOR_NODE_SOCKET_EVENT eventType);

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
	FEEditorNodeArea* getData();

	FEEditorNode* getNextNode();
	FEEditorNode* getLogicallyNextNode();

	Json::Value toJson();
	void fromJson(Json::Value json);
};

EDITOR_NODE_CHILD_AFTER_CLASS(regionNode)