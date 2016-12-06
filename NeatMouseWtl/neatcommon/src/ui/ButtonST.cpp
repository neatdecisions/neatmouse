//
//	Class:		CButtonST
//
//	Compiler:	Visual C++
//	Tested on:	Visual C++ 5.0
//				Visual C++ 6.0
//
//	Version:	See GetVersionC() or GetVersionI()
//
//	Created:	xx/xxxx/1998
//	Updated:	12/May/2001
//
//	Author:		Davide Calabro'		davide_calabro@yahoo.com
//

// --------------------------------------------------------------------------
// Description : Port of CButtonST to WTL (http://www.codeproject.com/buttonctrl/cbuttonst.asp)
// Author	   : Serge Weinstock
//
//	You are free to use, distribute or modify this code
//	as long as this header is not removed or modified.
// --------------------------------------------------------------------------

#include "stdafx.h"

#include "neatcommon/ui/ButtonST.h"
#include "neatcommon/ui/CustomizedControls.h"

namespace neatcommon {
namespace ui {

CButtonST::CButtonST()
{
	m_bMouseOnButton = false;
	FreeResources(false);
	// Default type is "flat" button
	m_bIsFlat = true; 
	// By default draw border in "flat" button 
	m_bDrawBorder = true; 
	// By default icon is aligned horizontally
	m_nAlign = ST_ALIGN_HORIZ; 
	// By default, for "flat" button, don't draw the focus rect
	m_bDrawFlatFocus = false;
	// By default the button is not the default button
	m_bIsDefault = false;
	// By default the button is not a checkbox
	m_bIsCheckBox = false;
	m_nCheck = false;
	// Set default colors
	SetDefaultColors(false);
	// No tooltip created
	m_ToolTip.m_hWnd = 0;
	// Do not draw as a transparent button
	m_bDrawTransparent = true;//false;
	m_pbmpOldBk = 0;
	// No URL defined
	::ZeroMemory(&m_szURL, sizeof(m_szURL));
	// No cursor defined
	m_hCursor = 0;
	// No autorepeat
	m_bAutoRepeat = false;
	m_dwPeriodAutoRepeat = 100;
	m_bIsPressed = false;
	m_bIsFocused = false;
	m_bIsDisabled = false;
}

CButtonST::~CButtonST()
{
	// Restore old bitmap (if any)
	if (m_dcBk != 0 && m_pbmpOldBk != 0)
		{
		m_dcBk.SelectBitmap(m_pbmpOldBk);
		}
	FreeResources();
	// Destroy the cursor (if any)
	if (m_hCursor != 0) ::DestroyCursor(m_hCursor);
}

DWORD CButtonST::SetDefaultColors(bool bRepaint)
{
	m_crColors[BTNST_COLOR_BK_IN]	= ::GetSysColor(COLOR_BTNFACE);
	m_crColors[BTNST_COLOR_FG_IN]	= ::GetSysColor(COLOR_BTNTEXT);
	m_crColors[BTNST_COLOR_BK_OUT]	= ::GetSysColor(COLOR_BTNFACE);
	m_crColors[BTNST_COLOR_FG_OUT]	= ::GetSysColor(COLOR_BTNTEXT);
	if (bRepaint) { Invalidate(); }
	return BTNST_OK;
}

void CButtonST::FreeResources(bool bCheckFor0)
{
	if (bCheckFor0)
		{
		// Destroy icons
		// Note: the following two lines MUST be here! even if
		// BoundChecker says they are unnecessary!
		if (m_csIcons[0].hIcon != 0) { ::DestroyIcon(m_csIcons[0].hIcon); }
		if (m_csIcons[1].hIcon != 0) { ::DestroyIcon(m_csIcons[1].hIcon); }
		// Destroy bitmaps
		if (m_csBitmaps[0].hBitmap != 0) { m_csBitmaps[0].hBitmap.DeleteObject(); }
		if (m_csBitmaps[1].hBitmap != 0) { m_csBitmaps[1].hBitmap.DeleteObject(); }
		// Destroy mask bitmaps
		if (m_csBitmaps[0].hMask != 0) { m_csBitmaps[0].hMask.DeleteObject(); }
		if (m_csBitmaps[1].hMask != 0) { m_csBitmaps[1].hMask.DeleteObject(); }
		}
	::ZeroMemory(&m_csIcons, sizeof(m_csIcons));
	::ZeroMemory(&m_csBitmaps, sizeof(m_csBitmaps));
} // End of FreeResources


DWORD CButtonST::SetIcon(_U_STRINGorID nIconInId, _U_STRINGorID nIconOutId, HMODULE rsrcModule)
{
	HICON		hIconIn;
	HICON		hIconOut;
	HINSTANCE	hInstResource;

	// Find correct resource handle
	hInstResource = (rsrcModule != 0 ? rsrcModule : _Module.GetResourceInstance());
	// Set icon when the mouse is IN the button
	hIconIn = (HICON)::LoadImage(hInstResource, nIconInId.m_lpstr, IMAGE_ICON, 0, 0, 0);
  	// Set icon when the mouse is OUT the button
	hIconOut = (HICON)::LoadImage(hInstResource, nIconOutId.m_lpstr, IMAGE_ICON, 0, 0, 0);
	return SetIcon(hIconIn, hIconOut);
}

DWORD CButtonST::SetIcon(HIMAGELIST imagelist, int idxIn, int idxOut)
{
	HICON		hIconIn;
	HICON		hIconOut;
	CImageList	il(imagelist);

	// Set icon when the mouse is IN the button
	hIconIn = il.GetIcon(idxIn, ILD_TRANSPARENT);
  	// Set icon when the mouse is OUT the button
	hIconOut = il.GetIcon(idxOut, ILD_TRANSPARENT);
	return SetIcon(hIconIn, hIconOut);
}

DWORD CButtonST::SetIcon(HICON hIconIn, HICON hIconOut)
{
	bool		bRetValue;
	ICONINFO	ii;

	// Free any loaded resource
	FreeResources();

	if (hIconIn != 0)
		{
		m_csIcons[0].hIcon = hIconIn;
		// Get icon dimension
		ZeroMemory(&ii, sizeof(ICONINFO));
		bRetValue = (::GetIconInfo(hIconIn, &ii) != 0);
		if (!bRetValue)
			{
			FreeResources();
			return BTNST_INVALIDRESOURCE;
			}
		m_csIcons[0].dwWidth	= (DWORD)(ii.xHotspot * 2);
		m_csIcons[0].dwHeight	= (DWORD)(ii.yHotspot * 2);
		::DeleteObject(ii.hbmMask);
		::DeleteObject(ii.hbmColor);
		if (hIconOut != 0)
			{
			m_csIcons[1].hIcon = hIconOut;
			// Get icon dimension
			ZeroMemory(&ii, sizeof(ICONINFO));
			bRetValue = (::GetIconInfo(hIconOut, &ii) != 0);
			if (!bRetValue)
				{
				FreeResources();
				return BTNST_INVALIDRESOURCE;
				}
			m_csIcons[1].dwWidth	= (DWORD)(ii.xHotspot * 2);
			m_csIcons[1].dwHeight	= (DWORD)(ii.yHotspot * 2);
			::DeleteObject(ii.hbmMask);
			::DeleteObject(ii.hbmColor);
			}
		}
	if (IsWindow()) { RedrawWindow(); }
	return BTNST_OK;
}

bool CButtonST::SetBtnCursor(_U_STRINGorID nCursorId, HMODULE rsrcModule)
{
	HINSTANCE hInstResource;
	// Destroy any previous cursor
	if (m_hCursor != 0) { ::DestroyCursor(m_hCursor); }
	m_hCursor = 0;
	// If we want a cursor
	if (nCursorId.m_lpstr != MAKEINTRESOURCE(0))
		{
		hInstResource = (rsrcModule != 0 ? rsrcModule : _Module.GetResourceInstance());
		// Load icon resource
		m_hCursor = (HCURSOR)::LoadImage(hInstResource, nCursorId.m_lpstr, IMAGE_CURSOR, 0, 0, 0);
		// If something wrong then return FALSE
		if (m_hCursor == 0) { return false; }
		}
	return true;
}

void CButtonST::SetFlat(bool bState)
{
	m_bIsFlat = bState;
	if (IsWindow()) { Invalidate(); }
}

bool CButtonST::GetFlat()
{
	return m_bIsFlat;
}

void CButtonST::SetFlatFocus(bool bDrawFlatFocus, bool bRepaint)
{
	m_bDrawFlatFocus = bDrawFlatFocus;
	// Repaint the button
	if (bRepaint && IsWindow()) { Invalidate(); }
}


bool CButtonST::GetDefault()
{
	return m_bIsDefault;
}

void CButtonST::SetAlign(Alignment nAlign)
{
	switch (nAlign)
		{   
		case ST_ALIGN_HORIZ:
		case ST_ALIGN_HORIZ_RIGHT:
		case ST_ALIGN_VERT:
			m_nAlign = nAlign;
			break;
		}
	if (IsWindow()) { Invalidate(); }
}

CButtonST::Alignment CButtonST::GetAlign()
{
	return m_nAlign;
}

void CButtonST::DrawBorder(bool bEnable)
{
	m_bDrawBorder = bEnable;
}

LRESULT CButtonST::OnMouseMessage(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	if(m_ToolTip.IsWindow())
		{
		m_ToolTip.RelayEvent((LPMSG)m_pCurrentMsg);
		}
	bHandled = false;
	return 1;
}

LRESULT CButtonST::OnMouseMove(UINT nFlags, CPoint point)
{
	CWindow pWnd;		// Active window
	CWindow pParent;	// Window that owns the button

	DefWindowProc();
	// If the mouse enter the button with the left button pressed then do nothing
	if ((nFlags & MK_LBUTTON) != 0 && !m_bMouseOnButton) { return 0; }
	// If our button is not flat then do nothing
	if (!m_bIsFlat) { return 0; };
	pWnd = ::GetActiveWindow();
	pParent = GetParent();

	if ((::GetCapture() != m_hWnd) && 
		(
#ifndef ST_LIKEIE
		pWnd.IsWindow() && 
#endif
		pParent.IsWindow())) 
		{
		m_bMouseOnButton = true;
		SetCapture();
		Invalidate();
		}
	else
		{
		CPoint p2 = point;
		ClientToScreen(&p2);
		CWindow wndUnderMouse = ::WindowFromPoint(p2);
		if (wndUnderMouse && (wndUnderMouse.m_hWnd != m_hWnd))
			{
			// Redraw only if mouse goes out
			if (m_bMouseOnButton)
				{
				m_bMouseOnButton = false;
				Invalidate();
				}
			// If user is NOT pressing left button then release capture!
			if (!(nFlags & MK_LBUTTON))
				{
				ReleaseCapture();
				}
			}
		}
	return 0;
}

LRESULT CButtonST::OnKillFocus(HWND /*hWnd*/)
{
	DefWindowProc();
	CancelHover();
	return 0;
}

LRESULT CButtonST::OnLButtonDown(UINT /*nFlags*/, CPoint /*pt*/)
{
	DefWindowProc();
	if (m_bAutoRepeat)
		{
		MSG		csMsg;
		int		nButtonID;
		HWND	hWndParent;
		bool	initialState = true;

		nButtonID = GetDlgCtrlID();
		hWndParent = GetParent();
		SetCapture();
		while(::PeekMessage(&csMsg, m_hWnd, WM_LBUTTONUP, WM_LBUTTONUP, PM_REMOVE) == FALSE)
			{
			::SendMessage(hWndParent, WM_COMMAND, MAKEWPARAM((WORD)nButtonID, BN_CLICKED), (LPARAM)m_hWnd);
			::Sleep(m_dwPeriodAutoRepeat);
			initialState = !initialState;
			}
		if(!initialState)
			{
			::SendMessage(hWndParent, WM_COMMAND, MAKEWPARAM((WORD)nButtonID, BN_CLICKED), (LPARAM)m_hWnd);
			}
		ReleaseCapture();
		SendMessage(WM_LBUTTONUP);
		CPoint ptCursor;
		::GetCursorPos(&ptCursor);
		ScreenToClient(&ptCursor);
		SendMessage(WM_MOUSEMOVE, 0, MAKELPARAM(ptCursor.x, ptCursor.y));
		}
	return 0;
}

void CButtonST::CancelHover()
{
	// If our button is not flat then do nothing
	if (!m_bIsFlat) { return; }
	if (m_bMouseOnButton)
		{
		m_bMouseOnButton = false;
		Invalidate();
		}
}

void CButtonST::OnActivate(UINT nState, BOOL /*bMinimized*/, HWND /*pWndOther*/) 
{
	DefWindowProc();
	if (nState == WA_INACTIVE)	{ CancelHover(); }
}

void CButtonST::OnEnable(BOOL bEnable) 
{
	DefWindowProc();
	if (!bEnable)	CancelHover();
}

void CButtonST::OnCancelMode() 
{
	DefWindowProc();
	CancelHover();
}

void CButtonST::OnCaptureChanged(HWND /*pWnd*/) 
{
	if (m_bMouseOnButton)
		{
		ReleaseCapture();
		Invalidate();
		}
	// Call base message handler
	SetMsgHandled(false);
}

BOOL CButtonST::OnSetCursor(HWND /*pWnd*/, UINT /*nHitTest*/, UINT /*message*/) 
{
	// If a cursor was specified then use it!
	if (m_hCursor != 0)
		{
		::SetCursor(m_hCursor);
		return true;
		}
	SetMsgHandled(false);
	return 0;
}

void CButtonST::PaintBk(CDCHandle pDC)
{
	CWindow		parent = GetParent();
	CClientDC	clDC(parent);
	CRect		rect;
	CRect		rect1;

	GetClientRect(rect);
	GetWindowRect(rect1);
	parent.ScreenToClient(rect1);


	if ( (rect1.left < 0) || (rect1.top < 0) ) return;

	if (m_dcBk == 0)
		{
		m_dcBk.CreateCompatibleDC(clDC);
		m_bmpBk.CreateCompatibleBitmap(clDC, rect.Width(), rect.Height());
		m_pbmpOldBk = m_dcBk.SelectBitmap(m_bmpBk);
		m_dcBk.BitBlt(0, 0, rect.Width(), rect.Height(), clDC, rect1.left, rect1.top, SRCCOPY);
		}
	pDC.BitBlt(0, 0, rect.Width(), rect.Height(), m_dcBk, 0, 0, SRCCOPY);
}

//
// Parameters:
//		[IN]	bHasTitle
//				TRUE if the button has a text
//		[IN]	rpItem
//				A pointer to a RECT structure indicating the allowed paint area
//		[IN/OUT]rpTitle
//				A pointer to a CRect object indicating the paint area reserved for the
//				text. This structure will be modified if necessary.
//		[IN]	bIsPressed
//				TRUE if the button is currently pressed
//		[IN]	dwWidth
//				Width of the image (icon or bitmap)
//		[IN]	dwHeight
//				Height of the image (icon or bitmap)
//		[OUT]	rpImage
//				A pointer to a CRect object that will receive the area available to the image
//
void CButtonST::PrepareImageRect(bool bHasTitle, RECT &rpItem, CRect &rpTitle, bool bIsPressed, DWORD dwWidth, DWORD dwHeight, CRect &rpImage)
{
	CRect rBtn;

	rpImage = rpItem;

	switch (m_nAlign)
		{
		case ST_ALIGN_HORIZ:
			if (!bHasTitle)
				{
				// Center image horizontally
				rpImage.left += ((rpImage.Width() - dwWidth)/2);
				}
			else
				{
				// Image must be placed just inside the focus rect
				rpImage.left += 3;  
				rpTitle.left += dwWidth + 3;
				}
			// Center image vertically
			rpImage.top += ((rpImage.Height() - dwHeight)/2);
			break;
		case ST_ALIGN_HORIZ_RIGHT:
			GetClientRect(&rBtn);
			if (bHasTitle == FALSE /*spTitle->IsEmpty()*/)
				{
				// Center image horizontally
				rpImage.left += ((rpImage.Width() - dwWidth)/2);
				}
			else
				{
				// Image must be placed just inside the focus rect
				rpTitle.right = rpTitle.Width() - dwWidth - 3;
				rpTitle.left = 3;
				rpImage.left = rBtn.right - dwWidth - 3;
				// Center image vertically
				rpImage.top += ((rpImage.Height() - dwHeight)/2);
				}
			break;
		case ST_ALIGN_VERT:
			// Center image horizontally
			rpImage.left += ((rpImage.Width() - dwWidth)/2);
			if (bHasTitle == FALSE /*spTitle->IsEmpty()*/)
				{
				// Center image vertically
				rpImage.top += ((rpImage.Height() - dwHeight)/2);           
				}
			else
				{
				rpImage.top = 3;
				rpTitle.top += dwHeight;
				}
			break;
		}
    
	// If button is pressed then press image also
	if (bIsPressed && !m_bIsCheckBox)
		{
		rpImage.OffsetRect(1, 1);
		}
}

void CButtonST::DrawTheBitmap(CDCHandle pDC, bool bHasTitle, RECT &rItem, CRect &rCaption, bool bIsPressed, bool bIsDisabled)
{
	CDC				hdcBmpMem	= 0;
	CBitmapHandle	hbmOldBmp	= 0;
	CDC				hdcMem		= 0;
	CBitmapHandle	hbmT		= 0;
	BYTE			byIndex		= 0;

	// Select the bitmap to use
	if (m_bIsCheckBox)
		{
		if (bIsPressed)
			{
			byIndex = 0;
			}
		else
			{
			byIndex = (m_csBitmaps[1].hBitmap == NULL ? 0 : 1);
			}
		}
	else
		{
		if (m_bMouseOnButton || bIsPressed)
			{
			byIndex = 0;
			}
		else
			{
			byIndex = (m_csBitmaps[1].hBitmap == NULL ? 0 : 1);
			}
		}

	CRect	rImage;
	
	
	if (!transparentBitmap)
	{
		PrepareImageRect(bHasTitle, rItem, rCaption, bIsPressed, m_csBitmaps[byIndex].dwWidth, m_csBitmaps[byIndex].dwHeight, rImage);
		hdcBmpMem.CreateCompatibleDC(pDC);
		hbmOldBmp = hdcBmpMem.SelectBitmap(m_csBitmaps[byIndex].hBitmap);
		hdcMem.CreateCompatibleDC(0);
		hbmT = hdcMem.SelectBitmap(m_csBitmaps[byIndex].hMask);

		pDC.BitBlt(rImage.left, rImage.top, m_csBitmaps[byIndex].dwWidth, m_csBitmaps[byIndex].dwHeight, hdcMem, 0, 0, SRCAND);
		pDC.BitBlt(rImage.left, rImage.top, m_csBitmaps[byIndex].dwWidth, m_csBitmaps[byIndex].dwHeight, hdcBmpMem, 0, 0, SRCPAINT);

		hdcMem.SelectBitmap(hbmT);
		hdcBmpMem.SelectBitmap(hbmOldBmp);
	
	} else
	{
		PrepareImageRect(bHasTitle, rItem, rCaption, bIsPressed, trasparentBitmapWidth, transparentBitmapHeight, rImage);
		DrawBitmapAdvanced(pDC, transparentBitmap, /*rImage.left*/0, /*rImage.top*/0, trasparentBitmapWidth, transparentBitmapHeight, bIsDisabled);
	}
}


static void DrawDisabledIcon(CDCHandle & DC, CRect& Rect, WTL::CIcon& Icon, bool disabled)
{
		DC.DrawState(
		Rect.TopLeft(), Rect.Size(), 
		Icon,
		(disabled ? DSS_DISABLED : DSS_NORMAL), 
		0);
	//DC.DrawIconEx(Rect.left, Rect.top, Icon, 16, 16);
	//Icon.DrawIconEx(DC, Rect.left, Rect.top, Rect.Width(), Rect.Height());

	/*

  WTL::CDC MemDC(CreateCompatibleDC(DC));

  BITMAPINFO bmi;
  ZeroMemory(&bmi, sizeof(BITMAPINFO));
  bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
  bmi.bmiHeader.biWidth = Rect.Width();
  bmi.bmiHeader.biHeight = Rect.Height();
  bmi.bmiHeader.biPlanes = 1;
  bmi.bmiHeader.biBitCount = 32;
  bmi.bmiHeader.biCompression = BI_RGB;
  bmi.bmiHeader.biSizeImage = bmi.bmiHeader.biWidth * bmi.bmiHeader.biHeight * 4;

  VOID *pvBits;
  WTL::CBitmap Bitmap(::CreateDIBSection(MemDC, &bmi, DIB_RGB_COLORS, &pvBits, NULL, 0));
  WTL::CBitmapHandle PrevBitmap(MemDC.SelectBitmap(Bitmap));

  Icon.DrawIconEx(MemDC, 0, 0, bmi.bmiHeader.biWidth, bmi.bmiHeader.biWidth);
	Icon.DrawIconEx(DC, Rect.left, Rect.top, Rect.Width(), Rect.Height());*/
	/*
  // convert to grayscale
	if (disabled)
  for (unsigned char *p = (unsigned char*)pvBits, *end = p + bmi.bmiHeader.biSizeImage; p < end; p += 4)
  {
    // Gray = 0.3*R + 0.59*G + 0.11*B
    p[0] = p[1] = p[2] = 
    (
      static_cast<unsigned int>(p[2]) *  77 +
      static_cast<unsigned int>(p[1]) * 151 +
      static_cast<unsigned int>(p[0]) *  28
    ) >> 8;
  }

  BLENDFUNCTION BlendFunction;
  BlendFunction.BlendOp = AC_SRC_OVER;
  BlendFunction.BlendFlags = 0;
  BlendFunction.SourceConstantAlpha = 0x60;  // half transparent
  BlendFunction.AlphaFormat = AC_SRC_ALPHA;  // use bitmap alpha

  BOOL r = AlphaBlend(DC, Rect.left, Rect.top, bmi.bmiHeader.biWidth, bmi.bmiHeader.biHeight,
    MemDC, 0, 0, bmi.bmiHeader.biWidth, bmi.bmiHeader.biHeight, BlendFunction);
	
	*/
  //MemDC.SelectBitmap(PrevBitmap);
} 

void CButtonST::DrawTheIcon(CDCHandle pDC, bool bHasTitle, RECT &rpItem, CRect &rpTitle, bool bIsPressed, bool bIsDisabled)
{
	BYTE byIndex = 0;

	// Select the icon to use
	if (m_bIsCheckBox)
		{
		if (bIsPressed)
			{
			byIndex = 0;
			}
		else
			{
			byIndex = (m_csIcons[1].hIcon == NULL ? 0 : 1);
			}
		}
	else
		{
		if (m_bMouseOnButton || bIsPressed)
			{
			byIndex = 0;
			}
		else
			{
			byIndex = (m_csIcons[1].hIcon == NULL ? 0 : 1);
			}
		}

	CRect	rImage;
	PrepareImageRect(bHasTitle, rpItem, rpTitle, bIsPressed, m_csIcons[byIndex].dwWidth, m_csIcons[byIndex].dwHeight, rImage);
	// Ole'!
	//pDC.
	//DrawDisabledIcon(pDC, rImage, CIcon(m_csIcons[byIndex].hIcon), bIsDisabled);
	/*
	if (bIsDisabled)
		DrawDisabledIcon(pDC, rImage, CIcon(m_csIcons[byIndex].hIcon));
	else*/
		/*pDC.DrawState(
			rImage.TopLeft(), rImage.Size(), 
			m_csIcons[byIndex].hIcon,
			(bIsDisabled ? DSS_DISABLED : DSS_NORMAL) | DST_ICON, 0);*/
	//pDC.DrawIconEx(rImage.TopLeft(), m_csIcons[byIndex].hIcon, rImage.Size());
		
	CRect Rect = rImage;
	//CIcon Icon(m_csIcons[byIndex].hIcon);
	CIcon Icon;
	Icon.Attach(m_csIcons[byIndex].hIcon);
	//Icon.Detach();




	
  WTL::CDC MemDC(CreateCompatibleDC(pDC));



	
  BITMAPINFO bmi;
  ZeroMemory(&bmi, sizeof(BITMAPINFO));
  bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
  bmi.bmiHeader.biWidth = Rect.Width();
  bmi.bmiHeader.biHeight = Rect.Height();
  bmi.bmiHeader.biPlanes = 1;
  bmi.bmiHeader.biBitCount = 32;
  bmi.bmiHeader.biCompression = BI_RGB;
  bmi.bmiHeader.biSizeImage = bmi.bmiHeader.biWidth * bmi.bmiHeader.biHeight * 4;

  VOID *pvBits;
  WTL::CBitmap Bitmap(::CreateDIBSection(MemDC, &bmi, DIB_RGB_COLORS, &pvBits, NULL, 0));
  WTL::CBitmapHandle PrevBitmap(MemDC.SelectBitmap(Bitmap));

  Icon.DrawIconEx(MemDC, 0, 0, bmi.bmiHeader.biWidth, bmi.bmiHeader.biWidth);
	
  // convert to grayscale
	if (bIsDisabled)
		for (unsigned char *p = (unsigned char*)pvBits, *end = p + bmi.bmiHeader.biSizeImage; p < end; p += 4)
		{
			// Gray = 0.3*R + 0.59*G + 0.11*B
			p[0] = p[1] = p[2] = 
			(
				static_cast<unsigned int>(p[2]) *  77 +
				static_cast<unsigned int>(p[1]) * 151 +
				static_cast<unsigned int>(p[0]) *  28
			) >> 8;
		}

  BLENDFUNCTION BlendFunction;
  BlendFunction.BlendOp = AC_SRC_OVER;
  BlendFunction.BlendFlags = 0;
  BlendFunction.SourceConstantAlpha = bIsDisabled ? 0x60 : 0xFF;  // half transparent
  BlendFunction.AlphaFormat = AC_SRC_ALPHA;  // use bitmap alpha

  pDC.AlphaBlend(Rect.left, Rect.top, bmi.bmiHeader.biWidth, bmi.bmiHeader.biHeight,
    MemDC, 0, 0, bmi.bmiHeader.biWidth, bmi.bmiHeader.biHeight, BlendFunction);
	
	
  MemDC.SelectBitmap(PrevBitmap);
	Icon.Detach();
}

void CButtonST::DrawItem(UINT /*ctrlID*/, LPDRAWITEMSTRUCT lpDIS)
{
	CDCHandle pDC = lpDIS->hDC;

	CPenHandle	pOldPen;

	// Checkbox or Radiobutton style ?
	if (m_bIsCheckBox)
		{
		m_bIsPressed  =  (lpDIS->itemState & ODS_SELECTED) || m_nCheck;
		}
	// Normal button OR other button style ...
	else
		{
		m_bIsPressed = (lpDIS->itemState & ODS_SELECTED);
		}

	m_bIsFocused  = (lpDIS->itemState & ODS_FOCUS) != 0;
	m_bIsDisabled = (lpDIS->itemState & ODS_DISABLED) != 0;
	
	CRect itemRect = lpDIS->rcItem;

	pDC.SetBkMode(TRANSPARENT);

	if (!m_bIsFlat)
		{
		if (m_bIsFocused || GetDefault())
			{
			CBrush br;
			br.CreateSolidBrush(RGB(0,0,0));  
			pDC.FrameRect(&itemRect, br);
			itemRect.DeflateRect(1, 1);
			}
		}

	// Prepare draw... paint button background

	// Draw transparent?
	if (m_bDrawTransparent)
		{
		PaintBk(pDC);
		}
	else
		{
		OnDrawBackground(pDC, &itemRect);
		}

	// Draw pressed button
	if (m_bIsPressed)
		{
		if (m_bIsFlat)
			{
			if (m_bDrawBorder)
				{
				OnDrawBorder(pDC, &itemRect);
				}
			}
		else    
			{
			CBrush brBtnShadow;
			brBtnShadow.CreateSolidBrush(GetSysColor(COLOR_BTNSHADOW));
			pDC.FrameRect(&itemRect, brBtnShadow);
			}
		}
	else // ...else draw non pressed button
		{
		CPen penBtnHiLight; // White
		CPen pen3DLight;       // Light gray
		CPen penBtnShadow;   // Dark gray
		CPen pen3DDKShadow; // Black

		penBtnHiLight.CreatePen(PS_SOLID, 0, ::GetSysColor(COLOR_BTNHILIGHT)); // White
		pen3DLight.CreatePen(PS_SOLID, 0, ::GetSysColor(COLOR_3DLIGHT));       // Light gray
		penBtnShadow.CreatePen(PS_SOLID, 0, ::GetSysColor(COLOR_BTNSHADOW));   // Dark gray
		pen3DDKShadow.CreatePen(PS_SOLID, 0, ::GetSysColor(COLOR_3DDKSHADOW)); // Black

		if (m_bIsFlat)
			{
			if (m_bMouseOnButton && m_bDrawBorder)
				{
				pDC.Draw3dRect(itemRect, ::GetSysColor(COLOR_BTNHILIGHT), ::GetSysColor(COLOR_BTNSHADOW));
				}
			}
		else
			{
			// Draw top-left borders
			// White line
			pOldPen = pDC.SelectPen(penBtnHiLight);
			pDC.MoveTo(itemRect.left, itemRect.bottom-1);
			pDC.LineTo(itemRect.left, itemRect.top);
			pDC.LineTo(itemRect.right, itemRect.top);
			// Light gray line
			pDC.SelectPen(pen3DLight);
			pDC.MoveTo(itemRect.left+1, itemRect.bottom-1);
			pDC.LineTo(itemRect.left+1, itemRect.top+1);
			pDC.LineTo(itemRect.right, itemRect.top+1);
			// Draw bottom-right borders
			// Black line
			pDC.SelectPen(pen3DDKShadow);
			pDC.MoveTo(itemRect.left, itemRect.bottom-1);
			pDC.LineTo(itemRect.right-1, itemRect.bottom-1);
			pDC.LineTo(itemRect.right-1, itemRect.top-1);
			// Dark gray line
			pDC.SelectPen(penBtnShadow);
			pDC.MoveTo(itemRect.left+1, itemRect.bottom-2);
			pDC.LineTo(itemRect.right-2, itemRect.bottom-2);
			pDC.LineTo(itemRect.right-2, itemRect.top);
			//
			pDC.SelectPen(pOldPen);
			}
		}

	// Read the button's title
	CString sTitle;
	int nLen = GetWindowTextLength();
	GetWindowText(sTitle.GetBufferSetLength(nLen), nLen + 1);

	CRect captionRect = lpDIS->rcItem;

	// Draw the icon
	if (m_csIcons[0].hIcon != 0)
		{
		DrawTheIcon(pDC, !sTitle.IsEmpty(), lpDIS->rcItem, captionRect, m_bIsPressed, m_bIsDisabled);
		}

	if (m_csBitmaps[0].hBitmap != 0 || transparentBitmap)
		{
		//	if (!transparentBitmap)
		pDC.SetBkColor(RGB(255,255,255));
		DrawTheBitmap(pDC, !sTitle.IsEmpty(), lpDIS->rcItem, captionRect, m_bIsPressed, m_bIsDisabled);
		}

	// Write the button title (if any)
	if (!sTitle.IsEmpty())
		{
		// Draw the button's title
		// If button is pressed then "press" title also
		if (m_bIsPressed && !m_bIsCheckBox)
			{
			captionRect.OffsetRect(1, 1);
			}
		// Center text
		CRect centerRect = captionRect;
		pDC.DrawText(sTitle, -1, captionRect, DT_WORDBREAK | DT_CENTER | DT_CALCRECT);
		captionRect.OffsetRect((centerRect.Width() - captionRect.Width())/2, (centerRect.Height() - captionRect.Height())/2);
		pDC.SetBkMode(TRANSPARENT);
		if (m_bIsDisabled)
			{
			captionRect.OffsetRect(1, 1);
			pDC.SetTextColor(::GetSysColor(COLOR_3DHILIGHT));
			pDC.DrawText(sTitle, -1, captionRect, DT_WORDBREAK | DT_CENTER);
			captionRect.OffsetRect(-1, -1);
			pDC.SetTextColor(::GetSysColor(COLOR_3DSHADOW));
			pDC.DrawText(sTitle, -1, captionRect, DT_WORDBREAK | DT_CENTER);
			}
		else
			{
			if (m_bMouseOnButton || m_bIsPressed) 
				{
				pDC.SetTextColor(m_crColors[BTNST_COLOR_FG_IN]);
				pDC.SetBkColor(m_crColors[BTNST_COLOR_BK_IN]);
				} 
			else 
				{
				pDC.SetTextColor(m_crColors[BTNST_COLOR_FG_OUT]);
				pDC.SetBkColor(m_crColors[BTNST_COLOR_BK_OUT]);
				}
			pDC.DrawText(sTitle, -1, captionRect, DT_WORDBREAK | DT_CENTER);
			}
		}
	if (!m_bIsFlat || (m_bIsFlat && m_bDrawFlatFocus))
		{
		// Draw the focus rect
		if (m_bIsFocused)
			{
			CRect focusRect = itemRect;
			focusRect.DeflateRect(3, 3);
			pDC.DrawFocusRect(&focusRect);
			}
		}
}

void CButtonST::InitToolTip()
{
	if (m_ToolTip.m_hWnd == 0)
		{
		// Create ToolTip control
		m_ToolTip.Create(*this);
		// Create inactive
		m_ToolTip.Activate(false);
		// Enable multiline
		m_ToolTip.SetMaxTipWidth(400);
		}
}

void CButtonST::SetTooltipText(LPCTSTR lpszText, bool bActivate)
{
	// We cannot accept 0 pointer
	if (lpszText == 0) { return; }
	// Initialize ToolTip
	InitToolTip();
	// If there is no tooltip defined then add it
	if (m_ToolTip.GetToolCount() == 0)
		{
		CRect rectBtn; 
		GetClientRect(rectBtn);
		m_ToolTip.AddTool(*this, lpszText, rectBtn, 1);
		}
	// Set text for tooltip
	m_ToolTip.UpdateTipText(lpszText, *this, 1);
	m_ToolTip.Activate(bActivate);
}


void CButtonST::SetTooltipText(int nId, bool bActivate)
{
	CString sText;

	// load string resource
	sText.LoadString(nId);
	// If string resource is not empty
	if (!sText.IsEmpty()) { SetTooltipText((LPCTSTR)sText, bActivate); }
}

void CButtonST::ActivateTooltip(bool bActivate)
{
	// If there is no tooltip then do nothing
	if (m_ToolTip.GetToolCount() == 0) { return; }
	// Activate tooltip
	m_ToolTip.Activate(bActivate);
}

LPARAM CButtonST::OnCtlColor(CDCHandle pDC, HWND /*button*/) 
{
	return (LPARAM)::GetStockObject(NULL_BRUSH); 
}


void CButtonST::OnSysColorChange() 
{
	DefWindowProc();
	m_dcBk.DeleteDC();
	m_bmpBk.DeleteObject();	
}

BOOL CButtonST::OnClicked(UINT /*wNotifyCode*/, int /*wID*/, HWND /*pWnd*/) 
{	
	if (m_bIsCheckBox)
		{
		m_nCheck = !m_nCheck;
		Invalidate();
		}
	else
		{
		// Handle the URL (if any)
		if (::lstrlen(m_szURL) > 0)
			{
			::ShellExecute(0, _T("open"), m_szURL, 0,0, SW_SHOWMAXIMIZED);
			return false;
			}
		}
	return false;
}

void CButtonST::SetCheck(bool nCheck, bool bRepaint)
{
	if (m_bIsCheckBox)
		{
		m_nCheck = nCheck;
		if (bRepaint && IsWindow()) Invalidate();
		}
}

bool CButtonST::GetCheck() const
{
	return m_nCheck;
}

DWORD CButtonST::SetBitmaps(HBITMAP hBitmapIn, COLORREF crTransColorIn, HBITMAP hBitmapOut, COLORREF crTransColorOut)
{
	int		nRetValue;
	BITMAP	csBitmapSize;

	// Free any loaded resource
	FreeResources();

	if (hBitmapIn != 0)
		{
		m_csBitmaps[0].hBitmap = hBitmapIn;
		m_csBitmaps[0].crTransparent = crTransColorIn;
		// Get bitmap size
		nRetValue = ::GetObject(hBitmapIn, sizeof(csBitmapSize), &csBitmapSize);
		if (nRetValue == 0)
			{
			FreeResources();
			return BTNST_INVALIDRESOURCE;
			}
		m_csBitmaps[0].dwWidth = (DWORD)csBitmapSize.bmWidth;
		m_csBitmaps[0].dwHeight = (DWORD)csBitmapSize.bmHeight;
		// Create mask for bitmap In
		m_csBitmaps[0].hMask = CreateBitmapMask(hBitmapIn, m_csBitmaps[0].dwWidth, m_csBitmaps[0].dwHeight, crTransColorIn);
		if (m_csBitmaps[0].hMask == 0)
			{
			FreeResources();
			return BTNST_FAILEDMASK;
			}
		if (hBitmapOut != 0)
			{
			m_csBitmaps[1].hBitmap = hBitmapOut;
			m_csBitmaps[1].crTransparent = crTransColorOut;
			// Get bitmap size
			nRetValue = ::GetObject(hBitmapOut, sizeof(csBitmapSize), &csBitmapSize);
			if (nRetValue == 0)
				{
				FreeResources();
				return BTNST_INVALIDRESOURCE;
				}
			m_csBitmaps[1].dwWidth = (DWORD)csBitmapSize.bmWidth;
			m_csBitmaps[1].dwHeight = (DWORD)csBitmapSize.bmHeight;
			// Create mask for bitmap Out
			m_csBitmaps[1].hMask = CreateBitmapMask(hBitmapOut, m_csBitmaps[1].dwWidth, m_csBitmaps[1].dwHeight, crTransColorOut);
			if (m_csBitmaps[1].hMask == 0)
				{
				FreeResources();
				return BTNST_FAILEDMASK;
				}
			}
		}
	RedrawWindow();
	return BTNST_OK;
}

DWORD CButtonST::SetBitmaps(_U_STRINGorID nBitmapIn, COLORREF crTransColorIn, _U_STRINGorID nBitmapOut, COLORREF crTransColorOut, HMODULE rsrcModule)
{
	HBITMAP		hBitmapIn	= 0;
	HBITMAP		hBitmapOut	= 0;
	HINSTANCE	hInstResource = 0;
	
	// Find correct resource handle
	hInstResource = (rsrcModule != 0 ? rsrcModule : _Module.GetResourceInstance());
	// Load bitmap In
	hBitmapIn = (HBITMAP)::LoadImage(hInstResource, nBitmapIn.m_lpstr, IMAGE_BITMAP, 0, 0, 0);
	// Load bitmap Out
	hBitmapOut = (HBITMAP)::LoadImage(hInstResource, nBitmapOut.m_lpstr, IMAGE_BITMAP, 0, 0, 0);
	return SetBitmaps(hBitmapIn, crTransColorIn, hBitmapOut, crTransColorOut);
}

HBITMAP CButtonST::CreateBitmapMask(HBITMAP hSourceBitmap, DWORD dwWidth, DWORD dwHeight, COLORREF crTransColor)
{
	HBITMAP		hMask		= NULL;
	HDC			hdcSrc		= NULL;
	HDC			hdcDest		= NULL;
	HBITMAP		hbmSrcT		= NULL;
	HBITMAP		hbmDestT	= NULL;
	COLORREF	crSaveBk;
	COLORREF	crSaveDestText;

	hMask = ::CreateBitmap(dwWidth, dwHeight, 1, 1, NULL);
	if (hMask == NULL)	return NULL;

	hdcSrc	= ::CreateCompatibleDC(NULL);
	hdcDest	= ::CreateCompatibleDC(NULL);

	hbmSrcT = (HBITMAP)::SelectObject(hdcSrc, hSourceBitmap);
	hbmDestT = (HBITMAP)::SelectObject(hdcDest, hMask);

	crSaveBk = ::SetBkColor(hdcSrc, crTransColor);

	::BitBlt(hdcDest, 0, 0, dwWidth, dwHeight, hdcSrc, 0, 0, SRCCOPY);

	crSaveDestText = ::SetTextColor(hdcSrc, RGB(255, 255, 255));
	::SetBkColor(hdcSrc,RGB(0, 0, 0));

	::BitBlt(hdcSrc, 0, 0, dwWidth, dwHeight, hdcDest, 0, 0, SRCAND);

	SetTextColor(hdcDest, crSaveDestText);

	::SetBkColor(hdcSrc, crSaveBk);
	::SelectObject(hdcSrc, hbmSrcT);
	::SelectObject(hdcDest, hbmDestT);

	::DeleteDC(hdcSrc);
	::DeleteDC(hdcDest);

	return hMask;
}

DWORD CButtonST::SetColor(ColorType byColorIndex, COLORREF crColor, bool bRepaint)
{
	if (byColorIndex >= BTNST_MAX_COLORS)	{ return BTNST_INVALIDINDEX; }
	// Set new color
	m_crColors[byColorIndex] = crColor;
	if (bRepaint && IsWindow())	{ Invalidate(); }
	return BTNST_OK;
}

DWORD CButtonST::GetColor(ColorType byColorIndex, COLORREF* crpColor)
{
	if (byColorIndex >= BTNST_MAX_COLORS)	{ return BTNST_INVALIDINDEX; }
	// Get color
	*crpColor = m_crColors[byColorIndex];
	return BTNST_OK;
}

bool CButtonST::SetURL(LPCTSTR lpszURL)
{
	if (lpszURL != 0)
		{
		// Store the URL
		::lstrcpyn(m_szURL, lpszURL, _MAX_PATH);
		}
	else
		{
		// Remove any existing URL
		::ZeroMemory(&m_szURL, sizeof(m_szURL));
		}
	return true;
}

LRESULT CButtonST::OnCreate(LPCREATESTRUCT /*pCreateStruct*/)
{
	UINT nBS = GetStyle();
	// Be sure that we have an owner drawn button
	ModifyStyle(0, BS_OWNERDRAW);
	// Check if this is a checkbox
	m_bIsCheckBox = (nBS & BS_CHECKBOX) != 0;
	// Check if this is the default button
	if (!m_bIsCheckBox)
		{
		m_bIsDefault =  (nBS & BS_DEFPUSHBUTTON) != 0;
		}
	SetMsgHandled(false);
	return 0;
}

BOOL CButtonST::SubclassWindow(HWND hWnd)
{
	UINT nBS = ::GetWindowLong(hWnd, GWL_STYLE);
	// Be sure that we have an owner drawn button
	::SetWindowLong(hWnd, GWL_STYLE, (nBS | BS_OWNERDRAW));
	// Check if this is a checkbox
	m_bIsCheckBox = (nBS & BS_CHECKBOX) != 0;
	// Check if this is the default button
	if (!m_bIsCheckBox)
		{
		m_bIsDefault =  (nBS & BS_DEFPUSHBUTTON) != 0;
		}
	return inherited::SubclassWindow(hWnd);
}

bool CButtonST::SetAutoRepeat(bool bSet, DWORD dwMilliseconds)
{
	m_bAutoRepeat = bSet;
	m_dwPeriodAutoRepeat = dwMilliseconds;
	return true;
}

void CButtonST::DrawTransparent(bool bRepaint)
{
	m_bDrawTransparent = true;
	// Restore old bitmap (if any)
	if (m_dcBk.m_hDC != 0 && m_pbmpOldBk != 0)
		{
		m_dcBk.SelectBitmap(m_pbmpOldBk);
		}
	if (m_bmpBk != 0) { m_bmpBk.DeleteObject(); }
	if (m_dcBk != 0) { m_dcBk.DeleteDC(); }
	// Repaint the button
	if (bRepaint && IsWindow()) Invalidate();
}

WNDPROC CButtonST::GetWindowProc()
{
	return WindowProc;
}

LRESULT CALLBACK CButtonST::WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_LBUTTONDBLCLK)
		{
		uMsg = WM_LBUTTONDOWN;
		}
	return inherited::WindowProc(hWnd, uMsg, wParam, lParam);
}

