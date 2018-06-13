#include "stdafx.h"
#include "MessageHandlerTitlewordTool.h"
#include "GraphEditor.h"
#include "UIPropertyView.h"

MessageHanderTitleword::MessageHanderTitleword()
	: m_bLButtonDown(false)
{
	m_ptBegin.x = 0.0f;
	m_ptBegin.y = 0.0f;
}

MessageHanderTitleword::~MessageHanderTitleword()
{
}

void MessageHanderTitleword::OnSetFocus(ISceneMessageHandler * pLastHandler)
{
	ISceneMessageHandler::OnSetFocus(pLastHandler);
	//广播出去自己被激活了
	UIControl_BroadcastMsg(WM_MSG_OnCreateObjTool, BaseGraph_DataBase::ObjectType_Titleword, (LPARAM)pLastHandler, nullptr);
	//拿到焦点后干掉选中项
	DataRender_ClearAllSelect();
	UIControl_SendMsgTo(UIElementMsg_MainViewWnd, WM_MSG_SerRenderTask, 0, 0);//通知刷新
}

void MessageHanderTitleword::DrawObjCreateBox(C3DSceneViewWnd * pWnd, pointf ptBegin, pointf ptEnd)
{
	EditorSettingParam stuDrawerParam = CGraphEditorApp::GetBaseGraphFrameWnd()->GetEditorSettingParam();
	DrawBoxToEditBuf(pWnd, ptBegin, ptEnd, PS_SOLID, 1, stuDrawerParam.nColorObjCreateLine);
}

EmMessageHandlerResult MessageHanderTitleword::OnLButtonDown(IN C3DSceneViewWnd * pWnd, UINT nFlags, pointf point)
{
	if (pWnd)
	{
		m_ptBegin = point;
		pWnd->SetCapture();
		m_bLButtonDown = true;
	}
	return Msg_Handle_Break;
}

EmMessageHandlerResult MessageHanderTitleword::OnMouseMove(IN C3DSceneViewWnd * pWnd, UINT nFlags, pointf point)
{
	if (m_bLButtonDown)
	{
		EditorSettingParam stuDrawerParam = CGraphEditorApp::GetBaseGraphFrameWnd()->GetEditorSettingParam();
		if (fabs(point.x - m_ptBegin.x) < stuDrawerParam.nObjectMinSize)
		{
			if (point.x > m_ptBegin.x)
			{
				point.x = m_ptBegin.x + stuDrawerParam.nObjectMinSize;
			}
			else
			{
				point.x = m_ptBegin.x - stuDrawerParam.nObjectMinSize;
			}
		}
		if (fabs(point.y - m_ptBegin.y) < stuDrawerParam.nObjectMinSize)
		{
			if (point.y > m_ptBegin.y)
			{
				point.y = m_ptBegin.y + stuDrawerParam.nObjectMinSize;
			}
			else
			{
				point.y = m_ptBegin.y - stuDrawerParam.nObjectMinSize;
			}
		}
		DrawObjCreateBox(pWnd, m_ptBegin, point);
	}
	return Msg_Handle_Break;
}

