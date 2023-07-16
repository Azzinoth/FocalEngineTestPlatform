#include "imageSearchNode.h"

bool imageSearchNode::isRegistered = []()
{
	NODE_FACTORY.RegisterNodeType("imageSearchNode",
		[]() -> VisualNode* {
			return new imageSearchNode();
		},

		[](const VisualNode& Node) -> VisualNode* {
			const imageSearchNode& NodeToCopy = static_cast<const imageSearchNode&>(Node);
			return new imageSearchNode(NodeToCopy);
		}
	);

	return true;
}();

imageSearchNode::imageSearchNode() : basicLogicNode()
{
	Type = "imageSearchNode";

	SetStyle(VISUAL_NODE_STYLE_DEFAULT);

	SetSize(ImVec2(290, 220));
	SetName("image search node");

	TitleBackgroundColor = ImColor(31, 117, 208);
	TitleBackgroundColorHovered = ImColor(35, 145, 255);

	AddSocket(new NodeSocket(this, "EXECUTE", "", false));
	AddSocket(new NodeSocket(this, "IMAGE", "Image", false));
	AddSocket(new NodeSocket(this, "FLOAT", "Simularity", false));
	AddSocket(new NodeSocket(this, "INT", "Color shift", false));

	AddSocket(new NodeSocket(this, "EXECUTE", "Out", true));
	AddSocket(new NodeSocket(this, "BOOL", "Found", true));
	Output[1]->SetFunctionToOutputData(BoolDataGetter);
	AddSocket(new NodeSocket(this, "VEC2", "Position", true));
	Output[2]->SetFunctionToOutputData(Vec2DataGetter);
}

imageSearchNode::imageSearchNode(const imageSearchNode& Src) : basicLogicNode(Src)
{
	SetStyle(VISUAL_NODE_STYLE_DEFAULT);

	Simularity = Src.Simularity;
	MaxColorShift = Src.MaxColorShift;

	// Here I am restoring the output data function.
	// Because the function is not serializable, I have to set it manually.
	Output[1]->SetFunctionToOutputData(BoolDataGetter);
	Output[2]->SetFunctionToOutputData(Vec2DataGetter);
}

Json::Value imageSearchNode::ToJson()
{
	Json::Value Result = VisualNode::ToJson();

	if (Input[2]->GetConnections().empty())
		Result["Simularity"] = Simularity;
	
	if (Input[3]->GetConnections().empty())
		Result["MaxColorShift"] = MaxColorShift;

	return Result;
}

void imageSearchNode::FromJson(Json::Value Json)
{
	VisualNode::FromJson(Json);

	if (Json.isMember("Simularity"))
		Simularity = Json["Simularity"].asFloat();

	if (Json.isMember("MaxColorShift"))
		MaxColorShift = Json["MaxColorShift"].asInt();

	// Here I am restoring the output data function.
	// Because the function is not serializable, I have to set it manually.
	Output[1]->SetFunctionToOutputData(BoolDataGetter);
	Output[2]->SetFunctionToOutputData(Vec2DataGetter);
}

void imageSearchNode::Draw()
{	
	VisualNode::Draw();

	int xPosition = ImGui::GetCursorScreenPos().x + 115.0f;
	int yPosition = ImGui::GetCursorScreenPos().y + 139.0f;

	ImGui::BeginDisabled(Input[2]->GetConnections().size() != 0);
	ImGui::SetCursorScreenPos(ImVec2(xPosition, yPosition));
	ImGui::SetNextItemWidth(35.0f);
	ImGui::DragFloat("##Simularity", &Simularity, 0.1f, 0.0f, 100.0f);
	if (Simularity > 100.0f)
		Simularity = 100.0f;

	if (Simularity < 0.0f)
		Simularity = 0.0f;

	ImGui::EndDisabled();

	xPosition += 7;
	yPosition += 48;
	ImGui::SetCursorScreenPos(ImVec2(xPosition, yPosition));
	ImGui::SetNextItemWidth(35.0f);
	ImGui::DragInt("##MaxColorShift", &MaxColorShift, 0.1f, 0.0f, 100.0f);
	if (MaxColorShift < 0)
		MaxColorShift = 0;
}

void imageSearchNode::SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, VISUAL_NODE_SOCKET_EVENT EventType)
{
	VisualNode::SocketEvent(OwnSocket,  ConnectedSocket, EventType);

	if (EventType == VISUAL_NODE_SOCKET_UPDATE)
	{
		if (Input[2]->GetConnections().size() > 0)
		{
			void* TempData = Input[2]->GetConnections()[0]->GetData();
			if (TempData != nullptr)
				Simularity = *reinterpret_cast<float*>(TempData);
		}

		if (Input[3]->GetConnections().size() > 0)
		{
			void* TempData = Input[3]->GetConnections()[0]->GetData();
			if (TempData != nullptr)
				MaxColorShift = *reinterpret_cast<int*>(TempData);
		}
	}

	if (EventType == VISUAL_NODE_SOCKET_EXECUTE)
	{
		FETPImage* ImageToLookFor = nullptr;

		if (Input[1]->GetConnections().size() > 0)
		{
			void* TempData = Input[1]->GetConnections()[0]->GetData();
			if (TempData != nullptr)
			{
				ImageToLookFor = reinterpret_cast<FETPImage*>(TempData);

				std::vector<unsigned char> tempScreenshoot;
				tempScreenshoot.resize(ImageToLookFor->getWidth() * ImageToLookFor->getHeight() * 4);

				std::vector<unsigned char> tempDifferenceData;
				tempDifferenceData.resize(tempScreenshoot.size());

				FETPImage* TestScreenShoot = nullptr;
				TestScreenShoot = SCREEN_SYSTEM.GetScreenDataAsImage();

				int similarity = 0;
				size_t x = 0;
				size_t y = 0;

				bool found = false;
				glm::vec2 Position = COMPUTE_SHADER_COMPARE.FindSubImage(TestScreenShoot, ImageToLookFor, Simularity, MaxColorShift);
				found = Position.x != -1 && Position.y != -1;
				x = Position.x;
				y = Position.y;

				FoundPosition = Position;
				bFound = found;

				if (found)
				{
					int y = 0;
					y++;
				}

				delete TestScreenShoot;

				if (Output[0]->GetConnections().size() > 0)
					ParentArea->TriggerSocketEvent(Output[0], Output[0]->GetConnections()[0], VISUAL_NODE_SOCKET_EXECUTE);
			}
		}
	}
}

bool imageSearchNode::CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser)
{
	if (!VisualNode::CanConnect(OwnSocket, CandidateSocket, nullptr))
		return false;

	return true;
}

basicLogicNode* imageSearchNode::GetNextNode()
{
	return nullptr;
}