#include "stdafx.h"
#include "UITLObjectArea.h"
#include "UITLRuler.h"
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

int UITLObjectArea::GetTotalLength()
{
	int nFrameLength = 0;
	if (DataBase_GetItemLength(nFrameLength))
	{
		nFrameLength = nFrameLength / 2;
	}
	else
	{
		nFrameLength = 5;
	}
	return nFrameLength;
}

void UITLObjectArea::DrawElement(CDC * pDC)
{
	int nXStart = GetRealX();
	int nYStart = GetRealY();
	int nItemHeight = UIControl_GetIntValue(ThemeInt_TimeLineItemActionHeight);
	int nItemTabWidth = UIControl_GetIntValue(ThemeInt_TimeLineItemTreeTabWidth);
	int nItemTreeWidth = UIControl_GetIntValue(ThemeInt_TimeLineItemTreeWidth);
	LOGFONT logFont = UIControl_GetFont(ThemeFont_TimeLineObjectTrackText);
	int nGraListTextColor = UIControl_GetColor(ThemeColor_TimeLineObjectTrackText_Nor);
	CImage * pLeftBkImage = UIControl_GetImage(ThemeImage_TimeLineLeftObjectBkGnd_Nor);
	CImage * pRightBkImage = UIControl_GetImage(ThemeImage_TimeLineRightObjectBkGnd_Nor);
	CImage * pActionBarImage = UIControl_GetImage(ThemeImage_TimeLineObjectActionBar_TotalLength);

	int nCurrentYPos = nYStart;
	//绘制一个总长度条
	if (pLeftBkImage
		&& pRightBkImage
		&& pActionBarImage)
	{
		CRect rtLeft(0, nCurrentYPos, nItemTreeWidth, nCurrentYPos + nItemHeight);
		CRect rtRight(nItemTreeWidth, nCurrentYPos, m_nWidth, nCurrentYPos + nItemHeight);

		pLeftBkImage->AlphaBlend(pDC->GetSafeHdc(), rtLeft.left, rtLeft.top, rtLeft.Width(), rtLeft.Height(), 0, 0, pLeftBkImage->GetWidth(), pLeftBkImage->GetHeight());
		pRightBkImage->AlphaBlend(pDC->GetSafeHdc(), rtRight.left, rtRight.top, rtRight.Width(), rtRight.Height(), 0, 0, pRightBkImage->GetWidth(), pRightBkImage->GetHeight());

		//取出当前总长及当前比例尺，画总长条
		int nBarLength = int(GetTotalLength() * UITLRuler::Instance().GetPixelPerFrame());

		CRect rtBar(nItemTreeWidth, nCurrentYPos + 1, nItemTreeWidth + nBarLength, nCurrentYPos + nItemHeight - 1);
		rtBar.OffsetRect(nXStart, 0);//bar需要考虑滚动条的滚动
		pActionBarImage->AlphaBlend(pDC->GetSafeHdc(), rtBar.left, rtBar.top, rtBar.Width(), rtBar.Height(), 0, 0, pActionBarImage->GetWidth(), pActionBarImage->GetHeight());
	}
	//在两个tab的地方绘制文本绘制文本
	CFont font;
	font.CreateFontIndirect(&logFont);
	CFont * pOldFont = pDC->SelectObject(&font);
	CRect rtText(nItemTabWidth, nCurrentYPos, nItemTabWidth * 2 + nItemTreeWidth, nCurrentYPos + nItemHeight);
	pDC->SetTextColor(nGraListTextColor);
	int nOldBkMode = pDC->SetBkMode(TRANSPARENT);
	pDC->DrawText(_T("字幕单"), rtText, DT_SINGLELINE | DT_LEFT | DT_VCENTER);
	pDC->SelectObject(pOldFont);
	pDC->SetBkMode(nOldBkMode);

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

void UITLObjectArea::AdjustScrollZoom()
{
	int nItemTreeWidth = UIControl_GetIntValue(ThemeInt_TimeLineItemTreeWidth);
	int nRightWidth = m_nWidth - nItemTreeWidth;
	int nFrameLength = GetTotalLength();
	int nAdjustPreservePercent = UIControl_GetIntValue(ThemeInt_TimeLineAdjustPreservePercent);

	nFrameLength = int(nFrameLength * (1 + 10 / 100.0));
	UITLRuler::Instance().RecalPixelPerFrame(nRightWidth, nFrameLength, 1.0f);
}
