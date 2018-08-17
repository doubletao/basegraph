#pragma once

class C3DViewInfo
{
public:
	int m_nSceneWidth;
	int m_nSceneHeight;
	
	int m_nWndWidth;
	int m_nWndHeight;

	int m_nSceneOffsetX;
	int m_nSceneOffsetY;

	float m_fScale;

	int m_hSwapChain;
public:
	C3DViewInfo();
	~C3DViewInfo();
};
