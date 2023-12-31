/***

Copyright 2023, SV Foster. All rights reserved.

License:
	This program is free for personal, educational and/or non-profit usage

Revision History:

***/

#pragma once


class SettingsBase
{
public:
	SettingsBase(CONST std::wstring& Path);
	virtual ~SettingsBase() noexcept;
	SettingsBase(const SettingsBase& oth) = delete;
	SettingsBase& operator=(const SettingsBase& oth) = delete;
	SettingsBase(SettingsBase&& oth) = delete;
	SettingsBase& operator=(SettingsBase&& oth) = delete;

protected:
	std::wstring RegPath;
	HKEY RegistryKey;

	DWORD ReadDWORD(CONST std::wstring& VName, CONST DWORD Default) const noexcept;
	COLORREF ReadRGB(CONST std::wstring& VName, CONST COLORREF Default) const noexcept;
	BOOL WriteDWORD(CONST std::wstring& VName, DWORD Value) const noexcept;
	BOOL WriteRGB(CONST std::wstring& VName, COLORREF Value) const noexcept;
};
