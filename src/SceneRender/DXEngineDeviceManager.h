#pragma once
class DXEngineDeviceManager
{
private:
	int m_nDevice;
	int m_nSceneWidth;
	int m_nSceneHeight;
private:
	DXEngineDeviceManager();
	~DXEngineDeviceManager();
public:
	static DXEngineDeviceManager & Instance();
	int Initialize3D(int nWidth, int nHeight);//¸ø¿í¸ß£¬·µ»Ødevice
	int GetMainDevice();
};

