#pragma once

boost::chrono::seconds ParseTimeString(const char* time);
boost::chrono::seconds ParseTimeString(const string& time);

int ParseTimePoints(const string& timePointExpr, vector<boost::posix_time::time_duration>& outTimePoints);

string GetISOTimeString(const boost::chrono::seconds& timepoint);

