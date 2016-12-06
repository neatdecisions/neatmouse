//
// Copyright © 2016 Neat Decisions. All rights reserved.
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
	~COptionsHolder();
	void Load(const std::wstring & filePath);
	void Save();
	void Save(const std::wstring & filePath);
	std::wstring GetDefaultSettingsFileName() const;
	void SetDefaultSettingsPresetFileName(const std::wstring & value);
	std::size_t GetSettingsCount() const;
	MouseParams::Ptr GetSettings(std::size_t i);

	MouseParams::Ptr CreateNewSettings(const std::wstring & proposedName);
	void DeleteSettings(MouseParams::Ptr * mouseParams);

	void SetOptionsFolder(const std::wstring & path);
	const std::wstring & GetOptionsFolder() const;

	std::string GetLanguageCode() const;
	void SetLanguageCode(const std::string & langCode);

protected:
	std::string lang;
	std::wstring defaultSettingsFileName;
	std::wstring optionsFolder;
	std::vector<MouseParams::Ptr> options;
	void LoadOptions();
	std::wstring mFileName;
	void ClearSettings();
};

}}