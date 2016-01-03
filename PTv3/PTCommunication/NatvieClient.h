#pragma once

#include "IClientRequestHandler.h"
#include "tcpsocket/TCPSocket.h"
#include "entity/message.pb.h"
#include "PortfolioItem.h"
#include "StrategyItem.h"

#include <gcroot.h>
#include <msclr/auto_gcroot.h>
#include <string>
#include <vector>

class CNatvieClient : public TCPSocket
{
public:
	CNatvieClient(PTCommunication::IClientRequestHandler ^reqHandler);
	~CNatvieClient(void);

	bool ServerLogin(entity::ServerType svrType, const char* address, const char* brokerId, const char* investorId, const char* userId, const char* password);
	void ServerLogout(entity::ServerType svrType);

	void OnLoginResponse(entity::LoginResponse& resp);
	void OnLoginPuzzleResponse(entity::LoginPuzzleResponse& resp);
	void OnServerLoginResponse(entity::ServerLoginResponse& resp);
	void OnPortfolioUpdateResponse(entity::PortfolioUpdateItem& resp);
	void OnMultilegOrderUpdateResponse(trade::MultiLegOrder& resp);
	void OnLegOrderUpdateResponse(entity::LegOrderUpdateParam& legOrderUpdateParam);
	void OnTradeUpdateResponse(trade::Trade& resp);
	void OnHeartbeatResponse(const char* timestamp);

	void SendHeartbeat(const char* timestamp);
	void AddPortfolio(PTEntity::PortfolioItem ^portfolioItem);
	void AddPortfolios(cli::array<PTEntity::PortfolioItem^> ^portfolioItems);
	void PortfEnableStrategy(const char* portfId, bool isEnabled, int lastOrderId);
	void PortfPauseStrategy(const char* portfId, bool isSuspending);
	void PortfTurnSwitches(const char* portfId, int triggerIndex, bool enabled);
	void PortfChangeArbitrage(const char* portfId, bool isArbitrage);
	void ApplyStrategySettings(const char* portfId, PTEntity::StrategyItem ^strategyItem);
	void PortfModifyQuantity(const char* portfId, int perOpenQty, int perStartQty, int totalOpenLimit, int maxCancelQty,
		vector<string>& endTimePointsVec);
	void PortfChangePreferredLeg(const char* portfId, const char* legName);

	void PortfOpenPosition(const char* portfId);
	void PortfClosePosition(const char* portfId, const char* orderId);
	void PortfClosePosition(const char* portfId, int quantity);

	bool logged();
	void Logout();
	void setPseudo(const char* pseudo){ m_pseudo = pseudo; }
	bool IsDisconnected(){ return getStatus() == Disconnected; }

	void SetAlternativeUserIds(vector<string>& userIds){ m_userIds = userIds; }
	void SetMultiClient(bool multiple){ m_multiClient = multiple; }
	bool MultiClient(){ return m_multiClient; }

protected:
	virtual void onConnected();
	virtual void onConnectionClosed();

private:
	bool tryLogin(const string& clientKey);

	msclr::auto_gcroot<PTCommunication::IClientRequestHandler^> m_clr;

	string m_sessionId;
	string m_pseudo;

	bool m_multiClient;
	vector<string> m_userIds;
};

