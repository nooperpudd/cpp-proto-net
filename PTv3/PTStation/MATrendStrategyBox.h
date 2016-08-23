#pragma once

#include "STStrategyBox.h"

class CMATrendStrategyBox : public CSTStrategyBox
{
public:
	CMATrendStrategyBox(CStrategyOperation* pStrategyOperation);
	~CMATrendStrategyBox();

protected:
	void OnTest(entity::Quote* pQuote);
};

