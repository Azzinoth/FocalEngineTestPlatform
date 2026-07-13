#pragma once

#include "FETest.h"

class FETest;
class TestManager
{
private:
	SINGLETON_PRIVATE_PART(TestManager)

	static VisNodeSys::Node* FoundNode;
	static FETPAction* SearchedAction;

	bool IsTestNameFree(std::string Name);
	size_t SelectedTestIndex = 0;
public:
	SINGLETON_PUBLIC_PART(TestManager)

	void AddTest(std::string FilePath = "");
	std::vector<FETest*> Tests;

	void Clear();

	void RenameTest(FETest* Test, std::string NewName);
	void RenameTest(size_t TestIndex, std::string NewName);

	void RemoveTest(FETest* Test);
	void RemoveTest(size_t TestIndex);

	//VisNodeSys::Node* GetNodeByAction(FETPAction* Action);
	//FETest* GetTestByAction(FETPAction* Action);

	std::string GetNewTestName();
	void SetSelectedTestIndex(size_t Index);
	size_t GetSelectedTestIndex();

	FETest* GetSelectedTest();

	void SaveAsTestSet(std::string FilePath);
	void OpenTestSet(std::string FilePath);
};

#define TEST_MANAGER TestManager::GetInstance()