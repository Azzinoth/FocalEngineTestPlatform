#include "RecognizeTextNode.h"
#include "../../FETPImage.h"
#include "tesseract/baseapi.h"
#include "tesseract/capi.h"
using namespace VisNodeSys;

bool RecognizeTextNode::bRecognizerInitialized = false;

tesseract::TessBaseAPI& RecognizeTextNode::GetSharedRecognizer()
{
	static tesseract::TessBaseAPI Instance;
	return Instance;
}

bool RecognizeTextNode::bIsRegistered = []()
{
	auto Creator = []() -> Node* {
		return new RecognizeTextNode();
	};

	auto Copier = [](const Node& CurrentNode) -> Node* {
		const RecognizeTextNode& NodeToCopy = static_cast<const RecognizeTextNode&>(CurrentNode);
		return new RecognizeTextNode(NodeToCopy);
	};

	NODE_FACTORY.RegisterNodeType("RecognizeTextNode", Creator, Copier);
	return true;
}();

RecognizeTextNode::RecognizeTextNode() : BaseExecutionFlowNode()
{
	Type = "RecognizeTextNode";

	SetStyle(DEFAULT);
	SetName("Recognize Text");

	TitleBackgroundColor = ImColor(31, 117, 208);
	TitleBackgroundColorHovered = ImColor(35, 145, 255);

	AddSocket(new NodeSocket(this, "IMAGE", "Image", NodeSocket::SocketFlow::Input));

	AddSocket(new NodeSocket(this, "EXECUTE", "", NodeSocket::SocketFlow::Output));
	AddSocket(new NodeSocket(this, "STRING", "Text", NodeSocket::SocketFlow::Output));

	SetSize(ImVec2(260, 130));
	Output[1]->SetFunctionToOutputData(TextDataGetter);
}

RecognizeTextNode::RecognizeTextNode(const RecognizeTextNode& Other) : BaseExecutionFlowNode(Other)
{
	SetStyle(DEFAULT);
	PageSegmentationMode = Other.PageSegmentationMode;
	RecognizedText = Other.RecognizedText;

	// Here I am restoring the output data function.
	// Because the function is not serializable, I have to set it manually.
	Output[1]->SetFunctionToOutputData(TextDataGetter);
}

Json::Value RecognizeTextNode::ToJson()
{
	Json::Value Result = Node::ToJson();
	Result["PageSegmentationMode"] = PageSegmentationMode;
	return Result;
}

bool RecognizeTextNode::FromJson(Json::Value Json)
{
	bool bResult = Node::FromJson(Json);
	if (!bResult)
		return false;

	if (Json.isMember("PageSegmentationMode") && Json["PageSegmentationMode"].isNumeric())
		PageSegmentationMode = Json["PageSegmentationMode"].asInt();

	if (Output.size() < 2 || Output[1] == nullptr)
		return false;

	Output[1]->SetFunctionToOutputData(TextDataGetter);
	return true;
}

void RecognizeTextNode::Draw()
{
	Node::Draw();

	static const char* ModeNames[] = { "Auto", "Single Block", "Single Line", "Single Word", "Single Char", "Sparse Text" };
	static const int ModeValues[] = { 3, 6, 7, 8, 10, 11 };
	const int ModeCount = IM_ARRAYSIZE(ModeNames);

	int CurrentIndex = 1;
	for (int i = 0; i < ModeCount; i++)
	{
		if (ModeValues[i] == PageSegmentationMode)
		{
			CurrentIndex = i;
			break;
		}
	}

	float Zoom = ParentArea->GetZoomFactor();
	ImVec2 OriginPosition = ImGui::GetCursorScreenPos();
	ImGui::SetCursorScreenPos(ImVec2(OriginPosition.x + 25.0f * Zoom, OriginPosition.y + 40.0f * Zoom));
	ImGui::SetNextItemWidth(160.0f * Zoom);
	if (ImGui::Combo(("##PSM" + GetID()).c_str(), &CurrentIndex, ModeNames, ModeCount))
		PageSegmentationMode = ModeValues[CurrentIndex];
}

void RecognizeTextNode::SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, NODE_SOCKET_EVENT EventType)
{
	Node::SocketEvent(OwnSocket, ConnectedSocket, EventType);

	if (EventType == EXECUTE)
	{
		RecognizedText = "";

		FETPImage* InputImage = nullptr;
		if (Input[1]->GetConnectedSockets().size() > 0)
		{
			void* TemporaryData = Input[1]->GetConnectedSockets()[0]->GetData();
			if (TemporaryData != nullptr)
				InputImage = reinterpret_cast<FETPImage*>(TemporaryData);
		}

		if (InputImage != nullptr)
		{
			tesseract::TessBaseAPI& Recognizer = GetSharedRecognizer();
			if (!bRecognizerInitialized)
			{
				if (Recognizer.Init("ThirdParty/tesseract/traineddata", "eng") == 0)
					bRecognizerInitialized = true;
			}

			if (bRecognizerInitialized)
			{
				unsigned char* RawData = InputImage->GetRawData();
				int Width = InputImage->GetWidth();
				int Height = InputImage->GetHeight();

				if (RawData != nullptr && Width > 0 && Height > 0)
				{
					Recognizer.SetPageSegMode(static_cast<tesseract::PageSegMode>(PageSegmentationMode));
					Recognizer.SetImage(RawData, Width, Height, 4, Width * 4);
					char* TextPointer = Recognizer.GetUTF8Text();
					if (TextPointer != nullptr)
					{
						RecognizedText = TextPointer;
						TessDeleteText(TextPointer);
					}
				}
			}
		}

		if (Output[0]->GetConnectedSockets().size() > 0)
			ParentArea->TriggerSocketEvent(Output[0], Output[0]->GetConnectedSockets()[0], EXECUTE);
	}
}

bool RecognizeTextNode::CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser)
{
	if (!Node::CanConnect(OwnSocket, CandidateSocket, nullptr))
		return false;

	return true;
}