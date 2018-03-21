#pragma once
#include "C3DViewInfo.h"
#include "C3DViewBufferInfo.h"
/*
* 回显窗视图子窗口，分布在回显窗视图主窗口中
*/

class C3DSceneViewWnd 
	: public CWnd
{
	DECLARE_DYNCREATE(C3DSceneViewWnd)
	DECLARE_MESSAGE_MAP()
protected:
	C3DViewInfo m_ViewInfo;
	C3DViewBufferInfo m_BufferInfo;
	int m_nSceneOutBackTex;//屏幕外的半透区域
	float m_fSceneOutBackTexAlpha;
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
	point2f SceneToWnd(point2f ptScene);
	point2f WndToScene(point2f ptWnd);
	point2f SceneToWnd(CPoint ptScene);
	point2f WndToScene(CPoint ptWnd);

	void RenderVirtualZone(int nDevice, const CRect& rtClient);//绘制周边区域（屏幕外的黑边）
	void RenderTripleBuffer(int nDevice);//绘制三层buf，安全框、选中框，物件选中框，编辑内容
	void RenderCtrlBuffer(int nDevice);//画安全框和选中框
	void RenderUIBuffer(int nDevice);//画界面元素
	BOOL UpdateBufferToHal(int hDevice, int hDesHal, RECT rtDirty, int n2DBufferID);
	void DrawEditBackBuffer(int hDevice, const CRect& rtInValid, const CRect& rtValid, int nRTTex, int hTex, float fAlpha);

	afx_msg LRESULT OnSetRenderTask(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT AddOneRenderTask(WPARAM, LPARAM);
	afx_msg void OnPaint();
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	static DWORD WINAPI PreDecoderThread(LPVOID pParam);
	void RenderAllItem(int hDevice, int hTex, int hDepth, float fScale, RECT &viewPort);
	//解码线程
public:
	C3DSceneViewWnd();
	~C3DSceneViewWnd();
public:
	void AdjustViewFit();
	void AdjustViewReal();
	void SetZoomScale(float fScale, point2f ptFixed);
	float GetZoomScale();
	void ResetScaledViewInfo(point2f ptFixedScene, point2f ptFixedWnd);
	void ResetViewInfo(float fScale);
};