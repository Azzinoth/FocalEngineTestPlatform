#include "FETest.h"
using namespace VisNodeSys;

ImColor* FETest::DefaultConnectionColor = new ImColor(200, 200, 200);
ImColor* FETest::MainPathConnectionColor = new ImColor(150, 255, 150);

FETest::FETest()
{
	DummyRootNodeArea = NODE_SYSTEM.CreateNodeArea();

	EntryPointNodeArea = NODE_SYSTEM.CreateNodeArea();
	EntryPointNodeArea->SetName("Main Node Area");
	EntryPointNodeArea->SetSaveExecutedNodes(true);

	Begin = new BeginNode();
	Begin->SetName("Begin node");
	Begin->SetPosition(ImVec2(300.0f, 430.0f));

	EntryPointNodeArea->AddNode(Begin);
}

FETest::~FETest()
{
	//delete Begin;
}

NodeArea* FETest::GetDummyRootNodeArea() const
{
	return DummyRootNodeArea;
}

void FETest::Save(const char* FilePath)
{
	NODE_SYSTEM.SaveToFile(FilePath);

	Json::Value Root;
	std::ofstream SaveFile;

	std::string FileNameWithoutExtension = FocalEngine::FILE_SYSTEM.GetFileName(FilePath, false);
	std::string DirectoryPath = FocalEngine::FILE_SYSTEM.GetDirectoryPath(FilePath);
	SaveFile.open(DirectoryPath + "/" + FileNameWithoutExtension + ".FETPTestInfo");

	Root["Name"] = Name;
	Root["DummyRootNodeArea"] = DummyRootNodeArea->GetID();
	Root["EntryPointNodeAreaID"] = EntryPointNodeArea->GetID();
	Root["BeginNodeID"] = Begin->GetID();
	Root["SpeedFactor"] = SpeedFactor;

	Json::Value BeforeStartActions;
	for (size_t i = 0; i < BeforeStart.size(); i++)
	{
		BeforeStartActions[std::to_string(i)]["Type"] = FETest::FEBeforeTestActionTypeToString(BeforeStart[i]->Type);
		BeforeStartActions[std::to_string(i)]["Path"] = BeforeStart[i]->Path;
		BeforeStartActions[std::to_string(i)]["NewObjectName"] = BeforeStart[i]->NewObjectName;
	}
	Root["BeforeStartActions"] = BeforeStartActions;

	Json::Value Macros;
	auto CurrentMacro = MacrosToReplace.begin();
	while (CurrentMacro != MacrosToReplace.end())
	{
		Macros[CurrentMacro->first] = CurrentMacro->second;
		CurrentMacro++;
	}
	Root["Macros"] = Macros;

	Json::StreamWriterBuilder Builder;
	const std::string JsonFile = Json::writeString(Builder, Root);

	SaveFile << JsonFile;
	SaveFile.close();
}

void FETest::Load()
{
	if (FilePath.empty())
		return;

	NODE_SYSTEM.Clear();
	NODE_SYSTEM.LoadFromFile(FilePath);

	// Load additional test data.
	std::ifstream FileData;
	std::string FileNameWithoutExtension = FocalEngine::FILE_SYSTEM.GetFileName(FilePath.c_str(), false);
	std::string DirectoryPath = FocalEngine::FILE_SYSTEM.GetDirectoryPath(FilePath.c_str());
	FileData.open(DirectoryPath + "/" + FileNameWithoutExtension + ".fetpTestInfo");

	std::string AdditionalFileData((std::istreambuf_iterator<char>(FileData)), std::istreambuf_iterator<char>());
	FileData.close();

	Json::Value Root;
	JSONCPP_STRING Error;
	Json::CharReaderBuilder Builder;

	const std::unique_ptr<Json::CharReader> Reader(Builder.newCharReader());
	if (!Reader->parse(AdditionalFileData.c_str(), AdditionalFileData.c_str() + AdditionalFileData.size(), &Root, &Error))
		return;

	Name = Root["Name"].asCString();
	std::string DummyRootNodeAreaID = Root["DummyRootNodeArea"].asCString();
	std::string EntryPointNodeAreaID = Root["EntryPointNodeAreaID"].asCString();
	std::string BeginNodeID = Root["BeginNodeID"].asCString();
	DummyRootNodeArea = NODE_SYSTEM.GetNodeAreaByID(DummyRootNodeAreaID);
	EntryPointNodeArea = NODE_SYSTEM.GetNodeAreaByID(EntryPointNodeAreaID);
	Begin = reinterpret_cast<BeginNode*>(EntryPointNodeArea->GetNodeByID(BeginNodeID));
	ReColorMainTestPath();
	SpeedFactor = Root["SpeedFactor"].asFloat();

	std::vector<Json::String> ActionList = Root["BeforeStartActions"].getMemberNames();
	BeforeStart.resize(ActionList.size());
	for (size_t i = 0; i < ActionList.size(); i++)
	{
		FETestBeforeAction* Action = new FETestBeforeAction();
		Action->Type = FETest::stringToFEBeforeTestActionType(Root["BeforeStartActions"][ActionList[i]]["Type"].asCString());
		Action->Path = Root["BeforeStartActions"][ActionList[i]]["Path"].asCString();
		Action->NewObjectName = Root["BeforeStartActions"][ActionList[i]]["NewObjectName"].asCString();

		BeforeStart[atoi(ActionList[i].c_str())] = Action;
	}

	std::vector<Json::String> MacrosList = Root["Macros"].getMemberNames();
	for (size_t i = 0; i < MacrosList.size(); i++)
	{
		AddMacro(MacrosList[i], Root["Macros"][MacrosList[i]].asCString());
	}
}

