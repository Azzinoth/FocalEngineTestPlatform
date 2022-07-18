#pragma once

#include "globalActionNode.h"

class testEditorWinow;
class nodeRegionWindow;
VISUAL_NODE_CHILD_PRIVATE_PART(combinedActionNode)
	friend testEditorWinow;
	friend nodeRegionWindow;

	std::vector<FETPAction*> Data;
	void Initialize(std::vector<FETPAction*> Data, FETP_COMBINED_ACTION_TYPE Type);
	FETP_COMBINED_ACTION_TYPE ActionType;

	glm::vec2 BeginPosition;
	glm::vec2 EndPosition;

	std::string Text;

	//bool contextMenu = false;
	//bool openContextMenu();

	bool CanConnect(FEVisualNodeSocket* OwnSocket, FEVisualNodeSocket* CandidateSocket, char** MsgToUser);
	void SocketEvent(FEVisualNodeSocket* OwnSocket, FEVisualNodeSocket* ConnectedSocket, FE_VISUAL_NODE_SOCKET_EVENT EventType);

	static combinedActionNode* NodeForCallback;
	static void ChangeTextCallback(std::string NewText);

	static FETPImage* MoveMouseCombineIcon;
	static FETPImage* LeftMouseCombineIcon;
	static FETPImage* RightMouseCombineIcon;
	static FETPImage* WheelMouseCombineIcon;

	static FETPImage* KeyCombinedIcon;
	static FETPImage* TextCombinedIcon;

	void ShowTooltip();
	void CheckIcons();
	void RenderIcon();
public:
	combinedActionNode();
	combinedActionNode(const combinedActionNode& Src);
	combinedActionNode(std::vector<FETPAction*> Data, FETP_COMBINED_ACTION_TYPE Type);

	void Draw();
	std::vector<FETPAction*> GetData();
	FEVisualNode* GetNextNode();

	Json::Value ToJson();
	void FromJson(Json::Value Json);

	void ChangeText(std::string NewText, int AvarageDelay = -1);
	FETP_COMBINED_ACTION_TYPE GetCombinedActionType();
	void Remove();
};

VISUAL_NODE_CHILD_AFTER_CLASS(combinedActionNode)