#include "LaunchApplicationNode.h"
#include "../../FEPTActionSystem.h"
using namespace VisNodeSys;

bool LaunchApplicationNode::bIsRegistered = []()
{
	auto Creator = []() -> Node* {
		return new LaunchApplicationNode();
	};

	auto Copier = [](const Node& CurrentNode) -> Node* {
		const LaunchApplicationNode& NodeToCopy = static_cast<const LaunchApplicationNode&>(CurrentNode);
		return new LaunchApplicationNode(NodeToCopy);
	};

	NODE_FACTORY.RegisterNodeType("LaunchApplicationNode", Creator, Copier);
	return true;
}();

LaunchApplicationNode::LaunchApplicationNode() : BaseExecutionFlowNode()
{
	Type = "LaunchApplicationNode";

	SetStyle(DEFAULT);

	SetSize(ImVec2(350, 180));
	SetName("Launch Application");

	TitleBackgroundColor = ImColor(31, 117, 208);
	TitleBackgroundColorHovered = ImColor(35, 145, 255);

	AddSocket(new NodeSocket(this, "STRING", "Path", NodeSocket::SocketFlow::Input));

	AddSocket(new NodeSocket(this, "EXECUTE", "", NodeSocket::SocketFlow::Output));
	AddSocket(new NodeSocket(this, "STRING", "Path", NodeSocket::SocketFlow::Output));

	Output[1]->SetFunctionToOutputData(DataGetter);
}

LaunchApplicationNode::LaunchApplicationNode(const LaunchApplicationNode& Other) : BaseExecutionFlowNode(Other)
{
	SetStyle(DEFAULT);
	Data = Other.Data;

	// Here I am restoring the output data function.
	// Because the function is not serializable, I have to set it manually.
	Output[1]->SetFunctionToOutputData(DataGetter);
}

Json::Value LaunchApplicationNode::ToJson()
{
	Json::Value Result = Node::ToJson();

	Result["Path"] = Data;

	return Result;
}

bool LaunchApplicationNode::FromJson(Json::Value Json)
{
	bool bResult = Node::FromJson(Json);
	if (!bResult)
		return false;

	if (!Json.isMember("Path") || !Json["Path"].isString())
		return false;

	Data = Json["Path"].asString();

	if (Output.size() < 2 || Output[1] == nullptr)
		return false;

	Output[1]->SetFunctionToOutputData(DataGetter);
	return true;
}

bool LaunchApplicationNode::SetPath(std::string Path)
{
	Data = Path;
	return true;
}

void LaunchApplicationNode::Draw()
{
	ImVec2 NodeLeftTop = ImGui::GetCursorScreenPos();
	Node::Draw();

	float Zoom = ParentArea->GetZoomFactor();
	float StartX = NodeLeftTop.x + 25.0f * Zoom;
	float StartY = NodeLeftTop.y + 48.0f * Zoom;

	bool bInputOverrides = Input.size() > 1 && Input[1]->GetConnectedSockets().size() > 0;

	static char TemporaryTextBuffer[4096];
	strcpy_s(TemporaryTextBuffer, Data.c_str());

	ImGui::SetCursorScreenPos(ImVec2(StartX, StartY));
	if (bInputOverrides)
		ImGui::BeginDisabled();

	if (ImGui::InputTextMultiline("##PathInput", TemporaryTextBuffer, IM_ARRAYSIZE(TemporaryTextBuffer), ImVec2(290, 70) * Zoom, ImGuiInputTextFlags_AllowTabInput))
	{
		Data = TemporaryTextBuffer;
	}

	if (bInputOverrides)
		ImGui::EndDisabled();

	float ButtonWidth = ImGui::CalcTextSize("Browse...").x + ImGui::GetStyle().FramePadding.x * 2.0f;
	float ButtonX = StartX + (290.0f * Zoom - ButtonWidth) * 0.5f;
	ImGui::SetCursorScreenPos(ImVec2(ButtonX, StartY + 78.0f * Zoom));
	if (ImGui::Button("Browse..."))
	{
		std::string NewPath;
		std::string InitialDirectory;
		if (!Data.empty() && FocalEngine::FILE_SYSTEM.DoesFileExist(Data))
			InitialDirectory = FocalEngine::FILE_SYSTEM.GetDirectoryPath(Data);
		FocalEngine::FILE_SYSTEM.ShowFileOpenDialog(NewPath, ApplicationLoadFilter, 1, InitialDirectory);
		if (!NewPath.empty())
			SetPath(NewPath);
	}
}

void LaunchApplicationNode::SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, NODE_SOCKET_EVENT EventType)
{
	Node::SocketEvent(OwnSocket,  ConnectedSocket, EventType);

	if (EventType == EXECUTE)
	{
		if (Input[1]->GetConnectedSockets().size() > 0)
		{
			void* TemporaryData = Input[1]->GetConnectedSockets()[0]->GetData();
			if (TemporaryData != nullptr)
			{
				std::string InputPath = *reinterpret_cast<std::string*>(TemporaryData);
				if (!InputPath.empty())
					Data = InputPath;
			}
		}

		ShellExecuteA(NULL, NULL, Data.c_str(), NULL, FocalEngine::FILE_SYSTEM.GetDirectoryPath(Data.c_str()).c_str(), SW_NORMAL);

		if (Output[0]->GetConnectedSockets().size() > 0)
			ParentArea->TriggerSocketEvent(Output[0], Output[0]->GetConnectedSockets()[0], EXECUTE);
	}
}

bool LaunchApplicationNode::CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser)
{
	if (!Node::CanConnect(OwnSocket, CandidateSocket, nullptr))
		return false;

	return true;
}