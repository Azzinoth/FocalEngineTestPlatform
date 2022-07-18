#pragma once

#include "../FEVisualNodeSystem/FEVisualNodeSystem.h"
#include "../SubSystems/FETPInput.h"
#include "../Windows/screenshootEditor.h"
#include "../Windows/textInputPopup.h"
#include "../Windows/actionEditPopup.h"

VISUAL_NODE_CHILD_PRIVATE_PART(globalActionNode)
	FETPAction* Data;
	void Initialize(FETPAction* Data);

	bool ContextMenu = false;
	bool OpenContextMenu();

	bool CanConnect(FEVisualNodeSocket* OwnSocket, FEVisualNodeSocket* CandidateSocket, char** MsgToUser);
	void SocketEvent(FEVisualNodeSocket* OwnSocket, FEVisualNodeSocket* ConnectedSocket, FE_VISUAL_NODE_SOCKET_EVENT EventType);

	void MouseClick(int MouseButton);

	static FETPImage* SleepIcon;

	static FETPImage* KeyDownIcon;
	static FETPImage* KeyUpIcon;

	static FETPImage* MoveMouseIcon;
	static FETPImage* LeftMouseDownIcon;
	static FETPImage* LeftMouseUpIcon;
	static FETPImage* RightMouseDownIcon;
	static FETPImage* RightMouseUpIcon;
	static FETPImage* ScrollMouseIcon;

	static FETPImage* LunchApplicationIcon;
	static FETPImage* ScreenshotIcon;

	void ShowTooltip();
	void CheckIcons();
	void RenderIcon();

	void CopyData(FETPAction* Src);
public:
	globalActionNode();
	globalActionNode(const globalActionNode& Src);
	globalActionNode(FETPAction* Data);

	~globalActionNode();

	void Draw();
	FETPAction* GetData();

	FEVisualNode* GetNextNode();
	FEVisualNode* GetLogicallyNextNode();

	Json::Value ToJson();
	void FromJson(Json::Value Json);
};

VISUAL_NODE_CHILD_AFTER_CLASS(globalActionNode)