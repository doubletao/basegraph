#include "stdafx.h"
#include "UIControl\UIColorButton.h"
#include "ThemeManager.h"

UIColorButton::UIColorButton()
{
	//给个红色当作默认颜色
	m_color.color = 0x88FF0000;
}


UIColorButton::~UIColorButton()
{
}

void UIColorButton::Initialize(int nX, int nY, int nWidth, int nHeight, int nID, HWND hParentWnd, UIElement * pParent)
{
	UIElement::Initialize(nX, nY, nWidth, nHeight, nID, hParentWnd, pParent);
	m_setMsgFilter.insert(WM_LBUTTONDOWN);
}

void UIColorButton::DrawElement(CDC * pDC)
{
	if (m_bVisible)
	{
		//先把底图搞上来
		CImage * pBkImg = ThemeManager::Instance().GetImage(ThemeImage_ColorButtonTranspBmp);
		if (pBkImg)
		{
			int nBkWidth = pBkImg->GetWidth();
			int nBkHeight = pBkImg->GetHeight();
			int nRealX = GetRealX();
			int nRealY = GetRealY();
			CDC	MemDC;
			MemDC.CreateCompatibleDC(pDC);
			CBitmap	TempBitmap;
			TempBitmap.CreateCompatibleBitmap(pDC, nBkWidth, nBkHeight);
			CBitmap * pOldBitmap = MemDC.SelectObject(&TempBitmap);
			int nOldModeDC = pDC->SetStretchBltMode(COLORONCOLOR);//HALFTONE);
			int nOldModeMem = MemDC.SetStretchBltMode(COLORONCOLOR);//HALFTONE);
			CRect rcDrawer(0, 0, nBkWidth, nBkHeight);
			BufferFunction::DrawImageWithAplha(&MemDC, pBkImg, rcDrawer, rcDrawer, CRect(0, 0, nBkWidth, nBkHeight), CRect(), TRUE, 255, FALSE);
			//绘制颜色块
			CSYBuffer	DrawBuffer(nBkWidth, nBkHeight);
			memset(DrawBuffer.m_dwBuffer, 0, nBkWidth * nBkHeight * sizeof(DWORD));
			CRect rt(0, 0, nBkWidth, nBkHeight);
			SYColor_FillColor(&m_color, DrawBuffer.m_dwBuffer, nBkWidth, nBkHeight, 0, 0, nBkWidth, nBkHeight, 1.0f);
			//颜色块贴到底图上
			BITMAPINFO  BmpInfo;
			BmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
			BmpInfo.bmiHeader.biWidth = nBkWidth;
			BmpInfo.bmiHeader.biHeight = nBkHeight;
			BmpInfo.bmiHeader.biPlanes = 1;
			BmpInfo.bmiHeader.biBitCount = 32;
			BmpInfo.bmiHeader.biCompression = BI_RGB;
			BmpInfo.bmiHeader.biSizeImage = 0;
			BmpInfo.bmiHeader.biXPelsPerMeter = 0;
			BmpInfo.bmiHeader.biYPelsPerMeter = 0;
			BmpInfo.bmiHeader.biClrUsed = 0;
			BmpInfo.bmiHeader.biClrImportant = 0;
			int iStretchModeOld = SetStretchBltMode(MemDC.m_hDC, COLORONCOLOR);
			::StretchDIBits(MemDC.m_hDC, 0, 0, nBkWidth, nBkHeight, 0, 0, nBkWidth, nBkHeight, DrawBuffer.m_dwBuffer, &BmpInfo, DIB_RGB_COLORS, SRCCOPY);
			//一起贴到DC上
			pDC->StretchBlt(nRealX, nRealY, m_nWidth, m_nHeight, &MemDC, 0, 0, nBkWidth, nBkHeight, SRCCOPY);
			//释放
			DrawBuffer.Release();
			pDC->SetStretchBltMode(nOldModeDC);
			MemDC.SetStretchBltMode(nOldModeMem);
			MemDC.SelectObject(pOldBitmap);
			TempBitmap.DeleteObject();
			MemDC.DeleteDC();
		}
	}
}

bool UIColorButton::Create(int nX, int nY, int nWidth, int nHeight, int nID, HWND hParentWnd, UIElement * pParent)
{
	if (!m_bCreated)
	{
		Initialize(nX, nY, nWidth, nHeight, nID, hParentWnd, pParent);
		return true;
	}
	return false;
}

EmUIElementMsgRet UIColorButton::HandleMsg(UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	if (m_setMsgFilter.count(nMsg) > 0)
	{
		switch (nMsg)
		{
		case WM_LBUTTONDOWN:
			if (HitTest(lParam))
			{
				OnButtonClickDown();
				return UIElementMsgSuccBreak;
			}
			break;
		default:
			break;
		}
	}
	return UIElementMsgSuccContinue;
}

void UIColorButton::OnButtonClickDown()
{
	int nA = COLORARGB_A(m_color.color);
	int nR = COLORARGB_R(m_color.color);
	int nG = COLORARGB_G(m_color.color);
	int nB = COLORARGB_B(m_color.color);
	CColorDialog dialog(RGB(nR, nG, nB));
	if (dialog.DoModal() == IDOK)
	{
		COLORREF cl = dialog.GetColor();
		int nR = GetRValue(cl);
		int nG = GetGValue(cl);
		int nB = GetBValue(cl);
		m_color.color = ARGBColor(nA, nR, nG, nB);
		EmitMsg(EmUIElementMsg_OnColorChanged, 0, 0);
	}
	Refresh();
}

void UIColorButton::SetColor(const sycolor & color, bool bRefresh/* = true*/)
{
	m_color = color;
	if (bRefresh)
	{
		Refresh();
	}
}

sycolor UIColorButton::GetColor()
{
	return m_color;
}
