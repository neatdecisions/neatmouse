//
// Copyright © 2016–2020 Neat Decisions. All rights reserved.
//
// This file is part of NeatMouse.
// The use and distribution terms for this software are covered by the
// Microsoft Public License (http://opensource.org/licenses/MS-PL)
// which can be found in the file LICENSE at the root folder.
//

#include "stdafx.h"

#include "neatcommon/ui/CustomizedControls.h"

namespace neatcommon {
namespace ui {

volatile int CGdiPlusInitializer::m_GdiPlusPresent = -1;


//---------------------------------------------------------------------------------------------------------------------
void SetMenuItemBitmapCallbackMode(CMenuHandle hmenu, UINT id, CBitmapHandle bitmap, BOOL byPosition)
{
	MENUITEMINFO mii{};
	mii.cbSize = sizeof(mii);
	mii.fMask = MIIM_BITMAP;
	if (neatcommon::system::IsWindowsVistaOrLater())
		mii.hbmpItem = bitmap;
	else
		mii.hbmpItem = HBMMENU_CALLBACK;
	hmenu.SetMenuItemInfo(id, byPosition, &mii);
}


//---------------------------------------------------------------------------------------------------------------------
void DrawBitmapAdvanced(HDC pDC, CBitmapHandle bitmap, int x, int y, int w, int h, bool isDisabled)
{
	CDC dcImage;
	dcImage.CreateCompatibleDC(pDC);

	if (isDisabled)
	{
		BITMAPINFO bmi{};
		bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bmi.bmiHeader.biWidth = w;
		bmi.bmiHeader.biHeight = h;
		bmi.bmiHeader.biPlanes = 1;
		bmi.bmiHeader.biBitCount = 32;
		bmi.bmiHeader.biCompression = BI_RGB;
		bmi.bmiHeader.biSizeImage = bmi.bmiHeader.biWidth * bmi.bmiHeader.biHeight * 4;

		VOID *pvBits;
		WTL::CBitmap Bitmap(::CreateDIBSection(dcImage, &bmi, DIB_RGB_COLORS, &pvBits, NULL, 0));
		HBITMAP hBmpOld = dcImage.SelectBitmap(Bitmap);

		CMemoryDC mdc(dcImage, CRect(0, 0, w, h));

		HBITMAP ho = mdc.SelectBitmap(bitmap);
		dcImage.BitBlt(0, 0, w, h, mdc, 0, 0, SRCCOPY);
		mdc.SelectBitmap(ho);

		// convert to grayscale
		for (unsigned char *p = (unsigned char*)pvBits, *end = p + bmi.bmiHeader.biSizeImage; p < end; p += 4)
		{
			// Gray = 0.3*R + 0.59*G + 0.11*B
			p[0] = p[1] = p[2] = //5;
			(
				static_cast<unsigned int>(p[2]) *  77 +
				static_cast<unsigned int>(p[1]) * 151 +
				static_cast<unsigned int>(p[0]) *  28
			) >> 8;
		}

		BLENDFUNCTION BlendFunction;
		BlendFunction.BlendOp = AC_SRC_OVER;
		BlendFunction.BlendFlags = 0;
		BlendFunction.SourceConstantAlpha = 0xFF;  // half transparent
		BlendFunction.AlphaFormat = AC_SRC_ALPHA;  // use bitmap alpha

		AlphaBlend(pDC, x, y, w, h, dcImage, 0, 0, w, h, BlendFunction);

		dcImage.SelectBitmap(hBmpOld);
		mdc.DeleteDC();

	} else
	{
		HBITMAP hBmpOld = dcImage.SelectBitmap(bitmap);
		BLENDFUNCTION BlendFunction;
		BlendFunction.BlendOp = AC_SRC_OVER;
		BlendFunction.BlendFlags = 0;
		BlendFunction.SourceConstantAlpha = 0xFF;   // half transparent
		BlendFunction.AlphaFormat = AC_SRC_ALPHA;  // use bitmap alpha

		AlphaBlend(pDC, x, y, w, h,	dcImage, 0, 0, w, h, BlendFunction);

		dcImage.SelectBitmap(hBmpOld);
	}
}


//---------------------------------------------------------------------------------------------------------------------
HBITMAP AtlLoadGdiplusImage(ATL::_U_STRINGorID bitmap, ATL::_U_STRINGorID type = (UINT) 0)
{
	USES_CONVERSION;

	if (!CGdiPlusInitializer::IsGdiPlusPresent())
	{
		return 0;
	}

	std::unique_ptr<Gdiplus::Bitmap> pBitmap;
	if (HIWORD(bitmap.m_lpstr) != NULL)
	{
		// Load from filename
		pBitmap = std::make_unique<Gdiplus::Bitmap>(T2CW(bitmap.m_lpstr));
	}
	else if (type.m_lpstr != NULL && type.m_lpstr != RT_BITMAP)
	{
		// Loading PNG, JPG resources etc
		WTL::CResource res;
		if( !res.Load(type, bitmap) ) return NULL;
		DWORD dwSize = res.GetSize();
		HANDLE hMemory = ::GlobalAlloc(GMEM_MOVEABLE, dwSize);
		if( hMemory == NULL ) return NULL;
		::memcpy(::GlobalLock(hMemory), res.Lock(), dwSize);
		::GlobalUnlock(hMemory);
		IStream* pStream = NULL;
		if( FAILED( ::CreateStreamOnHGlobal(hMemory, TRUE, &pStream) ) )
		{
			::GlobalFree(hMemory);
			return FALSE;
		}
		pBitmap = std::make_unique<Gdiplus::Bitmap>(pStream);
		pStream->Release();
	}
	else
	{
		// This only loads BMP resources
		pBitmap = std::make_unique<Gdiplus::Bitmap>(_Module.GetResourceInstance(), (LPCWSTR) (UINT) bitmap.m_lpstr);
	}
	if (!pBitmap) return NULL;
	HBITMAP hBitmap = NULL;
	pBitmap->GetHBITMAP(NULL, &hBitmap);
	return hBitmap;
}


//=====================================================================================================================
// CGdiPlusInitializer
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
bool
CGdiPlusInitializer::IsGdiPlusPresent()
{
	if (m_GdiPlusPresent == -1)
	{
		CStaticDataInitCriticalSectionLock lock;
		if(FAILED(lock.Lock()))
		{
			ATLTRACE2(atlTraceUI, 0, _T("ERROR : Unable to lock critical section in IsGdiPlusPresent.\n"));
			ATLASSERT(FALSE);
			return false;
		}

		if (m_GdiPlusPresent == -1)
		{
			HMODULE hThemeDLL = ::LoadLibrary(_T("gdiplus.dll"));
			m_GdiPlusPresent = (hThemeDLL != NULL) ? 1 : 0;
			if(hThemeDLL != NULL)
				::FreeLibrary(hThemeDLL);
		}

		lock.Unlock();
	}

	ATLASSERT(m_GdiPlusPresent != -1);
	return (m_GdiPlusPresent == 1);
}


//---------------------------------------------------------------------------------------------------------------------
bool
CGdiPlusInitializer::Init()
{
	if (IsGdiPlusPresent())
	{
		Gdiplus::GdiplusStartupInput gdiplusStartupInput;
		gdiPlusInit = Gdiplus::Ok == Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
	}

	return gdiPlusInit;
}


//---------------------------------------------------------------------------------------------------------------------
void
CGdiPlusInitializer::Uninit()
{
	if (gdiPlusInit)
	{
		Gdiplus::GdiplusShutdown(gdiplusToken);
	}
}


//---------------------------------------------------------------------------------------------------------------------
CGdiPlusInitializer::~CGdiPlusInitializer()
{
	Uninit();
}



//=====================================================================================================================
// CImageManager
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
CImageManager::~CImageManager()
{
	for (const auto & kv : bitmaps)
	{
		DeleteObject(kv.second);
	}
}


//---------------------------------------------------------------------------------------------------------------------
HBITMAP
CImageManager::GetBitmapFromPng(UINT id)
{
	std::map<UINT, HBITMAP>::iterator it = bitmaps.find(id);
	if (it != bitmaps.end())
	{
		return it->second;
	}
	HBITMAP b = AtlLoadGdiplusImage(id, _T("PNG"));
	bitmaps.emplace(id, b);
	return b;
}



//=====================================================================================================================
// MenuBitmapsManager
//=====================================================================================================================


//---------------------------------------------------------------------------------------------------------------------
CMenuBitmapsManager::CMenuBitmapsManager(int cx, int cy)
{
	sz.SetSize(cx, cy);
}


//---------------------------------------------------------------------------------------------------------------------
CBitmapHandle
CMenuBitmapsManager::GetBitmap(UINT id)
{
	std::map<UINT, CBitmapHandle>::iterator it = icons.find(id);
	return (it == icons.end()) ? CBitmapHandle() : it->second;
}


//---------------------------------------------------------------------------------------------------------------------
void
CMenuBitmapsManager::SetBitmap(UINT id, CBitmapHandle bmp)
{
	if (bmp.IsNull())
	{
		icons.erase(id);
	} else
	{
		icons[id] = bmp;
	}
}


//---------------------------------------------------------------------------------------------------------------------
void
CMenuBitmapsManager::Measure(LPMEASUREITEMSTRUCT lpmis)
{
	if (lpmis == NULL)
	{
		return;
	}

	std::map<UINT, CBitmapHandle>::iterator it = icons.find(lpmis->itemID);
	if (it == icons.end())
	{
		return;
	}

	lpmis->itemWidth += 2;
	if (static_cast<LONG>(lpmis->itemHeight) < sz.cy)
	{
		lpmis->itemHeight = sz.cy;
	}
}


//---------------------------------------------------------------------------------------------------------------------
void
CMenuBitmapsManager::Draw(LPDRAWITEMSTRUCT lpdis)
{
	if ((lpdis == NULL) || (lpdis->CtlType != ODT_MENU))
	{
		return; // not for a menu
	}

	std::map<UINT, CBitmapHandle>::iterator it = icons.find(lpdis->itemID);
	if (it == icons.end())
	{
		return;
	}

	DrawBitmapAdvanced(
			lpdis->hDC,
			it->second,
			lpdis->rcItem.left - sz.cx,
			lpdis->rcItem.top + (lpdis->rcItem.bottom - lpdis->rcItem.top - sz.cy) / 2,
			sz.cx,
			sz.cy,
			(lpdis->itemState & ODS_GRAYED) != FALSE);

}



//=====================================================================================================================
// CAutosizeStatic
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
void
CAutosizeStatic::SetText(const CString & s)
{
	CClientDC dc(*this);

	SIZE sz;
	HFONT oldFont = dc.SelectFont(GetFont());
	dc.GetTextExtent(s, s.GetLength(), &sz);
	dc.SelectFont(oldFont);
	RECT r;

	GetClientRect(&r);
	r.right = r.left + sz.cx;
	ResizeClient(sz.cx, sz.cy);

	SetWindowText(s);
}


}}