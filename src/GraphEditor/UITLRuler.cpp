#include "stdafx.h"
#include "UITLRuler.h"


UITLRuler::UITLRuler()
	: m_fPixelPerFrame(1.0f)
{
}


UITLRuler::~UITLRuler()
{
}

inline void UITLRuler::LimitValue()
{
	//边界值限定
	int nTimeLinePixelPerFrameMax = UIControl_GetIntValue(ThemeInt_TimeLinePixelPerFrameMax);
	int nTimeLinePixelPerFrameMin = UIControl_GetIntValue(ThemeInt_TimeLinePixelPerFrameMin);
	if (m_fPixelPerFrame * 100 > nTimeLinePixelPerFrameMax)
	{
		m_fPixelPerFrame = nTimeLinePixelPerFrameMax / 100.0f;
	}
	else if (m_fPixelPerFrame * 100 < nTimeLinePixelPerFrameMin)
	{
		m_fPixelPerFrame = nTimeLinePixelPerFrameMin / 100.0f;
	}
}

UITLRuler & UITLRuler::Instance()
{
	static UITLRuler inst;
	return inst;
}

float UITLRuler::GetPixelPerFrame()
{
	return m_fPixelPerFrame;
}

void UITLRuler::SetPixelPerFrame(float fPixelPerFrame)
{
	m_fPixelPerFrame = fPixelPerFrame;
	//边界值限定
	LimitValue();
}

float UITLRuler::RecalPixelPerFrame(int nWndWidth, int nFrameLength, float fScrollPercent)
{
	//计算原理
	//知道窗口宽度和滚动条百分比，可以求出逻辑像素宽度，将frame填入逻辑像素宽度即可求出每帧对应的像素数
	float fLogicPixelWidth = nWndWidth * 1.0f / fScrollPercent;
	SetPixelPerFrame(fLogicPixelWidth / nFrameLength);

	//返回
	return m_fPixelPerFrame;
}
