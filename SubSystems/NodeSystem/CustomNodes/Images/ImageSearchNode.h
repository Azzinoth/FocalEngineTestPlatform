#pragma once

#include "../../../NodeSystem/VisualNodeSystem/VisualNodeSystem.h"
#include "../../FETPInput.h"

enum class ACTION_NODE_STATUS
{
	WasNotExecuted = 0,
	Success = 1,
	Failure = 2,
	Warning = 3
};

class ImageSearchNode : public BaseExecutionFlowNode
{
	friend class NodeFactory;
	static bool bIsRegistered;

	bool CanConnect(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* CandidateSocket, char** MsgToUser);
	void SocketEvent(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* ConnectedSocket, VisNodeSys::NODE_SOCKET_EVENT EventType);

	float Similarity = 95.0f;
	int MaxColorShift = 4;
	glm::vec2 FoundPosition = glm::vec2(-1.0f);
	bool bFound = false;
	int MonitorIndex = -1;
	int FoundMonitorIndex = -1;

	struct LastSearchResult
	{
		bool bMatchFound = false;
		glm::vec2 Position = glm::vec2(-1.0f);
		float BestMatchScore = 0.0f;
		int MonitorIndex = -1;
		FETPImage* CroppedRegion = nullptr;
	};

	std::vector<LastSearchResult> LastResults;
	int BestResultIndex = -1;

	std::function<void* ()> MonitorIndexDataGetter = [this]() -> void* {
		return &FoundMonitorIndex;
	};

	std::function<void* ()> Vec2DataGetter = [this]() -> void* {
		return &FoundPosition;
	};

	std::function<void* ()> BoolDataGetter = [this]() -> void* {
		return &bFound;
	};

	std::function<void* ()> ImageDataGetter = [this]() -> void* {
		if (BestResultIndex < 0 || BestResultIndex >= int(LastResults.size()))
			return nullptr;
		return LastResults[BestResultIndex].CroppedRegion;
	};

	ACTION_NODE_STATUS Status = ACTION_NODE_STATUS::WasNotExecuted;
public:
	ImageSearchNode();
	ImageSearchNode(const ImageSearchNode& Other);
	~ImageSearchNode();

	Json::Value ToJson();
	bool FromJson(Json::Value Json);

	void Draw();

	ACTION_NODE_STATUS GetStatus() const;
	void ResetToDefaultStatus();
};