//
// Copyright � 2016�2020 Neat Decisions. All rights reserved.
//
// This file is part of NeatMouse.
// The use and distribution terms for this software are covered by the
// Microsoft Public License (http://opensource.org/licenses/MS-PL)
// which can be found in the file LICENSE at the root folder.
//

#pragma once

#include "logic/MouseParams.h"
#include "neatcommon/ui/CustomizedControls.h"
#include "neatcommon/system/Helpers.h"

namespace neatmouse {

class CNeatToolbar : public CWindowImpl<CNeatToolbar, CToolBarCtrl>
{
public:
	CComboBox comboPresets;

	BOOL SubclassWindow(HWND hWnd);

	void UpdateButtonStates();
	void ChangeLanguageMenu(const std::string & langCode);
	void SetToolbarButtonText(UINT nId, LPCTSTR text);

	logic::MouseParams::Ptr GetCurrentSettings();
	void AddSettings(logic::MouseParams::Ptr mouseParams);
	bool DeleteCurrentSettings();
	void FillSettings();

	void localize();
	CMenuHandle GetMenus();
	void Init();
	void RedrawMe();

protected:
	BEGIN_MSG_MAP(CDetwinnerToolbar)
		MSG_WM_CTLCOLORSTATIC(OnCtlColorStatic)
		NOTIFY_CODE_HANDLER(TBN_DROPDOWN, OnToolbarDropDown)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
	END_MSG_MAP()

	HWND wndSelect = NULL;
	WTL::CMenu menus;
	neatcommon::ui::CAutosizeStatic labelPresets;
	std::map<std::string, int> langs;

	HBRUSH OnCtlColorStatic(CDCHandle dc, CStatic wndStatic);
	LRESULT OnSize(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled);
	LRESULT OnToolbarDropDown(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/);

	void LoadBitmaps();
	CSize getGUIFontSize();
	void RepositionRightAlignedItems();
	void RepositionCombobox();
	void CreatePresetsCombobox();

};

} // namespace neatmouse