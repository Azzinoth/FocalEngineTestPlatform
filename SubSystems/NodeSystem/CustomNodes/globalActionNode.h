#pragma once

#include "../FEVisualNodeSystem/FEVisualNodeSystem.h"
#include "../SubSystems/FETPInput.h"
#include "../Windows/screenshootEditor.h"
#include "../Windows/textInputPopup.h"
#include "../Windows/actionEditPopup.h"

VISUAL_NODE_CHILD_PRIVATE_PART(globalActionNode)
	FETPAction* data;
	void initialize(FETPAction* data);

	bool contextMenu = false;
	bool openContextMenu();

	bool canConnect(FEVisualNodeSocket* ownSocket, FEVisualNodeSocket* candidateSocket, char** msgToUser);
	void socketEvent(FEVisualNodeSocket* ownSocket, FEVisualNodeSocket* connectedSocket, FE_VISUAL_NODE_SOCKET_EVENT eventType);

	void mouseClick(int mouseButton);

	static FETPImage* sleepIcon;

	static FETPImage* keyDownIcon;
	static FETPImage* keyUpIcon;

	static FETPImage* moveMouseIcon;
	static FETPImage* leftMouseDownIcon;
	static FETPImage* leftMouseUpIcon;
	static FETPImage* rightMouseDownIcon;
	static FETPImage* rightMouseUpIcon;
	static FETPImage* scrollMouseIcon;

	static FETPImage* lunchApplicationIcon;
	static FETPImage* screenshotIcon;

	void showTooltip();
	void checkIcons();
	void renderIcon();

	void copyData(FETPAction* src);
public:
	globalActionNode();
	globalActionNode(const globalActionNode& src);
	globalActionNode(FETPAction* data);

	~globalActionNode();

	void draw();
	FETPAction* getData();

	FEVisualNode* getNextNode();
	FEVisualNode* getLogicallyNextNode();

	Json::Value toJson();
	void fromJson(Json::Value json);
};

VISUAL_NODE_CHILD_AFTER_CLASS(globalActionNode)