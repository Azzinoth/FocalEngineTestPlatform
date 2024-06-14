#pragma once

#include "../VisualNodeSystem/VisualNodeSystem.h"
#include "../../FETPImage.h"
#include "../SubSystems/FETPInput.h"

class basicLogicNode : public VisNodeSys::Node
{
	friend class NodeFactory;
	static bool isRegistered;
public:
	basicLogicNode();
	virtual basicLogicNode* GetNextNode();
};