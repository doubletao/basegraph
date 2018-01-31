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
			CString strImgPath = stuValue.ToCString();
			if (UpdateImage(strImgPath))
			{
				if (StretchImage())
				{
					return true;
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
			//´´½¨Ô´Í¼Ïñbuf
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
	return false;
}

bool ImageCalculator::StretchImage()
{
	int nDispWidth = 0;
	int nDispHeight = 0;
	BaseGraph_Common::Value stuValue;
	if (DataBase_GetItemProperty(m_nObjID, BaseGraph_DataBase::Image_DispWidth, stuValue))
	{
		nDispWidth = stuValue.ToInt();
		if (DataBase_GetItemProperty(m_nObjID, BaseGraph_DataBase::Image_DispHeight, stuValue))
		{
			nDispHeight = stuValue.ToInt();
			if (nDispWidth > 0
				&& nDispHeight > 0)
			{
				ObjectBuffer * pImgBuf = DataBufferManager::Instance().GetObjectBuf(m_nImageBufferID);
				if (pImgBuf
					&& pImgBuf->GetWidth() > 0
					&& pImgBuf->GetHeight() > 0)
				{
					//BufferFunction::BufferBitblt();
				}
			}
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
