#pragma once
#include <string>

class StringConverter
{
public:
	static std::wstring StringToWide(const std::string & str)
	{
		return std::wstring(str.begin(), str.end());
	}
};