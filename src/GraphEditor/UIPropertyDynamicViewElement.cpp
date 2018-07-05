#include "stdafx.h"
#include "UIPropertyDynamicViewElement.h"
#include "GraphEditorCommonDef.h"
#include "UICheckColorButton.h"
#include "UIComboxFontSel.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif
UIPropertyDynamicViewElement::UIPropertyDynamicViewElement()
{
}

UIPropertyDynamicViewElement::~UIPropertyDynamicViewElement()
{
}

void UIPropertyDynamicViewElement::RefreshFromID(CString strObjID)
{
	BaseGraph_DataBase::ObjectType emNewType = DataBase_GetItemType(strObjID);
	BaseGraph_DataBase::ObjectType emOldType = m_emObjType;
	if (emNewType != emOldType)
	{
		//类型变动，需要清掉原来的属性控件，重新赋值属性范围并初始化控件
		BaseGraph_DataBase::ParamValueDef paramIDBegin;
		BaseGraph_DataBase::ParamValueDef paramIDEnd;
		if (DataBase_GetItemTypePropertyIDRange(emNewType, paramIDBegin, paramIDEnd))
		{
			m_nParamIDBegin = paramIDBegin;
			m_nParamIDEnd = paramIDEnd;
			EraseAllChild();
			InitializeObject();
			if (!m_bVisible)
			{
				SetVisible(m_bVisible);
			}
			RearrangeChildPos();
		}
	}
	UIPropertyViewElement::RefreshFromID(strObjID);
}

void UIPropertyDynamicViewElement::RefreshFromType(BaseGraph_DataBase::ObjectType emType)
{
	BaseGraph_DataBase::ObjectType emOldType = m_emObjType;
	if (emType != emOldType)
	{
		//类型变动，需要清掉原来的属性控件，重新赋值属性范围并初始化控件
		BaseGraph_DataBase::ParamValueDef paramIDBegin;
		BaseGraph_DataBase::ParamValueDef paramIDEnd;
		if (DataBase_GetItemTypePropertyIDRange(emType, paramIDBegin, paramIDEnd))
		{
			m_nParamIDBegin = paramIDBegin;
			m_nParamIDEnd = paramIDEnd;
			EraseAllChild();
			InitializeObject();
			if (!m_bVisible)
			{
				SetVisible(m_bVisible);
			}
			RearrangeChildPos();
		}
	}
	UIPropertyViewElement::RefreshFromType(emType);
}
