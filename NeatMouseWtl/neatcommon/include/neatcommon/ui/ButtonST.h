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


// Support for image list
// Support for dynamic creation

#if !defined(__buttonst_h__)
#define __buttonst_h__

#include <atlctrls.h>
#include <atlgdi.h>
#include <atlcrack.h>
#include <atlmisc.h>

// Comment this if you don't want that CButtonST hilights itself
// also when the window is inactive (like happens in Internet Explorer)
//#define ST_LIKEIE

namespace neatcommon {
namespace ui {

// Return values
#ifndef	BTNST_OK
#define	BTNST_OK						0
#endif
#ifndef	BTNST_INVALIDRESOURCE
#define	BTNST_INVALIDRESOURCE			1
#endif
#ifndef	BTNST_FAILEDMASK
#define	BTNST_FAILEDMASK				2
#endif
#ifndef	BTNST_INVALIDINDEX
#define	BTNST_INVALIDINDEX				3
#endif

// CButton - client side for a Windows BUTTON control

class CButtonST : public CWindowImpl<CButtonST, CButton, CControlWinTraits>
{
	CBitmapHandle transparentBitmap;
	int trasparentBitmapWidth, transparentBitmapHeight;
public:
	typedef CWindowImpl<CButtonST, CButton, CControlWinTraits>		inherited;
	typedef CButtonST												thisClass;

	//! Alignment
    enum Alignment {ST_ALIGN_HORIZ, ST_ALIGN_VERT, ST_ALIGN_HORIZ_RIGHT};
	//! Color types
	enum ColorType
		{
		BTNST_COLOR_BK_IN	= 0,		//!< Background color when mouse is INside
		BTNST_COLOR_FG_IN,				//!< Text color when mouse is INside
		BTNST_COLOR_BK_OUT,				//!< Background color when mouse is OUTside
		BTNST_COLOR_FG_OUT,				//!< Text color when mouse is OUTside
		BTNST_MAX_COLORS
		};

	//! Constructor
    CButtonST();
	//! Destructor
	~CButtonST();

	//! Sets the cursor of the button
	bool SetBtnCursor(_U_STRINGorID nCursorId, HMODULE rsrcModule = 0);
	//! Sets the icons of the button
	DWORD SetIcon(HICON hIconIn, HICON hIconOut);
	//! Sets the icons of the button
	DWORD SetIcon(_U_STRINGorID nIconInId, _U_STRINGorID nIconOutId = 0U, HMODULE rsrcModule = 0);
	//! Set the icons of the buttons form an image list
	DWORD SetIcon(HIMAGELIST imagelIst, int idxIn, int idxOut);


	DWORD SetTransparentBitmap(CBitmapHandle bh, int w=16, int h=16) {this->transparentBitmap = bh; trasparentBitmapWidth = w; transparentBitmapHeight = h; return 0;}
	//! Sets the bitmaps of the button
	DWORD SetBitmaps(HBITMAP hBitmapIn, COLORREF crTransColorIn, HBITMAP hBitmapOut, COLORREF crTransColorOut = 0);
	//! Sets the bitmaps of the button
	DWORD SetBitmaps(_U_STRINGorID nBitmapIn, COLORREF crTransColorIn, _U_STRINGorID nBitmapOut = 0U, COLORREF crTransColorOut = 0, HMODULE rsrcModule = 0);
	//! Sets the button flatness
	void SetFlat(bool bState);
	//! Gets the button flatness
	bool GetFlat();
	//! Sets the button alignment
	void SetAlign(Alignment nAlign);
	//! Gets the button alignment
	Alignment GetAlign();
	//! Shows or hides the border
	void DrawBorder(bool bEnable);
	//! Is the button the default button
	bool GetDefault();
	//! Shows the focus if the style is flat
	void SetFlatFocus(bool bDrawFlatFocus, bool bRepaint = true);
	//! Set the tooltip text
	void SetTooltipText(LPCTSTR lpszText, bool bActivate = true);
	//! Set the tooltip text
	void SetTooltipText(int nId, bool bActivate = true);
	//! Activate/Deactivate the tooltip
	void ActivateTooltip(bool bActivate);
	//! Set the state of a check box
	void SetCheck(bool nCheck, bool bRepaint = true);
	//! Set the check box state
	bool GetCheck() const;
	//! Set the color corresponding to a color type
	/*!
		\param byColorIndex: index of the color to set. This index is zero-based.
		\param crColor : New color.
		\param bRepaint: If TRUE the control will be repainted.
		\return BTNST_OK if the function executed successfully or BTNST_INVALIDINDEXif it was an invalid color index.
	*/
	DWORD SetColor(ColorType byColorIndex, COLORREF crColor, bool bRepaint = true);
	//! Get the color corresponding to a color type
	/*!
		\param byColorIndex: index of the color to set. This index is zero-based.
		\retval crpColor : New color.
		\return BTNST_OK if the function executed successfully or BTNST_INVALIDINDEXif it was an invalid color index.
	*/
	DWORD GetColor(ColorType byColorIndex, COLORREF* crpColor);
	//! Sets the URL
	bool SetURL(LPCTSTR lpszURL);
	//! Set the qutorepeat parameters
	bool SetAutoRepeat(bool bSet, DWORD dwMilliseconds);
	//§ Set the trqansparency of the button
	void DrawTransparent(bool bRepaint = true);
	//! Subclass an existing control
	BOOL SubclassWindow(HWND hWnd);

