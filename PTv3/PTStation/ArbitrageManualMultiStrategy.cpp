#include "StdAfx.h"
#include "DoubleCompare.h"
#include "Portfolio.h"
#include "PortfolioArbitrageOrderPlacer.h"
#include "ArbitrageManualMultiStrategy.h"
#include "globalmembers.h"


CArbitrageManualMultiStrategy::CArbitrageManualMultiStrategy(CAvatarClient* pAvatar, CPortfolio* pPortfolio)
	: CMultiOpenStrategy(pAvatar, pPortfolio)
	, m_direction(entity::LONG)
	, m_offset(entity::CLOSE)
	, m_openCondition(entity::LESS_EQUAL_THAN)
	, m_openThreshold(5)
	, m_closeCondition(entity::GREATER_EQUAL_THAN)
	, m_closeThreshold(8)
{
}

CArbitrageManualMultiStrategy::~CArbitrageManualMultiStrategy()
{
}

void CArbitrageManualMultiStrategy::OnApply(const entity::StrategyItem& strategyItem, CPortfolio* pPortfolio, bool withTriggers)
{
	if (m_openTimeout == 0)
		m_openTimeout = 350;
	if (m_retryTimes == 0)
		m_retryTimes = 8;

	m_direction = strategyItem.am_direction();
	m_offset = strategyItem.am_offset();
	m_openCondition = strategyItem.am_opencondition();
	m_openThreshold = strategyItem.am_openthreshold();
	m_closeCondition = strategyItem.am_closecondition();
	m_closeThreshold = strategyItem.am_closethreshold();
}

bool CArbitrageManualMultiStrategy::Prerequisite(entity::Quote* pQuote, CPortfolio* pPortfolio, StrategyContext& context, boost::chrono::steady_clock::time_point& timestamp)
{
	bool prerequisiteOK = CMultiOpenStrategy::Prerequisite(pQuote, pPortfolio, context, timestamp);
	if (prerequisiteOK)
	{
		if (!pPortfolio->LegsTimestampAligned())
			return false;
	}

	return prerequisiteOK;
}

void CArbitrageManualMultiStrategy::CalculateContext(entity::Quote* pQuote, CPortfolio* pPortfolio, boost::chrono::steady_clock::time_point& timestamp)
{
	m_context.LastDiff = pPortfolio->CalculateDiff(&m_context.StructLastDiff, BETTER_PRICE);
	m_context.LongDiff = pPortfolio->CalculateDiff(&m_context.StructLongDiff, BETTER_PRICE);
	m_context.ShortDiff = pPortfolio->CalculateDiff(&m_context.StructShortDiff, BETTER_PRICE);

	m_context.LongDiffFast = pPortfolio->CalculateDiff(&m_context.StructLongDiffFast, FAST_DEAL);
	m_context.ShortDiffFast = pPortfolio->CalculateDiff(&m_context.StructShortDiffFast, FAST_DEAL);

	// TODO: will be figured out in the future
	m_context.LongDiffSize = 0;
	m_context.ShortDiffSize = 0;
}

void CArbitrageManualMultiStrategy::GetStrategyUpdate(entity::PortfolioUpdateItem* pPortfUpdateItem)
{
	CStrategy::GetStrategyUpdate(pPortfUpdateItem);

	pPortfUpdateItem->set_ar_diff(m_context.LastDiff);
	pPortfUpdateItem->set_ar_longdiff(m_context.LongDiff);
	pPortfUpdateItem->set_ar_longsize(m_context.LongDiffSize);
	pPortfUpdateItem->set_ar_shortdiff(m_context.ShortDiff);
	pPortfUpdateItem->set_ar_shortsize(m_context.ShortDiffSize);
}

StrategyExecutorPtr CArbitrageManualMultiStrategy::CreateExecutor(int execId, int quantity)
{
	return StrategyExecutorPtr(new CArbitrageManualStrategyExecutor(execId, quantity, this));
}

