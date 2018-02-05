#include "stdafx.h"
#include "BaseGraphMainFrame.h"

IMPLEMENT_DYNAMIC(BaseGraphMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(BaseGraphMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_TIMER()
	ON_WM_SETFOCUS()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

BaseGraphMainFrame::BaseGraphMainFrame()
{
}

BaseGraphMainFrame::~BaseGraphMainFrame()
{
}

BOOL BaseGraphMainFrame::PreTranslateMessage(MSG * pMsg)
{
	return CFrameWnd::PreTranslateMessage(pMsg);
}

int BaseGraphMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	//≤‚ ‘¥˙¬Î
	CString strObjID = _T("");
	std::map<BaseGraph_DataBase::ParamValueDef, Value> mapParam = DataBase_GetDefaultParam(BaseGraph_DataBase::ObjectType_Image);
	mapParam[BaseGraph_DataBase::Image_Path].SetValue(_T("F:\\Media\\Õº∆¨\\1.png"));
	mapParam[BaseGraph_DataBase::Image_DispWidth].SetValue(800, BaseGraph_Common::ValueType_INT);
	mapParam[BaseGraph_DataBase::Image_DispHeight].SetValue(600, BaseGraph_Common::ValueType_INT);
	DataBase_CreateItem(BaseGraph_DataBase::ObjectType_Image, mapParam, strObjID);
	if (!strObjID.IsEmpty())
	{
		DataRender_UpdateRender(strObjID);
		byte * pBuf = nullptr;
		int nWidth = 0;
		int nHeight = 0;
		if (DataRender_GetBuffer(strObjID, pBuf, nWidth, nHeight))
		{
			BufferFunction::WriteBitmap(_T("e:\\4.bmp"), nWidth, nHeight, 32, pBuf);
		}
	}

	return 0;
}

void BaseGraphMainFrame::OnDestroy()
{
	CFrameWnd::OnDestroy();
}

void BaseGraphMainFrame::OnSetFocus(CWnd * pOldWnd)
{
	CFrameWnd::OnSetFocus(pOldWnd);
}

void BaseGraphMainFrame::OnSize(UINT nType, int cx, int cy)
{
	CFrameWnd::OnSize(nType, cx, cy);
}

void BaseGraphMainFrame::OnPaint()
{
	CPaintDC dc(this);

	CRect rcClient;
	GetClientRect(&rcClient);

	CDC dcMem;
	CBitmap bmpMem;
	dcMem.CreateCompatibleDC(NULL);
	bmpMem.CreateCompatibleBitmap(&dc, rcClient.Width(), rcClient.Height());
	dcMem.SelectObject(&bmpMem);
	dcMem.FillSolidRect(rcClient, RGB(0, 255, 255));

	dc.BitBlt(0, 0, rcClient.Width(), rcClient.Height(), &dcMem, 0, 0, SRCCOPY);

	bmpMem.DeleteObject();
	dcMem.DeleteDC();
}

BOOL BaseGraphMainFrame::OnEraseBkgnd(CDC * pDC)
{
	return TRUE;
}
