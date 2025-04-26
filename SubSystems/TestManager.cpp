#include "TestManager.h"
using namespace VisNodeSys;

VisNodeSys::Node* TestManager::foundNode = nullptr;
FETPAction* TestManager::searchedAction = nullptr;

TestManager::TestManager()
{
	
}

TestManager::~TestManager()
{

}

void TestManager::AddTest(std::string FilePath)
{
	if (FilePath != "")
	{
		FETest* newTest = new FETest();
		newTest->SetName(FocalEngine::FILE_SYSTEM.GetFileName(FilePath.c_str()));
		newTest->FilePath = FilePath;

		newTest->Load();

		list.push_back(newTest);
	}
	else
	{
		FETest* newTest = new FETest();
		newTest->SetName(GetNewTestName());

		list.push_back(newTest);
	}
}

VisNodeSys::Node* TestManager::getNodeByAction(FETPAction* action)
{
	if (action == nullptr)
		return nullptr;

	foundNode = nullptr;
	searchedAction = action;

	for (size_t i = 0; i < list.size(); i++)
	{
		if (list[i]->NodeArea == nullptr)
			continue;

		list[i]->NodeArea->RunOnEachNode([](VisNodeSys::Node* node) {
			if (node->GetType() == "globalActionNode")
			{
				globalActionNode* actionNode = reinterpret_cast<globalActionNode*>(node);
				if (actionNode->GetData()->GetID() == searchedAction->GetID())
				{
					foundNode = node;
				}
			}
			else if (node->GetType() == "combinedActionNode")
			{
				combinedActionNode* actionNode = reinterpret_cast<combinedActionNode*>(node);

				std::vector<FETPAction*> actionList = actionNode->GetData();
				for (size_t j = 0; j < actionList.size(); j++)
				{
					if (actionList[j]->GetID() == searchedAction->GetID())
					{
						foundNode = node;
					}
				}
			}
		});

		if (foundNode != nullptr)
		{
			searchedAction = nullptr;
			return foundNode;
		}
			
	}

	searchedAction = nullptr;
	return nullptr;
}

FETest* TestManager::getTestByAction(FETPAction* action)
{
	if (action == nullptr)
		return nullptr;

	foundNode = nullptr;
	searchedAction = action;

	for (size_t i = 0; i < list.size(); i++)
	{
		if (list[i]->NodeArea == nullptr)
			continue;

		list[i]->NodeArea->RunOnEachNode([](VisNodeSys::Node* node) {
			if (node->GetType() == "globalActionNode")
			{
				globalActionNode* actionNode = reinterpret_cast<globalActionNode*>(node);
				if (actionNode->GetData()->GetID() == searchedAction->GetID())
				{
					foundNode = node;
				}
			}
			else if (node->GetType() == "combinedActionNode")
			{
				combinedActionNode* actionNode = reinterpret_cast<combinedActionNode*>(node);

				std::vector<FETPAction*> actionList = actionNode->GetData();
				for (size_t j = 0; j < actionList.size(); j++)
				{
					if (actionList[j]->GetID() == searchedAction->GetID())
					{
						foundNode = node;
					}
				}
			}
		});

		if (foundNode != nullptr)
		{
			foundNode = nullptr;
			searchedAction = nullptr;
			return list[i];
		}
	}

	searchedAction = nullptr;
	return nullptr;
}

std::string TestManager::GetNewTestName()
{
	std::string NewName = "new test_";
	size_t Index = 0;

	while (!IsTestNameFree(NewName + std::to_string(Index)))
	{
		Index++;
	}

	return NewName + std::to_string(Index);
}

bool TestManager::IsTestNameFree(std::string Name)
{
	for (size_t i = 0; i < list.size(); i++)
	{
		if (list[i]->GetName() == Name)
			return false;
	}

	return true;
}

void TestManager::setSelelectedTestIndex(size_t index)
{
	if (index < list.size())
		SelelectedTestIndex = index;
}

