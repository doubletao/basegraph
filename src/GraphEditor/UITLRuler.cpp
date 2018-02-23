#include "stdafx.h"
#include "UITLRuler.h"


UITLRuler::UITLRuler()
	: m_fPixelPerFrame(1.0f)
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

UITLRuler & UITLRuler::Instance()
{
	static UITLRuler inst;
	return inst;
}

int UITLRuler::GetDispFrameLength()
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
