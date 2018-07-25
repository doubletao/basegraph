#include "stdafx.h"
#include "DataBase\DataTypeDefDetail.h"
#include "DataBase\DataTypeDef.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif
using namespace BaseGraph_DataBase;

BaseGraph_DataBase::artinfo::artinfo()
{
	Reset();
}

void BaseGraph_DataBase::artinfo::Reset()
{
	type = ARTNULL;
	cv_height = 0;
	cu_angle = 0;
	soft = 0;
	color = 0;
	metal.Reset();
}

int BaseGraph_DataBase::artinfo::GetSerializeSize() const
{
	int nSize = 0;
	SERIALIZE_PARAM_GET_SIZE(nSize, type);		//效果类型
	SERIALIZE_PARAM_GET_SIZE(nSize, cv_height);   //凹凸高度       
	SERIALIZE_PARAM_GET_SIZE(nSize, cu_angle);	//凹凸角度
	SERIALIZE_PARAM_GET_SIZE(nSize, soft);		//虚边宽
	SERIALIZE_PARAM_GET_SIZE(nSize, color);		//凹凸单色,不透明
	SERIALIZE_PARAM_GET_SIZE_STU(nSize, metal);		//立体边
	return nSize;
}

bool BaseGraph_DataBase::artinfo::GetSerializeBuf(byte * pBuf, int nBufSize) const
{
	int nSize = GetSerializeSize();
	if (nSize > nBufSize)
	{
		//给的buf不对
		assert(false);
		return false;
	}
	nSize = 0;
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, type);		//效果类型
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, cv_height);   //凹凸高度       
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, cu_angle);	//凹凸角度
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, soft);		//虚边宽
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, color);		//凹凸单色,不透明
	SERIALIZE_PARAM_TO_BUF_STU(pBuf, nSize, metal);		//立体边
	return true;
}

bool BaseGraph_DataBase::artinfo::RecoverFromBuf(byte * pBuf, int nBufSize)
{
	int nSize = GetSerializeSize();
	if (nSize > nBufSize)
	{
		//给的buf不对
		assert(false);
		return false;
	}
	nSize = 0;
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, type);		//效果类型
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, cv_height);   //凹凸高度       
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, cu_angle);	//凹凸角度
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, soft);		//虚边宽
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, color);		//凹凸单色,不透明
	SERIALIZE_BUF_TO_PARAM_STU(pBuf, nSize, metal);		//立体边
	return true;
}

BaseGraph_DataBase::faceinfo::faceinfo()
{
	Reset();
}

void BaseGraph_DataBase::faceinfo::Reset()
{
	disable = 0;
	soft = 0;
	effect_type = 0;
	effect_width = 0;
	avcolor = 0;
	art.Reset();
	color.Reset();
}

int BaseGraph_DataBase::faceinfo::GetSerializeSize() const
{
	int nSize = 0;
	SERIALIZE_PARAM_GET_SIZE(nSize, disable);
	SERIALIZE_PARAM_GET_SIZE(nSize, soft);
	SERIALIZE_PARAM_GET_SIZE(nSize, effect_type);
	SERIALIZE_PARAM_GET_SIZE(nSize, effect_width);
	SERIALIZE_PARAM_GET_SIZE(nSize, avcolor);
	SERIALIZE_PARAM_GET_SIZE_STU(nSize, art);
	SERIALIZE_PARAM_GET_SIZE_STU(nSize, color);
	return nSize;
}

bool BaseGraph_DataBase::faceinfo::GetSerializeBuf(byte * pBuf, int nBufSize) const
{
	int nSize = GetSerializeSize();
	if (nSize > nBufSize)
	{
		//给的buf不对
		assert(false);
		return false;
	}
	nSize = 0;
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, disable);
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, soft);
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, effect_type);
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, effect_width);
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, avcolor);
	SERIALIZE_PARAM_TO_BUF_STU(pBuf, nSize, art);
	SERIALIZE_PARAM_TO_BUF_STU(pBuf, nSize, color);
	return true;
}

