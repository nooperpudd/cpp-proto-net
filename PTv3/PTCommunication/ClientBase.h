// PTCommunication.h

#pragma once

#include "NatvieClient.h"
#include "Enums.h"
#include "PortfolioItem.h"
#include "MultiLegOrder.h"
#include "Order.h"
#include "AccountInfo.h"
#include "OperationResult.h"
#include "SymbolInfo.h"
#include "IClientRequestHandler.h"

using namespace System;
using namespace System::Runtime::InteropServices;
using namespace System::Threading;
using namespace PTEntity;

namespace PTCommunication {

	public ref class ClientBase
	{
	private:
		CNatvieClient *_nativeClient;
		Timer ^_heartTimer;

		Object^ _pHeartbeatSync = gcnew Object();

	public:
		ClientBase(IClientRequestHandler ^requestHandler)
		{
			_nativeClient = new CNatvieClient(requestHandler);
		}
		~ClientBase()
		{
			this->!ClientBase();
		}
		!ClientBase()
		{
			if(_nativeClient != 0)
			{
				delete _nativeClient;
				_nativeClient = 0;
			}
		}

		property bool IsConnected
		{
			bool get()
			{
				return _nativeClient->logged();
			}
		}

		property bool MultiClient
		{
			bool get()
			{
				return _nativeClient->MultiClient();
			}
			void set(bool val)
			{
				_nativeClient->SetMultiClient(val);
			}
		}

		property array<String^> ^AlternativeUserIds
		{
			array<String^> ^get()
			{
				return _userIds;
			}
			void set(array<String^> ^userIds)
			{
				_userIds = userIds;
				SetUserIds(userIds);
			}
		}

		void Disconnect();

		void SetPseudo(String ^pseudo);

		bool Connect(String ^host, int port);

		void ServerLogin(ServerType svrType, String ^address, String ^brokerId, String ^investorId, String ^userId, String ^password);

		void ServerLogout(ServerType svrType);

		void AddPortfolio(PortfolioItem ^portfItem);

		void AddPortfCollection(array<PortfolioItem ^> ^portfItems);

		void RemovePortf(String ^portfId)
		{

		}

		void PortfOpenPosition(String ^pid, int quantity, bool isVirtual);

		void SwitchPosition(String ^pid, int quantity)
		{

		}

		void PortfClosePosition(String ^pid, String ^mlOrder, String ^legRef);

		void PortfClosePosition(String ^pid, int quantity);

		void CancelOrder(Order ^legOrder)
		{

		}

		void PortfModifyQuantity(String ^portfId, int perOpenQty, int perStartQty, int totalOpenLimit, int maxCancelQty, array<String^> ^timepoints);

		void PortfSetPreferredLeg(String ^portfId, String ^legName);

		void PortfEnableStrategy(String ^portfId, bool isEnabled, int lastOrderId);

		void PortfPauseStrategy(String ^portfId, bool suspending);

		void PortfTurnSwitches(String ^portfId, int triggerIndex, bool enabled);

		void PortfChangeArbitrage(String ^portfId, bool isArbitrage);

		void PortfApplyStrategySettings(String ^portfId, StrategyItem ^strategyItem);

		AccountInfo^ QueryAccountInfo()
		{
			return nullptr;
		}

		void QueryPositionDetails(String ^symbol)
		{

		}

		OperationResult^ ManualCloseOrder(String ^symbol, TradeDirectionType direction,
			DateTime ^openDate, int quantity)
		{
			return nullptr;
		}

		SymbolInfo^ QuerySymbolInfo(String ^symbol)
		{
			return nullptr;
		}

		void BeginHeartbeat();
		void EndHeartbeat();

	private:
		void SendHeartbeat(Object ^obj);
		void SetUserIds(array<String^> ^userIds);

		array<String^> ^_userIds;
	};
}
