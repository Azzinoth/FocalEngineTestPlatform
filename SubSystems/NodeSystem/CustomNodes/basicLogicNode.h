#pragma once

#include "../VisualNodeSystem/VisualNodeSystem.h"
#include "../../FETPImage.h"
#include "../SubSystems/FETPInput.h"

class basicLogicNode : public VisualNode
{
	friend class NodeFactory;
	static bool isRegistered;
public:
	basicLogicNode();
	virtual basicLogicNode* GetNextNode();
};