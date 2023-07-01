#pragma once

#include "../VisualNodeSystem/VisualNodeSystem.h"
#include "../../FETPImage.h"

class basicLogicNode : public VisualNode
{
	friend class NodeFactory;
	static bool isRegistered;
public:
	basicLogicNode();
	virtual basicLogicNode* GetNextNode();
};