#pragma once

#include "../VisualNodeSystem/VisualNodeSystem.h"
#include "../../FETPImage.h"
#include "../SubSystems/FETPInput.h"

class BasicLogicNode : public VisNodeSys::Node
{
	friend class NodeFactory;
	static bool bIsRegistered;
public:
	BasicLogicNode();
	virtual BasicLogicNode* GetNextNode();
};