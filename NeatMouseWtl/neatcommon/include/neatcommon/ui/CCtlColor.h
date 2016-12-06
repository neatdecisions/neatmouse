/////////////////////////////////////////////////////////////////////////////////////////
// 
//  Author: Pablo Aliskevicius.
//  Copyright (C) 2004 Pablo Aliskevicius.
//
//  The code and information  is provided  by the  author and  copyright  holder 'as-is',
//  and any express or implied  warranties,  including,  but not  limited to, the implied
//  warranties of  merchantability and  fitness for a particular purpose  are disclaimed.
//  In no event shall the author or copyright holders be liable for any direct, indirect,
//  incidental, special, exemplary, or consequential damages (including,  but not limited
//  to, procurement of substitute goods or services;  loss of use,  data, or profits;  or
//  business  interruption)  however caused  and on any  theory of liability,  whether in
//  contract, strict liability,  or tort  (including negligence or otherwise)  arising in
//  any way out of the use of this software,  even if advised of the  possibility of such
//  damage.
//
/////////////////////////////////////////////////////////////////////////////////////////
/*
Usage:
======

1.  Include this header.
2.  Add this class to your inheritance list. In order to skip some handlers, use the ucFlags
    template parameter (e.g., its default value is ~(FLG_HANDLE_SCROLLBAR)).
3.  Chain its message map.
4.  On your creation handler, if you don't want the default colors, call SetTextColor and
	SetTextBackGround or SetBkBrush.
5.  If, from time to time, you want to change those colors, call those functions again: if you
    want to change the flags, use the protected variable m_Flags.


Example:
========

#include <CCtlColor.h>

class CAboutDlg : public CDialogImpl<CAboutDlg>
                , public CCtlColored<CAboutDlg, ~0>  // Add this line
{
public:
	enum { IDD = IDD_ABOUTBOX };

	BEGIN_MSG_MAP(CAboutDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDOK, OnCloseCmd)
		COMMAND_ID_HANDLER(IDCANCEL, OnCloseCmd)
		// Add this line. CCtlColoredThis is typedefed here for your comfort.
		CHAIN_MSG_MAP(CCtlColoredThis) 
	END_MSG_MAP()

	LRESULT CAboutDlg::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
       	// Add next two lines...
		SetTextBackGround(0xFfbF9F);  // Lightish kind of blue
		SetTextColor(RGB(0X60, 0, 0)); // Kind of red
		// or, if that's your pleasure, the next two.
		SetTextColor(::GetSysColor(COLOR_INFOTEXT)); 
		SetBkBrush(COLOR_INFOBK);

	    CenterWindow(GetParent());
		return TRUE;
    }

	LRESULT CAboutDlg::OnCloseCmd(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
	{
		EndDialog(wID);   
		return 0;
	}
	
};     
*/
/////////////////////////////////////////////////////////////////////////////////////////
#if !defined(__CCTLCOLOR_H__)
#define __CCTLCOLOR_H__

#pragma once
#include "stdafx.h"

// These flags define which WM_CTLCOLOR* messages will be handled.
enum {
	FLG_HANDLE_DLG          =  1     
  , FLG_HANDLE_STATIC       =  2
  , FLG_HANDLE_EDIT	        =  4
  , FLG_HANDLE_BTN     		=  8
  , FLG_HANDLE_LISTBOX		= 16
  , FLG_HANDLE_SCROLLBAR	= 32
//, FLG_HANDLE_MSGBOX       = 64 // This one is gone, check  
// "http://msdn.microsoft.com/library/default.asp?url=/library/en-us/winprog/winprog/deleted_windows_programming_elements.asp
// for details
};

template <class T, // This is a mixin
	UCHAR ucFlags = ~(FLG_HANDLE_SCROLLBAR)> // Flags to handle, all but scrollbar by default
	      class ATL_NO_VTABLE CCtlColored
{
		// Data members
	protected: 
		COLORREF m_TextColor;		 // The color used to draw text
		COLORREF m_clrBackGround;	 // The color used to draw text backgrounds
		HBRUSH m_brshBackGround;	 // The brush used to erase backgrounds
		bool m_bManaged;             // When true, ::DeleteObject(m_brshBackGround) will be called 
		UCHAR m_Flags;				 // Which messages the class will handle

