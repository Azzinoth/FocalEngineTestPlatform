#include "ImageVariableNode.h"
using namespace VisNodeSys;

bool ImageVariableNode::bIsRegistered = []()
{
	auto Creator = []() -> Node* {
		return new ImageVariableNode();
	};

	auto Copier = [](const Node& CurrentNode) -> Node* {
		const ImageVariableNode& NodeToCopy = static_cast<const ImageVariableNode&>(CurrentNode);
		return new ImageVariableNode(NodeToCopy);
	};

	NODE_FACTORY.RegisterNodeType("ImageVariableNode", Creator, Copier);
	return true;
}();

ImageVariableNode::ImageVariableNode() : BaseExecutionFlowNode()
{
	Type = "ImageVariableNode";

	SetStyle(DEFAULT);
	SetName("Image Variable");

	TitleBackgroundColor = ImColor(31, 117, 208);
	TitleBackgroundColorHovered = ImColor(35, 145, 255);

	AddSocket(new NodeSocket(this, "IMAGE", "", NodeSocket::SocketFlow::Input));

	AddSocket(new NodeSocket(this, "EXECUTE", "", NodeSocket::SocketFlow::Output));
	AddSocket(new NodeSocket(this, "IMAGE", "", NodeSocket::SocketFlow::Output));
	AddSocket(new NodeSocket(this, "VEC2", "Size", NodeSocket::SocketFlow::Output));

	SetSize(ImVec2(250, 220));
	Output[1]->SetFunctionToOutputData(ImageDataGetter);
	Output[2]->SetFunctionToOutputData(ImageSizeGetter);
}

ImageVariableNode::ImageVariableNode(const ImageVariableNode& Other) : BaseExecutionFlowNode(Other)
{
	SetStyle(DEFAULT);
	Data = Other.Data;
	ImageSize = Other.ImageSize;

	// Here I am restoring the output data function.
	// Because the function is not serializable, I have to set it manually.
	Output[1]->SetFunctionToOutputData(ImageDataGetter);
	Output[2]->SetFunctionToOutputData(ImageSizeGetter);
}

Json::Value ImageVariableNode::ToJson()
{
	return Node::ToJson();
}

bool ImageVariableNode::FromJson(Json::Value Json)
{
	bool bResult = Node::FromJson(Json);
	if (!bResult)
		return false;

	if (Output.size() < 3 || Output[1] == nullptr || Output[2] == nullptr)
		return false;

	Output[1]->SetFunctionToOutputData(ImageDataGetter);
	Output[2]->SetFunctionToOutputData(ImageSizeGetter);
	return true;
}

void ImageVariableNode::Draw()
{
	Node::Draw();

	float Zoom = ParentArea->GetZoomFactor();

	if (Data == nullptr)
	{
		const char* Label = "(NO IMAGE)";
		float TextWidth = ImGui::CalcTextSize(Label).x;
		float CenterOffsetX = (GetSize().x * Zoom - TextWidth) * 0.5f;
		ImGui::SetCursorScreenPos(ImVec2(ImGui::GetCursorScreenPos().x + CenterOffsetX, ImGui::GetCursorScreenPos().y + 115.0f * Zoom));
		ImGui::Text("%s", Label);
	}
	else
	{
		float CenterOffsetX = (GetSize().x - 128.0f) * 0.5f;
		ImGui::SetCursorScreenPos(ImVec2(ImGui::GetCursorScreenPos().x + CenterOffsetX * Zoom, ImGui::GetCursorScreenPos().y + 55.0f * Zoom));
		ImGui::Image(Data->GetTextureID(), ImVec2(128.0f, 128.0f) * Zoom, ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f));
	}
}

void ImageVariableNode::SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, NODE_SOCKET_EVENT EventType)
{
	Node::SocketEvent(OwnSocket, ConnectedSocket, EventType);

	if (EventType == EXECUTE)
	{
		if (Input[1]->GetConnectedSockets().size() > 0)
		{
			void* TemporaryData = Input[1]->GetConnectedSockets()[0]->GetData();
			if (TemporaryData != nullptr)
			{
				Data = reinterpret_cast<FETPImage*>(TemporaryData);
				ImageSize = glm::vec2(static_cast<float>(Data->GetWidth()), static_cast<float>(Data->GetHeight()));
			}
		}

		if (Output[0]->GetConnectedSockets().size() > 0)
			ParentArea->TriggerSocketEvent(Output[0], Output[0]->GetConnectedSockets()[0], EXECUTE);
	}
}

bool ImageVariableNode::CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser)
{
	if (!Node::CanConnect(OwnSocket, CandidateSocket, nullptr))
		return false;

	return true;
}