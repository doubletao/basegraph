#pragma once

class C3DViewBufferInfo
{
public:
	int m_nEditBuffer;			//操作的绘制，曲线做成时的临时线条之类
	int m_hEditTexture;
	int m_nUIBuffer;			//界面绘制使用，画个button之类
	int m_hUITexture;
	int m_nCtrlBuffer;			//选中框等的绘制
	int m_hCtrlTexture;
	int	m_nWidth;
	int m_nHeight;
public:
	C3DViewBufferInfo();
	~C3DViewBufferInfo();
public:
	bool Initialize(int nScreenWidth, int nScreenHeight);
	bool Release();

	int GetControlMemBuffer();
	int GetControlTexture();

	int GetEditMemBuffer();
	int GetEditTexture();

	int GetUIMemBuffer();
	int GetUITexture();
};