		// Convenience type definition, for enabling a standard CHAIN_MESSAGE_MAP.
		typedef CCtlColored<T, ucFlags> CCtlColoredThis;

		BEGIN_MSG_MAP(CCtlColoredThis)
			if (m_Flags & FLG_HANDLE_DLG)
				MESSAGE_HANDLER(WM_CTLCOLORDLG,       OnColorDlg) // For dialog background.
			if (m_Flags & FLG_HANDLE_STATIC)
				MESSAGE_HANDLER(WM_CTLCOLORSTATIC,    OnColorDlg) // For static and read only edit.
			if (m_Flags & FLG_HANDLE_EDIT)
				MESSAGE_HANDLER(WM_CTLCOLOREDIT,      OnColorDlg) // For edit boxes
			if (m_Flags & FLG_HANDLE_BTN)
				MESSAGE_HANDLER(WM_CTLCOLORBTN,       OnColorDlg) // Owner-drawn only will respond.
			if (m_Flags & FLG_HANDLE_LISTBOX)
				MESSAGE_HANDLER(WM_CTLCOLORLISTBOX,   OnColorDlg) // List and combo.
			if (m_Flags & FLG_HANDLE_SCROLLBAR)
				MESSAGE_HANDLER(WM_CTLCOLORSCROLLBAR, OnColorDlg) // Scroll bars. No good for edit-box children.
/*
			if (m_Flags & FLG_HANDLE_MSGBOX)
				MESSAGE_HANDLER(WM_CTLCOLORMSGBOX,    OnColorDlg) // Message boxes, gone with 16-bit windows.
*/
		END_MSG_MAP()

