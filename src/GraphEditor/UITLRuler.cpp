#include "stdafx.h"
#include "UITLRuler.h"
#include "GraphEditor.h"


UITLRuler::UITLRuler()
	: m_fPixelPerFrame(1.0f)
	, m_pScrollHor(nullptr)
	, m_pScrollVer(nullptr)
	, m_bMoving(false)
	, m_pTimeLine(nullptr)
{
}


UITLRuler::~UITLRuler()
{
}

inline int UITLRuler::LimitValue()
{
	//边界值限定
	int nTimeLinePixelPerFrameMax = UIControl_GetIntValue(ThemeInt_TimeLinePixelPerFrameMax);
	int nTimeLinePixelPerFrameMin = UIControl_GetIntValue(ThemeInt_TimeLinePixelPerFrameMin);
	if (m_fPixelPerFrame * 100 > nTimeLinePixelPerFrameMax)
	{
		m_fPixelPerFrame = nTimeLinePixelPerFrameMax / 100.0f;
		return 1;
	}
	else if (m_fPixelPerFrame * 100 < nTimeLinePixelPerFrameMin)
	{
		m_fPixelPerFrame = nTimeLinePixelPerFrameMin / 100.0f;
		return -1;
	}
	return 0;
}

void UITLRuler::OnLButtonDown(WPARAM wParam, LPARAM lParam)
{
	if (!HitTest(lParam))
	{
		return;
	}
	if (false)
	{
		CString strDebug;
		strDebug.Format(_T("OnLButtonDown\n"));
		OutputDebugString(strDebug);
	}
	SetElementCapture();
	m_bMoving = true;
	int nFrame = RulerPosToFrame(GET_X_LPARAM(lParam));
	SetFrame(nFrame);
}

void UITLRuler::OnLButtonUp(WPARAM wParam, LPARAM lParam)
{
	if (m_bMoving)
	{
		if (false)
		{
			CString strDebug;
			strDebug.Format(_T("OnLButtonUp\n"));
			OutputDebugString(strDebug);
		}
		int nFrame = RulerPosToFrame(GET_X_LPARAM(lParam));
		SetFrame(nFrame);
	}
	ReleaseElementCapture();
	m_bMoving = false;
}

void UITLRuler::OnMouseMove(WPARAM wParam, LPARAM lParam)
{
	if (m_bMoving)
	{
		int nFrame = RulerPosToFrame(GET_X_LPARAM(lParam));
		SetFrame(nFrame);
	}
}

