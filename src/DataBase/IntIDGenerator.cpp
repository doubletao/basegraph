#include "stdafx.h"
#include "IntIDGenerator.h"



#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif
IntIDGenerator::IntIDGenerator()
{
}


IntIDGenerator::~IntIDGenerator()
{
}

IntIDGenerator & IntIDGenerator::Instance()
{
	static IntIDGenerator m_inst;
	return m_inst;
}

bool IntIDGenerator::CheckID(int nID)
{
	if (m_setOccupy.count(nID) > 0)
	{
		return false;
	}
	return true;
}

bool IntIDGenerator::OccupyID(int nID)
{
	if (CheckID(nID))
	{
		m_setOccupy.insert(nID);
		return true;
	}
	return false;
}

int IntIDGenerator::GetNewID()
{
	if (m_setOccupy.size() > 0)
	{
		return *m_setOccupy.rbegin() + 1;
	}
	return 1;
}
