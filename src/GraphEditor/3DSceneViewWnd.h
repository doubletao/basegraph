#pragma once
#include "C3DViewInfo.h"
#include "C3DViewBufferInfo.h"
/*
* 回显窗视图子窗口，分布在回显窗视图主窗口中
*/

class C3DSceneViewWnd : public CWnd
{
	DECLARE_DYNCREATE(C3DSceneViewWnd)
	DECLARE_MESSAGE_MAP()
protected:
	C3DViewInfo m_ViewInfo;
	C3DViewBufferInfo m_BufferInfo;
protected:
	//渲染触发线程管理相关
	int m_nRenderCount;//待渲染次数
	CRITICAL_SECTION m_lockRendCount;//待渲染次数锁
	HANDLE	m_thPreDecoderThread;//渲染线程
	HANDLE  m_hExitPreDecoderEvent;//渲染线程退出事件
	HANDLE  m_hSeekReadyEvent;//单次渲染进行中

protected:
	void RenderCountLock();
	void RenderCountUnlock();

	void RecalcLayout();
	CPoint SceneToWnd(CPoint ptScene);
	CPoint WndToScene(CPoint ptWnd);

	afx_msg LRESULT OnSetRenderTask(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT AddOneRenderTask(WPARAM, LPARAM);
	afx_msg void OnPaint();
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	static DWORD WINAPI PreDecoderThread(LPVOID pParam);//解码线程
public:
	C3DSceneViewWnd();
	~C3DSceneViewWnd();
public:
	void AdjustViewFit();
	void SetZoomScale(float fScale, CPoint ptCenter);
	float GetZoomScale();
	void ResetScaledViewInfo(CPoint ptCenter);
};