#include "FETest.h"

ImColor* FETest::defaultConnectionColor = new ImColor(200, 200, 200);
ImColor* FETest::mainPathConnectionColor = new ImColor(150, 255, 150);

FETest::FETest()
{
	nodeArea = NODE_SYSTEM.createNodeArea();

	begin = new beginNode();
	begin->setName("Begin node");
	begin->setPosition(ImVec2(300.0f, 430.0f));

	nodeArea->addNode(begin);

	//addMacro("$(RESOURCES)", "C:\\Users\\Kindr\\Desktop\\Tests\\Resources - Copy\\");
	//addMacro("$(RESOURC234ES)", "C:\\Users\\Kin4dr\\Desktop\\Test435s\\Resources - Copy\\");
	//addMacro("$(RESOURCES1)", "C:\\Users\\Kin5345dr\\Desktop345\\Tests\\Reso345urces - Copy\\");
}

FETest::~FETest()
{
	delete begin;
}

void FETest::save(const char* fileName)
{
	filePath = fileName;
	// Before saving node area we should check if all images in nodes have correct file path.
	validateImagePathes();
	nodeArea->saveToFile(fileName);

	Json::Value root;
	std::ofstream saveFile;

	std::string fileNameWithoutExtension = FocalEngine::FILE_SYSTEM.getFileName(fileName);
	std::string directoryPath = FocalEngine::FILE_SYSTEM.getDirectoryPath(fileName);
	saveFile.open(directoryPath + fileNameWithoutExtension + ".fetpTestInfo");

	root["name"] = name;
	root["speedFactor"] = speedFactor;

	Json::Value beforeStartActions;
	for (size_t i = 0; i < beforeStart.size(); i++)
	{
		beforeStartActions[std::to_string(i)]["type"] = FETest::FEBeforeTestActionTypeToString(beforeStart[i]->type);
		beforeStartActions[std::to_string(i)]["path"] = beforeStart[i]->path;
		beforeStartActions[std::to_string(i)]["newObjectName"] = beforeStart[i]->newObjectName;
	}
	root["beforeStartActions"] = beforeStartActions;

	Json::Value macros;

	auto currentMacro = macrosToReplace.begin();
	while (currentMacro != macrosToReplace.end())
	{
		macros[currentMacro->first] = currentMacro->second;
		currentMacro++;
	}
	root["macros"] = macros;

	Json::StreamWriterBuilder builder;
	const std::string json_file = Json::writeString(builder, root);

	saveFile << json_file;
	saveFile.close();
}

void FETest::load()
{
	if (filePath == "")
		return;

	validateImagePathesInFile(filePath);
	
	nodeArea->clear();
	nodeArea->loadFromFile(filePath.c_str());

	auto result = nodeArea->getNodesByType("beginNode");
	if (result.size() == 1)
		begin = reinterpret_cast<beginNode*>(result[0]);

	reColorMainTestPath();

	// Load additional test data.
	std::ifstream dataFile;
	std::string fileNameWithoutExtension = FocalEngine::FILE_SYSTEM.getFileName(filePath.c_str());
	std::string directoryPath = FocalEngine::FILE_SYSTEM.getDirectoryPath(filePath.c_str());
	dataFile.open(directoryPath + fileNameWithoutExtension + ".fetpTestInfo");

	std::string additionalFileData((std::istreambuf_iterator<char>(dataFile)), std::istreambuf_iterator<char>());
	dataFile.close();

	Json::Value root;
	JSONCPP_STRING err;
	Json::CharReaderBuilder builder;

	const std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
	if (!reader->parse(additionalFileData.c_str(), additionalFileData.c_str() + additionalFileData.size(), &root, &err))
		return;

	name = root["name"].asCString();
	speedFactor = root["speedFactor"].asFloat();

	std::vector<Json::String> actionList = root["beforeStartActions"].getMemberNames();
	beforeStart.resize(actionList.size());
	for (size_t i = 0; i < actionList.size(); i++)
	{
		FETestBeforeAction* action = new FETestBeforeAction();
		action->type = FETest::stringToFEBeforeTestActionType(root["beforeStartActions"][actionList[i]]["type"].asCString());
		action->path = root["beforeStartActions"][actionList[i]]["path"].asCString();
		action->newObjectName = root["beforeStartActions"][actionList[i]]["newObjectName"].asCString();

		beforeStart[atoi(actionList[i].c_str())] = action;
	}

	std::vector<Json::String> macrosList = root["macros"].getMemberNames();
	for (size_t i = 0; i < macrosList.size(); i++)
	{
		addMacro(macrosList[i], root["macros"][macrosList[i]].asCString());
	}
}

beginNode* FETest::getBeginNode()
{
	return begin;
}

