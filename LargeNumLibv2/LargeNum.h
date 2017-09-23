#pragma once

#include <deque>

class LargeNum
{
public:
	LargeNum();
	virtual ~LargeNum();
protected:
	std::deque<int> value;
	std::deque<int> postDecVal;
	char sign;
};