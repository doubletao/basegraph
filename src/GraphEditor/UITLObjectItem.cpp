#include "stdafx.h"
#include "UITLObjectItem.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif
UITLObjectItem::UITLObjectItem()
	: m_strObjID(_T(""))
	, m_bHasTrack(false)//是否有轨道
	, m_bExpandTrack(false)//轨道是否展开
	, m_bHasSub(false)//是否有子物件
	, m_bExpandSub(false)//子物件是否展开
	, rtLeft(0)
	, rtRight(0)
	, rtActionBar(0)
{
}

UITLObjectItem::~UITLObjectItem()
{
}
