#include "FailTestNode.h"
#include "../../FEPTActionSystem.h"
using namespace VisNodeSys;

bool FailTestNode::bIsRegistered = []()
{
	auto Creator = []() -> Node* {
		return new FailTestNode();
	};

	auto Copier = [](const Node& CurrentNode) -> Node* {
		const FailTestNode& NodeToCopy = static_cast<const FailTestNode&>(CurrentNode);
		return new FailTestNode(NodeToCopy);
	};

	NODE_FACTORY.RegisterNodeType("FailTestNode", Creator, Copier);
	return true;
}();

FailTestNode::FailTestNode() : BaseExecutionFlowNode()
{
	Type = "FailTestNode";

	SetStyle(DEFAULT);

	SetSize(ImVec2(220, 78));
	SetName("Fail Test");

	TitleBackgroundColor = ImColor(208, 31, 31);
	TitleBackgroundColorHovered = ImColor(255, 60, 60);

	AddSocket(new NodeSocket(this, "STRING", "Reason", NodeSocket::SocketFlow::Input));
}

FailTestNode::FailTestNode(const FailTestNode& Other) : BaseExecutionFlowNode(Other)
{
	SetStyle(DEFAULT);
}

void FailTestNode::Draw()
{
	Node::Draw();
}

void FailTestNode::SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, NODE_SOCKET_EVENT EventType)
{
	Node::SocketEvent(OwnSocket, ConnectedSocket, EventType);

	if (EventType == EXECUTE)
	{
		std::string Reason;
		if (Input.size() > 1 && Input[1]->GetConnectedSockets().size() > 0)
		{
			void* TemporaryData = Input[1]->GetConnectedSockets()[0]->GetData();
			if (TemporaryData != nullptr)
				Reason = *reinterpret_cast<std::string*>(TemporaryData);
		}

		ACTION_SYSTEM.MarkCurrentTestFailed(Reason);
	}
}

bool FailTestNode::CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser)
{
	if (!Node::CanConnect(OwnSocket, CandidateSocket, nullptr))
		return false;

	return true;
}