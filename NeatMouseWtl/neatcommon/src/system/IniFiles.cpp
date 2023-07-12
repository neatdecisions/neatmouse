//
// Copyright © 2016–2019 Neat Decisions. All rights reserved.
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
#include "neatcommon/system/Helpers.h"


namespace neatcommon {
namespace system {


//---------------------------------------------------------------------------------------------------------------------
void 
MyIniFile::enumerateSections(std::vector<std::wstring> & sections)
{
	for (const IniSectionMap::value_type & aSection : values)
	{
		sections.push_back(aSection.first);
	}
}


//---------------------------------------------------------------------------------------------------------------------
const MyIniFile::IniValueMap &
MyIniFile::getSection(const std::wstring & section)
{
	IniSectionMap::iterator it = values.find(section);
	if (it != values.end())
	{
		return it->second;
	}
	else
	{
		values.emplace(section, IniValueMap());
		it = values.find(section);
		if (it == values.end()) throw std::exception();
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
		{
			it1->second = value;
		}
		else
		{
			it->second.emplace(name, value);
		}
	} else
	{
		IniValueMap t;
		t.emplace(name, value);
		values.emplace(section, t);
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
	writeStringValue(section, name, std::to_wstring(value));
}


//---------------------------------------------------------------------------------------------------------------------
void MyIniFile::writeIntValue(const std::wstring & section, const std::wstring & name, int value)
{
	writeStringValue(section, name, std::to_wstring(value));
}


//---------------------------------------------------------------------------------------------------------------------
void MyIniFile::writeIntVector(const std::wstring & section, const std::wstring & name, std::vector<int> value)
{
	writeStringValue(section, name, vector2wstring(value));
}


//---------------------------------------------------------------------------------------------------------------------
std::string
MyIniFile::readUtf8Value(const std::wstring & section, const std::wstring & name, const std::string & defaultValue)
{
	const std::wstring & wVal = readStringValue(section, name);
	std::string res;
	return (wstring2string(wVal, res)) ? res : defaultValue;
}


//---------------------------------------------------------------------------------------------------------------------
std::wstring 
MyIniFile::readStringValue(const std::wstring & section, const std::wstring & name, const std::wstring & defaultValue)
{
	IniSectionMap::const_iterator it = values.find(section);
	if (it == values.end()) return defaultValue;
	IniValueMap::const_iterator it1 = it->second.find(name);
	if (it1 == it->second.end()) return defaultValue;
	return it1->second;
}


//---------------------------------------------------------------------------------------------------------------------
bool 
MyIniFile::readBoolValue(const std::wstring & section, const std::wstring & name, bool defaultValue)
{
	switch (readUIntValue(section, name, 2))
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
std::vector<int>
MyIniFile::readIntVector(const std::wstring & section, const std::wstring & name, std::vector<int> defaultValue)
{
	const std::wstring & sValue = readStringValue(section, name);
	return vec_from_string_def<int>(sValue, defaultValue);
}


//---------------------------------------------------------------------------------------------------------------------
int 
MyIniFile::readIntValue(const std::wstring & section, const std::wstring & name, int defaultValue)
{
	const std::wstring & sValue = readStringValue(section, name);
	return from_string_def<int>(sValue, defaultValue);
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

	std::wstring currentSection;

	while (fgetws(line, 1024, fileHandle))
	{
		parseLine(line, currentSection);
	}

	fclose(fileHandle);

	return true;
}

}}