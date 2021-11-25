#pragma once

#include "../FEEditorNodeSystem.h"
#include "../SubSystems/FETPInput.h"
#include "../Windows/screenshootEditor.h"
#include "../Windows/textInputPopup.h"
#include "../Windows/actionEditPopup.h"

EDITOR_NODE_CHILD_PRIVATE_PART(globalActionNode)
	FETPAction* data;
	void initialize(FETPAction* data);

	bool contextMenu = false;
	bool openContextMenu();

	bool canConnect(FEEditorNodeSocket* ownSocket, FEEditorNodeSocket* candidateSocket, char** msgToUser);
	void socketEvent(FEEditorNodeSocket* ownSocket, FEEditorNodeSocket* connectedSocket, FE_EDITOR_NODE_SOCKET_EVENT eventType);

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

	FEEditorNode* getNextNode();
	FEEditorNode* getLogicallyNextNode();

	Json::Value toJson();
	void fromJson(Json::Value json);
};

EDITOR_NODE_CHILD_AFTER_CLASS(globalActionNode)