void UITLRuler::OnMouseWheel(WPARAM wParam, LPARAM lParam)
{
	//mousewheel拿到的坐标需要转化为客户区坐标再进一步判定
	CPoint ptClient(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
	::ScreenToClient(m_hParentWnd, &ptClient);
	if (!HitTest(ptClient))
	{
		return;
	}
	int nDelta = GET_WHEEL_DELTA_WPARAM(wParam);
	if (m_pTimeLine)
	{
		m_pTimeLine->AdjustScrollZoom(nDelta / 10);
	}
}

void UITLRuler::OnFrameChanged(int nFrame)
{
	Refresh();
}

void UITLRuler::SetFrame(int nFrame)
{
	DataRender_SetFrame(nFrame * 2);
	UIControl_BroadcastMsg(WM_MSG_FrameChanged, nFrame, m_nID, this);
}

UITLRuler & UITLRuler::Instance()
{
	static UITLRuler inst;
	return inst;
}

void UITLRuler::SetScrollBar(UIScrollBar * pScrollHor, UIScrollBar * pScrollVer)
{
	m_pScrollHor = pScrollHor;
	m_pScrollVer = pScrollVer;
}

UIScrollBar * UITLRuler::GetHScrollBar()
{
	return m_pScrollHor;
}

UIScrollBar * UITLRuler::GetVScrollBar()
{
	return m_pScrollVer;
}

void UITLRuler::Create(int nX, int nY, int nWidth, int nHeight, int nID, HWND hParentWnd, UITimeLine * pParent)
{
	Initialize(nX, nY, nWidth, nHeight, nID, hParentWnd, pParent);
	m_pTimeLine = pParent;
	m_setMsgFilter.insert(WM_LBUTTONDOWN);
	m_setMsgFilter.insert(WM_LBUTTONUP);
	m_setMsgFilter.insert(WM_MOUSEMOVE);
	m_setMsgFilter.insert(WM_MOUSEWHEEL);
	//注册消息中心
	UIControl_RegistMsgDispatcher(this);
}

void UITLRuler::DrawElement(CDC * pDC)
{
	if (m_bVisible)
	{
		int nXStart = GetRealX() + m_nScrollX;
		int nYStart = GetRealY() + m_nScrollY;
		CRect rcClient(nXStart, nYStart, nXStart + m_nWidth, nYStart + m_nHeight);

		CRgn rgnDC;
		rgnDC.CreateRectRgnIndirect(rcClient);
		pDC->SelectClipRgn(&rgnDC);

		//短线长，长线长，线色，字体，字色，背景图
		int nShortLineLength = UIControl_GetIntValue(ThemeInt_TimeLineRulerShortLineLength);
		int nLongLineLength = UIControl_GetIntValue(ThemeInt_TimeLineRulerLongLineLength);
		COLORREF clLine = UIControl_GetColor(ThemeColor_TimeLineRulerLine);
		LOGFONT lgFont = UIControl_GetFont(ThemeFont_TimeLineRuler);
		COLORREF clText = UIControl_GetColor(ThemeColor_TimeLineRulerText);
		CImage * pBkImage = UIControl_GetImage(ThemeImage_TimeLineRulerBk);

		//绘制背景
		if (pBkImage)
		{
			CRect rtBk(nXStart, nYStart, nXStart + m_nWidth, nYStart + m_nHeight);
			pBkImage->AlphaBlend(pDC->GetSafeHdc(), rtBk.left, rtBk.top, rtBk.Width(), rtBk.Height(), 0, 0, pBkImage->GetWidth(), pBkImage->GetHeight());
		}

		//绘制一个标尺底
		CRect rtBottomLine(nXStart, nYStart + m_nHeight - 1, nXStart + m_nWidth, nYStart + m_nHeight);
		pDC->FillSolidRect(rtBottomLine, clLine);

		//绘制刻度线及刻度文本
		if (m_pScrollHor && m_pScrollHor->GetPagePercent()
			&& m_fPixelPerFrame > 0)
		{
			//要进行绘制，需要先决定每一个刻度表达的帧数
			//为确保每5格可以显示一个5的倍数的文本，每个刻度应该是帧的整数倍
			int nFramePerGap = 1;//初始化为1倍
			//为确保刻度视觉效果，刻度应该有最小像素数和最大像素数
			int nGapMinPixel = UIControl_GetIntValue(ThemeInt_TimeLineRulerGapMinPixel);
			int nGapMaxPixel = UIControl_GetIntValue(ThemeInt_TimeLineRulerGapMaxPixel);
			//于是得出【公式】：每刻度表达的帧数乘以每帧像素数得出的每刻度像素数应当介于刻度最小像素数和最大像素数之间
			//由【公式】反推出每刻度表达帧数的最大值及最小值
			int nFramePerGapMin = (int)ceil(nGapMinPixel / m_fPixelPerFrame);
			int nFramePerGapMax = (int)ceil(nGapMaxPixel / m_fPixelPerFrame);
			if (nFramePerGapMin <= nFramePerGapMax)
			{
				//理论上区间[nFramePerGapMin, nFramePerGapMax]内的值都可以用，为了好看，我们干脆取个中间值
				nFramePerGap = (nFramePerGapMin + nFramePerGapMax) / 2;
			}
			else
			{
				//此时找不到合适的刻度，应当是比例尺失调的情况，此时以初始值为结果，画个夸张的效果出来
			}
			//求出刻度的总数，刻度总数为frame总数/每刻度帧数
			int nWholeFrameCount = (int)ceil(m_nWidth / m_pScrollHor->GetPagePercent() / m_fPixelPerFrame);
			int nGapCount = (int)ceil(nWholeFrameCount * 1.0f / nFramePerGap);
			//找到了刻度，剩下的就是画了
			for (int i = 0; i < nGapCount; i++)
			{
				//当前刻度的帧数
				int nFrame = i * nFramePerGap;
				//横向的X偏移
				int nXLoop = int(nFrame * m_fPixelPerFrame);
				if (i % 5 == 0)
				{
					//如果刻度是5的倍数，则画个长线画个刻度文本
					CRect rtLine(nXStart + nXLoop, nYStart + m_nHeight - nLongLineLength, nXStart + nXLoop + 1, nYStart + m_nHeight);
					rtLine.OffsetRect(-m_nScrollX, -m_nScrollY);
					pDC->FillSolidRect(rtLine, clLine);
					//画文本
					CString strText = CommonFunction::ConverFrameToText(nFrame);
					CFont font;
					font.CreateFontIndirect(&lgFont);
					CFont * pOldFont = pDC->SelectObject(&font);
					pDC->SetTextColor(clText);
					int nOldBkMode = pDC->SetBkMode(TRANSPARENT);
					CSize sizText = pDC->GetTextExtent(strText);
					CRect rtText(nXStart + nXLoop, nYStart, nXStart + nXLoop + sizText.cx, nYStart + m_nHeight - nLongLineLength);
					rtText.OffsetRect(-m_nScrollX, -m_nScrollY);
					pDC->DrawText(strText, rtText, DT_SINGLELINE | DT_LEFT | DT_VCENTER);
					pDC->SelectObject(pOldFont);
					pDC->SetBkMode(nOldBkMode);
				}
				else
				{
					//否则画个短线
					CRect rtLine(nXStart + nXLoop, nYStart + m_nHeight - nShortLineLength, nXStart + nXLoop + 1, nYStart + m_nHeight);
					rtLine.OffsetRect(-m_nScrollX, -m_nScrollY);
					pDC->FillSolidRect(rtLine, clLine);
				}
			}
		}

		pDC->SelectClipRgn(NULL);
	}
}

EmUIElementMsgRet UITLRuler::HandleMsg(UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	if (m_setMsgFilter.count(nMsg) > 0)
	{
		switch (nMsg)
		{
		case WM_LBUTTONDOWN:
			OnLButtonDown(wParam, lParam);
			break;
		case WM_LBUTTONUP:
			OnLButtonUp(wParam, lParam);
			break;
		case WM_MOUSEMOVE:
			OnMouseMove(wParam, lParam);
			break;
		case WM_MOUSEWHEEL:
			OnMouseWheel(wParam, lParam);
			break;
		default:
			break;
		}
		return UIElementMsgSuccBreak;
	}
	return UIElementMsgSuccContinue;
}

void UITLRuler::OnDispatchMsg(int nMsgID, WPARAM wParam, LPARAM lParam, int nIDFrom)
{
	switch (nMsgID)
	{
	case WM_MSG_FrameChanged:
		if (nIDFrom != GetMsgDispatchID())
		{
			OnFrameChanged((int)wParam);
		}
		break;
	default:
		break;
	}
}

int UITLRuler::GetDispFrameLength()
{
	if (CGraphEditorApp::GetBaseGraphFrameWnd())
	{
		return CGraphEditorApp::GetBaseGraphFrameWnd()->GetWholeFrameLength();
	}
	return 5;
}

float UITLRuler::GetPixelPerFrame()
{
	return m_fPixelPerFrame;
}

int UITLRuler::SetPixelPerFrame(float fPixelPerFrame)
{
	m_fPixelPerFrame = fPixelPerFrame;
	//边界值限定
	return LimitValue();
}

float UITLRuler::RecalPixelPerFrame(int nWndWidth, int nFrameLength, float fScrollPercent)
{
	//计算原理
	//知道窗口宽度和滚动条百分比，可以求出逻辑像素宽度，将frame填入逻辑像素宽度即可求出每帧对应的像素数
	float fLogicPixelWidth = nWndWidth * 1.0f / fScrollPercent;
	int nReachLimit = SetPixelPerFrame(fLogicPixelWidth / nFrameLength);
	if (nReachLimit != 0)
	{
		//到达边界,则返回边界限制值
		fScrollPercent = nWndWidth / m_fPixelPerFrame / nFrameLength;
	}
	return fScrollPercent;
}

int UITLRuler::RulerPosToFrame(int nX)
{
	int nLogicX = nX - GetRealX();
	if (nLogicX < 0)
	{
		nLogicX = 0;
	}
	if (nLogicX > m_nLogicWidth)
	{
		nLogicX = m_nLogicWidth;
	}
	int nFrame = int(nLogicX / m_fPixelPerFrame);
	return nFrame;
}

int UITLRuler::FrameToRulerPos(int nFrame)
{
	int nPos = 0;
	if (fabs(m_fPixelPerFrame) > 0.001)
	{
		int nLogicX = int(nFrame * m_fPixelPerFrame);
		if (nLogicX < 0)
		{
			nLogicX = 0;
		}
		if (nLogicX > m_nLogicWidth)
		{
			nLogicX = m_nLogicWidth;
		}
		nPos = nLogicX + GetRealX();
	}
	return nPos;
}

UITimeLine * UITLRuler::GetTimeLine()
{
	return m_pTimeLine;
}
