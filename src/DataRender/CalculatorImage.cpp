#include "stdafx.h"
#include "CalculatorImage.h"
#include "CalculatorManager.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif
CalculatorImage::CalculatorImage(CString strObjID)
	: ObjectCalculator(strObjID)
	, m_nImageBufferID(0)
{
}


CalculatorImage::~CalculatorImage()
{
}

bool CalculatorImage::UpdateRenderBuffer()
{
	if (NeedUpdateBuffer())
	{
		BaseGraph_Common::Value stuValue;
		if (DataRender_GetItemProperty(m_strObjID, BaseGraph_DataBase::Image_Path_Str, stuValue))
		{
			//创建物件buf
			if (PrepareObjBuffer())
			{
				CString strImgPath = stuValue.ToCString();
				if (UpdateImage(strImgPath))
				{
					if (StretchImage())
					{
						int nMainDevice = DX3DEngine_GetMainDevice();
						//创建texture
						PrepareItemHalBufer(nMainDevice);
						ObjectBuffer * pImgBuf = GetObjectBuffer();
						if (pImgBuf)
						{
							int nWidth = pImgBuf->GetWidth();
							int nHeight = pImgBuf->GetHeight();
							//将2D buffer传上去
							int hTrans = DXTextureManager_QueryTexture(DXTexture_MemTexture, nWidth, nHeight);
							int nOffsetX = 0;
							int nOffsetY = 0;
							assert(m_HalDes.m_hTex);
							DX3DEngine_UpLoad(m_HalDes.m_hTex, nMainDevice, hTrans, nOffsetX, nOffsetY, nWidth, nHeight,
								(DWORD *)pImgBuf->GetBuffer(), nOffsetX, nOffsetY, nWidth, nHeight, nWidth, nHeight);

							if (false)
							{
								pImgBuf->DebugWriteImage(_T("e:\\1.bmp"));
								DX3DEngine_DumpTexture(nMainDevice, m_HalDes.m_hTex, _T("e:\\2.bmp"));
							}

							DXTextureManager_UnQueryTexture(hTrans, TRUE);
						}
						return ObjectCalculator::UpdateRenderBuffer();
					}
				}
			} 
		}
		return false;
	}
	return true;
}

bool CalculatorImage::UpdateImage(CString &strImgPath)
{
	if (BaseCodec_QueryImageSupport(strImgPath))
	{
		int nXStart = 0;
		int nYStart = 0;
		int nImageWidth = 0;
		int nImageHeight = 0;
		if (BaseCodec_GetImgInfo(strImgPath, nXStart, nYStart, nImageWidth, nImageHeight))
		{
			if (nImageWidth > 0
				&& nImageHeight > 0)
			{
				ObjectBuffer * pImgBuf = DataBufferManager::Instance().Query2DBuffer(m_nImageBufferID, nImageWidth, nImageHeight);
				if (pImgBuf)
				{
					if (BaseCodec_ReadImage(strImgPath, nImageWidth, nImageHeight, pImgBuf->GetBuffer()))
					{
						DataRender_SetItemProperty(m_strObjID, BaseGraph_DataBase::Image_SrcWidth_Int, nImageWidth);
						DataRender_SetItemProperty(m_strObjID, BaseGraph_DataBase::Image_SrcHeight_Int, nImageHeight);
						//处理反转及旗飘
						BOOL bUpDownChange = FALSE;
						BOOL bLeftRightChange = FALSE;
						BOOL bFlagChange = FALSE;
						BaseGraph_Common::Value stuParam;
						if (DataRender_GetItemProperty(m_strObjID, BaseGraph_DataBase::Image_UpDownChange_Bool, stuParam))
						{
							bUpDownChange = stuParam.ToBOOL();
							if (DataRender_GetItemProperty(m_strObjID, BaseGraph_DataBase::Image_LeftRightChange_Bool, stuParam))
							{
								bLeftRightChange = stuParam.ToBOOL();
								if (DataRender_GetItemProperty(m_strObjID, BaseGraph_DataBase::Image_FlagChange_Bool, stuParam))
								{
									bFlagChange = stuParam.ToBOOL();
								}
							}
						}
						//上下反转
						if (bUpDownChange)
						{
							int nLoop = nImageHeight / 2;
							LPDWORD pSrc = NULL;
							LPDWORD pDes = NULL;
							DWORD dwTemp = 0;
							for (int i = 0; i<nLoop; i++)
							{
								pSrc = (DWORD *)pImgBuf->GetBuffer() + i * nImageWidth;
								pDes = (DWORD *)pImgBuf->GetBuffer() + (nImageHeight - 1 - i) * nImageWidth;
								for (int j = 0; j < nImageWidth; j++)
								{
									dwTemp = *pSrc;
									*pSrc = *pDes;
									*pDes = dwTemp;
									pSrc++;
									pDes++;
								}
							}
						}
						//左右反转
						if (bLeftRightChange)
						{
							LPDWORD pSrc = NULL;
							LPDWORD pDes = NULL;
							DWORD dwTemp = 0;
							for (int i = 0; i < nImageHeight; i++)
							{
								pSrc = (DWORD *)pImgBuf->GetBuffer() + i * nImageWidth;
								pDes = pSrc + nImageWidth - 1;
								int nLoop = nImageWidth / 2;
								for (int j = 0; j<nLoop; j++)
								{
									dwTemp = *pSrc;
									*pSrc = *pDes;
									*pDes = dwTemp;
									pSrc++;
									pDes--;
								}
							}
						}
						//旗飘
						if (bFlagChange)
						{
							CCommonBuffer buffer(pImgBuf->GetWidth(), pImgBuf->GetHeight(), (DWORD *)pImgBuf->GetBuffer());
							BufferFunction::MakeStaticFlag(&buffer);
						}
						if (false)
						{
							pImgBuf->DebugWriteImage(_T("e:\\2.bmp"));
						}
						return true;
					}
				}
			}
		}
	}
	return false;
}