	 	LRESULT OnColorDlg(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
		{
			HDC hdc = (HDC)	wParam;
			HWND hw = (HWND) lParam;
			T *pT = static_cast<T *>(this);
			return pT->DoHandleCtlColor(uMsg, hdc, hw);
		}
	public:
		// Overrideable function. You might want to set different colors for 
		// read-only edit boxes, or such...
		LRESULT DoHandleCtlColor(UINT /*uMsg*/,// One of the WM_CTLCOLOR* messages. 
			                     HDC hdc,  // DC of the control which sent the message.
								 HWND /*hw*/)  // Which control sent the message.
		{
			::SetTextColor(hdc, m_TextColor);
			if (CLR_INVALID != m_clrBackGround)
				::SetBkColor(hdc, m_clrBackGround);
			::SelectObject(hdc, m_brshBackGround);
			return (LRESULT) m_brshBackGround;
		}


		// Constructor, using 'reasonable defaults' (what I needed when I wrote it).
		CCtlColored(COLORREF txtcolor = CLR_INVALID, 
			     HBRUSH bkBrush = (HBRUSH) INVALID_HANDLE_VALUE)
		{
			if (CLR_INVALID == txtcolor)
				m_TextColor = ::GetSysColor(COLOR_WINDOWTEXT);
			else
				m_TextColor = txtcolor;
			if ((HBRUSH) INVALID_HANDLE_VALUE == bkBrush)
			{
				m_brshBackGround = ::GetSysColorBrush(COLOR_WINDOW);
				m_clrBackGround =  ::GetSysColor(COLOR_WINDOW);
			}
			else 
			{
				m_clrBackGround = CLR_INVALID;
				m_brshBackGround = bkBrush;
			}
			m_bManaged = false;
			m_Flags = ucFlags; 
		}
		~CCtlColored()
		{
			// Default behavior: do nothing, 
			// let caller delete the object if necessary.
			if (m_bManaged)
				::DeleteObject(m_brshBackGround);
		}

		// Function name	: SetTextColor
		// Description	    : Replaces the current text color.
		// Return type		: COLORREF (the former text color)
		// Argument         : COLORREF newColor - The new text color.
		COLORREF SetTextColor(COLORREF newColor)
		{
			COLORREF ret = m_TextColor;
			m_TextColor = newColor;
			return ret;
		}

		// Function name	: SetTextBackGround
		// Description	    : Sets the passed color as text background, and creates a solid
		//					  brush from it, to erase the background of controls before 
		//					  drawing on them.
		// Return type		: COLORREF (The former text background)
		// Argument         : COLORREF newColor - The new text background.
		COLORREF SetTextBackGround(COLORREF newColor)
		{	  
			// Careful, there! Make sure you delete objects when necessary!
			m_brshBackGround = ::CreateSolidBrush(newColor);
			DeleteOldObject();
			m_bManaged = true;
			COLORREF ret = m_clrBackGround;
			m_clrBackGround = newColor;
			return ret;
		}
		// Function name	: SetBkBrush
		// Description	    : This function sets the background color and brush, 
		//					  using ::GetSysColorBrush(nIndex) and ::GetSysColor(nIndex).
		//					  It returns the former brush (in case you want to delete it).
		// Return type		: HBRUSH 
		// Argument         : int nIndex - One of the ::GetSysColor() indexes.
		HBRUSH SetBkBrush(int nIndex)
		{
			HBRUSH ret = m_brshBackGround;
			m_brshBackGround = ::GetSysColorBrush(nIndex);
			DeleteOldObject();
			m_clrBackGround  = ::GetSysColor(nIndex); 
			m_bManaged = false;
			return ret;
		}

		// Function name	: SetBkBrush
		// Description	    : This function gives the caller maximum latitude, letting
		//                    it set any brush (not necessarily solid) and any bacground
		//                    color (not necessarily similar to the brush's color).
		// Return type		: HBRUSH 
		// Argument         : HBRUSH NewBrush - The new brush you'd like to set.
		// Argument         : bool bManaged   - If true, the class will adopt the brush
		//                                      and delete it as needed.
		// Argument         : COLORREF clrBackGround - Since any brush goes, the caller
		//										should send a background color as similar
		//                                      as possible to that of the brush.
		HBRUSH SetBkBrush(HBRUSH NewBrush, 
			              bool bManaged = false, 
						  COLORREF clrBackGround = CLR_INVALID)
		{
			HBRUSH ret = m_brshBackGround;
			m_brshBackGround = NewBrush;
			DeleteOldObject();
			m_bManaged = bManaged;
			m_clrBackGround = clrBackGround; 
			return ret;
		}
	private:
		void DeleteOldObject(void)
		{
			if (m_bManaged)
			{
				T *pT = static_cast<T *>(this);
				HDC hdc = pT->GetDC();
				::DeleteObject(::SelectObject(hdc, m_brshBackGround));
			}
		}
};

// Bonus color-game functions

inline COLORREF ClrRotateRight(COLORREF src)
{ 
	return (src >> 8) | ((src & 0xff) << 16);
}

inline COLORREF ClrRotateLeft(COLORREF src)
{ 
	return (src >> 16) | ((src & 0xffff) << 8);
}


inline COLORREF ClrComplement(COLORREF src)
{ 
	return (0xff - (src & 0xff)) |
		   ((0xff - ((src >> 8) & 0xff)) << 8) |
		   ((0xff - ((src >> 16) & 0xff)) << 16);
}

#ifndef TOLERANCE
   #define TOLERANCE 0x10
#endif

// Thanks to alucardx,
// http://www.codeproject.com/script/profile/whos_who.asp?id=1046439

inline COLORREF CalcContrastColor (COLORREF src)
{
    if (
        std::abs((int)((src ) & 0xFF) - 0x80) <= TOLERANCE &&
        std::abs((int)((src >> 8) & 0xFF) - 0x80) <= TOLERANCE &&
        std::abs((int)((src >> 16) & 0xFF) - 0x80) <= TOLERANCE
    ) 
		return (0x7F7F7F + src) & 0xFFFFFF;
    else 
		return src ^ 0xFFFFFF;
}

#endif //__CCTLCOLOR_H__