#pragma once
#include <afxmt.h>
#include "GraphEditorCommonDef.h"

class UIPlayControlBar :
	public UICheckBtnBar
{
public:
	UIPlayControlBar();
	virtual ~UIPlayControlBar();
protected:
	//播放线程相关
	CWinThread * m_pThreadPlay;//线程
	CEvent m_evtPlayQuit;//停止标记
	CEvent m_evtLoop;//循环标记
	UICheckBtn * m_pBtnPlay;
	UICheckBtn * m_pBtnLoop;
	static UINT PlayThread(LPVOID pParam);//播放线程
	void DoPlay();//播放执行函数
public:
	void StartPlay();//启动线程
	void StopPlay();//结束线程
protected:
	virtual void InitializeObject();
	virtual void RearrangeChildPos();
	virtual bool OnChildMsg(EmUIElementMsg emMsg, WPARAM wParam, LPARAM lParam);
	bool IsLoop();
	void SetLoop(bool bLoop);
public:
	void OnBtnClickedFirstFrame();
	void OnBtnClickedPreFrame();
	void OnBtnClickedPlayPause();
	void OnBtnClickedNextFrame();
	void OnBtnClickedLastFrame();
	void OnBtnClickedLoopPlay();
	bool IsPlaying();//正在播放
};

