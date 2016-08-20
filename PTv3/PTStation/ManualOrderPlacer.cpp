#include "stdafx.h"
#include "ManualOrderPlacer.h"
#include "globalmembers.h"


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

CLegOrderPlacer* CDualScalperOrderPlacer::CreateLegOrderPlacer(int openTimeout, int maxRetryTimes)
{
	return new CDualScalperLegOrderPlacer(this, openTimeout, maxRetryTimes);
}

void CDualScalperOrderPlacer::CloseOrder(double closePrice, bool cancelOnTimeout)
{
	if (m_legPlacers.size() > 1)
	{
		CDualScalperLegOrderPlacer* legOrderPlacer = dynamic_cast<CDualScalperLegOrderPlacer*>(m_legPlacers[1].get());
		if (legOrderPlacer != NULL)
			legOrderPlacer->SetCancelOnTimeout(cancelOnTimeout);
	}
	CPortfolioTrendOrderPlacer::CloseOrder(closePrice);
}

void CDualScalperOrderPlacer::Cleanup()
{
	ForceCleanup();
}