void CArbitrageManualMultiStrategy::TestForOpen(entity::Quote* pQuote, CPortfolio* pPortfolio, StrategyContext* pContext, boost::chrono::steady_clock::time_point& timestamp)
{
	bool taskDone = false;
	// Check if open/close task gets finished
	if (Offset() == entity::OPEN)
	{
		if (m_OpenedExecutors.size() + m_errorExecutors.size() == m_strategyExecutors.size())
		{
			LOG_DEBUG(logger, boost::str(boost::format("Portfolio(%s) STOP Strategy due to Done opening position") % pPortfolio->ID()));
			m_doneComment = boost::str(boost::format("开仓%d手已完成,策略停止") % m_maxQuantity);
			taskDone = true;
		}
		else
		{
			CMultiOpenStrategy::TestForOpen(pQuote, pPortfolio, pContext, timestamp);
		}
	}
	else if (Offset() == entity::CLOSE)
	{
		if (m_executorsPool.size() == m_strategyExecutors.size())
		{
			LOG_DEBUG(logger, boost::str(boost::format("Portfolio(%s) STOP Strategy due to Done closing position") % pPortfolio->ID()));
			m_doneComment = boost::str(boost::format("平仓%d手已完成,策略停止") % m_maxQuantity);
			taskDone = true;
		}
		else
		{
			CMultiOpenStrategy::TestForClose(pQuote, pPortfolio, pContext, timestamp);
		}
	}
	if (taskDone)
	{
		boost::thread(boost::bind(&CPortfolio::StopStrategyDueTo, pPortfolio, m_doneComment));
	}
}

void CArbitrageManualMultiStrategy::VirtualOpenPosition()
{
	if (m_activeExecutor != NULL)
	{
		m_executorsPool.push(m_activeExecutor);
		m_activeExecutor = NULL;
	}

	while (!m_executorsPool.empty())
	{
		CStrategyExecutor* pExecutor = m_executorsPool.front();
		pExecutor->Start();
		CArbitrageManualStrategyExecutor* manualExecutor = dynamic_cast<CArbitrageManualStrategyExecutor*>(pExecutor);
		if (manualExecutor != NULL)
			manualExecutor->VirtualFill();

		m_OpenedExecutors.insert(std::make_pair(pExecutor->ExecId(), pExecutor));
		m_executorsPool.pop();
	}
}

bool CArbitrageManualMultiStrategy::OnStart()
{
	bool start = CMultiOpenStrategy::OnStart();
	if (Offset() == entity::CLOSE)
	{
		VirtualOpenPosition();
	}
	return start;
}


void CArbitrageManualStrategyExecutor::InitOrderPlacer(CPortfolio* pPortf, COrderProcessor* pOrderProc, PortfolioTradedEvent porfTradedEventHandler)
{
	m_orderPlacer = OrderPlacerPtr(new CPortfolioArbitrageOrderPlacer(m_execId, m_quantity));
	if (m_orderPlacer.get() != NULL)
	{
		m_orderPlacer->Initialize(pPortf, pOrderProc);
		m_orderPlacer->SetPortfolioTradedEventHandler(porfTradedEventHandler);
	}
}

void CArbitrageManualStrategyExecutor::OnWorking(entity::Quote* pQuote, boost::chrono::steady_clock::time_point& timestamp)
{
	CPortfolio* pPortfolio = m_orderPlacer->Portfolio();
	LOG_DEBUG(logger, boost::str(boost::format("[%s - %d] Arbitrage Manual Strategy - Check and likely retry submit order %s:%.2f")
		% (pPortfolio != NULL ? pPortfolio->InvestorId() : "") % m_execId % pQuote->symbol() % pQuote->last()));
	m_orderPlacer->OnQuoteReceived(timestamp, pQuote);
}

void CArbitrageManualStrategyExecutor::OnFilled(int volumeTraded)
{
	ExecutorState state = State();
	if (state == PENDING_OPEN)
		m_volumeToClose = volumeTraded;

	CStrategyExecutor::OnFilled(volumeTraded);
}

