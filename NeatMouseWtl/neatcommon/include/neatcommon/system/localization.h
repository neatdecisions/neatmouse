//
// Copyright © 2016–2019 Neat Decisions. All rights reserved.
//
// This file is part of NeatMouse.
// The use and distribution terms for this software are covered by the 
// Microsoft Public License (http://opensource.org/licenses/MS-PL) 
// which can be found in the file LICENSE at the root folder.
//

#pragma once

#include "neatcommon/system/IniFiles.h"
#include <memory>

namespace neatcommon {
namespace system {

//=====================================================================================================================
// CFormatter
//=====================================================================================================================
class CFormatter
{
public:
	explicit CFormatter(const std::wstring & fmtString) : fmtString(fmtString) {}

	template <class T>
	CFormatter & operator % (const T & t)
	{
		std::wstringstream s;
		s << t;
		values.push_back(s.str());
		return *this;
	}

	std::wstring str() const;

protected:
	std::vector<std::wstring> values;
	std::wstring fmtString;
};


//=====================================================================================================================
// LocalizeSection
//=====================================================================================================================
class LocalizeSection
{
public:
	using Ptr = std::shared_ptr<LocalizeSection>;
	LocalizeSection();
	void Clear();
	const std::wstring & GetValue(const std::string & value) const;
	const Ptr GetSection(const std::string & section) const;
	Ptr GetSection(const std::string & section);
	Ptr AddSection(const std::string & name);
	void SetValue(const std::string & name, const std::wstring & value);

protected:
	std::wstring defaultString;
	std::map<std::string, std::wstring> values;
	std::map<std::string, Ptr> sections;

};


//=====================================================================================================================
// CLocalizer
//=====================================================================================================================
class CLocalizer
{
protected:
	std::wstring defaultString;
	LocalizeSection::Ptr section;
public:
	CLocalizer();
	void SetValue(const std::string & path, const std::wstring & value);
	const std::wstring & GetValue(const std::string & path) const;
	void loadFromIniFile(MyIniFile & iniFile);
	void load(UINT resourceId, const std::wstring & iResourceType);
	void load(const std::wstring & fileName);
};


//=====================================================================================================================
// LocaleUiDescriptor
//=====================================================================================================================
struct LocaleUiDescriptor
{
	std::string code;
	UINT fileId;
	UINT iconId;
	UINT menuItemId;
	LocaleUiDescriptor(const std::string & iCode, UINT iFileId, UINT iIconId, UINT iMenuItemId) :
		code(iCode), fileId(iFileId), iconId(iIconId), menuItemId(iMenuItemId)
	{}
};


}}