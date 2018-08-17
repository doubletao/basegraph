#pragma once

class UITLObjectItem
{
public:
	CString m_strObjID;
	bool m_bHasTrack;//是否有轨道
	bool m_bExpandTrack;//轨道是否展开
	bool m_bHasSub;//是否有子物件
	bool m_bExpandSub;//子物件是否展开
	CRect rtLeft;
	CRect rtRight;
	CRect rtActionBar;
public:
	UITLObjectItem();
	~UITLObjectItem();
};
