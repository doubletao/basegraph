#include "stdafx.h"
#include "UITLObjectItem.h"

UITLObjectItem::UITLObjectItem()
	: m_bExpand(false)
	, m_strObjID(_T(""))
{
}

UITLObjectItem::~UITLObjectItem()
{
}

bool UITLObjectItem::GetExpand()
{
	return m_bExpand;
}

void UITLObjectItem::SetExpand(bool bExpand)
{
	m_bExpand = bExpand;
}

CString UITLObjectItem::GetObjID()
{
	return m_strObjID;
}

void UITLObjectItem::SetObjID(CString strID)
{
	m_strObjID = strID;
}
