#include "StdAfx.h"
#include "ParseZeusingMarketData.h"
#include "ThostTraderApi/ThostFtdcUserApiStruct.h"

#ifndef WIN32
#define strcpy_s strcpy
#endif

bool ParseMarketData(char* buffer, int nCount, CThostFtdcDepthMarketDataField* pDepthMarketData)
{
	memset(pDepthMarketData, 0, sizeof(CThostFtdcDepthMarketDataField));

	if (nCount < sizeof(TFTDCHeader))
	{
		cout << "Invalid Package!" << endl;
		return false;
	}
	TFTDCHeader header;
	memcpy(&header, buffer, sizeof(header));

	///校验一下包是否正确
	if (header.Chain != 'L' && header.Chain != 'C')
	{
		cout << "Invalid Package!, Invalid Header[" << header.Chain << "]" << endl;
		return false;
	}

	if (header.FTDCContentLength != (nCount - sizeof(TFTDCHeader)))
	{
		cout << "Invalid Package Length!" << endl;
		cout << "Content Len[" << nCount << "], Expected Len[" << sizeof(TFTDCHeader) << "]" << endl;
		return false;
	}

	if (header.TransactionId != 0x0000F103)
	{
		cout << "Not a Market Package!" << endl;
		return false;
	}

	static CMarketDataUpdateTimeField updateTimeField;
	memset(&updateTimeField, 0, sizeof(CMarketDataUpdateTimeField));
	static CMarketDataBestPriceField  bestPriceField;
	memset(&bestPriceField, 0, sizeof(CMarketDataBestPriceField));
	static CMarketDataStaticField	   staticField;
	memset(&staticField, 0, sizeof(CMarketDataStaticField));
	static CMarketDataLastMatchField  lastMatchField;
	memset(&lastMatchField, 0, sizeof(CMarketDataLastMatchField));

	static CFTDMarketDataBaseField    databaseField;
	memset(&databaseField, 0, sizeof(CFTDMarketDataBaseField));

	static CMarketDataAsk23Field ask23Field;
	memset(&ask23Field, 0, sizeof(CMarketDataAsk23Field));
	static CMarketDataAsk45Field ask45Field;
	memset(&ask45Field, 0, sizeof(CMarketDataAsk45Field));
	static CMarketDataBid23Field bid23Field;
	memset(&bid23Field, 0, sizeof(CMarketDataBid23Field));
	static CMarketDataBid45Field bid45Field;
	memset(&bid45Field, 0, sizeof(CMarketDataBid45Field));

	TFieldHeader fieldHeader;
	///遍历Field
	char* pz = buffer + sizeof(TFTDCHeader);
	for (int i = 0; i < header.FieldCount; i++)
	{
		memcpy(&fieldHeader, pz, sizeof(TFieldHeader));
		pz += sizeof(TFieldHeader);
		switch (fieldHeader.FieldID)
		{
		case 0x2439:
		{
			memcpy(&updateTimeField, pz, fieldHeader.Size);
			pz += fieldHeader.Size;
			//printf("Receive Data\n");
			//printf("	Instrument[%s]\n", updateTimeField.InstrumentID);
			//printf("	Time[%s]\n", updateTimeField.UpdateTime);
			//printf("	ms[%d]\n", updateTimeField.UpdateMillisec);
		}
			break;
		case 0x2434:
		{
			memcpy(&bestPriceField, pz, fieldHeader.Size);
			pz += fieldHeader.Size;
			//printf("best price\n");
			//printf("         BidPrice1[%f]\n", bestPriceField.BidPrice1);
			//printf("        BidVolume1[%d]\n", bestPriceField.BidVolume1);
			//printf("        AskPrice1[%f]\n", bestPriceField.AskPrice1);
			//printf("        AskVolume1[%d]\n", bestPriceField.AskVolume1);
		}
			break;
		case 0x2432:
		{
			memcpy(&staticField, pz, fieldHeader.Size);
			pz += fieldHeader.Size;
			//printf("staticfield\n");
			//printf("         OpenPrice[%f]\n", staticField.OpenPrice);
			//printf("         HighestPrice[%f]\n", staticField.HighestPrice);
			//printf("         LowestPrice[%f]\n", staticField.LowestPrice);
			//printf("         ClosePrice[%f]\n", staticField.ClosePrice);
			//printf("         UpperLimitPrice[%f]\n", staticField.UpperLimitPrice);
			//printf("         LowerLimitPrice[%f]\n", staticField.LowerLimitPrice);
			//printf("         SettlementPrice[%f]\n", staticField.SettlementPrice);
			//printf("         CurrDelta[%f]\n", staticField.CurrDelta);
		}
			break;
		case 0x2433:
		{
			memcpy(&lastMatchField, pz, fieldHeader.Size);
			pz += fieldHeader.Size;
			//printf("lastMatchField\n");
			//printf("         LastPrice[%f]\n", lastMatchField.LastPrice);
			//printf("         Volume[%d]\n", lastMatchField.Volume);
			//printf("         Turnover[%f]\n", lastMatchField.Turnover);
			//printf("         OpenInterest[%f]\n", lastMatchField.OpenInterest);
		}
			break;
		case 0x2435:
		{
			memcpy(&bid23Field, pz, fieldHeader.Size);
			pz += fieldHeader.Size;
		}
			break;
		case 0x2436:
		{
			memcpy(&ask23Field, pz, fieldHeader.Size);
			pz += fieldHeader.Size;
		}
			break;
		case 0x2437:
		{
			memcpy(&bid45Field, pz, fieldHeader.Size);
			pz += fieldHeader.Size;
		}
			break;
		case 0x2438:
		{
			memcpy(&ask45Field, pz, fieldHeader.Size);
			pz += fieldHeader.Size;
		}
			break;
		case 0x2431:
		{

			memcpy(&databaseField, pz, fieldHeader.Size);
			pz += fieldHeader.Size;
			//printf("Receive Data\n");
			//printf("        Tradingday[%s]\n", databaseField.TradingDay);
		}
			break;
		}
	}

	strcpy_s(pDepthMarketData->InstrumentID, updateTimeField.InstrumentID);
	strcpy_s(pDepthMarketData->ActionDay, updateTimeField.ActionDay);
	strcpy_s(pDepthMarketData->UpdateTime, updateTimeField.UpdateTime);
	pDepthMarketData->UpdateMillisec = updateTimeField.UpdateMillisec;

	pDepthMarketData->AskPrice1 = bestPriceField.AskPrice1;
	pDepthMarketData->AskVolume1 = bestPriceField.AskVolume1;
	pDepthMarketData->BidPrice1 = bestPriceField.BidPrice1;
	pDepthMarketData->BidVolume1 = bestPriceField.BidVolume1;

	pDepthMarketData->OpenPrice = staticField.OpenPrice;
	pDepthMarketData->HighestPrice = staticField.HighestPrice;
	pDepthMarketData->LowestPrice = staticField.LowestPrice;
	pDepthMarketData->ClosePrice = staticField.ClosePrice;
	pDepthMarketData->UpperLimitPrice = staticField.UpperLimitPrice;
	pDepthMarketData->LowerLimitPrice = staticField.LowerLimitPrice;
	pDepthMarketData->SettlementPrice = staticField.SettlementPrice;
	pDepthMarketData->CurrDelta = staticField.CurrDelta;

	pDepthMarketData->LastPrice = lastMatchField.LastPrice;
	pDepthMarketData->Volume = lastMatchField.Volume;
	pDepthMarketData->Turnover = lastMatchField.OpenInterest;

	strcpy_s(pDepthMarketData->TradingDay, databaseField.TradingDay);
	pDepthMarketData->PreClosePrice = databaseField.PreClosePrice;
	pDepthMarketData->PreSettlementPrice = databaseField.PreSettlementPrice;
	pDepthMarketData->PreDelta = databaseField.PreDelta;
	pDepthMarketData->PreOpenInterest = databaseField.PreOpenInterest;

	return true;
}

