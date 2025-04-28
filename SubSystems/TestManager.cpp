#include "TestManager.h"
using namespace VisNodeSys;

VisNodeSys::Node* TestManager::FoundNode = nullptr;
FETPAction* TestManager::SearchedAction = nullptr;

TestManager::TestManager()
{
	
}

TestManager::~TestManager()
{

}

void TestManager::AddTest(std::string FilePath)
{
	FETest* NewTest = new FETest();
	if (FilePath != "")
	{
		NewTest->SetName(FocalEngine::FILE_SYSTEM.GetFileName(FilePath.c_str()));
		NewTest->FilePath = FilePath;

		NewTest->Load();
	}
	else
	{
		NewTest->SetName(GetNewTestName());
	}

	Tests.push_back(NewTest);
}

//VisNodeSys::Node* TestManager::GetNodeByAction(FETPAction* Action)
//{
//	if (Action == nullptr)
//		return nullptr;
//
//	FoundNode = nullptr;
//	SearchedAction = Action;
//
//	for (size_t i = 0; i < Tests.size(); i++)
//	{
//		if (Tests[i]->NodeArea == nullptr)
//			continue;
//
//		Tests[i]->NodeArea->RunOnEachNode([](VisNodeSys::Node* Node) {
//			if (Node->GetType() == "GlobalActionNode")
//			{
//				GlobalActionNode* ActionNode = reinterpret_cast<GlobalActionNode*>(Node);
//				if (ActionNode->GetData()->GetID() == SearchedAction->GetID())
//				{
//					FoundNode = Node;
//				}
//			}
//			else if (Node->GetType() == "CombinedActionNode")
//			{
//				CombinedActionNode* ActionNode = reinterpret_cast<CombinedActionNode*>(Node);
//
//				std::vector<FETPAction*> ActionList = ActionNode->GetData();
//				for (size_t j = 0; j < ActionList.size(); j++)
//				{
//					if (ActionList[j]->GetID() == SearchedAction->GetID())
//					{
//						FoundNode = Node;
//					}
//				}
//			}
//		});
//
//		if (FoundNode != nullptr)
//		{
//			SearchedAction = nullptr;
//			return FoundNode;
//		}
//			
//	}
//
//	SearchedAction = nullptr;
//	return nullptr;
//}

//FETest* TestManager::GetTestByAction(FETPAction* Action)
//{
//	if (Action == nullptr)
//		return nullptr;
//
//	FoundNode = nullptr;
//	SearchedAction = Action;
//
//	for (size_t i = 0; i < Tests.size(); i++)
//	{
//		if (Tests[i]->NodeArea == nullptr)
//			continue;
//
//		Tests[i]->NodeArea->RunOnEachNode([](VisNodeSys::Node* Node) {
//			if (Node->GetType() == "GlobalActionNode")
//			{
//				GlobalActionNode* actionNode = reinterpret_cast<GlobalActionNode*>(Node);
//				if (actionNode->GetData()->GetID() == SearchedAction->GetID())
//				{
//					FoundNode = Node;
//				}
//			}
//			else if (Node->GetType() == "CombinedActionNode")
//			{
//				CombinedActionNode* ActionNode = reinterpret_cast<CombinedActionNode*>(Node);
//
//				std::vector<FETPAction*> ActionList = ActionNode->GetData();
//				for (size_t j = 0; j < ActionList.size(); j++)
//				{
//					if (ActionList[j]->GetID() == SearchedAction->GetID())
//					{
//						FoundNode = Node;
//					}
//				}
//			}
//		});
//
//		if (FoundNode != nullptr)
//		{
//			FoundNode = nullptr;
//			SearchedAction = nullptr;
//			return Tests[i];
//		}
//	}
//
//	SearchedAction = nullptr;
//	return nullptr;
//}

std::string TestManager::GetNewTestName()
{
	std::string NewName = "New test_";
	size_t Index = 0;

	while (!IsTestNameFree(NewName + std::to_string(Index)))
		Index++;
	
	return NewName + std::to_string(Index);
}

bool TestManager::IsTestNameFree(std::string Name)
{
	for (size_t i = 0; i < Tests.size(); i++)
	{
		if (Tests[i]->GetName() == Name)
			return false;
	}

	return true;
}

