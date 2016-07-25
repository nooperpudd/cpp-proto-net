#pragma once

class CQuote
{
public:
	CQuote();
	~CQuote();
	void set_symbol(const char* val) { m_symbol = val; }
	const string& symbol() const { return m_symbol; }

	void set_trading_day(const char* val) { m_tradingDay = val; }
	const string& trading_day() const { return m_tradingDay; }

	void set_last(double lastPrice_) { m_last = lastPrice_; }
	double last() const { return m_last; }

	void set_open(double openPrice_) { m_open = openPrice_; }
	double open() const { return m_open; }

	void set_high(double highestPrice_) { m_high = highestPrice_; }
	double high() const { return m_high; }

	void set_low(double lowestPrice_) { m_low = lowestPrice_; }
	double low() const { return m_low; }

	void set_volume(int volume_) { m_volume = volume_; }
	int volume() const { return m_volume; }

	void set_close(double closePrice_) { m_close = closePrice_; }
	double close() const { return m_close; }

	void set_update_time(char* val) { m_updateTime = val; }
	const string& update_time() const { return m_updateTime; }

	void set_update_millisec(int updateMillisec_) { m_updateMillisec = updateMillisec_; }
	int update_millisec() const { return m_updateMillisec; }

	void set_bid(double bidPrice1_) { m_bid = bidPrice1_; }
	double bid() const { return m_bid; }

	void set_bid_size(int bidVolume1_) { m_bidSize = bidVolume1_; }
	int bid_size() const { return m_bidSize; }

	void set_ask(double askPrice1_) { m_ask = askPrice1_; }
	double ask() const { return m_ask; }

	void set_ask_size(int askVolume1_) { m_askSize = askVolume1_; }
	int ask_size() const { return m_askSize; }

	void set_exchange_id(char* val) { m_exchangeId = val; }
	const string& exchange_id() const { return m_exchangeId; }

	void set_exchange_symbol_id(char* val) { m_exchangeSymbolId = val; }
	const string& exchange_symbol_id() const { return m_exchangeSymbolId; }

	void set_average_price(double averagePrice_) { m_averagePrice = averagePrice_; }
	double average_price() const { return m_averagePrice; }

private:
	string m_symbol;
	string m_tradingDay;
	double m_last;
	double m_open;
	double m_high;
	double m_low;
	int	   m_volume;
	double m_close;
	string m_updateTime;
	int	   m_updateMillisec;
	double m_bid;
	int    m_bidSize;
	double m_ask;
	int	   m_askSize;
	string m_exchangeId;
	string m_exchangeSymbolId;
	double m_averagePrice;
};

typedef boost::function<void(boost::chrono::steady_clock::time_point&, CQuote*)> QuoteUpdateFunc;