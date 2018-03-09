#include "stdafx.h"
#include "DataRender\DataRenderDataDef.h"
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif
BASEGRAPH_DATARENDER_API void tag_sy2dunit::Reset()
{
	memset(this, 0, sizeof(tag_sy2dunit));
	staticeffect.renderline = 0;
	staticeffect.fwidth = 0;
	staticeffect.index = 0;
}

BASEGRAPH_DATARENDER_API void tag_sy3dunit::Reset()
{
	memset(this, 0, sizeof(tag_sy3dunit));
}

BASEGRAPH_DATARENDER_API void tag_syunit::Reset()
{
	memset(this, 0, sizeof(tag_syunit));
}

BASEGRAPH_DATARENDER_API void tag_sybonedef::Reset()
{
	nAnimID = 0;
	rotationOrder = 0;
	linkMode = 0;
	flags = 0;
	parent = 0;
	geoid = 0;
	memset(&pivot, 0, sizeof(vector3f));
	memset(name, 0, 128 * sizeof(TCHAR));
	memset(&pboneoffVMatrix, 0, sizeof(matrix4f));
}

BASEGRAPH_DATARENDER_API void tag_symodel::Reset()
{
	memset(this, 0, sizeof(SYModel));
}
