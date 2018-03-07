#include "stdafx.h"
#include "Common\ColorDef.h"

int tag_sycolor::GetSerializeSize() const
{
	return 0;
}

bool tag_sycolor::GetSerializeBuf(byte * pBuf, int nBufSize) const
{
	return false;
}

bool tag_sycolor::RecoverFromBuf(byte * pBuf, int nBufSize)
{
	return false;
}