void TestManager::SetSelectedTestIndex(size_t Index)
{
	if (Index < Tests.size())
		SelectedTestIndex = Index;
}

size_t TestManager::GetSelectedTestIndex()
{
	return SelectedTestIndex;
}

FETest* TestManager::GetSelectedTest()
{
	if (SelectedTestIndex < Tests.size())
		return Tests[SelectedTestIndex];

	return nullptr;
}

void TestManager::RemoveTest(FETest* Test)
{
	for (size_t i = 0; i < Tests.size(); i++)
	{
		if (Tests[i] == Test)
			RemoveTest(i);
	}
}

void TestManager::RemoveTest(size_t TestIndex)
{
	if (TestIndex >= Tests.size())
		return;

	if (SelectedTestIndex == TestIndex)
		SelectedTestIndex = 0;

	Tests.erase(Tests.begin() + TestIndex);
}

void TestManager::RenameTest(FETest* Test, std::string NewName)
{
	for (size_t i = 0; i < Tests.size(); i++)
	{
		if (Tests[i] == Test)
		{
			if (IsTestNameFree(NewName))
				Tests[i]->SetName(NewName);

			return;
		}
	}
}

void TestManager::RenameTest(size_t testIndex, std::string NewName)
{
	if (testIndex >= Tests.size())
		return;

	if (IsTestNameFree(NewName))
		Tests[testIndex]->SetName(NewName);
}

void TestManager::SaveAsTestSet(std::string FilePath)
{
	if (Tests.size() == 0)
		return;

	Json::Value Root;
	std::ofstream SaveFile;

	std::string FileNameWithoutExtension = FocalEngine::FILE_SYSTEM.GetFileName(FilePath.c_str());
	std::string DirectoryPath = FocalEngine::FILE_SYSTEM.GetDirectoryPath(FilePath.c_str());
	SaveFile.open(DirectoryPath + FileNameWithoutExtension + ".fetests");

	Root["name"] = "noName";

	Json::Value TestsData;
	for (size_t i = 0; i < Tests.size(); i++)
	{
		TestsData[std::to_string(i)]["path"] = Tests[i]->FilePath;
		TestsData[std::to_string(i)]["name"] = Tests[i]->GetName();
	}
	Root["tests"] = TestsData;

	Json::StreamWriterBuilder Builder;
	const std::string JsonFile = Json::writeString(Builder, Root);

	SaveFile << JsonFile;
	SaveFile.close();
}

void TestManager::OpenTestSet(std::string FilePath)
{
	if (FilePath == "")
		return;

	std::ifstream TestSetFile;
	TestSetFile.open(FilePath);

	std::string FileData((std::istreambuf_iterator<char>(TestSetFile)), std::istreambuf_iterator<char>());
	TestSetFile.close();

	Json::Value Root;
	JSONCPP_STRING Error;
	Json::CharReaderBuilder Builder;

	const std::unique_ptr<Json::CharReader> Reader(Builder.newCharReader());
	if (!Reader->parse(FileData.c_str(), FileData.c_str() + FileData.size(), &Root, &Error))
		return;

	std::vector<Json::String> TestList = Root["tests"].getMemberNames();
	for (size_t i = 0; i < TestList.size(); i++)
	{
		std::string OriginalPath = FocalEngine::FILE_SYSTEM.GetDirectoryPath(Root["tests"][TestList[i].c_str()]["path"].asCString());

		std::string OriginalDirectory = "";
		int SeenDelimiterCount = 0;
		for (size_t j = OriginalPath.size() - 1; j > 0; j--)
		{
			if (SeenDelimiterCount == 1)
				OriginalDirectory.insert(OriginalDirectory.begin(), OriginalPath[j]);

			if (OriginalPath[j] == '\\')
			{
				SeenDelimiterCount++;
				if (SeenDelimiterCount == 2)
				{
					OriginalDirectory.erase(OriginalDirectory.begin());
					break;
				}
			}
		}

		std::string LocalPath = FocalEngine::FILE_SYSTEM.GetDirectoryPath(FilePath.c_str());
		LocalPath += OriginalDirectory;
		LocalPath += "\\";
		LocalPath += FocalEngine::FILE_SYSTEM.GetFileName(Root["tests"][TestList[i].c_str()]["path"].asCString());
		LocalPath += ".fetp";

		TEST_MANAGER.AddTest(LocalPath);
	}
}