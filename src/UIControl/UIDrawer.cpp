#include "stdafx.h"
#include "UIDrawer.h"


UIDrawer::UIDrawer(int nResID/* = 0*/)
	: m_nResID(0)
{
}

UIDrawer::~UIDrawer()
{
}

void UIDrawer::SetResID(int nID)
{
	m_nResID = nID;
}

int UIDrawer::GetResID()
{
	return m_nResID;
}
