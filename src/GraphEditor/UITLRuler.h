#pragma once
/*
 * 杜江涛 2018年2月23日
 * 设计尺子是一个静态类目的是方便各模块绘制时获取当前比例尺
 * 同时尺子会根据当前的比例尺绘制一个刻度在时间线中
*/

class UITLRuler :
	public UIScrollElement
{
private:
	UITLRuler();
	virtual ~UITLRuler();
protected:
	//尺子上保存这两个滚动条的指针目的为方便全局操作滚动条
	UIScrollBar * m_pScrollHor;
	UIScrollBar * m_pScrollVer;
	float m_fPixelPerFrame;//每帧对应的像素数（放大一百倍取整）
	inline int LimitValue();
public:
	static UITLRuler & Instance();
	void SetScrollBar(UIScrollBar * pScrollHor, UIScrollBar * pScrollVer);
	UIScrollBar * GetHScrollBar();
	UIScrollBar * GetVScrollBar();
	void Create(int nX, int nY, int nWidth, int nHeight, int nID, HWND hParentWnd, UIElement * pParent);
	virtual void DrawElement(CDC * pDC);
	int GetDispFrameLength();
	float GetPixelPerFrame();
	int SetPixelPerFrame(float fPixelPerFrame);
	float RecalPixelPerFrame(int nWndWidth, int nFrameLength, float fScrollPercent);//重新计算比例尺需要知道当前窗口宽度和当前总帧长以及当前滚动条占整体的百分比
};
