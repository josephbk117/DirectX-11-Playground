#pragma once
#include <comdef.h>
#include "StringConverter.h"

#define COM_ERROR_IF_FAILED(hr, msg) if(FAILED(hr)) throw COMException(hr, msg, __FILE__ , __FUNCTION__ , __LINE__)

class COMException
{
public:
	COMException(HRESULT hr, const std::string& msg, const std::string& file, const std::string& function, int line)
	{
		_com_error er(hr);
		_msg = L"Error: " + StringConverter::StringToWide(msg) + L"\n";
		_msg += er.ErrorMessage();
		_msg += L"File: " + StringConverter::StringToWide(file);
		_msg += L"Function: " + StringConverter::StringToWide(function);
		_msg += L"Line: " + std::to_wstring(line);
	}

	const wchar_t* what()const
	{
		return _msg.c_str();
	}

private:
	std::wstring _msg;
};