bool CalculatorImage::StretchImage()
{
	auto BuferValid = [](ObjectBuffer * pBuf)->bool
	{
		if (pBuf
			&& pBuf->GetWidth() > 0
			&& pBuf->GetHeight() > 0)
		{
			return true;
		}
		return false;
	};
	ObjectBuffer * pImgBuf = DataBufferManager::Instance().Get2DBuf(m_nImageBufferID);
	ObjectBuffer * pObjBuf = DataBufferManager::Instance().Get2DBuf(m_nObjBufferID);
	if (BuferValid(pImgBuf) && BuferValid(pObjBuf))
	{
		int nDispWidth = pObjBuf->GetWidth();
		int nDispHeight = pObjBuf->GetHeight();
		if (nDispWidth > 0
			&& nDispHeight > 0)
		{
			BufferFunction::BufferBitblt((DWORD *)pObjBuf->GetBuffer(),
				0, 0, nDispWidth, nDispHeight, nDispWidth, nDispHeight,
				(DWORD *)pImgBuf->GetBuffer(),
				0, 0, pImgBuf->GetWidth(), pImgBuf->GetHeight(), pImgBuf->GetWidth(), pImgBuf->GetHeight(),
				BUFFER_COPY, BUFFER_PARITY_BOTH);
			if (false)
			{
				pObjBuf->DebugWriteImage(_T("e:\\1.bmp"));
			}
			return true;
		}
	}
	return false;
}

bool CalculatorImage::PrepareObjBuffer()
{
	auto GetPropertyValue = [](CString strObjID, BaseGraph_DataBase::ParamValueDef emValueDef)->BaseGraph_Common::Value
	{
		BaseGraph_Common::Value stuValue;
		if (DataRender_GetItemProperty(strObjID, emValueDef, stuValue))
		{
			return stuValue;
		}
		return 0;
	};
	int nObjWidth = GetPropertyValue(m_strObjID, BaseGraph_DataBase::Obj_Width_Int).ToInt();
	int nObjHeight = GetPropertyValue(m_strObjID, BaseGraph_DataBase::Obj_Height_Int).ToInt();
	if (nObjWidth > 0
		&& nObjHeight > 0)
	{
		//这里对宽高做一个scale缩放
		float fScale = CalculatorManager::Instance().GetScale();
		nObjWidth = nObjWidth * fScale;
		nObjHeight = nObjHeight * fScale;
		DataBufferManager::Instance().UnQuery2DBuffer(m_nObjBufferID);
		m_nObjBufferID = DataBufferManager::Instance().Query2DBuffer(nObjWidth, nObjHeight);
		if (m_nObjBufferID > 0)
		{
			return true;
		}
	}
	return false;
}
