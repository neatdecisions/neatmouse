//
// Copyright © 2016–2019 Neat Decisions. All rights reserved.
//
// This file is part of NeatMouse.
// The use and distribution terms for this software are covered by the 
// Microsoft Public License (http://opensource.org/licenses/MS-PL) 
// which can be found in the file LICENSE at the root folder.
//

#pragma once

#include "stdafx.h"
#include "neatcommon/system/localization.h"

namespace neatcommon {
namespace system {

//=====================================================================================================================
// CFormatter
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
std::wstring CFormatter::str() const
{
	std::wstring res;
	size_t prev = 0;
	size_t pos = fmtString.find(L'%');
	while (pos != fmtString.npos)
	{
		if (pos > prev)
			res.append(fmtString.substr(prev, pos - prev));
		size_t pos1 = fmtString.find(L'%', pos + 1);
		if (pos1 - pos == 1)
			res.append(L"%");
		else
		{
			if (pos1 == fmtString.npos)
			{
				prev = fmtString.size();
				//assert here
				break;
			}
			size_t paramNum = from_string_def(fmtString.substr(pos + 1, pos1 - pos - 1), 0);
			if (paramNum > 0 && paramNum <= values.size())
			{
				res.append(values[paramNum - 1]);
			} else
			{
				// assert here
			}
		}
		prev = pos1 + 1;
		pos = fmtString.find(L'%', prev);
	}
	if (prev < fmtString.size())
	{
		res.append(fmtString.substr(prev, fmtString.size() - prev));
	}
	return res;
}



//=====================================================================================================================
// LocalizeSection
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
LocalizeSection::LocalizeSection()
{
	defaultString = L"{}";
}


//---------------------------------------------------------------------------------------------------------------------
void 
LocalizeSection::Clear()
{
	sections.clear();
	values.clear();
}


//---------------------------------------------------------------------------------------------------------------------
const std::wstring &
LocalizeSection::GetValue(const std::string & value) const
{
	std::map<std::string, std::wstring>::const_iterator it = values.find(value);
	if (it != values.end())
		return it->second;
	else
	{
		ASSERT(false);
		return defaultString;
	}
}


//---------------------------------------------------------------------------------------------------------------------
const LocalizeSection::Ptr 
LocalizeSection::GetSection(const std::string & section) const
{
	std::map<std::string, LocalizeSection::Ptr>::const_iterator it = sections.find(section);
	return (it != sections.end()) ? it->second : LocalizeSection::Ptr();
}


//---------------------------------------------------------------------------------------------------------------------
LocalizeSection::Ptr 
LocalizeSection::GetSection(const std::string & section)
{
	std::map<std::string, LocalizeSection::Ptr>::iterator it = sections.find(section);
	return (it != sections.end()) ? it->second : LocalizeSection::Ptr();
}


//---------------------------------------------------------------------------------------------------------------------
LocalizeSection::Ptr
LocalizeSection::AddSection(const std::string & name)
{
	LocalizeSection::Ptr res = std::make_shared<LocalizeSection>();
	sections.emplace(name, res);
	return res;
}


//---------------------------------------------------------------------------------------------------------------------
void 
LocalizeSection::SetValue(const std::string & name, const std::wstring & value)
{
	values.emplace(name, value);
}



//=====================================================================================================================
// CLocalizer
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
CLocalizer::CLocalizer() : defaultString(L"{}"), section(std::make_shared<LocalizeSection>())
{}


//---------------------------------------------------------------------------------------------------------------------
void 
CLocalizer::SetValue(const std::string & path, const std::wstring & value)
{
	const std::vector<std::string> & splits = SplitString(path, ".");
	LocalizeSection::Ptr currentSection = section;
	for (size_t i = 0; i < splits.size() - 1; i++)
	{
		LocalizeSection::Ptr previousSection = currentSection;
		currentSection = currentSection->GetSection(splits[i]);
		if (!currentSection) currentSection = previousSection->AddSection(splits[i]);
	}
	return currentSection->SetValue(splits[splits.size() - 1], value);
}


//---------------------------------------------------------------------------------------------------------------------
const std::wstring & 
CLocalizer::GetValue(const std::string & path) const
{
	const std::vector<std::string> & splits = SplitString(path, ".");
	LocalizeSection::Ptr currentSection = section;
	for (size_t i = 0; i < splits.size() - 1; i++)
	{
		currentSection = currentSection->GetSection(splits[i]);
		if (!currentSection)
		{
			ASSERT(false);
			return defaultString;
		}
	}
	return currentSection->GetValue(splits[splits.size() - 1]);
}


//---------------------------------------------------------------------------------------------------------------------
void 
CLocalizer::loadFromIniFile(MyIniFile & iniFile)
{
	section->Clear();
	std::vector<std::wstring> sections;
	iniFile.enumerateSections(sections);
	for (const std::wstring & s : sections)
	{
		for (const auto & aValue: iniFile.getSection(s))
		{
			std::string ss;
			wstring2string(s + L"." + aValue.first, ss);
			SetValue(ss, aValue.second);
		}
	}
}


//---------------------------------------------------------------------------------------------------------------------
void 
CLocalizer::load(UINT resourceId, const std::wstring & iResourceType)
{
	HRSRC hrsrc = FindResource(0, MAKEINTRESOURCE(resourceId), iResourceType.c_str());
	if (hrsrc)
	{
		const DWORD sz = SizeofResource(0, hrsrc);
		HGLOBAL hResourceLoaded = LoadResource(NULL, hrsrc);
		if (hResourceLoaded != NULL)
		{
			LPWSTR str = static_cast<LPWSTR>(LockResource(hResourceLoaded));
			MyIniFile iniFile;
			
			iniFile.loadFromBuffer(std::wstring(str, sz / sizeof(TCHAR)).c_str());
			loadFromIniFile(iniFile);
		}
	}
}


//---------------------------------------------------------------------------------------------------------------------
void 
CLocalizer::load(const std::wstring & fileName)
{
	MyIniFile iniFile;
	iniFile.load(fileName);
	loadFromIniFile(iniFile);
}


}}