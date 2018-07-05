#include "stdafx.h"
#include "UIControl\UIDrawer.h"



#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif
UIDrawer::UIDrawer(int nResID/* = 0*/)
	: m_nResID(nResID)
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
