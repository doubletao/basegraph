#pragma once
class UIDrawer
{
private:
	int m_nResID;
public:
	UIDrawer(int nResID = 0);
	virtual ~UIDrawer();
public:
	virtual void Draw(CDC * pDC, int nX, int nY, int nWidth, int nHeight, LPCTSTR strText = nullptr) = 0;
	void SetResID(int nID);
	int GetResID();
};

