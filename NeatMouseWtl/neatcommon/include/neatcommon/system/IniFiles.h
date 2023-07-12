//
// Copyright © 2016–2019 Neat Decisions. All rights reserved.
//
// This file is part of NeatMouse.
// The use and distribution terms for this software are covered by the 
// Microsoft Public License (http://opensource.org/licenses/MS-PL) 
// which can be found in the file LICENSE at the root folder.
//

#pragma once

#include <map>
#include <string>
#include <vector>

namespace neatcommon {
namespace system {


class MyIniFile
{
public:
	using IniValueMap = std::map<std::wstring, std::wstring>;

	void writeUtf8Value(const std::wstring & section, const std::wstring & name, const std::string & value);
	void writeStringValue(const std::wstring & section, const std::wstring & name, const std::wstring & value);
	void writeBoolValue(const std::wstring & section, const std::wstring & name, bool value);
	void writeIntValue(const std::wstring & section, const std::wstring & name, int value);
	void writeUIntValue(const std::wstring & section, const std::wstring & name, unsigned int value);
	void writeIntVector(const std::wstring& section, const std::wstring& name, std::vector<int> value);

	std::string readUtf8Value(const std::wstring & section, const std::wstring & name, const std::string & defaultValue = "");
	std::wstring readStringValue(const std::wstring & section, const std::wstring & name, const std::wstring & defaultValue = L"");
	bool readBoolValue(const std::wstring & section, const std::wstring & name, bool defaultValue = false);
	int readIntValue(const std::wstring & section, const std::wstring & name, int defaultValue = 0);
	unsigned int readUIntValue(const std::wstring & section, const std::wstring & name, unsigned int defaultValue = 0);
	std::vector<int> readIntVector(const std::wstring & section, const std::wstring & name, std::vector<int> defaultValue = { 100 });
	
	const IniValueMap & getSection(const std::wstring & section);
	void enumerateSections(std::vector<std::wstring> & sections);

	bool save(const std::wstring & fileName);
	bool load(const std::wstring & fileName);
	bool loadFromBuffer(const TCHAR * buffer);

protected:
	using IniSectionMap = std::map<std::wstring, std::map<std::wstring, std::wstring>>;
	IniSectionMap values;
	void parseLine(const std::wstring & line, std::wstring & currentSection);
};


}}