bool BaseGraph_DataBase::faceinfo::RecoverFromBuf(byte * pBuf, int nBufSize)
{
	int nSize = GetSerializeSize();
	if (nSize > nBufSize)
	{
		//给的buf不对
		assert(false);
		return false;
	}
	nSize = 0;
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, disable);
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, soft);
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, effect_type);
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, effect_width);
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, avcolor);
	SERIALIZE_BUF_TO_PARAM_STU(pBuf, nSize, art);
	SERIALIZE_BUF_TO_PARAM_STU(pBuf, nSize, color);
	return true;
}

BaseGraph_DataBase::borderinfo::borderinfo()
{
	Reset();
}

void BaseGraph_DataBase::borderinfo::Reset()
{
	disable = 1;
	soft = 0;
	linestyle = 0;
	width = 0;
	interval = 0;//点
	effect_type = 0;
	effect_width = 0;
	avcolor = 0;
	art.Reset();
	color.Reset();
}

int BaseGraph_DataBase::borderinfo::GetSerializeSize() const
{
	int nSize = 0;
	SERIALIZE_PARAM_GET_SIZE(nSize, disable);
	SERIALIZE_PARAM_GET_SIZE(nSize, soft);
	SERIALIZE_PARAM_GET_SIZE(nSize, linestyle);
	SERIALIZE_PARAM_GET_SIZE(nSize, width);
	SERIALIZE_PARAM_GET_SIZE(nSize, interval);//点线的间距
	SERIALIZE_PARAM_GET_SIZE(nSize, effect_type);
	SERIALIZE_PARAM_GET_SIZE(nSize, effect_width);
	SERIALIZE_PARAM_GET_SIZE(nSize, avcolor);
	SERIALIZE_PARAM_GET_SIZE_STU(nSize, art);
	SERIALIZE_PARAM_GET_SIZE_STU(nSize, color);
	return nSize;
}

bool BaseGraph_DataBase::borderinfo::GetSerializeBuf(byte * pBuf, int nBufSize) const
{
	int nSize = GetSerializeSize();
	if (nSize > nBufSize)
	{
		//给的buf不对
		assert(false);
		return false;
	}
	nSize = 0;
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, disable);
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, soft);
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, linestyle);
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, width);
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, interval);//点线的间距
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, effect_type);
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, effect_width);
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, avcolor);
	SERIALIZE_PARAM_TO_BUF_STU(pBuf, nSize, art);
	SERIALIZE_PARAM_TO_BUF_STU(pBuf, nSize, color);
	return true;
}

bool BaseGraph_DataBase::borderinfo::RecoverFromBuf(byte * pBuf, int nBufSize)
{
	int nSize = GetSerializeSize();
	if (nSize > nBufSize)
	{
		//给的buf不对
		assert(false);
		return false;
	}
	nSize = 0;
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, disable);
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, soft);
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, linestyle);
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, width);
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, interval);//点线的间距
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, effect_type);
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, effect_width);
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, avcolor);
	SERIALIZE_BUF_TO_PARAM_STU(pBuf, nSize, art);
	SERIALIZE_BUF_TO_PARAM_STU(pBuf, nSize, color);
	return true;
}

BaseGraph_DataBase::shadowinfo::shadowinfo()
{
	Reset();
}

void BaseGraph_DataBase::shadowinfo::Reset()
{
	disable = 1;
	soft = 0;
	offsetx = 0;
	offsety = 0;
	effect_type = 0;
	effect_fxtitle = 0;
	effect_fyscale = 0;
	effect_fadeout = 0;
	effect_style = 0;
	avcolor = 0;
	art.Reset();
	color.Reset();
}