bool CArbitrageManualStrategyExecutor::TestForOpen(entity::Quote* pQuote, CPortfolio* pPortfolio, StrategyContext* pContext, boost::chrono::steady_clock::time_point& timestamp)
{
	ArbitrageStrategyContext* arbitrageContext = dynamic_cast<ArbitrageStrategyContext*>(pContext);
	entity::PosiDirectionType side = m_pParentStrategy->Direction();

	bool ret = false;
	if (side == entity::LONG)
	{
		ret = TestForLongOpen(arbitrageContext->LongDiffFast);
		if (ret)
		{
			string logTxt = boost::str(boost::format("Fast Long Open: Fast Long diff(%.2f) <= Threshold(%.2f)")
				% arbitrageContext->LongDiffFast % m_pParentStrategy->OpenThreshold());
			LOG_DEBUG(logger, logTxt);
			pPortfolio->PrintLegsQuote();
			string comment = boost::str(boost::format("对价多价差(%.2f)小于设定值(%.2f) -> 开仓")
				% arbitrageContext->LongDiffFast % m_pParentStrategy->OpenThreshold());
			OpenPosition(side, arbitrageContext->StructLongDiffFast, arbitrageContext, pQuote, timestamp, comment);

			return true;
		}
		else
		{
			ret = TestForLongOpen(arbitrageContext->LongDiff);
			if (ret)
			{
				string logTxt = boost::str(boost::format("Long Open: Long diff(%.2f) <= Threshold(%.2f)")
					% arbitrageContext->LongDiff % m_pParentStrategy->OpenThreshold());
				LOG_DEBUG(logger, logTxt);
				pPortfolio->PrintLegsQuote();
				string comment = boost::str(boost::format("多价差(%.2f)小于设定值(%.2f) -> 开仓")
					% arbitrageContext->LongDiff % m_pParentStrategy->OpenThreshold());

				OpenPosition(side, arbitrageContext->StructLongDiff, arbitrageContext, pQuote, timestamp, comment);
				return true;
			}
		}
	}
	else if (side == entity::SHORT)
	{
		ret = TestForShortOpen(arbitrageContext->ShortDiffFast);
		if (ret)
		{
			string logTxt = boost::str(boost::format("Fast Short Open: Fast Short diff(%.2f) >= Threshold(%.2f)")
				% arbitrageContext->ShortDiffFast % m_pParentStrategy->OpenThreshold());
			LOG_DEBUG(logger, logTxt);
			pPortfolio->PrintLegsQuote();
			string comment = boost::str(boost::format("对价空价差(%.2f)大于设定值(%.2f) -> 开仓")
				% arbitrageContext->ShortDiffFast % m_pParentStrategy->OpenThreshold());
			OpenPosition(side, arbitrageContext->StructShortDiffFast, arbitrageContext, pQuote, timestamp, comment);
			return true;
		}
		else
		{
			ret = TestForShortOpen(arbitrageContext->ShortDiff);
			if (ret)
			{
				string logTxt = boost::str(boost::format("Short Open: Short diff(%.2f) >= Threshold(%.2f)")
					% arbitrageContext->ShortDiff % m_pParentStrategy->OpenThreshold());
				LOG_DEBUG(logger, logTxt);
				pPortfolio->PrintLegsQuote();
				string comment = boost::str(boost::format("空价差(%.2f)大于设定值(%.2f) -> 开仓")
					% arbitrageContext->ShortDiff % m_pParentStrategy->OpenThreshold());
				OpenPosition(side, arbitrageContext->StructShortDiff, arbitrageContext, pQuote, timestamp, comment);
				return true;
			}
		}
	}

	return false;
}

