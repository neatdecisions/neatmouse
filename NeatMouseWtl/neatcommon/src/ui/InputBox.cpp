//
// Copyright © 2016–2020 Neat Decisions. All rights reserved.
//
// This file is part of NeatMouse.
// The use and distribution terms for this software are covered by the
// Microsoft Public License (http://opensource.org/licenses/MS-PL)
// which can be found in the file LICENSE at the root folder.
//

#include "stdafx.h"
#include "neatcommon/ui/InputBox.h"

namespace neatcommon {
namespace ui {


//---------------------------------------------------------------------------------------------------------------------
CInputBox::CInputBox(
	const CString & caption, const CString & prompt,
	const CString & ok, const CString & cancel) :
	m_caption(caption), m_prompt(prompt), m_ok(ok), m_cancel(cancel)
{}


//---------------------------------------------------------------------------------------------------------------------
CString CInputBox::getText() const
{
	return _text;
}


//---------------------------------------------------------------------------------------------------------------------
LRESULT CInputBox::OnButtonOk(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/) {
	ATL::CWindow editBox(GetDlgItem(ID_EDIT));
	int nLen = editBox.GetWindowTextLength();
	editBox.GetWindowText(_text.GetBufferSetLength(nLen), nLen + 1);
	EndDialog(TRUE);
	return TRUE;
}


//---------------------------------------------------------------------------------------------------------------------
LRESULT CInputBox::OnButtonCancel(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	return EndDialog(FALSE);
}


//---------------------------------------------------------------------------------------------------------------------
LRESULT CInputBox::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& 	bHandled)
{
	SetWindowText(m_caption);
	GetDlgItem(ID_STATIC).SetWindowText(m_prompt);
	GetDlgItem(IDOK).SetWindowText(m_ok);
	GetDlgItem(IDCANCEL).SetWindowText(m_cancel);
	CFont font = AtlGetStockFont(DEFAULT_GUI_FONT);
	SetFont(font);
	GetDlgItem(ID_STATIC).SetFont(font);
	GetDlgItem(IDOK).SetFont(font);
	GetDlgItem(IDCANCEL).SetFont(font);
	GetDlgItem(ID_EDIT).SetFont(font);
	CenterWindow();
	GetDlgItem(ID_EDIT).SetFocus();
	return bHandled = FALSE;
}


//---------------------------------------------------------------------------------------------------------------------
LRESULT CInputBox::OnEditTextChange(WORD, WORD, HWND)
{
	GetDlgItem(IDOK).EnableWindow(GetDlgItem(ID_EDIT).GetWindowTextLength() > 0 ? TRUE : FALSE);
	return TRUE;
}


}}