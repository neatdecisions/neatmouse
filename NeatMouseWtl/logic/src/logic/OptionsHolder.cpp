//
// Copyright © 2016–2019 Neat Decisions. All rights reserved.
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
void COptionsHolder::ClearSettings()
{
	for (MouseParams::Ptr pm : options)
	{
		delete pm;
	}
	options.clear();
}


//---------------------------------------------------------------------------------------------------------------------
COptionsHolder::~COptionsHolder()
{
	ClearSettings();
}


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

	this->defaultSettingsFileName = mif.readStringValue(L"General", L"dsfn", L"");
	this->lang = mif.readUtf8Value(L"General", L"lang", language);

	LoadOptions();
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

	mif.writeStringValue(L"General", L"dsfn", defaultSettingsFileName);
	mif.writeUtf8Value(L"General", L"lang", lang);

	mif.save(filePath);
	mFileName = filePath;
}


//---------------------------------------------------------------------------------------------------------------------
std::wstring COptionsHolder::GetDefaultSettingsFileName() const
{
	return defaultSettingsFileName;
}


//---------------------------------------------------------------------------------------------------------------------
void COptionsHolder::SetDefaultSettingsPresetFileName(const std::wstring & value)
{
	defaultSettingsFileName = value;
}

std::size_t COptionsHolder::GetSettingsCount() const
{
	return options.size();
}


//---------------------------------------------------------------------------------------------------------------------
MouseParams::Ptr COptionsHolder::GetSettings(std::size_t i)
{
	if (i >= options.size()) return MouseParams::Ptr(nullptr);
	return options[i];
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
	ClearSettings();

	MouseParams::Ptr opts(new MouseParams(true));
	opts->Load(optionsFolder + L"\\default");
	opts->Name = L"(Default)";
	opts->Save(optionsFolder + L"\\default");
	options.push_back(opts);

	WIN32_FIND_DATA fd;
	HANDLE hFind = FindFirstFile((optionsFolder + L"\\*.nmp").c_str(), &fd);

	bool res = INVALID_HANDLE_VALUE != hFind;

	if (res)
	{
		do
		{
			std::wstring fn = optionsFolder + L"\\" + fd.cFileName;
			MouseParams::Ptr optsItem(new MouseParams());
			optsItem->Load(fn);
			options.push_back(optsItem);
		} while (FindNextFile(hFind, &fd));

		FindClose(hFind);
	}
}


//---------------------------------------------------------------------------------------------------------------------
MouseParams::Ptr COptionsHolder::CreateNewSettings(const std::wstring & proposedName)
{
	MouseParams::Ptr mouseParams(new MouseParams());

	std::wstring name = proposedName;
	int n = 0;

	while (std::any_of(options.begin(), options.end(), [&name](MouseParams::Ptr & params) { return params->Name == name; }))
	{
		name = proposedName + _T(" (") + std::to_wstring(++n) + _T(")");
	}

	mouseParams->Name = name;

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

	mouseParams->Save(finalName);

	options.push_back(mouseParams);
	return mouseParams;
}


//---------------------------------------------------------------------------------------------------------------------
void COptionsHolder::DeleteSettings(MouseParams::Ptr * mouseParams)
{
	if ((*mouseParams)->IsPreset()) return;
	std::vector<MouseParams::Ptr>::iterator it;
	while ((it = std::find(options.begin(), options.end(), *mouseParams)) != options.end())
		options.erase(it);

	DeleteFile((*mouseParams)->FileName.c_str());
	delete *mouseParams;
	*mouseParams = 0;
}

}}