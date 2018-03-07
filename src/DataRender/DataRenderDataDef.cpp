#include "stdafx.h"
#include "DataRender\DataRenderDataDef.h"

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
