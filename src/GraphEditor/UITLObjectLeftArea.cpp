#include "stdafx.h"
#include "UITLObjectLeftArea.h"
#include "UITLObjectArea.h"

UITLObjectLeftArea::UITLObjectLeftArea()
	: m_pObjectArea(nullptr)
{
}

UITLObjectLeftArea::~UITLObjectLeftArea()
{
}

int UITLObjectLeftArea::DrawItemLeft(CDC * pDC, CString strID, int nCol, int nCurrentY)
{
	if (m_pObjectArea)
	{
		UITLObjectItem * pObjDrawerInfo = m_pObjectArea->GetObjDrawerInfo(strID);
		if (pObjDrawerInfo)
		{
			int nXStart = GetRealX() + m_nScrollX;
			int nYStart = nCurrentY;
			bool bSel = false;
			DataRender_IsItemSelected(strID, bSel);

			int nItemHeight = UIControl_GetIntValue(ThemeInt_TimeLineItemActionHeight);
			int nItemTabWidth = UIControl_GetIntValue(ThemeInt_TimeLineItemTreeTabWidth);
			LOGFONT logFont = UIControl_GetFont(ThemeFont_TimeLineObjectTrackText);
			CImage * pLeftAreaBkImage = UIControl_GetImage(ThemeImage_TimeLineObjectLeftAreaBk);

			CImage * pImgBk = nullptr; 
			int nGraListTextColor = 0;
			if (bSel)
			{
				pImgBk = UIControl_GetImage(ThemeImage_TimeLineLeftObjectBkGnd_Sel);
				nGraListTextColor = UIControl_GetColor(ThemeColor_TimeLineObjectTrackText_Sel);
			}
			else
			{
				pImgBk = UIControl_GetImage(ThemeImage_TimeLineLeftObjectBkGnd_Nor);
				nGraListTextColor = UIControl_GetColor(ThemeColor_TimeLineObjectTrackText_Nor);
			}

			//绘制底色
			if (pImgBk)
			{
				CRect rtLeft(nXStart, nYStart, nXStart + m_nWidth, nYStart + nItemHeight);
				rtLeft.OffsetRect(-m_nScrollX, -m_nScrollY);
				pImgBk->AlphaBlend(pDC->GetSafeHdc(), rtLeft.left, rtLeft.top, rtLeft.Width(), rtLeft.Height(), 0, 0, pImgBk->GetWidth(), pImgBk->GetHeight());

				//将自己的左边矩形记录下来
				pObjDrawerInfo->rtLeft = rtLeft;
			}

			//绘制文本
			CString strText = _T("未命名");
			BaseGraph_Common::Value stuNameValue;
			if (DataBase_GetItemProperty(strID, BaseGraph_DataBase::ObjName_Str, stuNameValue))
			{
				strText = stuNameValue.ToCString();
			}
			CFont font;
			font.CreateFontIndirect(&logFont);
			CFont * pOldFont = pDC->SelectObject(&font);
			pDC->SetTextColor(nGraListTextColor);
			int nOldBkMode = pDC->SetBkMode(TRANSPARENT);
			CSize sizText = pDC->GetTextExtent(strText);
			CRect rtText(nXStart + nItemTabWidth * nCol, nYStart, nXStart + nItemTabWidth * nCol + sizText.cx, nYStart + nItemHeight);
			rtText.OffsetRect(-m_nScrollX, -m_nScrollY);
			pDC->DrawText(strText, rtText, DT_SINGLELINE | DT_LEFT | DT_VCENTER);
			pDC->SelectObject(pOldFont);
			pDC->SetBkMode(nOldBkMode);

			int nRet = nCurrentY + nItemHeight;
			//画自己的子
			std::vector<CString> vecSubID;
			if (DataBase_GetSubItem(strID, vecSubID))
			{
				for (size_t i = 0; i < vecSubID.size(); i++)
				{
					CString strSubID = vecSubID[i];
					nRet = DrawItemLeft(pDC, strSubID, nCol + 1, nCurrentY + nItemHeight);
				}
			}
			return nRet;
		}
	}
	return nCurrentY;
}

