#pragma once
#include "../../../NodeSystem/VisualNodeSystem/VisualNodeSystem.h"

namespace tesseract { class TessBaseAPI; }

class RecognizeTextNode : public BaseExecutionFlowNode
{
	friend class NodeFactory;
	static bool bIsRegistered;

	bool CanConnect(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* CandidateSocket, char** MsgToUser);
	void SocketEvent(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* ConnectedSocket, VisNodeSys::NODE_SOCKET_EVENT EventType);

	static tesseract::TessBaseAPI& GetSharedRecognizer();
	static bool bRecognizerInitialized;

	int PageSegmentationMode = 6; // PSM_SINGLE_BLOCK
	std::string RecognizedText = "";

	std::function<void* ()> TextDataGetter = [this]() -> void* {
		return &RecognizedText;
	};

public:
	RecognizeTextNode();
	RecognizeTextNode(const RecognizeTextNode& Other);

	Json::Value ToJson();
	bool FromJson(Json::Value Json);

	void Draw();
};