int BaseGraph_DataBase::shadowinfo::GetSerializeSize() const
{
	int nSize = 0;
	SERIALIZE_PARAM_GET_SIZE(nSize, disable);
	SERIALIZE_PARAM_GET_SIZE(nSize, soft);
	SERIALIZE_PARAM_GET_SIZE(nSize, offsetx);
	SERIALIZE_PARAM_GET_SIZE(nSize, offsety);
	SERIALIZE_PARAM_GET_SIZE(nSize, effect_type);
	SERIALIZE_PARAM_GET_SIZE(nSize, effect_fxtitle);
	SERIALIZE_PARAM_GET_SIZE(nSize, effect_fyscale);
	SERIALIZE_PARAM_GET_SIZE(nSize, effect_fadeout);
	SERIALIZE_PARAM_GET_SIZE(nSize, effect_style);
	SERIALIZE_PARAM_GET_SIZE(nSize, avcolor);
	SERIALIZE_PARAM_GET_SIZE_STU(nSize, art);
	SERIALIZE_PARAM_GET_SIZE_STU(nSize, color);
	return nSize;
}

bool BaseGraph_DataBase::shadowinfo::GetSerializeBuf(byte * pBuf, int nBufSize) const
{
	int nSize = GetSerializeSize();
	if (nSize > nBufSize)
	{
		//给的buf不对
		assert(false);
		return false;
	}
	nSize = 0;
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, disable);
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, soft);
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, offsetx);
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, offsety);
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, effect_type);
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, effect_fxtitle);
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, effect_fyscale);
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, effect_fadeout);
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, effect_style);
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, avcolor);
	SERIALIZE_PARAM_TO_BUF_STU(pBuf, nSize, art);
	SERIALIZE_PARAM_TO_BUF_STU(pBuf, nSize, color);
	return true;
}

bool BaseGraph_DataBase::shadowinfo::RecoverFromBuf(byte * pBuf, int nBufSize)
{
	int nSize = GetSerializeSize();
	if (nSize > nBufSize)
	{
		//给的buf不对
		assert(false);
		return false;
	}
	nSize = 0;
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, disable);
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, soft);
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, offsetx);
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, offsety);
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, effect_type);
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, effect_fxtitle);
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, effect_fyscale);
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, effect_fadeout);
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, effect_style);
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, avcolor);
	SERIALIZE_BUF_TO_PARAM_STU(pBuf, nSize, art);
	SERIALIZE_BUF_TO_PARAM_STU(pBuf, nSize, color);
	return true;
}

BaseGraph_DataBase::cubeinfo::cubeinfo()
{
	Reset();
}

void BaseGraph_DataBase::cubeinfo::Reset()
{
	disable = 1;
	width = 0;
	angle = 0;
	effect_type = 0;
	effect_width = 0;
	avcolor = 0;
	art.Reset();
	color.Reset();
}

int BaseGraph_DataBase::cubeinfo::GetSerializeSize() const
{
	int nSize = 0;
	SERIALIZE_PARAM_GET_SIZE(nSize, disable);
	SERIALIZE_PARAM_GET_SIZE(nSize, width);
	SERIALIZE_PARAM_GET_SIZE(nSize, angle);
	SERIALIZE_PARAM_GET_SIZE(nSize, effect_type);
	SERIALIZE_PARAM_GET_SIZE(nSize, effect_width);
	SERIALIZE_PARAM_GET_SIZE(nSize, avcolor);
	SERIALIZE_PARAM_GET_SIZE_STU(nSize, art);
	SERIALIZE_PARAM_GET_SIZE_STU(nSize, color);
	return nSize;
}

bool BaseGraph_DataBase::cubeinfo::GetSerializeBuf(byte * pBuf, int nBufSize) const
{
	int nSize = GetSerializeSize();
	if (nSize > nBufSize)
	{
		//给的buf不对
		assert(false);
		return false;
	}
	nSize = 0;
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, disable);
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, width);
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, angle);
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, effect_type);
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, effect_width);
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, avcolor);
	SERIALIZE_PARAM_TO_BUF_STU(pBuf, nSize, art);
	SERIALIZE_PARAM_TO_BUF_STU(pBuf, nSize, color);
	return true;
}

