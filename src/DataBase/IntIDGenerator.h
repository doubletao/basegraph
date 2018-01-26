#pragma once
#include <set>
class IntIDGenerator
{
private:
	std::set<int> m_setOccupy;
private:
	IntIDGenerator();
	~IntIDGenerator();
public:
	static IntIDGenerator & Instance();
	bool CheckID(int nID);
	bool OccupyID(int nID);
	int GetNewID();
};
