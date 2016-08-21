#include "stdafx.h"
#include "STPriceBarDataSource.h"


CSTPriceBarDataSource::CSTPriceBarDataSource(const string& symbol, int precision)
{
}


CSTPriceBarDataSource::~CSTPriceBarDataSource()
{
}

bool CSTPriceBarDataSource::BuildKey(const string& symbol, int precision, string* outKey)
{
	*outKey = boost::str(boost::format("%s-%d") % symbol % precision);
	return !outKey->empty();
}