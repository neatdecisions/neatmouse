//
// Copyright © 2016–2020 Neat Decisions. All rights reserved.
//
// This file is part of NeatMouse.
// The use and distribution terms for this software are covered by the
// Microsoft Public License (http://opensource.org/licenses/MS-PL)
// which can be found in the file LICENSE at the root folder.
//

#if !defined(_CUSTOMIZED_CONTROLS_H_)
#define _CUSTOMIZED_CONTROLS_H_


namespace neatcommon {
namespace ui {

//=====================================================================================================================
// functions
//=====================================================================================================================
HBITMAP AtlLoadGdiplusImage(ATL::_U_STRINGorID bitmap, ATL::_U_STRINGorID type);
void DrawBitmapAdvanced(HDC pDC, CBitmapHandle bitmap, int x, int y, int w, int h, bool isDisabled = false);
void SetMenuItemBitmapCallbackMode(CMenuHandle hmenu, UINT id, CBitmapHandle bitmap, BOOL byPosition = FALSE);


//=====================================================================================================================
// CGdiPlusInitializer
//=====================================================================================================================
class CGdiPlusInitializer
{
public:
	static bool IsGdiPlusPresent();
	bool Init();
	void Uninit();
	~CGdiPlusInitializer();

private:
	volatile static int m_GdiPlusPresent;
	bool gdiPlusInit = false;
	ULONG_PTR gdiplusToken = 0;
};


//=====================================================================================================================
// CImageManager
//=====================================================================================================================
class CImageManager
{
public:
	~CImageManager();
	HBITMAP GetBitmapFromPng(UINT id);

protected:
	std::map<UINT, HBITMAP> bitmaps;
};


//=====================================================================================================================
// CImageManager
//=====================================================================================================================
class CMenuBitmapsManager
{
public:
	CMenuBitmapsManager(int cx = 16, int cy = 16);
	CBitmapHandle GetBitmap(UINT id);
	void SetBitmap(UINT id, CBitmapHandle bmp);
	void Measure(LPMEASUREITEMSTRUCT lpmis);
	void Draw(LPDRAWITEMSTRUCT lpdis);

protected:
	std::map<UINT, CBitmapHandle> icons;
	CSize sz;
};


//=====================================================================================================================
// CAutosizeStatic
//=====================================================================================================================
class CAutosizeStatic : public CStatic
{
public:
	void SetText(const CString & s);
};


}}

#endif