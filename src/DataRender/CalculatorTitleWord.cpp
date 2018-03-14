#include "stdafx.h"
#include "CalculatorTitleWord.h"
#include "CalculatorManager.h"
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
	SYText_Release(&m_text);
}

bool CalculatorTitleword::PrepareObjBuffer(int nWidth, int nHeight)
{
	if (nWidth > 0
		&& nHeight > 0)
	{
		DataBufferManager::Instance().UnQuery2DBuffer(m_nObjBufferID);
		m_nObjBufferID = DataBufferManager::Instance().Query2DBuffer(nWidth, nHeight);
		if (m_nObjBufferID > 0)
		{
			return true;
		}
	}
	return false;
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
			//造一个Text并由此对象生成一个排版器并排版
			m_text.Reset();
			SYText_Init(&m_text);

			SYFont newFont;
			newFont.lfWidth = 35;
			newFont.lfHeight = 40;
			newFont.lfDistance = 0;
			_tcscpy_s(newFont.lfFaceName, LF_FACESIZE, strFontName.GetBuffer());
			strFontName.ReleaseBuffer();
			m_text.bHorizontal = bHorizontal;

			m_TypeSet.SetFont(&newFont);
			m_TypeSet.Attach(&m_text, this);
			m_TypeSet.ReplaceString(strText, TRUE);

			//计算出bound
			CalUnitBound();

			//拿到宽高
			int nWidth = GetWidth();
			int nHeight = GetHeight();

			m_emChgLevel = ChgLevel_Create;

			if (nWidth > 0 && nHeight > 0)
			{
				int nDevice = DX3DEngine_GetMainDevice();
				if (PrepareItemHalBufer(nDevice))
				{
					DX3DEngine_ClearTexture(nDevice, m_HalDes.m_hTex, 0);
					RenderUnitBufferForGpu();
				}
			}
		}
	}
	return false;
}

rectf CalculatorTitleword::CalUnitBound()
{
	m_rtUnitBound.left = 65535.0f;
	m_rtUnitBound.top = 65535.0f;
	m_rtUnitBound.right = -65535.0f;
	m_rtUnitBound.bottom = -65535.0f;

	SYArray & arRenderUnitArray = m_text.ar2DUnit;
	int nRenderCount = (int)SYAR_GetSize(&arRenderUnitArray);
	RECT rtTemp = { 0 };
	rectf fbound = { 0 };
	vector3f v3d[4] = { 0 };
	for (int i = 0; i<nRenderCount; i++)
	{
		SYUnit* pRenderUnit = (SYUnit*)SYAR_GetAt(&arRenderUnitArray, i);
		
		//得到每一个Unit的边界(包括面边体影信息)在这个层面图形特技已经完成，RenderUnit单元中已经包含了图形特技的因素
		GetRenderUnitBound(i, fbound);
		rip_extend_fbound_long(&fbound, &rtTemp.left, &rtTemp.top, &rtTemp.right, &rtTemp.bottom);
#if 0
		//djt 特技size计算暂时不考虑
		//vector3f v = { 0,0,0 };
		//v *= pRenderUnit->MatVToItem;
		//POINT ptPos;
		//pEnv->ItemToScene(v);
		//ptPos.x = (LONG)v.x;
		//ptPos.y = (LONG)v.y;


		BOOL bRet = pEnv->CalcEffectSize(pEnv->m_dCurFrame, i, nRenderCount, 0.0, &rtTemp, EFFECT_CALC_CONDITIONS_PER, rtScene, ptPos);

		if (bRet)//如果有逐字特技需要计算
		{
			fbound.left = (float)rtTemp.left;
			fbound.top = (float)rtTemp.top;
			fbound.right = (float)rtTemp.right;
			fbound.bottom = (float)rtTemp.bottom;
		}
#endif

		//计算每一个Unit的矩形
		matrix4f matBuffer;
		SceneToBufferMat(&matBuffer, 0, 0);
		matBuffer = pRenderUnit->MatVToItem * matBuffer;
		CalcBufferProjectSize(pRenderUnit->u.u2d.graph.bound,
			v3d,
			fbound,
			&matBuffer);

		//总体矩形的计算需要考虑位置信息
		matrix4f matItem;
		matrix4f_from_pos(&matItem, pRenderUnit->pos.x, pRenderUnit->pos.y, pRenderUnit->pos.z);
		rectf bound;
		matBuffer = matItem * matBuffer;
		CalcBufferProjectSize(bound,
			v3d,
			fbound,
			&matBuffer);

		//计算总体的矩形
		m_rtUnitBound.left = min(m_rtUnitBound.left, bound.left);
		m_rtUnitBound.right = max(m_rtUnitBound.right, bound.right);
		m_rtUnitBound.top = min(m_rtUnitBound.top, bound.top);
		m_rtUnitBound.bottom = max(m_rtUnitBound.bottom, bound.bottom);
	}
	return m_rtUnitBound;
}