EmMessageHandlerResult MessageHanderTitleword::OnLButtonUp(IN C3DSceneViewWnd * pWnd, UINT nFlags, pointf point)
{
	if (pWnd)
	{
		//点击方式创建物件
		BOOL bClickCreate = TRUE;
		EditorSettingParam stuDrawerParam = CGraphEditorApp::GetBaseGraphFrameWnd()->GetEditorSettingParam();
		if ((fabs(point.x - m_ptBegin.x) < stuDrawerParam.nObjectMinSize)
			&& (fabs(point.y - m_ptBegin.y) < stuDrawerParam.nObjectMinSize))
		{
			//横纵坐标差距都小于mini时视为同一点
			point = m_ptBegin;
			bClickCreate = TRUE;
		}
		else
		{
			//有一个方向大于mini时，则将另一个规范化
			if (fabs(point.x - m_ptBegin.x) < stuDrawerParam.nObjectMinSize)
			{
				if (point.x > m_ptBegin.x)
				{
					point.x = m_ptBegin.x + stuDrawerParam.nObjectMinSize;
				}
				else
				{
					point.x = m_ptBegin.x - stuDrawerParam.nObjectMinSize;
				}
			}
			if (fabs(point.y - m_ptBegin.y) < stuDrawerParam.nObjectMinSize)
			{
				if (point.y > m_ptBegin.y)
				{
					point.y = m_ptBegin.y + stuDrawerParam.nObjectMinSize;
				}
				else
				{
					point.y = m_ptBegin.y - stuDrawerParam.nObjectMinSize;
				}
			}
		}
		int nSceneWidth = DataBase_GetSceneWidth();
		int nSceneHeight = DataBase_GetSceneHeight();
		BufferToScene(&m_ptBegin, &m_ptBegin, nSceneWidth / 2, nSceneHeight / 2, 1.0f);
		BufferToScene(&point, &point, nSceneWidth / 2, nSceneHeight / 2, 1.0f);

		vector3f vStart = { m_ptBegin.x, m_ptBegin.y, 0 };
		vector3f vEnd = { point.x, point.y, 0 };

		matrix4f matItem;
		rectf rtItemBound;
		SYCaclTransMatrixFromTwoPointInReal(&matItem, &rtItemBound, &vStart, &vEnd, DataRender_GetCurParentObjID());
		//分解得到SRP各矩阵
		matrix4f matS, matR, matP;
		matrix4f_issolvesrp(&matItem, &matS, &matR, &matP);
		float3 posItem = { matP._41, matP._42, matP._43 };
		float3 angleItem;
		matrix4f_toeuler_lyxz(&matR, &angleItem.y, &angleItem.x, &angleItem.z);

		vStart.x = rtItemBound.left;
		vStart.y = -rtItemBound.top;
		vEnd.x = rtItemBound.right;
		vEnd.y = -rtItemBound.bottom;
		vStart += posItem;
		vEnd += posItem;

		float3 ptLeftTop, ptRightBottom;
		ptLeftTop.x = min(vStart.x, vEnd.x);
		ptLeftTop.y = max(vStart.y, vEnd.y);
		ptLeftTop.z = vStart.z;
		ptRightBottom.x = max(vStart.x, vEnd.x);
		ptRightBottom.y = min(vStart.y, vEnd.y);
		ptRightBottom.z = vEnd.z;

		//这个两个点拿到的是位置和字宽或者字高，还需要从属性窗里拿到当前属性
		std::map<BaseGraph_DataBase::ParamValueDef, Value> mapParamValue;
		//拿到属性窗
		UIPropertyView * pPropertyView = (UIPropertyView *)UIControl_GetMsgDispatcher(UIElementMsg_PropertyView);
		//取出属性
		if (pPropertyView)
		{
			mapParamValue = pPropertyView->GetCurrentParamValue();
		}
		//填充创建工具确定的属性
		CString strNewItemName = DataBase_GetNextItemName(BaseGraph_DataBase::ObjectType_Titleword);
		mapParamValue[BaseGraph_DataBase::Obj_Name_Str].SetValue(strNewItemName);
		mapParamValue[BaseGraph_DataBase::SpacePosition_X_Float].SetValue(ptLeftTop.x);
		mapParamValue[BaseGraph_DataBase::SpacePosition_Y_Float].SetValue(ptLeftTop.y);
		mapParamValue[BaseGraph_DataBase::SpacePosition_Z_Float].SetValue(0.0f);
		//测试
		mapParamValue[BaseGraph_DataBase::Titleword_Text_Str].SetValue(_T(" "));
		//生成一个名字出来
		CString strNewObjID = _T("");
		DataBase_CreateItem(BaseGraph_DataBase::ObjectType_Titleword, mapParamValue, strNewObjID);
		//广播通知新物件被创建
		UIControl_BroadcastMsg(WM_MSG_WholeFrameLengthChanged, 0, 0, nullptr);
		//创建成功则选中
		if (!strNewObjID.IsEmpty())
		{
			DataRender_SetItemSelected(strNewObjID, true);
			//广播通知选中项变更
			UIControl_BroadcastMsg(WM_MSG_OnSelectChanged, 0, 0, nullptr);
		}

		ReleaseCapture();
		m_ptBegin.x = 0.0f;
		m_ptBegin.y = 0.0f;
		m_bLButtonDown = false;
		ClearEditBuf(pWnd);
	}
	return Msg_Handle_Exit;
}

