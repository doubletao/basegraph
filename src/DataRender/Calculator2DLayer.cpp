#include "stdafx.h"
#include "Calculator2DLayer.h"

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
