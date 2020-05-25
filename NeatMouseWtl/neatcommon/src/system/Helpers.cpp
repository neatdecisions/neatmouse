//
// Copyright © 2016–2020 Neat Decisions. All rights reserved.
//
// This file is part of NeatMouse.
// The use and distribution terms for this software are covered by the
// Microsoft Public License (http://opensource.org/licenses/MS-PL)
// which can be found in the file LICENSE at the root folder.
//

#include "stdafx.h"
#include "neatcommon/system/Helpers.h"


namespace neatcommon {
namespace system {

#define CP_CURRENT CP_ACP


//---------------------------------------------------------------------------------------------------------------------
bool
wstring2string(const std::wstring & wVal, std::string & outValue)
{
	int sz = WideCharToMultiByte(CP_CURRENT, 0, wVal.c_str(), wVal.size(), 0, 0, 0, 0);
	if (sz > 0)
	{
		char * cVal = new char[sz + 1];
		WideCharToMultiByte(CP_CURRENT, 0, wVal.c_str(), wVal.size(), cVal, sz, 0, 0);
		cVal[sz] = 0;
		outValue = cVal;
		delete [] cVal;
		return true;
	}
	return false;
}


//---------------------------------------------------------------------------------------------------------------------
bool
string2wstring(const std::string & value, std::wstring & outValue)
{
	int sz = MultiByteToWideChar(CP_CURRENT, 0, value.c_str(), value.size(), 0, 0);
	if (sz > 0)
	{
		wchar_t * wcVal = new wchar_t[sz + 1];
		MultiByteToWideChar(CP_CURRENT, 0, value.c_str(), value.size(), wcVal, sz);
		wcVal[sz] = 0;
		outValue = wcVal;
		delete [] wcVal;
		return true;
	}
	return false;
}


//---------------------------------------------------------------------------------------------------------------------
bool
wstring2utf8string(const std::wstring & value, std::string & outValue)
{
	int sz = WideCharToMultiByte(CP_UTF8, 0, value.c_str(), value.size(), 0, 0, 0, 0);
	if (sz > 0)
	{
		char * cVal = new char[sz + 1];
		WideCharToMultiByte(CP_UTF8, 0, value.c_str(), value.size(), cVal, sz, 0, 0);
		cVal[sz] = 0;
		outValue = cVal;
		delete [] cVal;
		return true;
	}
	return false;
}


//---------------------------------------------------------------------------------------------------------------------
std::vector<std::string>
SplitString(const std::string & s, const std::string & delimeter)
{
	std::vector<std::string> splits;
	size_t prevPos = 0;
	size_t pos = s.find(delimeter, 0);

	while (true)
	{
		if (pos == s.npos)
		{
			splits.push_back(s.substr(prevPos));
			break;
		}
		splits.push_back(s.substr(prevPos, pos - prevPos));
		prevPos = pos + delimeter.size();
		pos = s.find(delimeter, prevPos);
	}
	return splits;
}


//---------------------------------------------------------------------------------------------------------------------
bool FileExists(const std::wstring & fileName)
{
	return GetFileAttributes(fileName.c_str()) != INVALID_FILE_ATTRIBUTES;
}


//---------------------------------------------------------------------------------------------------------------------
bool GetProductVersion(ProductInfo & info)
{
	bool res = false;
	TCHAR t[1001];
	DWORD n = GetModuleFileName(NULL, t, _countof(t) - 1);
	if (n == 0) return res;
	t[n] = _T('\0');
	DWORD dummy = 0;
	n = GetFileVersionInfoSize(t, &dummy);
	if (n == 0) return res;

	LPSTR verData = new char[n];
	UINT size = 0;
	LPBYTE lpBuffer = NULL;

	if ( GetFileVersionInfo(t, NULL, n, verData) &&
	     VerQueryValue(verData, _T("\\"), (VOID FAR * FAR*) & lpBuffer, &size) &&
	     (size > 0) )
	{
		const VS_FIXEDFILEINFO * verInfo = reinterpret_cast<VS_FIXEDFILEINFO *>(lpBuffer);
		if (verInfo->dwSignature == 0xfeef04bd)
		{
			info.major = HIWORD(verInfo->dwFileVersionMS);
			info.minor = LOWORD(verInfo->dwFileVersionMS);
			info.build1 = HIWORD(verInfo->dwFileVersionLS);
			info.build2 = LOWORD(verInfo->dwFileVersionLS);
			res = true;
		}
	}
	delete [] verData;

	return res;
}


//---------------------------------------------------------------------------------------------------------------------
BOOL IsWindowsVistaOrLater()
{
	OSVERSIONINFOEX osvi{};
	DWORDLONG dwlConditionMask = 0;
	BYTE op = VER_GREATER_EQUAL;

	// Initialize the OSVERSIONINFOEX structure.
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	osvi.dwMajorVersion = 6;
	osvi.dwMinorVersion = 0;
	osvi.wServicePackMajor = 2;
	osvi.wServicePackMinor = 0;

	// Initialize the condition mask.
	VER_SET_CONDITION( dwlConditionMask, VER_MAJORVERSION, op );
	VER_SET_CONDITION( dwlConditionMask, VER_MINORVERSION, op );
	VER_SET_CONDITION( dwlConditionMask, VER_SERVICEPACKMAJOR, op );
	VER_SET_CONDITION( dwlConditionMask, VER_SERVICEPACKMINOR, op );

	// Perform the test.
	return VerifyVersionInfo(
		&osvi,
		VER_MAJORVERSION | VER_MINORVERSION |
		VER_SERVICEPACKMAJOR | VER_SERVICEPACKMINOR,
		dwlConditionMask);
}


//---------------------------------------------------------------------------------------------------------------------
std::wstring
GetFileName(const std::wstring & fileName)
{
	size_t pos = fileName.rfind(L'\\');
	if (pos != fileName.npos)
	{
		return fileName.substr(pos + 1, fileName.size() - pos - 1);
	}
	return std::wstring();
}

}}