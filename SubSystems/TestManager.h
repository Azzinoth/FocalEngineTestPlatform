#pragma once

#include "FETest.h"

class FETest;
class TestManager
{
private:
	SINGLETON_PRIVATE_PART(TestManager)

	static VisNodeSys::Node* foundNode;
	static FETPAction* searchedAction;

	bool IsTestNameFree(std::string Name);
	size_t SelelectedTestIndex = 0;
public:
	SINGLETON_PUBLIC_PART(TestManager)

	void AddTest(std::string FilePath = "");
	std::vector<FETest*> list;

	void renameTest(FETest* test, std::string newName);
	void renameTest(size_t testIndex, std::string newName);

	void removeTest(FETest* test);
	void removeTest(size_t testIndex);

	VisNodeSys::Node* getNodeByAction(FETPAction* action);
	FETest* getTestByAction(FETPAction* action);

	std::string GetNewTestName();
	void setSelelectedTestIndex(size_t index);
	size_t getSelectedTestIndex();

	FETest* getSelectedTest();

	void saveAsTestSet(std::string FilePath);
	void openTestSet(std::string FilePath);
};

#define TEST_MANAGER TestManager::GetInstance()