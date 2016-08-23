#pragma once

class CStrategyOperation
{
protected:
	~CStrategyOperation()
	{}

public:

	virtual void OpenPosition(const string& symbol, double price, int vol, const string& timestamp) = 0;
	virtual void ClosePosition(const string& symbol, double price, int vol, const string& timestamp) = 0;
};
