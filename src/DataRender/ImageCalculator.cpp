#include "stdafx.h"
#include "ImageCalculator.h"

ImageCalculator::ImageCalculator()
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
		if (DataBase_GetItemProperty(m_nObjID, BaseGraph_DataBase::Image_Path, stuValue))
		{
			//创建物件buf
			if (PrepareObjBuffer())
			{
				CString strImgPath = stuValue.ToCString();
				if (UpdateImage(strImgPath))
				{
					if (StretchImage())
					{
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
				//创建源图像buf
				DataBufferManager::Instance().UnQuery2DBuffer(m_nImageBufferID);
				m_nImageBufferID = DataBufferManager::Instance().Query2DBuffer(nImageWidth, nImageHeight);

				ObjectBuffer * pImgBuf = DataBufferManager::Instance().GetObjectBuf(m_nImageBufferID);
				if (pImgBuf)
				{
					if (BaseCodec_ReadImage(strImgPath, nImageWidth, nImageHeight, pImgBuf->GetBuffer()))
					{
						DataBase_SetItemProperty(m_nObjID, BaseGraph_DataBase::Image_SrcWidth, nImageWidth);
						DataBase_SetItemProperty(m_nObjID, BaseGraph_DataBase::Image_SrcHeight, nImageHeight);
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
		ObjectBuffer * pImgBuf = DataBufferManager::Instance().GetObjectBuf(m_nImageBufferID);
		ObjectBuffer * pObjBuf = DataBufferManager::Instance().GetObjectBuf(m_nObjBufferID);
		if (BuferValid(pImgBuf) && BuferValid(pObjBuf))
		{
			BufferFunction::BufferBitblt((DWORD *)pObjBuf->GetBuffer(),
				0, 0, pObjBuf->GetWidth(), pObjBuf->GetHeight(), pObjBuf->GetWidth(), pObjBuf->GetHeight(),
				(DWORD *)pImgBuf->GetBuffer(),
				0, 0, pImgBuf->GetWidth(), pImgBuf->GetHeight(), pImgBuf->GetWidth(), pImgBuf->GetHeight(),
				BUFFER_COPY, BUFFER_PARITY_BOTH);
		}
	}
	return false;
}

bool ImageCalculator::PrepareObjBuffer()
{
	auto GetPropertyValue = [](int nObjID, BaseGraph_DataBase::ParamValueDef emValueDef)->BaseGraph_Common::Value
	{
		BaseGraph_Common::Value stuValue;
		if (DataBase_GetItemProperty(nObjID, BaseGraph_DataBase::Image_DispWidth, stuValue))
		{
			return stuValue;
		}
		return 0;
	};
	int nObjWidth = GetPropertyValue(m_nObjID, BaseGraph_DataBase::Image_SrcWidth).ToInt();
	int nObjHeight = GetPropertyValue(m_nObjID, BaseGraph_DataBase::Image_SrcHeight).ToInt();
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
	if (DataBase_GetItemProperty(m_nObjID, BaseGraph_DataBase::Image_DispWidth, stuValue))
	{
		nRet = stuValue.ToInt();
	}
	return nRet;
}

int ImageCalculator::GetHeight()
{
	int nRet = 0;
	BaseGraph_Common::Value stuValue;
	if (DataBase_GetItemProperty(m_nObjID, BaseGraph_DataBase::Image_DispHeight, stuValue))
	{
		nRet = stuValue.ToInt();
	}
	return nRet;
}