	static short GetVersionI()		{return 32;}
	static LPCTSTR GetVersionC()	{return _T("3.2");}

	BEGIN_MSG_MAP_EX(thisClass)
		MESSAGE_RANGE_HANDLER(WM_MOUSEFIRST, WM_MOUSELAST, OnMouseMessage)
		MSG_WM_MOUSEMOVE(OnMouseMove)
		MSG_WM_KILLFOCUS(OnKillFocus)
		MSG_WM_LBUTTONDOWN(OnLButtonDown)
		MSG_WM_ACTIVATE(OnActivate)
		MSG_WM_ENABLE(OnEnable)
		MSG_WM_CANCELMODE(OnCancelMode)
		MSG_WM_CAPTURECHANGED(OnCaptureChanged)
		MSG_WM_SETCURSOR(OnSetCursor)
		MSG_WM_SYSCOLORCHANGE(OnSysColorChange)
		MSG_WM_CREATE(OnCreate)
		REFLECTED_COMMAND_CODE_HANDLER_EX(BN_CLICKED, OnClicked)
		MSG_OCM_DRAWITEM(DrawItem)
		MSG_OCM_CTLCOLORBTN(OnCtlColor)
	END_MSG_MAP()

protected:
	//! Set the default colors
	/*!
	\param bRepaint : If true the control will be repainted
	\return BTNST_OK if the function executed successfully
	 */
	DWORD SetDefaultColors(bool bRepaint);
	//! Frees all internal resources
	void FreeResources(bool bCheckFor0 = true);
	//! Resets the hover state variables
	void CancelHover();
	//! Paints the background
	void PaintBk(CDCHandle pDC);
	//! Draw the bitmap
	void DrawTheBitmap(CDCHandle pDC, bool bHasTitle, RECT &rItem, CRect &rCaption, bool bIsPressed, bool bIsDisabled);
	//! Draw the icon
	void DrawTheIcon(CDCHandle pDC, bool bHasTitle, RECT &rpItem, CRect &rpTitle, bool bIsPressed, bool bIsDisabled);
	//! Computes the button rectangle
	void PrepareImageRect(bool bHasTitle, RECT &rpItem, CRect &rpTitle, bool bIsPressed, DWORD dwWidth, DWORD dwHeight, CRect &rpImage);
	//! Inits the tooltip
	void InitToolTip();
	//§ Creates the mask corresponding to a bitmap
	HBITMAP CreateBitmapMask(HBITMAP hSourceBitmap, DWORD dwWidth, DWORD dwHeight, COLORREF crTransColor);

