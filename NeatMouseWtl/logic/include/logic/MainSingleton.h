//
// Copyright © 2016 Neat Decisions. All rights reserved.
//
// This file is part of NeatMouse.
// The use and distribution terms for this software are covered by the 
// Microsoft Public License (http://opensource.org/licenses/MS-PL) 
// which can be found in the file LICENSE at the root folder.
//

#pragma once

#include <neatcommon/system/localization.h>
#include <neatcommon/ui/CustomizedControls.h>
#include "IEmulationNotifier.h"
#include "MouseParams.h"
#include "MouseActioner.h"
#include "OptionsHolder.h"

namespace neatmouse {
namespace logic {

class MainSingleton
{
public:
	static MainSingleton & Instance();

	neatcommon::ui::CImageManager & GetImageManager() { return imageManager; }
	neatcommon::system::CLocalizer & GetLocalizer() { return localizer; }
	COptionsHolder & GetOptionsHolder() { return optionsHolder; }
	MouseActioner & GetMouseActioner() { return mouseActioner; }

	bool selectLocale(const std::string & langCode);
	
	unsigned short Init(const std::vector<neatcommon::system::LocaleUiDescriptor> & iLocales);
	MouseParams::Ptr GetMouseParams();
	bool WereParametersChanged();
	void AcceptMouseParams();
	void RevertMouseParams();
	void SetMouseParams(MouseParams::Ptr value);
	const MouseParams * const GetInitialMouseParams() const;

	void NotifyEnabling(bool enabled);
	void UpdateCursor();
	void SetEmulationNotifier(IEmulationNotifier::Ptr notifier);

	const std::vector<neatcommon::system::LocaleUiDescriptor> & GetLocales() const;
	const neatcommon::system::LocaleUiDescriptor & GetFallbackLocale() const;
private:
	IEmulationNotifier::Ptr emulationNotifier;
	MouseActioner mouseActioner;
	HWND hwndMainWindow;
	COptionsHolder optionsHolder;
	MouseParams::Ptr mouseParams;
	MouseParams initialMouseParams;
	std::wstring ParamsPath;
	std::vector<neatcommon::system::LocaleUiDescriptor> locales;
	neatcommon::system::CLocalizer localizer;
	neatcommon::ui::CImageManager imageManager;
	HANDLE mutex;

	MainSingleton();
	~MainSingleton();
};


}}