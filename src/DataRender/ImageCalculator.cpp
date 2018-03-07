#include "stdafx.h"
#include "ImageCalculator.h"

ImageCalculator::ImageCalculator(CString strObjID)
	: ObjectCalculator(strObjID)
	, m_nImageBufferID(0)
{
}


ImageCalculator::~ImageCalculator()
{
}

bool ImageCalculator::UpdateRenderBuffer()
{
	if (NeedRenderBuffer())
	{
		BaseGraph_Common::Value stuValue;
		if (DataBase_GetItemProperty(m_strObjID, BaseGraph_DataBase::Image_Path_Str, stuValue))
		{
			//创建物件buf
			if (PrepareObjBuffer())
			{
				CString strImgPath = stuValue.ToCString();
				if (UpdateImage(strImgPath))
				{
					if (StretchImage())
					{
						int nWidth = GetWidth();
						int nHeight = GetHeight();
						int nMainDevice = DX3DEngine_GetMainDevice();
						//创建texture
						PrepareItemHalBufer(nMainDevice);
						ObjectBuffer * pImgBuf = GetObjectBuffer();
						if (pImgBuf)
						{
							//将2D buffer传上去
							int hTrans = DXTextureManager_QueryTexture(DXTexture_MemTexture, nWidth, nHeight);
							int nOffsetX = 0;
							int nOffsetY = 0;
							assert(m_HalDes.m_hTex);
							DX3DEngine_UpLoad(m_HalDes.m_hTex, nMainDevice, hTrans, nOffsetX, nOffsetY, nWidth, nHeight,
								(DWORD *)pImgBuf->GetBuffer(), nOffsetX, nOffsetY, nWidth, nHeight, pImgBuf->GetWidth(), pImgBuf->GetHeight());

							if (false)
							{
								pImgBuf->DebugWriteImage(_T("e:\\1.bmp"));
								DX3DEngine_DumpTexture(nMainDevice, m_HalDes.m_hTex, _T("e:\\2.bmp"));
							}

							DXTextureManager_UnQueryTexture(hTrans, TRUE);
						}
						return true;
					}
				}
			} 
		}
		return false;
	}
	return true;
}

bool ImageCalculator::UpdateImage(CString &strImgPath)
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
						DataBase_SetItemProperty(m_strObjID, BaseGraph_DataBase::Image_SrcWidth_Int, nImageWidth);
						DataBase_SetItemProperty(m_strObjID, BaseGraph_DataBase::Image_SrcHeight_Int, nImageHeight);
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

bool ImageCalculator::StretchImage()
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
	int nDispWidth = GetWidth();
	int nDispHeight = GetHeight();
	if (nDispWidth > 0
		&& nDispHeight > 0)
	{
		ObjectBuffer * pImgBuf = DataBufferManager::Instance().Get2DBuf(m_nImageBufferID);
		ObjectBuffer * pObjBuf = DataBufferManager::Instance().Get2DBuf(m_nObjBufferID);
		if (BuferValid(pImgBuf) && BuferValid(pObjBuf))
		{
			BufferFunction::BufferBitblt((DWORD *)pObjBuf->GetBuffer(),
				0, 0, pObjBuf->GetWidth(), pObjBuf->GetHeight(), pObjBuf->GetWidth(), pObjBuf->GetHeight(),
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

bool ImageCalculator::PrepareObjBuffer()
{
	auto GetPropertyValue = [](CString strObjID, BaseGraph_DataBase::ParamValueDef emValueDef)->BaseGraph_Common::Value
	{
		BaseGraph_Common::Value stuValue;
		if (DataBase_GetItemProperty(strObjID, emValueDef, stuValue))
		{
			return stuValue;
		}
		return 0;
	};
	int nObjWidth = GetPropertyValue(m_strObjID, BaseGraph_DataBase::Image_DispWidth_Int).ToInt();
	int nObjHeight = GetPropertyValue(m_strObjID, BaseGraph_DataBase::Image_DispHeight_Int).ToInt();
	if (nObjWidth > 0
		&& nObjHeight > 0)
	{
		DataBufferManager::Instance().UnQuery2DBuffer(m_nObjBufferID);
		m_nObjBufferID = DataBufferManager::Instance().Query2DBuffer(nObjWidth, nObjHeight);
		if (m_nObjBufferID > 0)
		{
			return true;
		}
	}
	return false;
}

int ImageCalculator::GetWidth()
{
	int nRet = 0;
	BaseGraph_Common::Value stuValue;
	if (DataBase_GetItemProperty(m_strObjID, BaseGraph_DataBase::Image_DispWidth_Int, stuValue))
	{
		nRet = stuValue.ToInt();
	}
	return nRet;
}

int ImageCalculator::GetHeight()
{
	int nRet = 0;
	BaseGraph_Common::Value stuValue;
	if (DataBase_GetItemProperty(m_strObjID, BaseGraph_DataBase::Image_DispHeight_Int, stuValue))
	{
		nRet = stuValue.ToInt();
	}
	return nRet;
}
