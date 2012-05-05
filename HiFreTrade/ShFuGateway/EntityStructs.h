#pragma once

#define SYMBOL_LEN 31
#define TRADINDG_DAY_LEN 9
#define UPDATE_TIME_LEN 9

struct QuoteData
{
	char caSymbol[SYMBOL_LEN];
	char caTradingDay[TRADINDG_DAY_LEN];
	double dLast;
	double dPrevClose;
	double dOpen;
	double dHigh;
	double dLow;
	int iVolume;
	double dTurnover;
	double dClose;
	char caUpdateTime[UPDATE_TIME_LEN];
	int iUpdateMillisec;

	double dBid;
	int iBidSize;
	double dAsk;
	int iAskSize;
};

struct BreakOutStrategy
{
	// Entry condition
	int iRectPeriod;
	int iRectRange;
	bool bUpBreak;
	bool bDownBreak;
	int iBreakoutCriterion;
	int iBreakoutTimespan;

	// Exit condition
	int iGainLimit;

	// StopLoss condition
	int iLossLimit;
};

// iDirection
#define SHORT_BREAKOUT 0
#define LONG_BREAKOUT 1

// iEntryType and iExitType
#define SHORT_OPEN = 0
#define LONG_OPEN = 1
#define SHORT_CLOSE = 2
#define LONG_CLOSE = 3

// iEntryStatus and iExitStatus
#define UNKNOWN = 0
#define ORDER_SUBMIT = 1
#define PARTIALLY_FILLED = 2
#define FULL_FILLED = 3
#define CANCELED = 4

// iExitReason
#define STOP_GAIN = 0
#define STOP_LOSS = 1
#define MANUAL_CLOSE = 2

struct OperationRecord
{
	int iRecordId;
	char caSymbol[SYMBOL_LEN];
	char caRectPeriodBegin[UPDATE_TIME_LEN];
	char caRectPeriodEnd[UPDATE_TIME_LEN];

	double dUpperBoundary;
	double dLowerBoundary;
	double dRange;
	int iDirection;

	double dEntryPoint;
	char caEntryTime[UPDATE_TIME_LEN];
	int iEntryType;
	int iEntryQuantity;
	int iEntryStatus;

	double dExitPoint;
	char caExitTime[UPDATE_TIME_LEN];
	int iExitReason;
	int iExitType;
	int iExitQuantity;
	int iExitStatus;

	double ProfitLoss;
};

struct TimeNSalesData
{
	char caSymbol[SYMBOL_LEN];

	char caTimeStamp[UPDATE_TIME_LEN];
	
	int iQuantity;
	int iDirection;
};

