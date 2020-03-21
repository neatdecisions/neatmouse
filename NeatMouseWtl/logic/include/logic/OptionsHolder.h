//
// Copyright © 2016–2020 Neat Decisions. All rights reserved.
//
// This file is part of NeatMouse.
// The use and distribution terms for this software are covered by the
// Microsoft Public License (http://opensource.org/licenses/MS-PL)
// which can be found in the file LICENSE at the root folder.
//

#pragma once

#include "MouseParams.h"

namespace neatmouse {
namespace logic {

class COptionsHolder
{
public:
	void Load(const std::wstring & filePath);
	void Save();
	void Save(const std::wstring & filePath);
	std::wstring GetDefaultSettingsName() const;
	void SetDefaultSettingsName(const std::wstring & value);
	std::vector<std::wstring> GetAllSettingNames() const;
	MouseParams GetSettings(const std::wstring & name);
	void SetSettings(const std::wstring & name, const MouseParams & params);
	MouseParams CreateNewSettings(const std::wstring & proposedName);
	void DeleteSettings(const std::wstring & name);

	void SetOptionsFolder(const std::wstring & path);
	std::wstring GetOptionsFolder() const;

	std::string GetLanguageCode() const;
	void SetLanguageCode(const std::string & langCode);

protected:
	std::string m_lang;
	std::wstring m_defaultSettingsName;
	std::wstring m_optionsFolder;
	std::map<std::wstring, MouseParams> m_settings;
	void LoadOptions();
	std::wstring m_fileName;
};

}}