#include "stdafx.h"
#include "ExportCommon.h"

#define FLOAT_EQUAL(f1,f2)		(fabs((f1) - (f2)) < FLOAT_0)
#define POINTF_EQUAL(p1,p2)		(FLOAT_EQUAL((p1).x,(p2).x) && FLOAT_EQUAL((p1).y,(p2).y))

/*
注意到如果从P作水平向左的射线的话，如果P在多边形内部，那么这条射线与多边形的交点必为奇数，如果P在多边形外部，则交点个数必为偶数（0也在内）。所以，我们可以顺序考虑多边形的每条边，求出交点的总个数。还有一些特殊情况要考虑。假如考虑边(P1,P2)，
1)如果射线正好穿过P1或者P2,那么这个交点会被算作2次，处理办法是如果P的从坐标与P1,P2中较小的纵坐标相同，则直接忽略这种情况
2)如果射线水平，则射线要么与其无交点，要么有无数个，这种情况也直接忽略。
3)如果射线竖直，而P0的横坐标小于P1,P2的横坐标，则必然相交。
4)再判断相交之前，先判断P是否在边(P1,P2)的上面，如果在，则直接得出结论：P再多边形内部。
*/
BASEGRAPH_COMMON_API bool Common_IsPointInPolygon(IN pointf ptHit, IN std::vector<pointf>& vecPolygon)
{
	// 交点个数  
	int nCross = 0;
	size_t nCount = vecPolygon.size();
	for (int i = 0; i < nCount; i++)
	{
		pointf p1 = vecPolygon[i];
		pointf p2 = vecPolygon[(i + 1) % nCount];// 最后一个点与第一个点连线  
		if (p1.y == p2.y)
			continue;
		if (ptHit.y < min(p1.y, p2.y))
			continue;
		if (ptHit.y >= max(p1.y, p2.y))
			continue;
		// 求交点的x坐标  
		float x = (ptHit.y - p1.y) * (p2.x - p1.x) / (p2.y - p1.y) + p1.x;
		// 只统计p1p2与p向右射线的交点  
		if (x > ptHit.x)
		{
			nCross++;
		}
	}
	// 交点为偶数，点在多边形之外  
	return (nCross % 2 == 1);
}

//叉积
float mult(pointf a, pointf b, pointf c)
{
	return (a.x - c.x)*(b.y - c.y) - (b.x - c.x)*(a.y - c.y);
}
BASEGRAPH_COMMON_API bool Common_IsCrossLine(IN pointf pt11, IN pointf pt12, IN pointf pt21, IN pointf pt22)
{
	if (max(pt11.x, pt12.x) < min(pt21.x, pt22.x))
	{
		return false;
	}
	if (max(pt11.y, pt12.y) < min(pt21.y, pt22.y))
	{
		return false;
	}
	if (max(pt21.x, pt22.x) < min(pt11.x, pt12.x))
	{
		return false;
	}
	if (max(pt21.y, pt22.y) < min(pt11.y, pt12.y))
	{
		return false;
	}
	if (mult(pt21, pt12, pt11) * mult(pt12, pt22, pt11) < 0)
	{
		return false;
	}
	if (mult(pt11, pt22, pt21) * mult(pt22, pt12, pt21) < 0)
	{
		return false;
	}
	return true;
}
