#include "stdafx.h"
#include "MessageHandlerImageTool.h"
#include "GraphEditor.h"
#include "UIPropertyView.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif
MessageHandlerImageTool::MessageHandlerImageTool()
	: m_bLButtonDown(false)
{
	m_ptBegin.x = 0.0f;
	m_ptBegin.y = 0.0f;
}

MessageHandlerImageTool::~MessageHandlerImageTool()
{
}

void MessageHandlerImageTool::OnSetFocus(ISceneMessageHandler * pLastHandler)
{
	ISceneMessageHandler::OnSetFocus(pLastHandler);
	//广播出去自己被激活了
	UIControl_BroadcastMsg(WM_MSG_OnCreateObjTool, BaseGraph_DataBase::ObjectType_Image, (LPARAM)pLastHandler, nullptr);
	//拿到焦点后干掉选中项
	DataRender_ClearAllSelect();
	UIControl_SendMsgTo(UIElementMsg_MainViewWnd, WM_MSG_SerRenderTask, 0, 0);//通知刷新
}

void MessageHandlerImageTool::DrawObjCreateBox(C3DSceneViewWnd * pWnd, pointf ptBegin, pointf ptEnd)
{
	EditorSettingParam stuDrawerParam = CGraphEditorApp::GetBaseGraphFrameWnd()->GetEditorSettingParam();
	DrawBoxToEditBuf(pWnd, ptBegin, ptEnd, PS_SOLID, 1, stuDrawerParam.nColorObjCreateLine);
}

EmMessageHandlerResult MessageHandlerImageTool::OnLButtonDown(IN C3DSceneViewWnd * pWnd, UINT nFlags, pointf point)
{
	if (pWnd)
	{
		m_ptBegin = point;
		pWnd->SetCapture();
		m_bLButtonDown = true;
	}
	return Msg_Handle_Break;
}

EmMessageHandlerResult MessageHandlerImageTool::OnMouseMove(IN C3DSceneViewWnd * pWnd, UINT nFlags, pointf point)
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

EmMessageHandlerResult MessageHandlerImageTool::OnLButtonUp(IN C3DSceneViewWnd * pWnd, UINT nFlags, pointf point)
{
	if (pWnd)
	{
		//点击方式创建物件
		BOOL bClickCreate = FALSE;
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
		CString strNewItemName = DataBase_GetNextItemName(BaseGraph_DataBase::ObjectType_Image);
		mapParamValue[BaseGraph_DataBase::Obj_Name_Str].SetValue(strNewItemName);
		//弹出窗口拿到文件名
		CString strFilter = BaseCodec_EnumSuportImageType();
		CFileDialog selectFile(TRUE, NULL, NULL, 0, strFilter);
		if (selectFile.DoModal() == IDOK)
		{
			CString strPath = selectFile.GetPathName();
			int nWidth = 0;
			int nHeight = 0;
			if (bClickCreate)
			{
				//如果是单点创建，则保持图片源宽高
				int nX = 0;
				int nY = 0;
				BaseCodec_GetImgInfo(strPath, nX, nY, nWidth, nHeight);
			}
			else
			{
				//否则设置宽高为创建时的矩形
				nWidth = fabsf(ptRightBottom.x - ptLeftTop.x);
				nHeight = fabsf(ptRightBottom.y - ptLeftTop.y);
			}
			//宽高都要大于最小值才可以用
			if (nWidth > stuDrawerParam.nObjectMinSize && nHeight > stuDrawerParam.nObjectMinSize)
			{
				mapParamValue[BaseGraph_DataBase::SpacePosition_X_Float].SetValue(ptLeftTop.x + nWidth / 2);
				mapParamValue[BaseGraph_DataBase::SpacePosition_Y_Float].SetValue(ptLeftTop.y - nHeight / 2);
				mapParamValue[BaseGraph_DataBase::SpacePosition_Z_Float].SetValue(0.0f);
				mapParamValue[BaseGraph_DataBase::Obj_Width_Int].SetValue(nWidth);
				mapParamValue[BaseGraph_DataBase::Obj_Height_Int].SetValue(nHeight);
				//文件名赋值
				mapParamValue[BaseGraph_DataBase::Image_Path_Str].SetValue(strPath);
				//生成一个名字出来
				CString strNewObjID = _T("");
				DataBase_CreateItem(BaseGraph_DataBase::ObjectType_Image, mapParamValue, strNewObjID);
				//广播通知新物件被创建
				UIControl_BroadcastMsg(WM_MSG_ObjectCreateDelete, 0, 0, nullptr);
				//创建成功则选中
				if (!strNewObjID.IsEmpty())
				{
					DataRender_SetItemSelected(strNewObjID, true);
					//广播通知选中项变更
					UIControl_BroadcastMsg(WM_MSG_OnSelectChanged, 0, 0, nullptr);
				}
			}
		}

		ReleaseCapture();
		m_ptBegin.x = 0.0f;
		m_ptBegin.y = 0.0f;
		m_bLButtonDown = false;
		ClearEditBuf(pWnd);
	}
	return Msg_Handle_Exit;
}

