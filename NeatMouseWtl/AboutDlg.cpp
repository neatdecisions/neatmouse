//
// Copyright © 2016–2020 Neat Decisions. All rights reserved.
//
// This file is part of NeatMouse.
// The use and distribution terms for this software are covered by the
// Microsoft Public License (http://opensource.org/licenses/MS-PL)
// which can be found in the file LICENSE at the root folder.
//

#include "stdafx.h"
#include "resource.h"
#include "AboutDlg.h"

#include "logic/MainSingleton.h"
#include "neatcommon/ui/CustomizedControls.h"
#include "neatcommon/system/AutorunManager.h"

namespace neatmouse {

namespace {
	constexpr TCHAR * kRegistryStartupValueName = _T("NeatMouse");
}


//=====================================================================================================================
// CAboutDlg
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
void
CAboutDlg::localize()
{
	GetDlgItem(IDC_STATIC_ITALIAN).SetWindowText(_("about.translation-contrib"));
	SetWindowText(_("about.caption"));
	visitLink.SetLabel(_("about.lnk-visit-site"));
	someIcons.SetText(_("about.some-icons"));

	CRect layoutRect;
	someIcons.GetWindowRect(&layoutRect);
	ScreenToClient(&layoutRect);

	ykLink.SetWindowPos(HWND_BOTTOM, layoutRect.right, layoutRect.top, 0, 0, SWP_NOSIZE);

	CButton btn;
	btn.Attach(GetDlgItem(IDOK));
	btn.Detach();

	btn.Attach(GetDlgItem(IDC_CHECK_RUN_AT_STARTUP));
	btn.SetWindowText(_("about.chk-run-at-startup"));
	btn.Detach();
}


//---------------------------------------------------------------------------------------------------------------------
LRESULT
CAboutDlg::OnInitDialog(HWND, LRESULT)
{
	CenterWindow(GetParent());

	// check if registry setting says that program should run at startup
	CButton cfu;
	cfu.Attach(GetDlgItem(IDC_CHECK_RUN_AT_STARTUP));
	cfu.SetCheck(neatcommon::system::AutorunManager().getAutostartEnabled(kRegistryStartupValueName) ? BST_CHECKED : BST_UNCHECKED);
	cfu.Detach();

	SetTextBackGround(0xFFFFFF);

	LONG lStyle = GetWindowLong(GWL_STYLE);
	lStyle &= ~(WS_THICKFRAME | WS_MINIMIZE | WS_MAXIMIZE | WS_SYSMENU);
	SetWindowLong(GWL_STYLE, lStyle);

	SetWindowPos(NULL, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER);

	neatcommon::system::ProductInfo info;
	neatcommon::system::GetProductVersion(info);

	CString s(_T("NeatMouse"));

	titleFont.CreateFont(32, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, OEM_CHARSET, 0, 0, DEFAULT_QUALITY, FF_ROMAN, _T("Verdana"));
	programName.Attach(GetDlgItem(IDC_STATIC_PROGRAMNAME));
	programName.SetFont(titleFont);
	programName.SetText(s);

	s.Format(_T("v%d.%02d.%03d"),
		info.major,
		info.minor,
		MAKELONG(info.build2, info.build1));

	versionFont.CreateFont(0, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, OEM_CHARSET, 0, 0, DEFAULT_QUALITY, FF_ROMAN, _T("Verdana"));
	programVersion.Attach(GetDlgItem(IDC_STATIC_VERSION));
	programVersion.SetWindowText(s);

	ykLink.SetHyperLinkExtendedStyle(HLINK_UNDERLINEHOVER);
	ykLink.SubclassWindow(GetDlgItem(IDC_STATIC_YK));
	ykLink.SetHyperLink(L"https://p.yusukekamiyamane.com");

	visitLink.SetHyperLinkExtendedStyle(HLINK_UNDERLINEHOVER);
	visitLink.SubclassWindow(GetDlgItem(IDC_STATIC_VISIT));
	visitLink.SetHyperLink(L"https://neatdecisions.com");

	link.SetHyperLinkExtendedStyle(HLINK_UNDERLINEHOVER);
	link.SubclassWindow(GetDlgItem(IDC_STATIC_UPDATE));

	link.SetHyperLink(L"https://github.com/neatdecisions/neatmouse");
	link.SetLabel(_("about.github"));

	someIcons.Attach(GetDlgItem(IDC_STATIC_SOMEICONS));

	localize();
	Invalidate();

	return TRUE;
}


//---------------------------------------------------------------------------------------------------------------------
LRESULT
CAboutDlg::OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	EndDialog(wID);
	return 0;
}


//---------------------------------------------------------------------------------------------------------------------
LRESULT
CAboutDlg::OnRunAtStartup(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CButton checkBox(GetDlgItem(IDC_CHECK_RUN_AT_STARTUP));
	neatcommon::system::AutorunManager().enableAutostart(kRegistryStartupValueName, checkBox.GetCheck() == BST_CHECKED);
	checkBox.Detach();
	return 0;
}


//---------------------------------------------------------------------------------------------------------------------
LRESULT
CAboutDlg::OnSetCursor(CWindow /*wndTopLevel*/, UINT /*nHitTest*/, UINT /*message*/)
{
	SetCursor(AtlLoadSysCursorImage(OCR_NORMAL, LR_SHARED | LR_DEFAULTSIZE));
	SetMsgHandled(TRUE);
	return TRUE;
}


//---------------------------------------------------------------------------------------------------------------------
LRESULT
CAboutDlg::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	titleFont.DeleteObject();
	versionFont.DeleteObject();
	bHandled = FALSE;
	return 1;
}


//---------------------------------------------------------------------------------------------------------------------
LRESULT
CAboutDlg::OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	PAINTSTRUCT ps;
	HDC hDC = BeginPaint(&ps);
	{
		CSize sz;
		CRect rc;
		GetClientRect(rc);
		CBitmapHandle bmp = SafeLoadPng(IDB_PNG_LOGO);
		bmp.GetSize(sz);
		neatcommon::ui::DrawBitmapAdvanced(hDC, bmp, rc.right - sz.cx - 10, 0, sz.cx, sz.cy);

		CWindow wnd = GetDlgItem(IDC_STATIC_UPDATE);
		if (wnd.GetWindowRect(&rc))
		{
			ScreenToClient(&rc);
			rc.left -= 20;
			neatcommon::ui::DrawBitmapAdvanced(hDC, SafeLoadPng(IDB_PNG_GITHUB), rc.left, rc.top, 16, 16);
		}
	}
	EndPaint(&ps);
	bHandled = FALSE;
	return 1;
}

} // namespace neatmouse