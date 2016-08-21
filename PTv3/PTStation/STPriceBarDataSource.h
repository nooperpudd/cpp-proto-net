#pragma once

#include "STHistDataReader.h"

class CSTPriceBarDataSource
{
public:
	CSTPriceBarDataSource(const string& symbol, int precision);
	~CSTPriceBarDataSource();

	static bool BuildKey(const string& symbol, int precision, string* outKey);

private:
	CSTHistDataReader m_histDataReader;
};

typedef boost::shared_ptr<CSTPriceBarDataSource> PriceBarDataSourcePtr;

