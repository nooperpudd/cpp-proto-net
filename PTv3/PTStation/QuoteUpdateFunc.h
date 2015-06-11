#pragma once
#include "entity/quote.pb.h"

typedef boost::function<void(boost::chrono::steady_clock::time_point&, entity::Quote*)> QuoteUpdateFunc;