bool BaseGraph_DataBase::cubeinfo::RecoverFromBuf(byte * pBuf, int nBufSize)
{
	int nSize = GetSerializeSize();
	if (nSize > nBufSize)
	{
		//给的buf不对
		assert(false);
		return false;
	}
	nSize = 0;
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, disable);
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, width);
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, angle);
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, effect_type);
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, effect_width);
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, avcolor);
	SERIALIZE_BUF_TO_PARAM_STU(pBuf, nSize, art);
	SERIALIZE_BUF_TO_PARAM_STU(pBuf, nSize, color);
	return true;
}

BaseGraph_DataBase::lightinfo::lightinfo()
{
	Reset();
}

void BaseGraph_DataBase::lightinfo::Reset()
{
	ambience = 0;
	ambience_degree = 0;
	reflectcolor = 0;
	dwReverse1 = 0;
	dwReverse2 = 0;
	memset(szPath, 0, sizeof(szPath));
}

int BaseGraph_DataBase::lightinfo::GetSerializeSize() const
{
	int nSize = 0;
	SERIALIZE_PARAM_GET_SIZE(nSize, ambience);	//环镜单色,不透明
	SERIALIZE_PARAM_GET_SIZE(nSize, ambience_degree);  //明亮程度1-100
	SERIALIZE_PARAM_GET_SIZE(nSize, reflectcolor);		//反射单色,不透明
	SERIALIZE_PARAM_GET_SIZE(nSize, dwReverse1);
	SERIALIZE_PARAM_GET_SIZE(nSize, dwReverse2);
	SERIALIZE_PARAM_GET_SIZE(nSize, szPath);
	return nSize;
}

bool BaseGraph_DataBase::lightinfo::GetSerializeBuf(byte * pBuf, int nBufSize) const
{
	int nSize = GetSerializeSize();
	if (nSize > nBufSize)
	{
		//给的buf不对
		assert(false);
		return false;
	}
	nSize = 0;
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, ambience);	//环镜单色,不透明
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, ambience_degree);  //明亮程度1-100
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, reflectcolor);		//反射单色,不透明
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, dwReverse1);
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, dwReverse2);
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, szPath);
	return true;
}

bool BaseGraph_DataBase::lightinfo::RecoverFromBuf(byte * pBuf, int nBufSize)
{
	int nSize = GetSerializeSize();
	if (nSize > nBufSize)
	{
		//给的buf不对
		assert(false);
		return false;
	}
	nSize = 0;
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, ambience);	//环镜单色,不透明
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, ambience_degree);  //明亮程度1-100
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, reflectcolor);		//反射单色,不透明
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, dwReverse1);
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, dwReverse2);
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, szPath);
	return true;
}

BaseGraph_DataBase::channelinfo::channelinfo()
{
	Reset();
}

void BaseGraph_DataBase::channelinfo::Reset()
{
	memset(szPath, 0, sizeof(szPath));
	weight = 0;
	channelid = 0;
	dwReverse1 = 0;
	dwReverse2 = 0;
}

int BaseGraph_DataBase::channelinfo::GetSerializeSize() const
{
	int nSize = 0;
	SERIALIZE_PARAM_GET_SIZE(nSize, szPath);
	SERIALIZE_PARAM_GET_SIZE(nSize, weight);
	SERIALIZE_PARAM_GET_SIZE(nSize, channelid);
	SERIALIZE_PARAM_GET_SIZE(nSize, dwReverse1);
	SERIALIZE_PARAM_GET_SIZE(nSize, dwReverse2);
	return nSize;
}

bool BaseGraph_DataBase::channelinfo::GetSerializeBuf(byte * pBuf, int nBufSize) const
{
	int nSize = GetSerializeSize();
	if (nSize > nBufSize)
	{
		//给的buf不对
		assert(false);
		return false;
	}
	nSize = 0;
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, szPath);
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, weight);
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, channelid);
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, dwReverse1);
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, dwReverse2);
	return true;
}

bool BaseGraph_DataBase::channelinfo::RecoverFromBuf(byte * pBuf, int nBufSize)
{
	int nSize = GetSerializeSize();
	if (nSize > nBufSize)
	{
		//给的buf不对
		assert(false);
		return false;
	}
	nSize = 0;
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, szPath);
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, weight);
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, channelid);
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, dwReverse1);
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, dwReverse2);
	return true;
}

