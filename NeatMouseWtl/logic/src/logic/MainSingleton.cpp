//
// Copyright © 2016–2019 Neat Decisions. All rights reserved.
//
// This file is part of NeatMouse.
// The use and distribution terms for this software are covered by the 
// Microsoft Public License (http://opensource.org/licenses/MS-PL) 
// which can be found in the file LICENSE at the root folder.
//

#include "StdAfx.h"

#include <shellapi.h>
#include <Shlobj.h>

#include "logic/MainSingleton.h"


namespace neatmouse {
namespace logic {

//=====================================================================================================================
// MainSingleton
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
MainSingleton & 
MainSingleton::Instance()
{
	static MainSingleton mainSingleton;
	return mainSingleton;
}


//---------------------------------------------------------------------------------------------------------------------
void
MainSingleton::NotifyEnabling(bool enabled)
{
	if (!emulationNotifier) return;

	if (GetMouseParams()->showNotifications)
	{
		emulationNotifier->Notify(enabled);
	}

	UpdateCursor();
}


//---------------------------------------------------------------------------------------------------------------------
bool 
MainSingleton::selectLocale(const std::string & langCode)
{
	for (const neatcommon::system::LocaleUiDescriptor & aLocale : locales)
	{
		if (aLocale.code == langCode)
		{
			localizer.load(aLocale.fileId, L"LANG");
			return true;
		}
	}
	return false;
}


//---------------------------------------------------------------------------------------------------------------------
const std::vector<neatcommon::system::LocaleUiDescriptor> & 
MainSingleton::GetLocales() const
{
	return locales;
}


//---------------------------------------------------------------------------------------------------------------------
const neatcommon::system::LocaleUiDescriptor &
MainSingleton::GetFallbackLocale() const
{
	assert(!locales.empty());
	return locales.front();
}


//---------------------------------------------------------------------------------------------------------------------
MainSingleton::~MainSingleton() 
{
	if (GetMouseParams() != nullptr)
	{
		optionsHolder.SetDefaultSettingsPresetFileName(neatcommon::system::GetFileName(GetMouseParams()->FileName));
	}
	optionsHolder.Save();

	if (mutex) CloseHandle(mutex);
}


//---------------------------------------------------------------------------------------------------------------------
unsigned short 
MainSingleton::Init(const std::vector<neatcommon::system::LocaleUiDescriptor> & iLocales)
{
	locales = iLocales;
	selectLocale(GetFallbackLocale().code);

	TCHAR szPath[MAX_PATH];
	bool optionsLoaded = false;

	const DWORD n = GetModuleFileName(NULL, szPath, _countof(szPath) - 1);

	if (n > 0)
	{
		szPath[n] = _T('\0');
		std::wstring folderPath(szPath);
		const size_t pos = folderPath.find_last_of(L'\\');
		if (pos != folderPath.npos)
		{
			folderPath = folderPath.substr(0, pos);
			if (!neatcommon::system::FileExists(folderPath + L"\\nonportable"))
			{
				folderPath.append(_T("\\settings"));
				CreateDirectory(folderPath.c_str(), NULL);
				optionsHolder.SetOptionsFolder(folderPath);
				paramsPath = folderPath + _T("\\settings.ini");
				optionsHolder.Load(paramsPath);
				optionsLoaded = true;
			}
		}
	}

	if (!optionsLoaded && (S_OK == ::SHGetFolderPath(NULL, CSIDL_LOCAL_APPDATA, NULL, SHGFP_TYPE_CURRENT, szPath)))
	{
		std::wstring s(szPath);
		s = s + _T("\\Neat Decisions");
		CreateDirectory(s.c_str(), NULL);
		s = s + _T("\\NeatMouse");
		CreateDirectory(s.c_str(), NULL);
		optionsHolder.SetOptionsFolder(s);
		paramsPath = s + _T("\\settings.ini");
		optionsHolder.Load(paramsPath);
	}

	for (size_t i = 0; i < optionsHolder.GetSettingsCount(); i++)
	{
		if (neatcommon::system::GetFileName(optionsHolder.GetSettings(i)->FileName) == optionsHolder.GetDefaultSettingsFileName())
		{
			SetMouseParams(optionsHolder.GetSettings(i));
		}
	}

	if ((mouseParams == nullptr) && (optionsHolder.GetSettingsCount() > 0))
	{
		SetMouseParams(optionsHolder.GetSettings(0));
	}

	assert(mouseParams != nullptr);

	selectLocale(optionsHolder.GetLanguageCode());

	SetLastError(0);
	mutex = CreateMutex(NULL, FALSE, _T("NeatMouse"));
	if (GetLastError() == ERROR_ALREADY_EXISTS || GetLastError() == ERROR_ACCESS_DENIED) return 1;

	UpdateCursor();
	
	return 0;
}


//---------------------------------------------------------------------------------------------------------------------
MouseParams::Ptr
MainSingleton::GetMouseParams()
{
	return mouseParams;
}


//---------------------------------------------------------------------------------------------------------------------
bool 
MainSingleton::WereParametersChanged()
{
	ASSERT(mouseParams);
	return !initialMouseParams.IsEqual(mouseParams);
}


//---------------------------------------------------------------------------------------------------------------------
void
MainSingleton::AcceptMouseParams()
{
	initialMouseParams = *mouseParams;
	mouseParams->Save();
}


//---------------------------------------------------------------------------------------------------------------------
void
MainSingleton::RevertMouseParams()
{
	ASSERT(mouseParams);
	*mouseParams = initialMouseParams;
}


//---------------------------------------------------------------------------------------------------------------------
void 
MainSingleton::SetMouseParams(MouseParams::Ptr value)
{
	mouseParams = value;
	initialMouseParams = *mouseParams;
}


//---------------------------------------------------------------------------------------------------------------------
const MouseParams * const 
MainSingleton::GetInitialMouseParams() const
{
	return &initialMouseParams;
}


//---------------------------------------------------------------------------------------------------------------------
void 
MainSingleton::SetEmulationNotifier(const IEmulationNotifier::Ptr & notifier)
{
	emulationNotifier = notifier;
}


//---------------------------------------------------------------------------------------------------------------------
void 
MainSingleton::UpdateCursor()
{
	if (emulationNotifier) emulationNotifier->RefreshOverlay(GetMouseActioner().isEmulationActivated() && GetMouseParams()->changeCursor);
}

}}