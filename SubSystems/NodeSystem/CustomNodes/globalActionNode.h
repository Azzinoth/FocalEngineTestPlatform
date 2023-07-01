#pragma once

#include "../VisualNodeSystem/VisualNodeSystem.h"
#include "../SubSystems/FETPInput.h"
#include "../Windows/screenshootEditor.h"
#include "../Windows/textInputPopup.h"
#include "../Windows/actionEditPopup.h"

class globalActionNode : public basicLogicNode
{
	friend class NodeFactory;
	static bool isRegistered;

	FETPAction* Data;
	void Initialize(FETPAction* Data);

	bool ContextMenu = false;
	bool OpenContextMenu();

	bool CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser);
	void SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, VISUAL_NODE_SOCKET_EVENT EventType);

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

	basicLogicNode* GetNextNode();

	Json::Value ToJson();
	void FromJson(Json::Value Json);
};