#include "stdafx.h"
#include "UITLObjectRightArea.h"
#include "UITLObjectArea.h"
#include "UITLRuler.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif
UITLObjectRightArea::UITLObjectRightArea()
	: m_pObjectArea(nullptr)
{
}

UITLObjectRightArea::~UITLObjectRightArea()
{
}

int UITLObjectRightArea::DrawItemRight(CDC * pDC, CString strID, int nCurrentY)
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

			CImage * pActionBarImage = nullptr;
			CImage * pRightBkImage = nullptr;
			if (bSel)
			{
				pActionBarImage = UIControl_GetImage(ThemeImage_TimeLineObjectActionBar_Sel);
				pRightBkImage = UIControl_GetImage(ThemeImage_TimeLineRightObjectBkGnd_Sel);
			}
			else
			{
				pActionBarImage = UIControl_GetImage(ThemeImage_TimeLineObjectActionBar_Nor);
				pRightBkImage = UIControl_GetImage(ThemeImage_TimeLineRightObjectBkGnd_Nor);
			}

			//绘制底色
			if (pRightBkImage)
			{
				CRect rtRight(nXStart, nYStart, nXStart + m_nWidth, nYStart + nItemHeight);
				rtRight.OffsetRect(0, -m_nScrollY);//底色受Y的影响,不受X的影响
				pRightBkImage->AlphaBlend(pDC->GetSafeHdc(), rtRight.left, rtRight.top, rtRight.Width(), rtRight.Height(), 0, 0, pRightBkImage->GetWidth(), pRightBkImage->GetHeight());
				//将右边矩形记录下来
				pObjDrawerInfo->rtRight = rtRight;
			}

			//绘制bar
			if (pActionBarImage)
			{
				BaseGraph_Common::Value stuValueActionBegin;
				BaseGraph_Common::Value stuValueActionEnd;
				if (DataBase_GetItemProperty(strID, BaseGraph_DataBase::Obj_ActionBegin_Int, stuValueActionBegin)
					&& DataBase_GetItemProperty(strID, BaseGraph_DataBase::Obj_ActionEnd_Int, stuValueActionEnd))
				{
					//属性中取出来的是场,需要换算成帧
					int nObjActionBegin = stuValueActionBegin.ToInt() / 2;
					int nObjActionEnd = stuValueActionEnd.ToInt() / 2;

					//取出当前物件及当前比例尺，画总长条
					int nBarPixelBegin = int(nObjActionBegin * UITLRuler::Instance().GetPixelPerFrame());
					int nBarPixelEnd = int(nObjActionEnd * UITLRuler::Instance().GetPixelPerFrame());

					CRect rtBar(nXStart + nBarPixelBegin, nYStart, nXStart + nBarPixelEnd, nYStart + nItemHeight);
					rtBar.OffsetRect(-m_nScrollX, -m_nScrollY);
					pActionBarImage->AlphaBlend(pDC->GetSafeHdc(), rtBar.left, rtBar.top, rtBar.Width(), rtBar.Height(), 0, 0, pActionBarImage->GetWidth(), pActionBarImage->GetHeight());

					//将bar矩形记录下来
					pObjDrawerInfo->rtActionBar = rtBar;
				}
			}

			int nRet = nCurrentY + nItemHeight;
			//画自己的子
			std::vector<CString> vecSubID;
			if (DataBase_GetSubItem(strID, vecSubID))
			{
				for (size_t i = 0; i < vecSubID.size(); i++)
				{
					CString strSubID = vecSubID[i];
					nRet = DrawItemRight(pDC, strSubID, nCurrentY + nItemHeight);
				}
			}
			return nRet;
		}
	}
	return nCurrentY;
}

bool UITLObjectRightArea::Create(int nX, int nY, int nWidth, int nHeight, int nID, HWND hParentWnd, UITLObjectArea * pParentElement)
{
	if (!m_bCreated)
	{
		Initialize(nX, nY, nWidth, nHeight, nID, hParentWnd, pParentElement);
		m_pObjectArea = pParentElement;
		return true;
	}
	return false;
}

void UITLObjectRightArea::DrawElement(CDC * pDC)
{
	if (m_bVisible)
	{
		if (m_pObjectArea)
		{
			m_pObjectArea->SetVScrollHeight(0, true);
		}

		int nXStart = GetRealX() + m_nScrollX;
		int nYStart = GetRealY() + m_nScrollY;
		CRect rcClient(nXStart, nYStart, nXStart + m_nWidth, nYStart + m_nHeight);

		CRgn rgnDC;
		rgnDC.CreateRectRgnIndirect(rcClient);
		pDC->SelectClipRgn(&rgnDC);

		int nItemHeight = UIControl_GetIntValue(ThemeInt_TimeLineItemActionHeight);
		CImage * pRightAreaBkImage = UIControl_GetImage(ThemeImage_TimeLineObjectRightAreaBk);
		CImage * pActionTotalBarImage = UIControl_GetImage(ThemeImage_TimeLineObjectActionBar_TotalLength);
		CImage * pRightBkImageNor = UIControl_GetImage(ThemeImage_TimeLineRightObjectBkGnd_Nor);

		//绘制背景
		if (pRightAreaBkImage)
		{
			CRect rtBk(nXStart, nYStart, nXStart + m_nWidth, nYStart + m_nHeight);
			pRightAreaBkImage->AlphaBlend(pDC->GetSafeHdc(), rtBk.left, rtBk.top, rtBk.Width(), rtBk.Height(), 0, 0, pRightAreaBkImage->GetWidth(), pRightAreaBkImage->GetHeight());
		}

		//绘制一个总长度条底色
		if (pRightBkImageNor)
		{
			CRect rtRight(nXStart, nYStart, nXStart + m_nWidth, nYStart + nItemHeight);
			rtRight.OffsetRect(0, -m_nScrollY);//底色受Y的影响,不受X的影响
			pRightBkImageNor->AlphaBlend(pDC->GetSafeHdc(), rtRight.left, rtRight.top, rtRight.Width(), rtRight.Height(), 0, 0, pRightBkImageNor->GetWidth(), pRightBkImageNor->GetHeight());
		}

		//绘制总长度条
		if (pActionTotalBarImage)
		{
			//取出当前总长及当前比例尺，画总长条
			int nBarLength = int(UITLRuler::Instance().GetDispFrameLength() * UITLRuler::Instance().GetPixelPerFrame());

			CRect rtBar(nXStart, nYStart, nXStart + nBarLength, nYStart + nItemHeight);
			rtBar.OffsetRect(-m_nScrollX, -m_nScrollY);
			pActionTotalBarImage->AlphaBlend(pDC->GetSafeHdc(), rtBar.left, rtBar.top, rtBar.Width(), rtBar.Height(), 0, 0, pActionTotalBarImage->GetWidth(), pActionTotalBarImage->GetHeight());
		}

		//绘制物件
		int nItemGap = UIControl_GetIntValue(ThemeInt_TimeLineItemActionGap);
		int nCurentY = nYStart + nItemHeight - m_nScrollY + nItemGap;
		std::vector<CString> vecObjID;
		if (DataBase_GetAllItemIDExceptSubItem(vecObjID))
		{
			for (size_t i = 0; i < vecObjID.size(); i++)
			{
				CString strID = vecObjID[i];
				nCurentY = DrawItemRight(pDC, strID, nCurentY) + nItemGap;
			}
		}

		if (m_pObjectArea)
		{
			m_pObjectArea->SetVScrollHeight(nCurentY - nYStart + m_nScrollY + nItemHeight * 2);
		}

		pDC->SelectClipRgn(NULL);
	}
}