// This function is called every time the button background needs to be painted.
// If the button is in transparent mode this function will NOT be called.
//
// Parameters:
//		[IN]	pDC
//				Pointer to a CDC object that indicates the device context.
//		[IN]	pRect
//				Pointer to a CRect object that indicates the bounds of the
//				area to be painted.
//
// Return value:
//		BTNST_OK
//			Function executed successfully.
//
DWORD CButtonST::OnDrawBackground(CDCHandle pDC, LPCRECT pRect)
{
	COLORREF	crColor;

	if (m_bMouseOnButton || m_bIsPressed)
		{
		crColor = m_crColors[BTNST_COLOR_BK_IN];
		}
	else
		{
		crColor = m_crColors[BTNST_COLOR_BK_OUT];
		}

	CBrush		brBackground;

	brBackground.CreateSolidBrush(crColor);
	pDC.FillRect(pRect, brBackground);

	return BTNST_OK;
} // End of OnDrawBackground

// This function is called every time the button border needs to be painted.
// If the button is in standard (not flat) mode this function will NOT be called.
//
// Parameters:
//		[IN]	pDC
//				Pointer to a CDC object that indicates the device context.
//		[IN]	pRect
//				Pointer to a CRect object that indicates the bounds of the
//				area to be painted.
//
// Return value:
//		BTNST_OK
//			Function executed successfully.
//
DWORD CButtonST::OnDrawBorder(CDCHandle pDC, LPCRECT pRect)
{
	if (m_bIsPressed)
		{
		pDC.Draw3dRect(pRect, ::GetSysColor(COLOR_BTNSHADOW), ::GetSysColor(COLOR_BTNHILIGHT));
		}
	else
		{
		pDC.Draw3dRect(pRect, ::GetSysColor(COLOR_BTNHILIGHT), ::GetSysColor(COLOR_BTNSHADOW));
		}

	return BTNST_OK;
} // End of OnDrawBorder


}}