size_t TestManager::getSelectedTestIndex()
{
	return SelelectedTestIndex;
}

FETest* TestManager::getSelectedTest()
{
	if (SelelectedTestIndex < list.size())
		return list[SelelectedTestIndex];

	return nullptr;
}

void TestManager::removeTest(FETest* test)
{
	for (size_t i = 0; i < list.size(); i++)
	{
		if (list[i] == test)
			removeTest(i);
	}
}

void TestManager::removeTest(size_t testIndex)
{
	if (testIndex >= list.size())
		return;

	if (SelelectedTestIndex == testIndex)
		SelelectedTestIndex = 0;

	list.erase(list.begin() + testIndex);
}

void TestManager::renameTest(FETest* test, std::string newName)
{
	for (size_t i = 0; i < list.size(); i++)
	{
		if (list[i] == test)
		{
			if (IsTestNameFree(newName))
				list[i]->SetName(newName);

			return;
		}
	}
}

void TestManager::renameTest(size_t testIndex, std::string newName)
{
	if (testIndex >= list.size())
		return;

	if (IsTestNameFree(newName))
		list[testIndex]->SetName(newName);
}

void TestManager::saveAsTestSet(std::string FilePath)
{
	if (list.size() == 0)
		return;

	Json::Value root;
	std::ofstream saveFile;

	std::string fileNameWithoutExtension = FocalEngine::FILE_SYSTEM.GetFileName(FilePath.c_str());
	std::string directoryPath = FocalEngine::FILE_SYSTEM.GetDirectoryPath(FilePath.c_str());
	saveFile.open(directoryPath + fileNameWithoutExtension + ".fetests");

	root["name"] = "noName";

	Json::Value Tests;
	for (size_t i = 0; i < list.size(); i++)
	{
		Tests[std::to_string(i)]["path"] = list[i]->FilePath;
		Tests[std::to_string(i)]["name"] = list[i]->GetName();
	}
	root["tests"] = Tests;

	Json::StreamWriterBuilder builder;
	const std::string json_file = Json::writeString(builder, root);

	saveFile << json_file;
	saveFile.close();
}

void TestManager::openTestSet(std::string FilePath)
{
	if (FilePath == "")
		return;

	std::ifstream testSetFile;
	testSetFile.open(FilePath);

	std::string fileData((std::istreambuf_iterator<char>(testSetFile)), std::istreambuf_iterator<char>());
	testSetFile.close();

	Json::Value root;
	JSONCPP_STRING err;
	Json::CharReaderBuilder builder;

	const std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
	if (!reader->parse(fileData.c_str(), fileData.c_str() + fileData.size(), &root, &err))
		return;

	std::vector<Json::String> testList = root["tests"].getMemberNames();
	for (size_t i = 0; i < testList.size(); i++)
	{
		std::string originalPath = FocalEngine::FILE_SYSTEM.GetDirectoryPath(root["tests"][testList[i].c_str()]["path"].asCString());

		std::string originalDirectory = "";
		int seenDelimiterCount = 0;
		for (size_t j = originalPath.size() - 1; j > 0; j--)
		{
			if (seenDelimiterCount == 1)
				originalDirectory.insert(originalDirectory.begin(), originalPath[j]);

			if (originalPath[j] == '\\')
			{
				seenDelimiterCount++;
				if (seenDelimiterCount == 2)
				{
					originalDirectory.erase(originalDirectory.begin());
					break;
				}
			}
		}

		std::string localPath = FocalEngine::FILE_SYSTEM.GetDirectoryPath(FilePath.c_str());
		localPath += originalDirectory;
		localPath += "\\";
		localPath += FocalEngine::FILE_SYSTEM.GetFileName(root["tests"][testList[i].c_str()]["path"].asCString());
		localPath += ".fetp";

		TEST_MANAGER.AddTest(localPath);
	}
}