void CalculatorTitleword::GetRenderUnitBound(int nIndex, rectf & rtBoundf)
{
	SYArray* pRenderUnitArray = &m_text.ar2DUnit;
	SYUnit* pRenderUnit = (SYUnit*)SYAR_GetAt(pRenderUnitArray, nIndex);
	if (!pRenderUnit)
	{
		memset(&rtBoundf, 0, sizeof(rtBoundf));
		return;
	}
	rectf rtface = pRenderUnit->u.u2d.graph.bound;
	rtBoundf = rtface;
	float fcubedeltX = 0;
	float fcubedeltY = 0;
	if (!pRenderUnit->u.u2d.staticeffect.cube.disable && pRenderUnit->u.u2d.staticeffect.cube.width)
	{
		fcubedeltX = pRenderUnit->u.u2d.staticeffect.cube.width * cos(pRenderUnit->u.u2d.staticeffect.cube.angle);
		fcubedeltY = pRenderUnit->u.u2d.staticeffect.cube.width * sin(pRenderUnit->u.u2d.staticeffect.cube.angle);

		rectf rtcubef = rtBoundf;
		//SYRectf_OffsetRect1(&rtcubef,fcubedeltX,fcubedeltY);
		SYRectf_InflateRect1(&rtcubef, abs(fcubedeltX), abs(fcubedeltY));
		SYRectf_UnionRect(&rtBoundf, &rtBoundf, &rtcubef);
	}
	if (!pRenderUnit->u.u2d.staticeffect.shadow.disable &&
		(pRenderUnit->u.u2d.staticeffect.shadow.offsetx
			|| pRenderUnit->u.u2d.staticeffect.shadow.soft
			|| pRenderUnit->u.u2d.staticeffect.shadow.offsety
			|| pRenderUnit->u.u2d.staticeffect.shadow.effect_fxtitle
			|| pRenderUnit->u.u2d.staticeffect.shadow.effect_fyscale))
	{
		rectf rtShadow = rtface;
		BaseGraph_DataBase::shadowinfo* pinfo = &pRenderUnit->u.u2d.staticeffect.shadow;
		float fheight = rtBoundf.bottom - rtBoundf.top;
		float fwidth = rtBoundf.right - rtBoundf.left;
		float bottom = rtBoundf.bottom;
		bottom = 0;

		vector3f vVector[4];
		//SYRectf_OffsetRect1(&rtShadow,(float)pinfo->offsetx,(float)pinfo->offsety);
		SYRectf_InflateRect1(&rtShadow, abs(pinfo->offsetx), abs(pinfo->offsety));
		SYRectf_InflateRect1(&rtShadow, abs(fcubedeltX), abs(fcubedeltY));	// 在立体边基础上扩大
																			//SYRectf_OffsetRect1(&rtShadow,fcubedeltX,fcubedeltY);

																			// 此值暂时无用，之前版本错误，有些模板存有错误值，特此修正
		pinfo->effect_fxtitle = 0;

		vVector[0].x = rtShadow.left + pinfo->effect_fxtitle * fwidth * fabs(rtShadow.top - bottom) / fheight;
		vVector[0].y = bottom + (rtShadow.top - bottom) * (pinfo->effect_fyscale + 1.0f);

		vVector[1].x = rtShadow.right + pinfo->effect_fxtitle * fwidth * fabs(rtShadow.top - bottom) / fheight;
		vVector[1].y = bottom + (rtShadow.top - bottom) * (pinfo->effect_fyscale + 1.0f);

		vVector[2].x = rtShadow.right + pinfo->effect_fxtitle * fwidth * fabs(rtShadow.bottom - bottom) / fheight;
		vVector[2].y = bottom + (rtShadow.bottom - bottom) * (pinfo->effect_fyscale + 1.0f);

		vVector[3].x = rtShadow.left + pinfo->effect_fxtitle * fwidth * fabs(rtShadow.bottom - bottom) / fheight;
		vVector[3].y = bottom + (rtShadow.bottom - bottom) * (pinfo->effect_fyscale + 1.0f);

		for (int i = 0; i < 4; i++)
		{
			rtShadow.left = min(vVector[i].x, rtShadow.left);
			rtShadow.top = min(vVector[i].y, rtShadow.top);
			rtShadow.right = max(vVector[i].x, rtShadow.right);
			rtShadow.bottom = max(vVector[i].y, rtShadow.bottom);
		}
		if (pRenderUnit->u.u2d.staticeffect.shadow.soft)
		{
			SYRectf_InflateRect1(&rtShadow,
				(float)pRenderUnit->u.u2d.staticeffect.shadow.soft,
				(float)pRenderUnit->u.u2d.staticeffect.shadow.soft);
		}
		SYRectf_UnionRect(&rtBoundf, &rtBoundf, &rtShadow);
	}
	if (!pRenderUnit->u.u2d.staticeffect.border.disable)
	{
		int nRadWidth = SY_FToI(pRenderUnit->u.u2d.staticeffect.border.width) + pRenderUnit->u.u2d.staticeffect.border.soft;
		rectf rtborder = rtface;
		SYRectf_InflateRect1(&rtborder, (float)nRadWidth, (float)nRadWidth);
		//SYRectf_OffsetRect1(&rtborder,fcubedeltX,fcubedeltY);
		SYRectf_UnionRect(&rtBoundf, &rtBoundf, &rtborder);

		float fdeltX = 0;
		float fdeltY = 0;
		if (!pRenderUnit->u.u2d.staticeffect.bordercube.disable && pRenderUnit->u.u2d.staticeffect.bordercube.width)
		{
			fdeltX = pRenderUnit->u.u2d.staticeffect.bordercube.width * cos(pRenderUnit->u.u2d.staticeffect.bordercube.angle);
			fdeltY = pRenderUnit->u.u2d.staticeffect.bordercube.width * sin(pRenderUnit->u.u2d.staticeffect.bordercube.angle);

			rectf rtbordercubef = rtborder;
			SYRectf_InflateRect1(&rtbordercubef, abs(fdeltX), abs(fdeltY));
			SYRectf_UnionRect(&rtBoundf, &rtBoundf, &rtbordercubef);
		}

		if (!pRenderUnit->u.u2d.staticeffect.bordershadow.disable &&
			(pRenderUnit->u.u2d.staticeffect.bordershadow.offsetx
				|| pRenderUnit->u.u2d.staticeffect.bordershadow.soft
				|| pRenderUnit->u.u2d.staticeffect.bordershadow.offsety
				|| pRenderUnit->u.u2d.staticeffect.bordershadow.effect_fxtitle
				|| pRenderUnit->u.u2d.staticeffect.bordershadow.effect_fyscale))
		{
			rectf rtBorderShadow = rtborder;
			BaseGraph_DataBase::shadowinfo* pinfo = &pRenderUnit->u.u2d.staticeffect.bordershadow;
			float fheight = rtBoundf.bottom - rtBoundf.top;
			float fwidth = rtBoundf.right - rtBoundf.left;
			float bottom = rtBoundf.bottom;
			bottom = 0;

			vector3f vVector[4];
			SYRectf_InflateRect1(&rtBorderShadow, abs(pinfo->offsetx), abs(pinfo->offsety));
			// 			SYRectf_OffsetRect1(&rtBorderShadow,fdeltX,fdeltY);

			// 此值暂时无用，之前版本错误，有些模板存有错误值，特此修正
			pinfo->effect_fxtitle = 0;

			vVector[0].x = rtBorderShadow.left + pinfo->effect_fxtitle * fwidth * fabs(rtBorderShadow.top - bottom) / fheight;
			vVector[0].y = bottom + (rtBorderShadow.top - bottom) * (pinfo->effect_fyscale + 1.0f);

			vVector[1].x = rtBorderShadow.right + pinfo->effect_fxtitle * fwidth * fabs(rtBorderShadow.top - bottom) / fheight;
			vVector[1].y = bottom + (rtBorderShadow.top - bottom) * (pinfo->effect_fyscale + 1.0f);

			vVector[2].x = rtBorderShadow.right + pinfo->effect_fxtitle * fwidth * fabs(rtBorderShadow.bottom - bottom) / fheight;
			vVector[2].y = bottom + (rtBorderShadow.bottom - bottom) * (pinfo->effect_fyscale + 1.0f);

			vVector[3].x = rtBorderShadow.left + pinfo->effect_fxtitle * fwidth * fabs(rtBorderShadow.bottom - bottom) / fheight;
			vVector[3].y = bottom + (rtBorderShadow.bottom - bottom) * (pinfo->effect_fyscale + 1.0f);

			for (int i = 0; i < 4; i++)
			{
				rtBorderShadow.left = min(vVector[i].x, rtBorderShadow.left);
				rtBorderShadow.top = min(vVector[i].y, rtBorderShadow.top);
				rtBorderShadow.right = max(vVector[i].x, rtBorderShadow.right);
				rtBorderShadow.bottom = max(vVector[i].y, rtBorderShadow.bottom);
			}
			if (pRenderUnit->u.u2d.staticeffect.shadow.soft)
			{
				SYRectf_InflateRect1(&rtBorderShadow,
					(float)pinfo->soft,
					(float)pinfo->soft);
			}
			SYRectf_UnionRect(&rtBoundf, &rtBoundf, &rtBorderShadow);
		}
	}
}

