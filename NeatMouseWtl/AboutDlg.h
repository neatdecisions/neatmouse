//
// Copyright © 2016 Neat Decisions. All rights reserved.
//
// This file is part of NeatMouse.
// The use and distribution terms for this software are covered by the 
// Microsoft Public License (http://opensource.org/licenses/MS-PL) 
// which can be found in the file LICENSE at the root folder.
//

#if !defined(AFX_ABOUTDLG_H__9FC06A67_16FB_4850_A710_A019A43266BF__INCLUDED_)
#define AFX_ABOUTDLG_H__9FC06A67_16FB_4850_A710_A019A43266BF__INCLUDED_


#include "neatcommon/ui/CCtlColor.h"
#include "neatcommon/ui/CustomizedControls.h"

namespace neatmouse {

//=====================================================================================================================
// CAboutDlg
//=====================================================================================================================
class CAboutDlg : public CDialogImpl<CAboutDlg>, 
                  public CCtlColored<CAboutDlg>
{
public:
  enum { IDD = IDD_ABOUTBOX };

  BEGIN_MSG_MAP_EX(CAboutDlg)
			MSG_WM_INITDIALOG(OnInitDialog)
      COMMAND_ID_HANDLER(IDOK, OnCloseCmd)
      COMMAND_ID_HANDLER(IDCANCEL, OnCloseCmd)
			COMMAND_ID_HANDLER(IDC_CHECK_RUN_AT_STARTUP, OnRunAtStartup) 
			MSG_WM_SETCURSOR(OnSetCursor)
			MESSAGE_HANDLER(WM_PAINT, OnPaint)
			MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
			CHAIN_MSG_MAP(CCtlColored<CAboutDlg>)
  END_MSG_MAP()

  LRESULT OnInitDialog(HWND, LRESULT);
  LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnSetCursor(CWindow wndTopLevel, UINT nHitTest, UINT message);
	LRESULT OnRunAtStartup(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
	LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);

	void localize();
	
protected:
	CHyperLink link, ykLink, visitLink;
	neatcommon::ui::CAutosizeStatic programName;
	CStatic programVersion;
	neatcommon::ui::CAutosizeStatic someIcons;

	CFont titleFont, versionFont;
};

} // namespace neatmouse

#endif // !defined(AFX_ABOUTDLG_H__9FC06A67_16FB_4850_A710_A019A43266BF__INCLUDED_)
