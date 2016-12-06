//
// Copyright © 2016 Neat Decisions. All rights reserved.
//
// This file is part of NeatMouse.
// The use and distribution terms for this software are covered by the 
// Microsoft Public License (http://opensource.org/licenses/MS-PL) 
// which can be found in the file LICENSE at the root folder.
//

#pragma once

namespace neatcommon {
namespace system {


typedef std::map<std::wstring, std::map<std::wstring, std::wstring>> IniSectionMap;
typedef std::map<std::wstring, std::wstring> IniValueMap;

class MyIniFile
{
public:
	void writeUtf8Value(const std::wstring & section, const std::wstring & name, const std::string & value);
	void writeStringValue(const std::wstring & section, const std::wstring & name, const std::wstring & value);
	void writeBoolValue(const std::wstring & section, const std::wstring & name, bool value);
	void writeIntValue(const std::wstring & section, const std::wstring & name, int value);
	void writeUIntValue(const std::wstring & section, const std::wstring & name, unsigned int value);
	void writeFileTimeValue(const std::wstring & section, const std::wstring & name, const FILETIME & value);
	void writeFloatValue(const std::wstring & section, const std::wstring & name, const float value);

	std::string readUtf8Value(const std::wstring & section, const std::wstring & name, const std::string & defaultValue = "");
	std::wstring readStringValue(const std::wstring & section, const std::wstring & name, const std::wstring & defaultValue = L"");
	bool readBoolValue(const std::wstring & section, const std::wstring & name, bool defaultValue = false);
	int readIntValue(const std::wstring & section, const std::wstring & name, int defaultValue = 0);
	unsigned int readUIntValue(const std::wstring & section, const std::wstring & name, unsigned int defaultValue = 0);
	FILETIME readFileTimeValue(const std::wstring & section, const std::wstring & name, const FILETIME & defaultValue);
	float readFloatValue(const std::wstring & section, const std::wstring & name, const float defaultValue = 0);
	
	const IniValueMap & getSection(const std::wstring & section);
	void enumerateSections(std::vector<std::wstring> & sections);

	bool save(const std::wstring & fileName);
	bool load(const std::wstring & fileName);
	bool loadFromBuffer(const TCHAR * buffer);

protected:
	IniSectionMap values;
	void parseLine(const std::wstring & line, std::wstring & currentSection);
};


}}