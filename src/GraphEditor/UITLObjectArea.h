#pragma once
#include "UITLObjectItem.h"

class UITLObjectArea :
	public UIScrollElement
{
public:
	UITLObjectArea();
	virtual ~UITLObjectArea();
protected:
	std::map<CString, UITLObjectItem *> m_mapObjDrawerInfo;//绘制信息的集合
protected:
	UITLObjectItem * CreateObjectItem(CString strID);
	int GetTotalLength();
public:
	virtual void DrawElement(CDC * pDC);
	bool Create(int nX, int nY, int nWidth, int nHeight, int nID, HWND hParentWnd, UIElement * pParentElement);

	void AdjustScrollZoom();//按比例缩放frame总长使得横向滚动条满屏
};
