//
// Copyright © 2016–2020 Neat Decisions. All rights reserved.
//
// This file is part of NeatMouse.
// The use and distribution terms for this software are covered by the
// Microsoft Public License (http://opensource.org/licenses/MS-PL)
// which can be found in the file LICENSE at the root folder.
//

#include "stdafx.h"

#include "logic/OptionsHolder.h"

namespace neatmouse {
namespace logic {


//---------------------------------------------------------------------------------------------------------------------
void COptionsHolder::Load(const std::wstring & filePath)
{
	neatcommon::system::MyIniFile mif;
	mif.load(filePath);
	mFileName = filePath;

	std::string language = "en";

	LANGID langId = GetUserDefaultUILanguage() & 0x00FF;
	switch (langId)
	{
	case LANG_FRENCH:
		language = "fr";
		break;
	case LANG_ITALIAN:
		language = "it";
		break;
	case LANG_RUSSIAN:
	case LANG_BELARUSIAN:
	case LANG_KAZAK:
	case LANG_TAJIK:
	case LANG_TATAR:
	case LANG_UZBEK:
		language = "ru";
		break;
	case LANG_UKRAINIAN:
		language = "ua";
		break;
	case LANG_GERMAN:
		language = "de";
		break;
	case LANG_POLISH:
		language = "pl";
		break;
	case LANG_GREEK:
		language = "gr";
		break;
	case LANG_ROMANIAN:
		language = "ro";
		break;
	}

	const std::wstring defaultSettingsFileName = mif.readStringValue(L"General", L"dsfn", L"");
	m_defaultSettingsName = mif.readStringValue(L"General", L"dsn", L"");
	this->lang = mif.readUtf8Value(L"General", L"lang", language);

	LoadOptions();

	// backward-compatibility loop to favor settings name over file name
	if (m_defaultSettingsName.empty() && !defaultSettingsFileName.empty())
	{
		for (const auto & kv : m_settings)
		{
			if (neatcommon::system::GetFileName(kv.second.GetFilePath()) == defaultSettingsFileName)
			{
				m_defaultSettingsName = kv.first;
				break;
			}
		}
	}
}


//---------------------------------------------------------------------------------------------------------------------
void COptionsHolder::Save()
{
	Save(mFileName);
}


//---------------------------------------------------------------------------------------------------------------------
void COptionsHolder::Save(const std::wstring & filePath)
{
	neatcommon::system::MyIniFile mif;

	mif.writeStringValue(L"General", L"dsn", m_defaultSettingsName);
	mif.writeUtf8Value(L"General", L"lang", lang);

	mif.save(filePath);
	mFileName = filePath;
}


//---------------------------------------------------------------------------------------------------------------------
std::wstring COptionsHolder::GetDefaultSettingsName() const
{
	return m_defaultSettingsName;
}


//---------------------------------------------------------------------------------------------------------------------
void COptionsHolder::SetDefaultSettingsName(const std::wstring & value)
{
	m_defaultSettingsName = value;
}


//---------------------------------------------------------------------------------------------------------------------
std::vector<std::wstring> COptionsHolder::GetAllSettingNames() const
{
	std::vector<std::wstring> result;
	for (const auto & kv : m_settings) result.push_back(kv.first);
	return result;
}


//---------------------------------------------------------------------------------------------------------------------
MouseParams COptionsHolder::GetSettings(const std::wstring & name)
{
	const auto it = m_settings.find(name);
	return (it == m_settings.end()) ? CreateNewSettings(name) : it->second;
}


//---------------------------------------------------------------------------------------------------------------------
void COptionsHolder::SetSettings(const std::wstring& name, const MouseParams& params)
{
	m_settings[name] = params;
}


//---------------------------------------------------------------------------------------------------------------------
void COptionsHolder::SetOptionsFolder(const std::wstring & path)
{
	optionsFolder = path;
}


//---------------------------------------------------------------------------------------------------------------------
const std::wstring & COptionsHolder::GetOptionsFolder() const
{
	return optionsFolder;
}


//---------------------------------------------------------------------------------------------------------------------
std::string COptionsHolder::GetLanguageCode() const
{
	return lang;
}


//---------------------------------------------------------------------------------------------------------------------
void COptionsHolder::SetLanguageCode(const std::string & langCode)
{
	lang = langCode;
}


//---------------------------------------------------------------------------------------------------------------------
void COptionsHolder::LoadOptions()
{
	m_settings.clear();

	MouseParams opts(true);
	opts.Load(optionsFolder + L"\\default");
	opts.Save(optionsFolder + L"\\default");
	m_settings.emplace(opts.GetName(), opts);

	WIN32_FIND_DATA fd;
	HANDLE hFind = FindFirstFile((optionsFolder + L"\\*.nmp").c_str(), &fd);

	bool res = INVALID_HANDLE_VALUE != hFind;

	if (res)
	{
		do
		{
			std::wstring fn = optionsFolder + L"\\" + fd.cFileName;
			MouseParams optsItem;
			optsItem.Load(fn);
			m_settings.emplace(optsItem.GetName(), optsItem);
		} while (FindNextFile(hFind, &fd));

		FindClose(hFind);
	}
}


//---------------------------------------------------------------------------------------------------------------------
MouseParams COptionsHolder::CreateNewSettings(const std::wstring & proposedName)
{
	std::wstring name = proposedName;
	int n = 0;

	while (m_settings.count(name) > 0)
	{
		name = proposedName + _T(" (") + std::to_wstring(++n) + _T(")");
	}

	MouseParams mouseParams(name);


	std::wstring fname;
	std::wstring badChars(L":\\/*?|<>");

	for (size_t i = 0; i < name.size(); i++)
	{
		if (badChars.find(name.at(i)) != badChars.npos)
			fname.append(L"_");
		else

			fname = fname + name.at(i);
	}

	if (fname.empty()) fname = L"(Untitled)";

	std::wstring finalName = optionsFolder + L"\\" + fname + L".nmp";
	int i = 1;
	while (::GetFileAttributes(finalName.c_str()) != DWORD(-1))
	{
		finalName = optionsFolder + L"\\" + fname + L" (" + std::to_wstring(i++) + L").nmp";
	}

	mouseParams.Save(finalName);

	m_settings.emplace(mouseParams.GetName(), mouseParams);
	return mouseParams;
}


//---------------------------------------------------------------------------------------------------------------------
void COptionsHolder::DeleteSettings(const std::wstring & name)
{
	auto it = m_settings.find(name);
	if (it == m_settings.end()) return;
	if (it->second.IsPreset()) return;
	DeleteFile(it->second.GetFilePath().c_str());
	m_settings.erase(it);
}

}}