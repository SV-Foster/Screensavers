/***

Copyright 2023, SV Foster. All rights reserved.

License:
	This program is free for personal, educational and/or non-profit usage

Revision History:

***/

#include <Windows.h>
#include <memory>
#include <string>
#include "SettingsBase.h"


SettingsBase::SettingsBase(CONST std::wstring& Path):
	RegPath(Path)
{
	LSTATUS RegCall;


	RegCall = RegCreateKeyEx
	(
		HKEY_CURRENT_USER,
		this->RegPath.c_str(),
		0,
		NULL,
		0,
		KEY_READ | KEY_WRITE,
		NULL,
		&this->RegistryKey,
		NULL
	);
	if (RegCall != ERROR_SUCCESS)
		this->RegistryKey = 0;
}

SettingsBase::~SettingsBase() noexcept
{
	RegCloseKey(this->RegistryKey);
}

DWORD SettingsBase::ReadDWORD(CONST std::wstring& VName, CONST DWORD Default) const noexcept
{
	LSTATUS RegCall;
	DWORD Data;
	DWORD Size = sizeof(Data);

	RegCall = RegGetValue
	(
		this->RegistryKey,
		NULL,
		VName.c_str(),
		RRF_RT_REG_DWORD,
		NULL,
		&Data,
		&Size
	);

	if (RegCall != ERROR_SUCCESS)
		return Default;

	return Data;
}

COLORREF SettingsBase::ReadRGB(CONST std::wstring& VName, CONST COLORREF Default) const noexcept
{
	LSTATUS RegCall;
	COLORREF Data;
	COLORREF Size = sizeof(Data);

	RegCall = RegGetValue
	(
		this->RegistryKey,
		NULL,
		VName.c_str(),
		RRF_RT_REG_BINARY,
		NULL,
		&Data,
		&Size
	);

	if (RegCall != ERROR_SUCCESS)
		return Default;

	return Data;
}

BOOL SettingsBase::WriteDWORD(CONST std::wstring& VName, DWORD Value) const noexcept
{
	LSTATUS RegCall;
	constexpr DWORD Size = sizeof(DWORD);

	RegCall = RegSetValueEx
	(
		this->RegistryKey,
		VName.c_str(),
		NULL,
		REG_DWORD,
		(CONST BYTE*)(&Value),
		Size
	);

	return (RegCall == ERROR_SUCCESS);
}

BOOL SettingsBase::WriteRGB(CONST std::wstring& VName, COLORREF Value) const noexcept
{
	LSTATUS RegCall;
	constexpr DWORD Size = sizeof(COLORREF);

	RegCall = RegSetValueEx
	(
		this->RegistryKey,
		VName.c_str(),
		NULL,
		REG_BINARY,
		(CONST BYTE*)(&Value),
		Size
	);

	return (RegCall == ERROR_SUCCESS);
}
