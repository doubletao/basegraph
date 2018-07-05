#include "stdafx.h"
#include "Calculator2DLayer.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif
const CString Calculator2DLayer::strCalculator2DLayer = _T("2DLayer");;

Calculator2DLayer::Calculator2DLayer()
{
}

Calculator2DLayer::~Calculator2DLayer()
{
}

BaseGraph_DataBase::ITEM_FORMTYPE Calculator2DLayer::Get3DType() const
{
	return BaseGraph_DataBase::ITEM_FORMTYPE_2D;
}
