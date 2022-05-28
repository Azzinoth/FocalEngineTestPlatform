#pragma once

#include "FETest.h"

class FETest;
class TestManager
{
private:
	SINGLETON_PRIVATE_PART(TestManager)

	static FEVisualNode* foundNode;
	static FETPAction* searchedAction;

	bool isTestNameFree(std::string name);
	size_t selelectedTestIndex = 0;
public:
	SINGLETON_PUBLIC_PART(TestManager)

	void addTest(std::string filePath = "");
	std::vector<FETest*> list;

	void renameTest(FETest* test, std::string newName);
	void renameTest(size_t testIndex, std::string newName);

	void removeTest(FETest* test);
	void removeTest(size_t testIndex);

	FEVisualNode* getNodeByAction(FETPAction* action);
	FETest* getTestByAction(FETPAction* action);

	std::string getNewTestName();
	void setSelelectedTestIndex(size_t index);
	size_t getSelectedTestIndex();

	FETest* getSelectedTest();

	void saveAsTestSet(std::string filePath);
	void openTestSet(std::string filePath);
};

#define TEST_MANAGER TestManager::getInstance()