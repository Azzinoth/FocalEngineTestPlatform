#pragma once

#include "globalActionNode.h"

class testEditorWinow;
class nodeRegionWindow;
VISUAL_NODE_CHILD_PRIVATE_PART(combinedActionNode)
	friend testEditorWinow;
	friend nodeRegionWindow;

	std::vector<FETPAction*> data;
	void initialize(std::vector<FETPAction*> Data, FETP_COMBINED_ACTION_TYPE type);
	FETP_COMBINED_ACTION_TYPE actionType;

	glm::vec2 beginPosition;
	glm::vec2 endPosition;

	std::string text;

	//bool contextMenu = false;
	//bool openContextMenu();

	bool canConnect(FEVisualNodeSocket* ownSocket, FEVisualNodeSocket* candidateSocket, char** msgToUser);
	void socketEvent(FEVisualNodeSocket* ownSocket, FEVisualNodeSocket* connectedSocket, FE_VISUAL_NODE_SOCKET_EVENT eventType);

	static combinedActionNode* nodeForCallback;
	static void changeTextCallback(std::string newText);

	static FETPImage* moveMouseCombineIcon;
	static FETPImage* leftMouseCombineIcon;
	static FETPImage* rightMouseCombineIcon;
	static FETPImage* wheelMouseCombineIcon;

	static FETPImage* keyCombinedIcon;
	static FETPImage* textCombinedIcon;

	void showTooltip();
	void checkIcons();
	void renderIcon();
public:
	combinedActionNode();
	combinedActionNode(const combinedActionNode& src);
	combinedActionNode(std::vector<FETPAction*> data, FETP_COMBINED_ACTION_TYPE type);

	void draw();
	std::vector<FETPAction*> getData();
	FEVisualNode* getNextNode();

	Json::Value toJson();
	void fromJson(Json::Value json);

	void changeText(std::string newText, int avarageDelay = -1);
	FETP_COMBINED_ACTION_TYPE getCombinedActionType();
	void remove();
};

VISUAL_NODE_CHILD_AFTER_CLASS(combinedActionNode)