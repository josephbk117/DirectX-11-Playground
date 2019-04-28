#pragma once
#include <Windows.h>
#include <comdef.h>
#include "COMException.h"
#include "StringConverter.h"
class ErrorLogger
{
public:
	static void log(const std::string& message)
	{
		std::string errorMsg = "Error :" + message;
		MessageBoxA(NULL, errorMsg.c_str(), "Error", MB_ICONERROR);
	}
	static void log(HRESULT hr, const std::string& message)
	{
		_com_error comError(hr);
		std::wstring errorMsg = L"Error :" + StringConverter::StringToWide(message) + L"\n" + comError.ErrorMessage();
		MessageBoxW(NULL, errorMsg.c_str(), L"Error", MB_ICONERROR);
	}
	static void log(HRESULT hr, const std::wstring& message)
	{
		_com_error comError(hr);
		std::wstring errorMsg = L"Error :" + message + L"\n" + comError.ErrorMessage();
		MessageBoxW(NULL, errorMsg.c_str(), L"Error", MB_ICONERROR);
	}

	static void log(const COMException& e)
	{
		std::wstring errorMsg = e.what();
		MessageBoxW(NULL, errorMsg.c_str(), L"Error", MB_ICONERROR);
	}
};