bool CArbitrageManualStrategyExecutor::TestForClose(entity::Quote* pQuote, CPortfolio* pPortfolio, StrategyContext* pContext, boost::chrono::steady_clock::time_point& timestamp)
{
	ArbitrageStrategyContext* arbitrageContext = dynamic_cast<ArbitrageStrategyContext*>(pContext);
	entity::PosiDirectionType side = m_pParentStrategy->Direction();

	// Check if force closing
	bool forceClosing = IsForceClosing();
	if (forceClosing)
	{
		if (State() == HOLD_POSITION)
		{
			string logTxt = boost::str(boost::format("Manual Close Portfolio(%s) - Executor (%d) at %s")
				% pPortfolio->ID() % ExecId() % pQuote->update_time());
			LOG_DEBUG(logger, logTxt);
			pPortfolio->PrintLegsQuote();
			string comment = "强制平仓";
			ARBI_DIFF_CALC& diffPrices = side == entity::LONG ? arbitrageContext->StructShortDiffFast : arbitrageContext->StructLongDiffFast;
			return ClosePosition(diffPrices, pQuote, timestamp, comment, trade::SR_Manual);
		}
		else
		{
			ResetForceClose();	// ignore and reset unexpected force closing
		}
		return false;
	}

	bool ret = false;
	if (side == entity::LONG)
	{
		ret = TestForLongClose(arbitrageContext->ShortDiffFast);
		if (ret)
		{
			string logTxt = boost::str(boost::format("Fast Long Close: Fast Short diff(%.2f) >= Threshold(%.2f)")
				% arbitrageContext->ShortDiffFast % m_pParentStrategy->CloseThreshold());
			LOG_DEBUG(logger, logTxt);
			pPortfolio->PrintLegsQuote();
			string comment = boost::str(boost::format("对价空价差(%.2f)大于设定值(%.2f) -> 平仓")
				% arbitrageContext->ShortDiffFast % m_pParentStrategy->CloseThreshold());
			return ClosePosition(arbitrageContext->StructShortDiffFast, pQuote, timestamp, comment, trade::SR_Manual);
		}
		else
		{
			ret = TestForLongClose(arbitrageContext->ShortDiff);
			if (ret)
			{
				string logTxt = boost::str(boost::format("Long Close: Short diff(%.2f) >= Threshold(%.2f)")
					% arbitrageContext->ShortDiff % m_pParentStrategy->CloseThreshold());
				LOG_DEBUG(logger, logTxt);
				pPortfolio->PrintLegsQuote();
				string comment = boost::str(boost::format("空价差(%.2f)大于设定值(%.2f) -> 平仓")
					% arbitrageContext->ShortDiff % m_pParentStrategy->CloseThreshold());
				return ClosePosition(arbitrageContext->StructShortDiff, pQuote, timestamp, comment, trade::SR_Manual);
			}
		}
	}
	else if (side == entity::SHORT)
	{
		ret = TestForShortClose(arbitrageContext->LongDiffFast);
		if (ret)
		{
			string logTxt = boost::str(boost::format("Fast Short Close: Fast Long diff(%.2f) <= Threshold(%.2f)")
				% arbitrageContext->LongDiffFast % m_pParentStrategy->CloseThreshold());
			LOG_DEBUG(logger, logTxt);
			pPortfolio->PrintLegsQuote();
			string comment = boost::str(boost::format("对价多价差(%.2f)小于设定值(%.2f) -> 平仓")
				% arbitrageContext->LongDiffFast % m_pParentStrategy->CloseThreshold());
			return ClosePosition(arbitrageContext->StructLongDiffFast, pQuote, timestamp, comment, trade::SR_Manual);
		}
		else
		{
			ret = TestForShortClose(arbitrageContext->LongDiff);
			if (ret)
			{
				string logTxt = boost::str(boost::format("Short Close: Long diff(%.2f) <= Threshold(%.2f)")
					% arbitrageContext->LongDiff % m_pParentStrategy->CloseThreshold());
				LOG_DEBUG(logger, logTxt);
				pPortfolio->PrintLegsQuote();
				string comment = boost::str(boost::format("多价差(%.2f)小于设定值(%.2f) -> 平仓")
					% arbitrageContext->LongDiff % m_pParentStrategy->CloseThreshold());
				return ClosePosition(arbitrageContext->StructLongDiff, pQuote, timestamp, comment, trade::SR_Manual);
			}
		}
	}

	return false;
}

bool CArbitrageManualStrategyExecutor::TestForLongClose(double diffPx)
{
	bool ret = false;
	if (m_pParentStrategy->CloseCondition() == entity::GREATER_THAN)
	{
		ret = diffPx - m_pParentStrategy->CloseThreshold() > 0.1;
	}
	else if (m_pParentStrategy->CloseCondition() == entity::GREATER_EQUAL_THAN)
	{
		ret = DoubleGreaterEqual(diffPx, m_pParentStrategy->CloseThreshold());
	}
	return ret;
}

bool CArbitrageManualStrategyExecutor::TestForShortClose(double diffPx)
{
	bool ret = false;
	if (m_pParentStrategy->CloseCondition() == entity::LESS_THAN)
	{
		ret = m_pParentStrategy->CloseThreshold() - diffPx > 0.1;
	}
	else if (m_pParentStrategy->CloseCondition() == entity::LESS_EQUAL_THAN)
	{
		ret = DoubleLessEqual(diffPx, m_pParentStrategy->CloseThreshold());
	}
	return ret;
}

bool CArbitrageManualStrategyExecutor::TestForLongOpen(double diffPx)
{
	bool ret = false;
	if (m_pParentStrategy->OpenCondition() == entity::LESS_THAN)
	{
		ret = m_pParentStrategy->OpenThreshold() - diffPx > 0.1;
	}
	else if (m_pParentStrategy->OpenCondition() == entity::LESS_EQUAL_THAN)
	{
		ret = DoubleLessEqual(diffPx, m_pParentStrategy->OpenThreshold());
	}
	return ret;
}

bool CArbitrageManualStrategyExecutor::TestForShortOpen(double diffPx)
{
	bool ret = false;
	if (m_pParentStrategy->OpenCondition() == entity::GREATER_THAN)
	{
		ret = diffPx - m_pParentStrategy->OpenThreshold() > 0.1;
	}
	else if (m_pParentStrategy->OpenCondition() == entity::GREATER_EQUAL_THAN)
	{
		ret = DoubleGreaterEqual(diffPx, m_pParentStrategy->OpenThreshold());
	}
	return ret;
}

