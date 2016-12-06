//
// Copyright © 2016 Neat Decisions. All rights reserved.
//
// This file is part of NeatMouse.
// The use and distribution terms for this software are covered by the 
// Microsoft Public License (http://opensource.org/licenses/MS-PL) 
// which can be found in the file LICENSE at the root folder.
//

#include "stdafx.h"

#include "neatcommon/system/AutorunManager.h"

#include "Winreg.h"

namespace neatcommon {
namespace system {

const unsigned int kExecutablePathBufSize = 16383;


//---------------------------------------------------------------------------------------------------------------------
void 
AutorunManager::enableAutostart(const std::wstring & iKey, bool value) const
{
	HKEY hOpened;
	if (ERROR_SUCCESS != RegOpenKeyEx(HKEY_CURRENT_USER, _T("Software\\Microsoft\\Windows\\CurrentVersion\\Run"), 0, KEY_ALL_ACCESS, &hOpened))
	{
		return;
	}

	if (value)
	{
		TCHAR pPath[kExecutablePathBufSize];
		ZeroMemory(pPath, sizeof(pPath));
		GetModuleFileName(0, pPath, kExecutablePathBufSize - 1);
		unsigned int size = _tcslen(pPath) * sizeof(TCHAR) + 1;
		RegSetValueEx(hOpened, iKey.c_str(), 0, REG_SZ, (LPBYTE)pPath, size);
	}
	else
	{
		RegDeleteValueW(hOpened, iKey.c_str());
	}

	RegCloseKey(hOpened);
}


//---------------------------------------------------------------------------------------------------------------------
bool 
AutorunManager::getAutostartEnabled(const std::wstring & iKey) const
{
	bool result = false;
	HKEY hOpened;

	if (RegOpenKeyEx(HKEY_CURRENT_USER, _T("Software\\Microsoft\\Windows\\CurrentVersion\\Run"), 0, KEY_READ, &hOpened) == ERROR_SUCCESS)
	{
		TCHAR value[kExecutablePathBufSize];
		DWORD valueSize = kExecutablePathBufSize - 1;
		DWORD i = 0;

		while (RegEnumValue(hOpened, i, value, &valueSize, NULL, NULL, NULL, NULL) == ERROR_SUCCESS)
		{
			std::wstring sValue(value, valueSize);
			if (sValue == iKey)
			{
				result = true;
				break;
			}
			
			i++;
			valueSize = kExecutablePathBufSize - 1;
		}
		RegCloseKey(hOpened);
	}	

	return result;
}


}}