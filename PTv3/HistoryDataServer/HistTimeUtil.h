#pragma once

boost::chrono::seconds ParseTimeString(const char* time);

int ParseTimePoints(const string& timePointExpr, vector<boost::posix_time::time_duration>& outTimePoints);

