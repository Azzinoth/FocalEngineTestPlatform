#include "LunchApplicationNode.h"
#include "../FEPTActionSystem.h"
using namespace VisNodeSys;

bool LunchApplicationNode::bIsRegistered = []()
{
	NODE_FACTORY.RegisterNodeType("LunchApplicationNode",
		[]() -> Node* {
			return new LunchApplicationNode();
		},

		[](const Node& CurrentNode) -> Node* {
			const LunchApplicationNode& NodeToCopy = static_cast<const LunchApplicationNode&>(CurrentNode);
			return new LunchApplicationNode(NodeToCopy);
		}
	);

	return true;
}();

LunchApplicationNode::LunchApplicationNode() : BaseExecutionFlowNode()
{
	Type = "LunchApplicationNode";

	SetStyle(DEFAULT);

	SetSize(ImVec2(250, 200));
	SetName("Lunch Application");

	TitleBackgroundColor = ImColor(31, 117, 208);
	TitleBackgroundColorHovered = ImColor(35, 145, 255);

	AddSocket(new NodeSocket(this, "EXECUTE", "", NodeSocket::SocketFlow::Output));
}

LunchApplicationNode::LunchApplicationNode(const LunchApplicationNode& Other) : BaseExecutionFlowNode(Other)
{
	SetStyle(DEFAULT);
	Data = Other.Data;
}

Json::Value LunchApplicationNode::ToJson()
{
	Json::Value Result = Node::ToJson();

	Result["Path"] = Data;

	return Result;
}

bool LunchApplicationNode::FromJson(Json::Value Json)
{
	bool bResult = Node::FromJson(Json);
	if (!bResult)
		return false;

	if (!Json.isMember("Path") || !Json["Path"].isString())
		return false;

	Data = Json["Path"].asString();

	return true;
}

bool LunchApplicationNode::SetPath(std::string Path)
{
	Data = Path;
	return true;
}

void LunchApplicationNode::Draw()
{	
	Node::Draw();
}

void LunchApplicationNode::SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, NODE_SOCKET_EVENT EventType)
{
	Node::SocketEvent(OwnSocket,  ConnectedSocket, EventType);

	if (EventType == EXECUTE)
	{
		ShellExecuteA(NULL, NULL, Data.c_str(), NULL, FocalEngine::FILE_SYSTEM.GetDirectoryPath(Data.c_str()).c_str(), SW_NORMAL);

		if (Output[0]->GetConnectedSockets().size() > 0)
			ParentArea->TriggerSocketEvent(Output[0], Output[0]->GetConnectedSockets()[0], EXECUTE);
	}
}

bool LunchApplicationNode::CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser)
{
	if (!Node::CanConnect(OwnSocket, CandidateSocket, nullptr))
		return false;

	return true;
}