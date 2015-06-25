#include "StdAfx.h"
#include "ManualOrderPlacer.h"


CManualOrderPlacer::CManualOrderPlacer()
{
}


CManualOrderPlacer::~CManualOrderPlacer()
{
}

bool CManualOrderPlacer::Prepare()
{
	if (IsReadyForPrepare())
		return CPortfolioOrderPlacer::Prepare();

	return false;
}

CLegOrderPlacer* CManualOrderPlacer::CreateLegOrderPlacer(int openTimeout, int maxRetryTimes)
{
	return new CManualLegOrderPlacer(this, openTimeout, maxRetryTimes);
}