void FETest::reColorMainTestPath()
{
	// Change color of all connections to default.
	nodeArea->runOnEachNode([](FEEditorNode* node) {
		size_t outSocketCount = node->outSocketCount();
		for (size_t i = 0; i < outSocketCount; i++)
		{
			node->setForcedOutSocketColor(FETest::defaultConnectionColor, i);
		}
	});

	// Start at begin node at change each connection color from it.
	if (getBeginNode() == nullptr)
		return;

	nodeArea->runOnEachConnectedNode(getBeginNode(),
		[](FEEditorNode* node) {
			size_t outSocketCount = node->outSocketCount();
			for (size_t i = 0; i < outSocketCount; i++)
			{
				node->setForcedOutSocketColor(FETest::mainPathConnectionColor, i);
			}
		}
	);
}

void FETest::addResult(FETestResult* newResult)
{
	if (newResult == nullptr)
		return;

	results.push_back(newResult);
}

FETestResult* FETest::getLastTestResult()
{
	if (results.size() == 0)
		return nullptr;

	return results.back();
}

float FETest::getSpeedFactor()
{
	return speedFactor;
}

void FETest::setSpeedFactor(float newValue)
{
	if (newValue < 0.1f || newValue > 10.0f)
		return;

	speedFactor = newValue;
}

std::string FETest::getName()
{
	return name;
}

void FETest::setName(std::string newValue)
{
	name = newValue;
}

void FETest::addBeforeStartAction(FETestBeforeAction* action)
{
	beforeStart.push_back(action);
}

void FETest::beforeBegin()
{
	for (size_t i = 0; i < beforeStart.size(); i++)
	{
		if (beforeStart[i]->type == FE_BEFORE_TEST_ACTION_DELETE_DIRECTORY)
		{
			FocalEngine::FILE_SYSTEM.deleteFolder(beforeStart[i]->path.c_str());
		}
	}
}

void FETest::validateImagePathes(FEEditorNodeArea* nodeArea, std::string filePath)
{
	if (nodeArea == nullptr)
		nodeArea = this->nodeArea;

	if (filePath == "")
		filePath = this->filePath;

	std::string directoryPath = FocalEngine::FILE_SYSTEM.getDirectoryPath(filePath.c_str());
	std::vector<FEEditorNode*> list = nodeArea->getNodesByType("globalActionNode");
	std::unordered_map<std::string, bool> actionSeenIDs;
	for (size_t i = 0; i < list.size(); i++)
	{
		globalActionNode* node = reinterpret_cast<globalActionNode*>(list[i]);

		if (actionSeenIDs.find(node->getData()->getID()) != actionSeenIDs.end())
			node->getData()->setID(APPLICATION.getUniqueHexID());
		actionSeenIDs[node->getData()->getID()] = true;

		if (node->getData()->getType() == FETP_SCREENSHOOT_COMPARE_ACTION)
		{
			ScreenshootCompareAction* action = reinterpret_cast<ScreenshootCompareAction*>(node->getData());
			for (size_t j = 0; j < action->imagesInfo.size(); j++)
			{
				if (action->imagesInfo[j]->image != nullptr)
				{
					std::string fileName = directoryPath + "screenshot_";
					fileName += action->getID();
					fileName += "_" + std::to_string(j) + "_";
					fileName += ".png";

					action->imagesInfo[j]->image->setFullPath(fileName);
				}

				if (action->imagesInfo[j]->partialImage != nullptr)
				{
					std::string fileName = directoryPath + "partial_";
					fileName += action->getID();
					fileName += "_" + std::to_string(j) + "_";
					fileName += ".png";

					action->imagesInfo[j]->partialImage->setFullPath(fileName);
				}
			}
		}
	}

	// Validate nodes in node regions.
	/*std::vector<FEEditorNode*> regionList = nodeArea->getNodesByType("regionNode");
	for (size_t i = 0; i < regionList.size(); i++)
	{
		regionNode* node = reinterpret_cast<regionNode*>(regionList[i]);
		
		if (node->getData() != nullptr)
			validateImagePathes(node->getData(), filePath);
	}*/

	// Validate nodes IDs in node regions with IDs in main node area.
	std::vector<FEEditorNode*> regionList = nodeArea->getNodesByType("regionNode");
	for (size_t i = 0; i < regionList.size(); i++)
	{
		regionNode* rNode = reinterpret_cast<regionNode*>(regionList[i]);

		if (rNode->getData() != nullptr)
		{
			std::vector<FEEditorNode*> list = rNode->getData()->getNodesByType("globalActionNode");
			for (size_t j = 0; j < list.size(); j++)
			{
				globalActionNode* node = reinterpret_cast<globalActionNode*>(list[j]);

				if (actionSeenIDs.find(node->getData()->getID()) != actionSeenIDs.end())
					node->getData()->setID(APPLICATION.getUniqueHexID());
				actionSeenIDs[node->getData()->getID()] = true;

				if (node->getData()->getType() == FETP_SCREENSHOOT_COMPARE_ACTION)
				{
					ScreenshootCompareAction* action = reinterpret_cast<ScreenshootCompareAction*>(node->getData());
					for (size_t k = 0; k < action->imagesInfo.size(); k++)
					{
						if (action->imagesInfo[k]->image != nullptr)
						{
							std::string fileName = directoryPath + "screenshot_";
							fileName += action->getID();
							fileName += "_" + std::to_string(k) + "_";
							fileName += ".png";

							action->imagesInfo[k]->image->setFullPath(fileName);
						}

						if (action->imagesInfo[k]->partialImage != nullptr)
						{
							std::string fileName = directoryPath + "partial_";
							fileName += action->getID();
							fileName += "_" + std::to_string(k) + "_";
							fileName += ".png";

							action->imagesInfo[k]->partialImage->setFullPath(fileName);
						}
					}
				}
			}
		}
	}
}

