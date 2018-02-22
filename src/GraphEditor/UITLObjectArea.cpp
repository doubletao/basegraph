#include "stdafx.h"
#include "UITLObjectArea.h"
#include <assert.h>

UITLObjectArea::UITLObjectArea()
{
}


UITLObjectArea::~UITLObjectArea()
{
	for (std::map<CString, UITLObjectItem *>::iterator it = m_mapObjDrawerInfo.begin(); it != m_mapObjDrawerInfo.end(); it++)
	{
		UITLObjectItem *& pObjEle = it->second;
		if (pObjEle)
		{
			delete pObjEle;
			pObjEle = nullptr;
		}
	}
	m_mapObjDrawerInfo.clear();
}

UITLObjectItem * UITLObjectArea::CreateObjectItem(CString strID)
{
	UITLObjectItem * pObjectItem = new UITLObjectItem;
	pObjectItem->SetObjID(strID);
	pObjectItem->SetExpand(false);
	m_mapObjDrawerInfo.insert(std::make_pair(strID, pObjectItem));
	return pObjectItem;
}

void UITLObjectArea::DrawElement(CDC * pDC)
{
	int nXStart = GetRealX();
	int nYStart = GetRealY();
	int nItemHeight = UIControl_GetIntValue(ThemeInt_TimeLineItemActionHeight);
	int nItemTabWidth = UIControl_GetIntValue(ThemeInt_TimeLineItemTreeTabWidth);
	int nItemTreeWidth = UIControl_GetIntValue(ThemeInt_TimeLineItemTreeWidth);
	int nPresetPix = UIControl_GetIntValue(ThemeInt_TimeLinePresetPixel);
	LOGFONT logFont = UIControl_GetFont(ThemeFont_TimeLineObjectTrackText);
	int nGraListTextColor = UIControl_GetColor(ThemeColor_TimeLineObjectTrackText_Nor);
	CImage * pLeftBkImage = UIControl_GetImage(ThemeImage_TimeLineLeftObjectBkGnd_Nor);
	CImage * pRightBkImage = UIControl_GetImage(ThemeImage_TimeLineRightObjectBkGnd_Nor);
	CImage * pActionBarImage = UIControl_GetImage(ThemeImage_TimeLineObjectActionBar_TotalLength);

	int nCurrentYPos = 0;
	//绘制一个总长度条
	if (pLeftBkImage
		&& pRightBkImage
		&& pActionBarImage)
	{
		CRect rtLeft(0, nCurrentYPos, nItemTreeWidth, nCurrentYPos + nItemHeight);
		rtLeft.OffsetRect(nXStart, nYStart);
		CRect rtRight(nItemTreeWidth, nCurrentYPos, m_nWidth, nCurrentYPos + nItemHeight);
		rtRight.OffsetRect(nXStart, nYStart);

		pLeftBkImage->AlphaBlend(pDC->GetSafeHdc(), rtLeft.left, rtLeft.top, rtLeft.Width(), rtLeft.Height(), 0, 0, pLeftBkImage->GetWidth(), pLeftBkImage->GetHeight());
		pRightBkImage->AlphaBlend(pDC->GetSafeHdc(), rtRight.left, rtRight.top, rtRight.Width(), rtRight.Height(), 0, 0, pRightBkImage->GetWidth(), pRightBkImage->GetHeight());
	}
	//在两个tab的地方绘制文本绘制文本
	CFont font;
	font.CreateFontIndirect(&logFont);
	CFont * pOldFont = pDC->SelectObject(&font);
	CRect rtText(nItemTabWidth * 2, nCurrentYPos, nItemTabWidth * 2 + nItemTreeWidth, nCurrentYPos + nItemHeight);
	pDC->SetTextColor(nGraListTextColor);
	pDC->DrawText(_T("字幕单"), rtText, DT_SINGLELINE | DT_LEFT | DT_CENTER);
	pDC->SelectObject(pOldFont);

	nCurrentYPos += nItemHeight;
	//绘制各个物件
	std::vector<CString> strObjID;
	DataBase_GetAllItemID(strObjID);
	UITLObjectItem * pObjectEle = nullptr;
	for (size_t i = 0; i < strObjID.size(); i++)
	{
		CString strID = strObjID[i];
		if (m_mapObjDrawerInfo.count(strID) > 0)
		{
			pObjectEle = m_mapObjDrawerInfo[strID];
		}
		else
		{
			//不存在则创建
			pObjectEle = CreateObjectItem(strID);
		}
		if (pObjectEle)
		{
			//pObjectEle->MoveTo(0, nCurrentYPos, false);
			//pObjectEle->DrawElement(pDC);
			//nCurrentYPos += pObjectEle->GetHeight();
		}
		else
		{
			assert(false);
		}
	}
}

bool UITLObjectArea::Create(int nX, int nY, int nWidth, int nHeight, int nID, HWND hParentWnd, UIElement * pParentElement)
{
	if (!m_bCreated)
	{
		Initialize(nX, nY, nWidth, nHeight, nID, hParentWnd, pParentElement);
		return true;
	}

	return false;
}
