#include "TestManager.h"

TestManager* TestManager::Instance = nullptr;
VisualNode* TestManager::foundNode = nullptr;
FETPAction* TestManager::searchedAction = nullptr;

TestManager::TestManager()
{
	
}

TestManager::~TestManager()
{

}

void TestManager::addTest(std::string filePath)
{
	if (filePath != "")
	{
		FETest* newTest = new FETest();
		newTest->setName(FocalEngine::FILE_SYSTEM.getFileName(filePath.c_str()));
		newTest->filePath = filePath;

		newTest->load();

		list.push_back(newTest);
	}
	else
	{
		FETest* newTest = new FETest();
		newTest->setName(getNewTestName());

		list.push_back(newTest);
	}
}

VisualNode* TestManager::getNodeByAction(FETPAction* action)
{
	if (action == nullptr)
		return nullptr;

	foundNode = nullptr;
	searchedAction = action;

	for (size_t i = 0; i < list.size(); i++)
	{
		if (list[i]->nodeArea == nullptr)
			continue;

		list[i]->nodeArea->RunOnEachNode([](VisualNode* node) {
			if (node->GetType() == "globalActionNode")
			{
				globalActionNode* actionNode = reinterpret_cast<globalActionNode*>(node);
				if (actionNode->GetData()->getID() == searchedAction->getID())
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
					if (actionList[j]->getID() == searchedAction->getID())
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
		if (list[i]->nodeArea == nullptr)
			continue;

		list[i]->nodeArea->RunOnEachNode([](VisualNode* node) {
			if (node->GetType() == "globalActionNode")
			{
				globalActionNode* actionNode = reinterpret_cast<globalActionNode*>(node);
				if (actionNode->GetData()->getID() == searchedAction->getID())
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
					if (actionList[j]->getID() == searchedAction->getID())
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

std::string TestManager::getNewTestName()
{
	std::string newName = "new test_";
	size_t index = 0;

	while (!isTestNameFree(newName + std::to_string(index)))
	{
		index++;
	}

	return newName + std::to_string(index);
}

bool TestManager::isTestNameFree(std::string name)
{
	for (size_t i = 0; i < list.size(); i++)
	{
		if (list[i]->getName() == name)
			return false;
	}

	return true;
}

void TestManager::setSelelectedTestIndex(size_t index)
{
	if (index < list.size())
		selelectedTestIndex = index;
}

size_t TestManager::getSelectedTestIndex()
{
	return selelectedTestIndex;
}

FETest* TestManager::getSelectedTest()
{
	if (selelectedTestIndex < list.size())
		return list[selelectedTestIndex];

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

	if (selelectedTestIndex == testIndex)
		selelectedTestIndex = 0;

	list.erase(list.begin() + testIndex);
}

void TestManager::renameTest(FETest* test, std::string newName)
{
	for (size_t i = 0; i < list.size(); i++)
	{
		if (list[i] == test)
		{
			if (isTestNameFree(newName))
				list[i]->setName(newName);

			return;
		}
	}
}

void TestManager::renameTest(size_t testIndex, std::string newName)
{
	if (testIndex >= list.size())
		return;

	if (isTestNameFree(newName))
		list[testIndex]->setName(newName);
}

void TestManager::saveAsTestSet(std::string filePath)
{
	if (list.size() == 0)
		return;

	Json::Value root;
	std::ofstream saveFile;

	std::string fileNameWithoutExtension = FocalEngine::FILE_SYSTEM.getFileName(filePath.c_str());
	std::string directoryPath = FocalEngine::FILE_SYSTEM.getDirectoryPath(filePath.c_str());
	saveFile.open(directoryPath + fileNameWithoutExtension + ".fetests");

	root["name"] = "noName";

	Json::Value Tests;
	for (size_t i = 0; i < list.size(); i++)
	{
		Tests[std::to_string(i)]["path"] = list[i]->filePath;
		Tests[std::to_string(i)]["name"] = list[i]->getName();
	}
	root["tests"] = Tests;

	Json::StreamWriterBuilder builder;
	const std::string json_file = Json::writeString(builder, root);

	saveFile << json_file;
	saveFile.close();
}

void TestManager::openTestSet(std::string filePath)
{
	if (filePath == "")
		return;

	std::ifstream testSetFile;
	testSetFile.open(filePath);

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
		std::string originalPath = FocalEngine::FILE_SYSTEM.getDirectoryPath(root["tests"][testList[i].c_str()]["path"].asCString());

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

		std::string localPath = FocalEngine::FILE_SYSTEM.getDirectoryPath(filePath.c_str());
		localPath += originalDirectory;
		localPath += "\\";
		localPath += FocalEngine::FILE_SYSTEM.getFileName(root["tests"][testList[i].c_str()]["path"].asCString());
		localPath += ".fetp";

		TEST_MANAGER.addTest(localPath);
	}
}