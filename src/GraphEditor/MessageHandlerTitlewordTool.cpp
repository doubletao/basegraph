#include "stdafx.h"
#include "MessageHandlerTitlewordTool.h"
#include "GraphEditor.h"

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
		int nWidth = DataBase_GetSceneWidth();
		int nHeight = DataBase_GetSceneHeight();
		BufferToScene(&m_ptBegin, &m_ptBegin, nWidth / 2, nHeight / 2, 1.0f);
		BufferToScene(&point, &point, nWidth / 2, nHeight / 2, 1.0f);

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

		ReleaseCapture();
		m_ptBegin.x = 0.0f;
		m_ptBegin.y = 0.0f;
		m_bLButtonDown = false;
		ClearEditBuf(pWnd);
	}
	return Msg_Handle_Exit;
}