BeginNode* FETest::GetBeginNode()
{
	return Begin;
}

void FETest::ReColorMainTestPath()
{
	// Change style of all connections to default.
	EntryPointNodeArea->RunOnEachNode([](VisNodeSys::Node* Node) {
		size_t outSocketCount = Node->GetOutputSocketCount();
		for (size_t i = 0; i < outSocketCount; i++)
		{
			ConnectionStyle TempStyle;
			Node->GetParentArea()->GetConnectionStyle(Node, true, i, TempStyle);
			TempStyle.bMarchingAntsEffect = false;
			Node->GetParentArea()->SetConnectionStyle(Node, true, i, TempStyle);
		}
	});

	// Start at begin node at change each connection color from it.
	if (GetBeginNode() == nullptr)
		return;

	EntryPointNodeArea->RunOnEachConnectedNode(GetBeginNode(),
		[](VisNodeSys::Node* Node) {
			size_t outSocketCount = Node->GetOutputSocketCount();
			for (size_t i = 0; i < outSocketCount; i++)
			{
				ConnectionStyle TempStyle;
				Node->GetParentArea()->GetConnectionStyle(Node, true, i, TempStyle);
				TempStyle.bMarchingAntsEffect = true;
				Node->GetParentArea()->SetConnectionStyle(Node, true, i, TempStyle);
			}
		}
	);
}

void FETest::AddResult(FETestResult* NewResult)
{
	if (NewResult == nullptr)
		return;

	Results.push_back(NewResult);
}

FETestResult* FETest::GetLastTestResult()
{
	if (Results.size() == 0)
		return nullptr;

	return Results.back();
}

float FETest::GetSpeedFactor()
{
	return SpeedFactor;
}

void FETest::SetSpeedFactor(float NewValue)
{
	if (NewValue < 0.1f || NewValue > 10.0f)
		return;

	SpeedFactor = NewValue;
}

std::string FETest::GetName()
{
	return Name;
}

void FETest::SetName(std::string NewValue)
{
	Name = NewValue;
}

void FETest::AddBeforeStartAction(FETestBeforeAction* Action)
{
	BeforeStart.push_back(Action);
}

void FETest::BeforeBegin()
{
	for (size_t i = 0; i < BeforeStart.size(); i++)
	{
		if (BeforeStart[i]->Type == FE_BEFORE_TEST_ACTION_DELETE_DIRECTORY)
		{
			FocalEngine::FILE_SYSTEM.DeleteDirectory(BeforeStart[i]->Path.c_str());
		}
	}
}

void FETest::AddMacro(std::string Macro, std::string ReplaceWith)
{
	MacrosToReplace[Macro] = ReplaceWith;
}

bool FETest::ReplaceMacro(std::string& Text)
{
	bool bWasChanged = false;
	auto CurrentMacro = MacrosToReplace.begin();
	while (CurrentMacro != MacrosToReplace.end())
	{
		size_t Index = Text.find(CurrentMacro->first);
		if (Index != std::string::npos)
		{
			Text.replace(Index, CurrentMacro->first.size(), CurrentMacro->second);
			bWasChanged = true;
		}
		CurrentMacro++;
	}

	return bWasChanged;
}

int FETest::GetLoopCount()
{
	return LoopCount;
}

void FETest::SetLoopCount(int NewValue)
{
	if (NewValue <= 0)
		NewValue = 1;

	LoopCount = NewValue;
}