void CArbitrageManualStrategyExecutor::OpenPosition(entity::PosiDirectionType direction, ARBI_DIFF_CALC diffPrices, StrategyContext* pContext, entity::Quote* pQuote, boost::chrono::steady_clock::time_point& timestamp, const string& comment)
{
	if (direction > entity::NET)
	{
		double lmtPrice[2];
		if (direction == entity::LONG)
		{
			lmtPrice[0] = diffPrices.BuyPrice;
			lmtPrice[1] = diffPrices.SellPrice;
		}
		else if (direction == entity::SHORT)
		{
			lmtPrice[0] = diffPrices.SellPrice;
			lmtPrice[1] = diffPrices.BuyPrice;
		}

		LOG_DEBUG(logger, boost::str(boost::format("Arbitrage Manual - %s Open position @ %.2f - %.2f (%s)")
			% GetPosiDirectionText(direction) % lmtPrice[0] % lmtPrice[1] % pQuote->update_time()));

		CPortfolioArbitrageOrderPlacer* pOrderPlacer = dynamic_cast<CPortfolioArbitrageOrderPlacer*>(m_orderPlacer.get());
		ArbitrageStrategyContext* arbiContext = dynamic_cast<ArbitrageStrategyContext*>(pContext);

		pOrderPlacer->SetMlOrderStatus(comment);

		FireEvent(EXEC_OPEN);
		pOrderPlacer->OpenPosition(direction, lmtPrice, 2, timestamp, trade::SR_Manual);
	}
}

bool CArbitrageManualStrategyExecutor::ClosePosition(ARBI_DIFF_CALC diffPrices, entity::Quote* pQuote, boost::chrono::steady_clock::time_point& timestamp, const string& comment, trade::SubmitReason reason)
{
	entity::PosiDirectionType direction = m_pParentStrategy->Direction();
	if (direction != entity::NET)
	{
		double lmtPrice[2];
		if (direction == entity::LONG)
		{
			lmtPrice[0] = diffPrices.SellPrice;
			lmtPrice[1] = diffPrices.BuyPrice;
		}
		else if (direction == entity::SHORT)
		{
			lmtPrice[0] = diffPrices.BuyPrice;
			lmtPrice[1] = diffPrices.SellPrice;
		}

		LOG_DEBUG(logger, boost::str(boost::format("Arbitrage Manual - %s Close position @ %.2f - %.2f (%s)")
			% GetPosiDirectionText(direction) % lmtPrice[0] % lmtPrice[1] % pQuote->update_time()));

		FireEvent(EXEC_CLOSE);
		CPortfolioArbitrageOrderPlacer* pOrderPlacer = dynamic_cast<CPortfolioArbitrageOrderPlacer*>(m_orderPlacer.get());
		pOrderPlacer->ClosePosition(m_volumeToClose, direction, lmtPrice, 2, timestamp, reason);

		ResetForceClose();
		pOrderPlacer->OutputStatus(boost::str(boost::format("%s - %s 平仓 @ %.2f - %.2f")
			% comment % GetPosiDirectionText(direction, true) % lmtPrice[0] % lmtPrice[1]));

		return true;
	}

	return false;
}

bool CArbitrageManualStrategyExecutor::GetLastOpenOrderId(string& outMlOrderId)
{
	if (State() == HOLD_POSITION)
	{
		CPortfolioArbitrageOrderPlacer* pOrderPlacer = dynamic_cast<CPortfolioArbitrageOrderPlacer*>(m_orderPlacer.get());
		if (pOrderPlacer->IsOpened())
		{
			outMlOrderId = pOrderPlacer->LastOpenOrderId();
			return true;
		}
	}
	return false;
}

void CArbitrageManualStrategyExecutor::Cleanup()
{
	LOG_DEBUG(logger, boost::str(boost::format("Executor(%d) Did 'Actual' Cleanup") % m_execId));
	if (m_orderPlacer.get() != NULL)
		m_orderPlacer->Cleanup();
}

void CArbitrageManualStrategyExecutor::SetOpened()
{
	CPortfolioArbitrageOrderPlacer* pOrderPlacer = dynamic_cast<CPortfolioArbitrageOrderPlacer*>(m_orderPlacer.get());
	if (pOrderPlacer != NULL)
		pOrderPlacer->SetOpened();
}
