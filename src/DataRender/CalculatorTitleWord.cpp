#include "stdafx.h"
#include "CalculatorTitleWord.h"
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif
CalculatorTitleword::CalculatorTitleword(CString strObjID)
	: ObjectCalculator(strObjID)
{
}

CalculatorTitleword::~CalculatorTitleword()
{
}

bool CalculatorTitleword::UpdateRenderBuffer()
{
	if (NeedRenderBuffer())
	{
		//取得基本属性
		CString strText = _T("");
		CString strFontName = _T("");
		BOOL bUnderline = FALSE;
		int nFontDistance = 0;
		BOOL bHorizontal = FALSE;
		bool bContinue = false;
		BaseGraph_Common::Value stuValue;
		if (DataBase_GetItemProperty(m_strObjID, BaseGraph_DataBase::Titleword_Text_Str, stuValue))
		{
			strText = stuValue.ToCString();
			if (DataBase_GetItemProperty(m_strObjID, BaseGraph_DataBase::Titleword_FontName_Str, stuValue))
			{
				strFontName = stuValue.ToCString();
				if (DataBase_GetItemProperty(m_strObjID, BaseGraph_DataBase::Titleword_UnderLine_Bool, stuValue))
				{
					bUnderline = stuValue.ToBOOL();
					if (DataBase_GetItemProperty(m_strObjID, BaseGraph_DataBase::Titleword_FontDistance_Int, stuValue))
					{
						nFontDistance = stuValue.ToInt();
						if (DataBase_GetItemProperty(m_strObjID, BaseGraph_DataBase::Titleword_Horizontal_Bool, stuValue))
						{
							bHorizontal = stuValue.ToBOOL();
							bContinue = true;
						}
					}
				}
			}
		}
		if (bContinue)
		{
			//造一个Text并由此对象生成一个排版器
			m_text.Reset();
			m_text.bHorizontal = bHorizontal;
			m_TypeSet.Attach(&m_text);
			m_TypeSet.ReplaceString(strText, TRUE);
		}
	}
	return false;
}