BaseGraph_DataBase::materialinfo::materialinfo()
{
	Reset();
}

void BaseGraph_DataBase::materialinfo::Reset()
{
	memset(szPath, 0, sizeof(szPath));
	dwReverse1 = 0;
	dwReverse2 = 0;
}

int BaseGraph_DataBase::materialinfo::GetSerializeSize() const
{
	int nSize = 0;
	SERIALIZE_PARAM_GET_SIZE(nSize, szPath);
	SERIALIZE_PARAM_GET_SIZE(nSize, dwReverse1);
	SERIALIZE_PARAM_GET_SIZE(nSize, dwReverse2);
	return nSize;
}

bool BaseGraph_DataBase::materialinfo::GetSerializeBuf(byte * pBuf, int nBufSize) const
{
	int nSize = GetSerializeSize();
	if (nSize > nBufSize)
	{
		//给的buf不对
		assert(false);
		return false;
	}
	nSize = 0;
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, szPath);
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, dwReverse1);
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, dwReverse2);
	return true;
}

bool BaseGraph_DataBase::materialinfo::RecoverFromBuf(byte * pBuf, int nBufSize)
{
	int nSize = GetSerializeSize();
	if (nSize > nBufSize)
	{
		//给的buf不对
		assert(false);
		return false;
	}
	nSize = 0;
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, szPath);
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, dwReverse1);
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, dwReverse2);
	return true;
}

BaseGraph_DataBase::dynamictexture::dynamictexture()
{
	Reset();
}

void BaseGraph_DataBase::dynamictexture::Reset()
{
	memset(szPath, 0, sizeof(szPath));
	disable = 1;
	speed = 0;
	offsetx = 0;
	offsety = 0;
	vRunDirx = 0;
	vRunDiry = 0;
	scalex = 0;
	scaley = 0;
	centerx = 0;
	centery = 0;
	rotate = 0;
	alpha = 0;
	bRepeat = 0;
	bUseAlpha = 0;
}

int BaseGraph_DataBase::dynamictexture::GetSerializeSize() const
{
	int nSize = 0;
	SERIALIZE_PARAM_GET_SIZE(nSize, szPath);
	SERIALIZE_PARAM_GET_SIZE(nSize, disable);
	SERIALIZE_PARAM_GET_SIZE(nSize, speed);
	SERIALIZE_PARAM_GET_SIZE(nSize, offsetx);
	SERIALIZE_PARAM_GET_SIZE(nSize, offsety);
	SERIALIZE_PARAM_GET_SIZE(nSize, vRunDirx);
	SERIALIZE_PARAM_GET_SIZE(nSize, vRunDiry);
	SERIALIZE_PARAM_GET_SIZE(nSize, scalex);
	SERIALIZE_PARAM_GET_SIZE(nSize, scaley);
	SERIALIZE_PARAM_GET_SIZE(nSize, centerx);
	SERIALIZE_PARAM_GET_SIZE(nSize, centery);
	SERIALIZE_PARAM_GET_SIZE(nSize, rotate);
	SERIALIZE_PARAM_GET_SIZE(nSize, alpha);
	SERIALIZE_PARAM_GET_SIZE(nSize, bRepeat);
	SERIALIZE_PARAM_GET_SIZE(nSize, bUseAlpha);
	return nSize;
}

bool BaseGraph_DataBase::dynamictexture::GetSerializeBuf(byte * pBuf, int nBufSize) const
{
	int nSize = GetSerializeSize();
	if (nSize > nBufSize)
	{
		//给的buf不对
		assert(false);
		return false;
	}
	nSize = 0;
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, szPath);
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, disable);
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, speed);
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, offsetx);
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, offsety);
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, vRunDirx);
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, vRunDiry);
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, scalex);
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, scaley);
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, centerx);
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, centery);
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, rotate);
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, alpha);
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, bRepeat);
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, bUseAlpha);
	return true;
}

