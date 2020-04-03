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
	m_fileName = filePath;

	std::string language = "en";

	const LANGID langId = GetUserDefaultUILanguage() & 0x00FF;
	switch (langId)
	{
	case LANG_CHINESE:
		language = "cn";
		break;
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
	m_lang = mif.readUtf8Value(L"General", L"lang", language);

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

	if (m_defaultSettingsName.empty() && !m_settings.empty())
	{
		m_defaultSettingsName = m_settings.begin()->first;
	}
}


//---------------------------------------------------------------------------------------------------------------------
void COptionsHolder::Save()
{
	Save(m_fileName);
}


//---------------------------------------------------------------------------------------------------------------------
void COptionsHolder::Save(const std::wstring & filePath)
{
	neatcommon::system::MyIniFile mif;

	mif.writeStringValue(L"General", L"dsn", m_defaultSettingsName);
	mif.writeUtf8Value(L"General", L"lang", m_lang);

	mif.save(filePath);
	m_fileName = filePath;
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
	m_optionsFolder = path;
}


//---------------------------------------------------------------------------------------------------------------------
std::wstring COptionsHolder::GetOptionsFolder() const
{
	return m_optionsFolder;
}


//---------------------------------------------------------------------------------------------------------------------
std::string COptionsHolder::GetLanguageCode() const
{
	return m_lang;
}


//---------------------------------------------------------------------------------------------------------------------
void COptionsHolder::SetLanguageCode(const std::string & langCode)
{
	m_lang = langCode;
}


//---------------------------------------------------------------------------------------------------------------------
void COptionsHolder::LoadOptions()
{
	m_settings.clear();

	WIN32_FIND_DATA fd;
	HANDLE hFind = FindFirstFile((m_optionsFolder + L"\\*.nmp").c_str(), &fd);

	bool res = INVALID_HANDLE_VALUE != hFind;

	if (res)
	{
		do
		{
			const std::wstring fn = m_optionsFolder + L"\\" + fd.cFileName;
			MouseParams optsItem;
			optsItem.Load(fn);
			m_settings.emplace(optsItem.GetName(), optsItem);
		} while (FindNextFile(hFind, &fd));

		FindClose(hFind);
	}

	const std::wstring defaultSettingsPath = m_optionsFolder + L"\\default";
	if (neatcommon::system::FileExists(defaultSettingsPath))
	{
		MouseParams opts;
		opts.Load(defaultSettingsPath);
		m_settings.emplace(opts.GetName(), opts);
	}

	if (m_settings.empty())
	{
		MouseParams opts(L"(Default)");
		opts.Save(defaultSettingsPath);
		m_settings.emplace(opts.GetName(), opts);
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

	std::wstring finalName = m_optionsFolder + L"\\" + fname + L".nmp";
	int i = 1;
	while (::GetFileAttributes(finalName.c_str()) != DWORD(-1))
	{
		finalName = m_optionsFolder + L"\\" + fname + L" (" + std::to_wstring(i++) + L").nmp";
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
	DeleteFile(it->second.GetFilePath().c_str());
	m_settings.erase(it);
}

}}