Json::Value FETest::validateImagePathesInNodeArea(std::string nodeAreaText)
{
	Json::Value root;
	JSONCPP_STRING err;
	Json::CharReaderBuilder builder;

	const std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
	if (!reader->parse(nodeAreaText.c_str(), nodeAreaText.c_str() + nodeAreaText.size(), &root, &err))
		return root;

	std::string newPath = std::string(FocalEngine::FILE_SYSTEM.getDirectoryPath(this->filePath.c_str()));

	std::vector<Json::String> nodesList = root["nodes"].getMemberNames();
	for (size_t i = 0; i < nodesList.size(); i++)
	{
		std::string nodeType = root["nodes"][std::to_string(i)]["nodeType"].asCString();
		if (nodeType == "globalActionNode")
		{
			if (root["nodes"][std::to_string(i)]["action"]["internalType"].asInt() == FETP_SCREENSHOOT_COMPARE_ACTION)
			{
				std::vector<Json::String> compareImageInfosList = root["nodes"][std::to_string(i)]["action"]["compareImageInfos"].getMemberNames();
				for (size_t j = 0; j < compareImageInfosList.size(); j++)
				{
					std::string fileName = newPath + "screenshot_";
					fileName += root["nodes"][std::to_string(i)]["action"]["ID"].asCString();
					fileName += "_" + std::to_string(j) + "_";
					fileName += ".png";

					root["nodes"][std::to_string(i)]["action"]["compareImageInfos"][std::to_string(j)]["screenshot_fullPath"] = fileName;

					if (root["nodes"][std::to_string(i)]["action"]["compareImageInfos"][std::to_string(j)].isMember("partial_fileName"))
					{
						std::string fileName = newPath + "partial_";
						fileName += root["nodes"][std::to_string(i)]["action"]["ID"].asCString();
						fileName += "_" + std::to_string(j) + "_";
						fileName += ".png";

						root["nodes"][std::to_string(i)]["action"]["compareImageInfos"][std::to_string(j)]["partial_fullPath"] = fileName;
					}
				}
			}
		}
		else if (nodeType == "regionNode")
		{
			Json::StreamWriterBuilder saveBuilder;
			root["nodes"][std::to_string(i)]["nodeArea"] = Json::writeString(saveBuilder, validateImagePathesInNodeArea(root["nodes"][std::to_string(i)]["nodeArea"].asCString()));
		}
	}

	return root;
}

void FETest::validateImagePathesInFile(std::string filePath)
{
	if (filePath == "")
		return;

	std::ifstream nodesFile;
	nodesFile.open(filePath);

	std::string fileData((std::istreambuf_iterator<char>(nodesFile)), std::istreambuf_iterator<char>());
	nodesFile.close();

	Json::Value result = validateImagePathesInNodeArea(fileData);

	std::ofstream reSaveNodes;
	reSaveNodes.open(filePath);

	Json::StreamWriterBuilder saveBuilder;
	const std::string json_file = Json::writeString(saveBuilder, result);

	reSaveNodes << json_file;
	reSaveNodes.close();
}

void FETest::addMacro(std::string macro, std::string replaceWith)
{
	macrosToReplace[macro] = replaceWith;
}

bool FETest::replaceMacro(std::string& text)
{
	bool wasChanged = false;
	auto currentMacro = macrosToReplace.begin();
	while (currentMacro != macrosToReplace.end())
	{
		size_t index = text.find(currentMacro->first);
		if (index != std::string::npos)
		{
			text.replace(index, currentMacro->first.size(), currentMacro->second);
			wasChanged = true;
		}
		currentMacro++;
	}

	return wasChanged;
}

int FETest::getLoopCount()
{
	return loopCount;
}

void FETest::setLoopCount(int newValue)
{
	if (newValue <= 0)
		newValue = 1;

	loopCount = newValue;
}