bool BaseGraph_DataBase::dynamictexture::RecoverFromBuf(byte * pBuf, int nBufSize)
{
	int nSize = GetSerializeSize();
	if (nSize > nBufSize)
	{
		//给的buf不对
		assert(false);
		return false;
	}
	nSize = 0;
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, szPath);
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, disable);
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, speed);
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, offsetx);
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, offsety);
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, vRunDirx);
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, vRunDiry);
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, scalex);
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, scaley);
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, centerx);
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, centery);
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, rotate);
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, alpha);
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, bRepeat);
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, bUseAlpha);
	return true;
}

BaseGraph_DataBase::metallight::metallight()
{
	Reset();
}

void BaseGraph_DataBase::metallight::Reset()
{
	enable = 0;
	memset(&pos, 0, sizeof(pos));
	memset(strength, 0, sizeof(strength));
}

int BaseGraph_DataBase::metallight::GetSerializeSize() const
{
	int nSize = 0;
	SERIALIZE_PARAM_GET_SIZE(nSize, enable);
	SERIALIZE_PARAM_GET_SIZE(nSize, pos);
	SERIALIZE_PARAM_GET_SIZE(nSize, strength);
	return nSize;
}

bool BaseGraph_DataBase::metallight::GetSerializeBuf(byte * pBuf, int nBufSize) const
{
	int nSize = GetSerializeSize();
	if (nSize > nBufSize)
	{
		//给的buf装不下
		assert(false);
		return false;
	}
	nSize = 0;
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, enable);
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, pos);
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, strength);

	return true;
}

bool BaseGraph_DataBase::metallight::RecoverFromBuf(byte * pBuf, int nBufSize)
{
	int nSize = GetSerializeSize();
	if (nSize > nBufSize)
	{
		//给的buf不对
		assert(false);
		return false;
	}
	nSize = 0;
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, enable);
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, pos);
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, strength);

	return true;
}

BaseGraph_DataBase::metalinfo::metalinfo()
{
	Reset();
}

void BaseGraph_DataBase::metalinfo::Reset()
{
	highlight = 0;
	barc = 0;
	specular = 0;
	width = 0;
	height = 0;
	light[0].Reset();
	light[1].Reset();
	light[2].Reset();
	memset(&eyepos, 0, sizeof(eyepos));
}

int BaseGraph_DataBase::metalinfo::GetSerializeSize() const
{
	int nSize = 0;
	SERIALIZE_PARAM_GET_SIZE(nSize, highlight);
	SERIALIZE_PARAM_GET_SIZE(nSize, barc);
	SERIALIZE_PARAM_GET_SIZE(nSize, specular);
	SERIALIZE_PARAM_GET_SIZE(nSize, width);
	SERIALIZE_PARAM_GET_SIZE(nSize, height);
	SERIALIZE_PARAM_GET_SIZE_STU_ARR(nSize, light, 3);
	SERIALIZE_PARAM_GET_SIZE(nSize, eyepos);
	return nSize;
}

bool BaseGraph_DataBase::metalinfo::GetSerializeBuf(byte * pBuf, int nBufSize) const
{
	int nSize = GetSerializeSize();
	if (nSize > nBufSize)
	{
		//给的buf装不下
		assert(false);
		return false;
	}
	nSize = 0;
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, highlight);
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, barc);
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, specular);
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, width);
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, height);
	SERIALIZE_PARAM_TO_BUF_STU_ARR(pBuf, nSize, light, 3);
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, eyepos);
	return true;
}

bool BaseGraph_DataBase::metalinfo::RecoverFromBuf(byte * pBuf, int nBufSize)
{
	int nSize = GetSerializeSize();
	if (nSize > nBufSize)
	{
		//给的buf装不下
		assert(false);
		return false;
	}
	nSize = 0;
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, highlight);
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, barc);
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, specular);
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, width);
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, height);
	SERIALIZE_BUF_TO_PARAM_STU_ARR(pBuf, nSize, light, 3);
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, eyepos);
	return true;
}