	//! Creation of the window
	LRESULT OnCreate(LPCREATESTRUCT pCreateStruct);
	//! Relays the mouse messages to the tooltip
	LRESULT OnMouseMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	//! Called when the mouse is over the button and moving
	LRESULT OnMouseMove(UINT fwKeys, CPoint pt);
	//! Called when the window loses the focus
	LRESULT OnKillFocus(HWND hWnd);
	//! Called when the button is being activated or deactivated
	void OnActivate(UINT nState, BOOL bMinimized, HWND pWndOther);
	//! Called when an application changes the enabled state of the button
	void OnEnable(BOOL bEnable);
	//! Called in order to inform CWnd to cancel any internal mode
	void OnCancelMode();
	//! Called when the button is losing the mouse capture
	void OnCaptureChanged(HWND pWnd);
	//! Draws the button
	void DrawItem(UINT ctrlID, LPDRAWITEMSTRUCT lpDIS);
	//! Called when the user presses the left mouse button
	LRESULT OnLButtonDown(UINT nFlags, CPoint pt);
	//! Sent to the window if the mouse causes the cursor to move within a window and mouse input is not captured.
	BOOL OnSetCursor(HWND pWnd, UINT nHitTest, UINT message);
	//! Called when a change is made in the system color setting
	void OnSysColorChange();
	//! Called when the button notifies of a click
	BOOL OnClicked(UINT wNotifyCode, int wID, HWND pWnd);
	//! Sets the button background brush
	LRESULT OnCtlColor(CDCHandle pDC, HWND button);
	//! the window procedure is overriden in order to translate double clicks into button up events
	virtual WNDPROC GetWindowProc();
	//! the new window procedure
	static LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	//§ Can be overriden for drawing a custom background
	virtual DWORD OnDrawBackground(CDCHandle pDC, LPCRECT pRect);
	virtual DWORD OnDrawBorder(CDCHandle pDC, LPCRECT pRect);

	//! Internal structure used for storing icons
	struct STRUCT_ICONS
	{
		HICON		hIcon;			// Handle to icon
		DWORD		dwWidth;		// Width of icon
		DWORD		dwHeight;		// Height of icon
	};

	//! Internal structure used for storing bitmaps
	struct STRUCT_BITMAPS
	{
		CBitmap		hBitmap;		// Handle to bitmap
		DWORD		dwWidth;		// Width of bitmap
		DWORD		dwHeight;		// Height of bitmap
		CBitmap		hMask;			// Handle to mask bitmap
		COLORREF	crTransparent;	// Transparent color
	};

	Alignment		m_nAlign;						//!< Button alignment
	DWORD			m_dwPeriodAutoRepeat;			//!< the period for the auto-repeat feature
	HCURSOR			m_hCursor;						//!< the cursor shown with the button
	CToolTipCtrl	m_ToolTip;						//!< the tooltip control
	bool			m_bMouseOnButton:1;				//!< mouse currently on button ?
	bool			m_bDrawTransparent:1;			//!< tranparent button ?
	bool			m_bIsPressed:1;
	bool			m_bIsFocused:1;
	bool			m_bIsDisabled:1;
	bool			m_bDrawBorder:1;				//!< draw the border
	bool			m_bIsFlat:1;					//!< flat style
	bool			m_bDrawFlatFocus:1;				//!< do we need to the draw the focus of a flat button
	bool			m_bAutoRepeat:1;				//!< auto-repeat ?
	bool			m_bIsDefault:1;					//!< default button ?
	bool			m_bIsCheckBox:1;				//!< used as check box ?
	bool			m_nCheck:1;						//!< button check state
	CDC				m_dcBk;							//!< offscreen DC
	CBitmap			m_bmpBk;						//!< Bitmap used for drawing offscreen
	CBitmapHandle	m_pbmpOldBk;					//!< Original bitmap selected in m_dcBk
	COLORREF		m_crColors[BTNST_MAX_COLORS];	//!< Coilors associated to the color types
	TCHAR			m_szURL[_MAX_PATH];				//!< URL
	STRUCT_ICONS	m_csIcons[2];					//!< icons used for drawing the button
	STRUCT_BITMAPS	m_csBitmaps[2];					//!< bitmaps used for drawing the button

};


}}

#endif // !defined(__buttonst_h__)