int CalculatorTitleword::GetWidth()
{
	return SYRectf_Width(&m_rtUnitBound);
}

int CalculatorTitleword::GetHeight()
{
	return SYRectf_Height(&m_rtUnitBound);
}

SYText * CalculatorTitleword::GetText()
{
	return &m_text;
}

SYArray * CalculatorTitleword::Get2DTextUnit()
{
	return &m_text.ar2DUnit;
}

void CalculatorTitleword::DeleteRenderUnitView(LPCTSTR szUnitID)
{
	for (int i = 1; i < SYAR_GetCount(&m_text.ar2DUnit); ++i)
	{
		SYUnit* pTemplUnit = (SYUnit*)SYAR_GetAt(&m_text.ar2DUnit, i);
		if (pTemplUnit)
		{
			assert(false);
		}
	}
}

void CalculatorTitleword::RenderUnitBufferForGpu()
{
	if (m_emChgLevel < ChgLevel_Figure)
		return;

	int nDevice = DX3DEngine_GetMainDevice();
	SYArray* pUnitArray = &m_text.ar2DUnit;
	int nUnitCnt = SYAR_GetCount(pUnitArray);
	for (int i = 0; i < nUnitCnt; i++)
	{
		SYUnit* pRenderUnit = (SYUnit*)SYAR_GetAt(pUnitArray, i);
		//计算原始buffer
		DWORD *pBuffer = NULL;
		int facew = 0, faceh = 0, pitch = 0;
		pointf ptOrg;
		RECT rtResultBound;// = pRenderUnit->unit2D.bound; 

		if (!pRenderUnit || pRenderUnit->u.u2d.graph.ncell == 0)
		{
			//清空原来的Buf
			if (m_HalDes.m_hTex != NULL)
				DX3DEngine_ClearTexture(nDevice, m_HalDes.m_hTex, 0);
			continue;
		}

		CSYBuffer* pBufferPool = NULL;
		//MAC_STARTTIME_TEST();
		SYRenderGraphToOpBuff(&pRenderUnit->u.u2d.graph,
			&pRenderUnit->u.u2d.staticeffect,
			pBufferPool,
			rtResultBound,
			facew,
			faceh,
			pitch,
			CalculatorManager::Instance().GetScale(),
			ptOrg,
			CalculatorManager::Instance().GetFrame());

		if (0)
		{
			BufferFunction::WriteBitmap(_T("e:\\8.bmp"), facew, faceh, 32, (BYTE*)pBufferPool->m_dwBuffer);
		}
		if (!pBufferPool)
		{
			//清空原来的Buf
			if (m_HalDes.m_hTex != NULL)
				DX3DEngine_ClearTexture(nDevice, m_HalDes.m_hTex, 0);
			continue;
		}

		DXHalDef32* pHalBuffer = &m_HalDes;
		if (pHalBuffer)
		{		
				int hTransTex = DXTextureManager_QueryTexture(DXTexture_MemTexture, facew, faceh);
				//djt 这里的pos计算有问题，待纠正
				DX3DEngine_UpLoad(pHalBuffer->m_hTex, nDevice, hTransTex,
					pRenderUnit->pos.x, 60 * i/*pRenderUnit->pos.x*/, facew, faceh, pBufferPool->m_dwBuffer, 0, 0, facew, faceh, facew, faceh);
				if (0)
					DX3DEngine_DumpTexture(nDevice, pHalBuffer->m_hTex, _T("E:\\123.bmp"));
				DXTextureManager_UnQueryTexture(hTransTex);
		}
		if (pBufferPool)
			SYData_ReleaseCalcBuffer(pBufferPool);
		pBufferPool = NULL;
	}
}
