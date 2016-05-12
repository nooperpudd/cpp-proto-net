#include "stdafx.h"
#include "HistAvatar.h"
#include "LogFactory.h"
#include "HistConfiguration.h"

log4cpp::Category& CHistAvatar::logger = CLogFactory::GetInstance().GetLogger("Scheduler");

CHistAvatar::CHistAvatar()
{
}


CHistAvatar::~CHistAvatar()
{
}

bool CHistAvatar::Start()
{
	CHistConfiguration* pConfig = CHistConfiguration::GetInstance();
	const string& tradingAddr = pConfig->GetTradingServerAddr();
	const string& brokerId = pConfig->GetBroker();
	const string& investorId = pConfig->GetAcct();
	const string& userId = pConfig->GetUserId();
	const string& passwd = pConfig->GetPasswd();

	if(!m_tradingConnection.Login(tradingAddr, brokerId, investorId, userId, passwd))
	{
		logger.error("Failed to login Trading Server %s.", tradingAddr);
		return false;
	}
	logger.info("Logged IN Trading Server successfully.");

	const string& marketDataAddr = pConfig->GetMarketDataSvrAddr();
	if(!m_marketDataConnection.Login(marketDataAddr, brokerId, investorId, userId, passwd))
	{
		logger.error("Failed to login MarketData Server %s.", marketDataAddr);
		return false;
	}
	logger.info("Logged IN Market Data Server successfully.");

	logger.info("Subscribed Quoting for symbols.");
	SubscribeQuotes();

	return true;
}

void CHistAvatar::Stop()
{
	logger.info("Unsubscribed symbols quoting.");
	UnsubscribeQuotes();

	if(m_marketDataConnection.IsConnected())
	{
		m_marketDataConnection.Logout();
		logger.info("Logged out Market Data Server.");
	}

	if(m_tradingConnection.IsConnected())
	{
		m_tradingConnection.Logout();
		logger.info("Logged out Trading Server.");
	}
}

void CHistAvatar::SubscribeQuotes()
{
	CHistConfiguration* pConfig = CHistConfiguration::GetInstance();
	vector<boost::tuple<string, string>> symbolItems;
	int count = pConfig->GetSymbolInfos(symbolItems);
	logger.info("Got %d symbols' subscription", count);

	for (int i = 0; i < count; ++i)
	{
		const string& symb = symbolItems[i].get<0>();
		const string& timeFrame = symbolItems[i].get<1>();
		SymbolQuotingPtr symbolQuoting(new CSymbolQuoting(symb, timeFrame));
		m_vecSymbolQuoting.push_back(symbolQuoting);
		symbolQuoting->Subscribe(&m_quoteRepositry);
	}
}

void CHistAvatar::UnsubscribeQuotes()
{
	for (SymbolQuotingVecIter iter = m_vecSymbolQuoting.begin();iter != m_vecSymbolQuoting.end(); ++iter)
	{
		SymbolQuotingPtr& symbolQuoting = *iter;
		symbolQuoting->Unsubscribe(&m_quoteRepositry);
	}

	m_vecSymbolQuoting.clear();
}
