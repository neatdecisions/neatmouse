//
// Copyright © 2016 Neat Decisions. All rights reserved.
//
// This file is part of NeatMouse.
// The use and distribution terms for this software are covered by the 
// Microsoft Public License (http://opensource.org/licenses/MS-PL) 
// which can be found in the file LICENSE at the root folder.
//

#include "StdAfx.h"

#include <sstream>
#include <fstream>

#include "neatcommon/system/IniFiles.h"


namespace neatcommon {
namespace system {


//---------------------------------------------------------------------------------------------------------------------
void 
MyIniFile::enumerateSections(std::vector<std::wstring> & sections)
{
	for (const IniSectionMap::value_type aSection : values)
	{
		sections.push_back(aSection.first);
	}
}


//---------------------------------------------------------------------------------------------------------------------
const IniValueMap & 
MyIniFile::getSection(const std::wstring & section)
{
	IniSectionMap::iterator it = values.find(section);
	if (it != values.end())
		return it->second;
	else
	{
		values.insert(std::pair<std::wstring, IniValueMap>(section, IniValueMap()));
		it = values.find(section);
		if (it == values.end())
			throw std::exception();
		return it->second;
	}	
}


//---------------------------------------------------------------------------------------------------------------------
void 
MyIniFile::writeUtf8Value(const std::wstring & section, const std::wstring & name, const std::string & value)
{
	std::wstring wVal;
	if (string2wstring(value, wVal))
	{
		writeStringValue(section, name, wVal);
	}
}


//---------------------------------------------------------------------------------------------------------------------
void 
MyIniFile::writeStringValue(const std::wstring & section, const std::wstring & name, const std::wstring & value)
{
	IniSectionMap::iterator it = values.find(section);
	if (it != values.end())
	{
		IniValueMap::iterator it1 = it->second.find(name);
		if (it1 != it->second.end())
			it1->second = value;
		else
			it->second.insert(std::make_pair(name, value));
	} else
	{
		IniValueMap t;
		t.insert(std::make_pair(name, value));
		values.insert(std::make_pair(section, t));
	}
}


//---------------------------------------------------------------------------------------------------------------------
void MyIniFile::writeBoolValue(const std::wstring & section, const std::wstring & name, bool value)
{
	writeUIntValue(section, name, value ? 1 : 0);
}


//---------------------------------------------------------------------------------------------------------------------
void MyIniFile::writeUIntValue(const std::wstring & section, const std::wstring & name, unsigned int value)
{
	writeStringValue(section, name, to_string<unsigned int>(value));
}


//---------------------------------------------------------------------------------------------------------------------
void MyIniFile::writeIntValue(const std::wstring & section, const std::wstring & name, int value)
{
	writeStringValue(section, name, to_string<int>(value));
}


//---------------------------------------------------------------------------------------------------------------------
void 
MyIniFile::writeFileTimeValue(const std::wstring & section, const std::wstring & name, const FILETIME & value)
{
	const std::wstring & s = to_string<DWORD>(value.dwHighDateTime) + L':' + to_string<DWORD>(value.dwLowDateTime);
	writeStringValue(section, name, s);
}


//---------------------------------------------------------------------------------------------------------------------
void 
MyIniFile::writeFloatValue(const std::wstring & section, const std::wstring & name, const float value)
{
	float intpart, fractpart;
	fractpart = modf(value, &intpart);
  std::wstring v1 = to_string(intpart);
	size_t t = v1.find_first_not_of(L"0123456789");
	if (t != v1.npos)
	{
		v1.erase(v1.begin() + t);
	}

	std::wstring v2 = to_string(fractpart);
	t = v2.find_last_not_of(L"0123456789");

	if (t != v1.npos)
	{
		v2.erase(0, t);
		v1.append(L".");
		v1.append(v2);
	}
		
	writeStringValue(section, name, v1);
}

	
//---------------------------------------------------------------------------------------------------------------------
float 
MyIniFile::readFloatValue(const std::wstring & section, const std::wstring & name, const float defaultValue)
{
	const std::wstring & s = readStringValue(section, name, to_string(defaultValue));
	size_t t = s.find(L".");
	float intpart = 0;
	float fracpart = 0;
	if (t != s.npos)
	{
		intpart = static_cast<float>(from_string_def<int>(s.substr(0, t - 1), 0));
		fracpart = static_cast<float>(from_string_def<int>(s.substr(t + 1), 0));

	} else
		intpart = static_cast<float>(from_string_def<int>(s, 0));

	while (fracpart > 1)
		fracpart /= 10;

	return intpart + fracpart;

}


//---------------------------------------------------------------------------------------------------------------------
std::string
MyIniFile::readUtf8Value(const std::wstring & section, const std::wstring & name, const std::string & defaultValue)
{
	const std::wstring & wVal = readStringValue(section, name);
	std::string res;
	if (wstring2string(wVal, res))
		return res;
	else
		return defaultValue;
}


//---------------------------------------------------------------------------------------------------------------------
std::wstring 
MyIniFile::readStringValue(const std::wstring & section, const std::wstring & name, const std::wstring & defaultValue)
{
	IniSectionMap::const_iterator it = values.find(section);
	if (it == values.end())
		return defaultValue;
	IniValueMap::const_iterator it1 = it->second.find(name);
	if (it1 == it->second.end())
		return defaultValue;
	return it1->second;
}


//---------------------------------------------------------------------------------------------------------------------
bool 
MyIniFile::readBoolValue(const std::wstring & section, const std::wstring & name, bool defaultValue)
{
	int iValue = readUIntValue(section, name, 2);
	switch (iValue)
	{
	case 0:
		return false;
	case 1:
		return true;
	default:
		return defaultValue;
	}
}


//---------------------------------------------------------------------------------------------------------------------
unsigned int 
MyIniFile::readUIntValue(const std::wstring & section, const std::wstring & name, unsigned int defaultValue)
{
	const std::wstring & sValue = readStringValue(section, name);
	return from_string_def<unsigned int>(sValue, defaultValue);
}


//---------------------------------------------------------------------------------------------------------------------
int 
MyIniFile::readIntValue(const std::wstring & section, const std::wstring & name, int defaultValue)
{
	const std::wstring & sValue = readStringValue(section, name);
	return from_string_def<int>(sValue, defaultValue);
}


//---------------------------------------------------------------------------------------------------------------------
FILETIME 
MyIniFile::readFileTimeValue(const std::wstring & section, const std::wstring & name, const FILETIME & defaultValue)
{
	const std::wstring & sValue = readStringValue(section, name);
	const size_t delimPos = sValue.find(L':');
	if (delimPos == std::string::npos)
	{
		return defaultValue;
	}
	else
	{
		const std::wstring & sHi = sValue.substr(0, delimPos);
		const std::wstring & sLo = sValue.substr(delimPos + 1);
		FILETIME ft;
		ft.dwHighDateTime = from_string_def<DWORD>(sHi, 0);
		ft.dwLowDateTime = from_string_def<DWORD>(sLo, 0);
		return ft;
	}
}


//---------------------------------------------------------------------------------------------------------------------
bool 
MyIniFile::save(const std::wstring & fileName)
{
	static const unsigned char BOM[2] = { 0xFF, 0xFE };
	FILE * fileHandle;
	if (_wfopen_s( &fileHandle, fileName.c_str(), L"wb" )) return false;
	if (fileHandle == NULL) return false;
	fwrite(BOM, sizeof(char), 2, fileHandle);
	for (const IniSectionMap::value_type aSection : values)
	{
		std::wstring s = L'[' + aSection.first + L"]\n";
		const wchar_t * t = s.c_str();
		std::size_t strSize = wcslen(t);
		if (fwrite(t, sizeof(wchar_t), strSize, fileHandle) != strSize) return false;
		for (const IniValueMap::value_type aValue : aSection.second)
		{
			s = aValue.first + L'=' + aValue.second + L'\n';
			t = s.c_str();
			strSize = wcslen(t);
			if (fwrite(t, sizeof(wchar_t), strSize, fileHandle) != strSize) return false;
		}
	}
	fclose(fileHandle);
	return true;
}


//---------------------------------------------------------------------------------------------------------------------
void
MyIniFile::parseLine(const std::wstring & line, std::wstring & currentSection)
{
	std::wstring s = line;
	// left trim
	while( (!s.empty()) && (s.at(0) == L' ' || s.at(0) == L'\t'))
		s.erase(0, 1);
			
	bool isSection = s.empty() ? false : s.at(0) == '[';

	// right trim
	if (isSection)
	{
		while ( !s.empty() && (s.at(s.size() - 1) != L']') )
			s.erase(s.size() - 1);
	} else
		while( (!s.empty()) && (s.at(s.size() - 1) == L'\n' || s.at(s.size() - 1) == L'\r'))
			s.erase(s.size() - 1);
	
	if (!s.empty())
	{
		if ((s.at(0) == L'[') && ((s.at(s.size() - 1) == L']')))
		{
			if (s.size() > 2)
				currentSection = s.substr(1, s.size() - 2);
			else
				currentSection = L"";
		} else
		{
			const size_t delimPos = s.find(L'=');
			if (delimPos == std::string::npos)
				writeStringValue(currentSection, s, L"");
			else
			{
				const std::wstring & name = s.substr(0, delimPos);
				const std::wstring & value = s.substr(delimPos + 1);
				writeStringValue(currentSection, name, value);
			}
		}
	}
}


//---------------------------------------------------------------------------------------------------------------------
bool
MyIniFile::loadFromBuffer(const TCHAR * iBuffer)
{
	const TCHAR *buffer = (TCHAR*)((char*)iBuffer + 2);
	values.clear();
	std::wstring currentSection = L"";
	wchar_t line[1025];
	std::wstringstream w(buffer);
	
	while (!w.eof())
	{
		w.getline(line, 1024);
		parseLine(line, currentSection);
	}
	return true;
}


//---------------------------------------------------------------------------------------------------------------------
bool 
MyIniFile::load(const std::wstring & fileName)
{
	values.clear();
	FILE * fileHandle;

	char BOM[2];
	if (_wfopen_s( &fileHandle, fileName.c_str(), L"rb" ))
		return false;
	wchar_t line[1025];

	fread(BOM, sizeof(char), 2, fileHandle);

	std::wstring currentSection = L"";

	while (fgetws(line, 1024, fileHandle))
	{
		parseLine(line, currentSection);
	}

	fclose(fileHandle);

	return true;
}

}}