void UITLObjectLeftArea::DrawElement(CDC * pDC)
{
	int nXStart = GetRealX() + m_nScrollX;
	int nYStart = GetRealY() + m_nScrollY;
	CRect rcClient(nXStart, nYStart, nXStart + m_nWidth, nYStart + m_nHeight);

	CRgn rgnDC;
	rgnDC.CreateRectRgnIndirect(rcClient);
	pDC->SelectClipRgn(&rgnDC);

	int nItemHeight = UIControl_GetIntValue(ThemeInt_TimeLineItemActionHeight);
	int nItemTabWidth = UIControl_GetIntValue(ThemeInt_TimeLineItemTreeTabWidth);
	LOGFONT logFont = UIControl_GetFont(ThemeFont_TimeLineObjectTrackText);
	CImage * pLeftAreaBkImage = UIControl_GetImage(ThemeImage_TimeLineObjectLeftAreaBk);
	int nGraListTextColorNor = UIControl_GetColor(ThemeColor_TimeLineObjectTrackText_Nor);
	CImage * pLeftBkImageNor = UIControl_GetImage(ThemeImage_TimeLineLeftObjectBkGnd_Nor);
	int nGraListTextColorSel = UIControl_GetColor(ThemeColor_TimeLineObjectTrackText_Sel);
	CImage * pLeftBkImageSel = UIControl_GetImage(ThemeImage_TimeLineLeftObjectBkGnd_Sel);

	//绘制背景
	if (pLeftAreaBkImage)
	{
		CRect rtBk(nXStart, nYStart, nXStart + m_nWidth, nYStart + m_nHeight);
		pLeftAreaBkImage->AlphaBlend(pDC->GetSafeHdc(), rtBk.left, rtBk.top, rtBk.Width(), rtBk.Height(), 0, 0, pLeftAreaBkImage->GetWidth(), pLeftAreaBkImage->GetHeight());
	}

	//绘制首条底色
	if (pLeftBkImageSel)
	{
		CRect rtLeft(nXStart, nYStart, nXStart + m_nWidth, nYStart + nItemHeight);
		rtLeft.OffsetRect(-m_nScrollX, -m_nScrollY);
		pLeftBkImageSel->AlphaBlend(pDC->GetSafeHdc(), rtLeft.left, rtLeft.top, rtLeft.Width(), rtLeft.Height(), 0, 0, pLeftBkImageSel->GetWidth(), pLeftBkImageSel->GetHeight());
	}

	//绘制文本
	CString strText = _T("物件列表");
	int nCol = 1;
	CFont font;
	font.CreateFontIndirect(&logFont);
	CFont * pOldFont = pDC->SelectObject(&font);
	pDC->SetTextColor(nGraListTextColorSel);
	int nOldBkMode = pDC->SetBkMode(TRANSPARENT);
	CSize sizText = pDC->GetTextExtent(strText);
	CRect rtText(nXStart + nItemTabWidth * nCol, nYStart, nXStart + nItemTabWidth * nCol + sizText.cx, nYStart + nItemHeight);
	rtText.OffsetRect(-m_nScrollX, -m_nScrollY);
	pDC->DrawText(strText, rtText, DT_SINGLELINE | DT_LEFT | DT_VCENTER);
	pDC->SelectObject(pOldFont);
	pDC->SetBkMode(nOldBkMode);

	//绘制物件
	int nCurentY = nYStart + nItemHeight - m_nScrollY;
	std::vector<CString> vecObjID;
	if (DataBase_GetAllItemIDExceptSubItem(vecObjID))
	{
		for (size_t i = 0; i < vecObjID.size(); i++)
		{
			CString strID = vecObjID[i];
			nCurentY = DrawItemLeft(pDC, strID, nCol + 1, nCurentY);
		}
	}

	pDC->SelectClipRgn(NULL);
}

bool UITLObjectLeftArea::Create(int nX, int nY, int nWidth, int nHeight, int nID, HWND hParentWnd, UITLObjectArea * pParentElement)
{
	if (!m_bCreated)
	{
		Initialize(nX, nY, nWidth, nHeight, nID, hParentWnd, pParentElement);
		m_pObjectArea = pParentElement;
		return true;
	}
	return false;
}
