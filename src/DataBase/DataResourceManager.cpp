#include "stdafx.h"
#include "DataResourceManager.h"



#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif
DataResourceManager::DataResourceManager()
{
	//构造的时候就把临时文件夹创建出来，本次启动的所有临时文件都往里扔，退出的时候干掉
	TCHAR szPath[MAX_PATH] = { 0 };
	GetTempPath(MAX_PATH, szPath);
	m_strTempPath = szPath;
	m_strTempPath.AppendFormat(_T("BaseGraph\\%s\\"), CommonFunction::CreateGUID());
}


DataResourceManager::~DataResourceManager()
{
}

bool DataResourceManager::SerializeOneResource(CArchive & archive, CString strResID, bool bResIncluded)
{
	bool bRet = false;
	int nBufSize = 1024 * 1024;//读写文件的切割块的大小
	if (archive.IsStoring())
	{
		if (bResIncluded)
		{
			CString strFullFileName = GetResource(strResID);
			CString strFileExt = CommonFunction::GetFileNameExt(strFullFileName);
			CString strNewFileName = CommonFunction::CreateGUID();
			strNewFileName.Append(strFileExt);
			CFile file;
			if (file.Open(strFullFileName, CFile::modeRead))
			{
				//先写ID再写文件名
				archive << strResID;
				archive << strNewFileName;
				//之后是文件size
				ULONGLONG nFileSize = file.GetLength();
				archive << nFileSize;
				//最后写文件内容
				byte * pBuf = new byte[nBufSize];
				int nRead = 0;
				do
				{
					nRead = file.Read(pBuf, nBufSize);
					archive.Write(pBuf, nRead);
				} while (nRead == nBufSize);
				file.Close();
				delete pBuf;
				pBuf = nullptr;
				bRet = true;
			}
		}
		else
		{
			assert(false);//存储不存在调用本函数但不存的情况
		}
	}
	else
	{
		//不管需不需要，文件名和文件大小都是要读的
		CString strFileName = _T("");
		CString strResID = _T("");
		ULONGLONG nFileSize = 0;
		archive >> strResID;
		archive >> strFileName;
		archive >> nFileSize;
		if (bResIncluded)
		{
			byte * pBuf = new byte[nBufSize];
			//需要读，则正常读，先创建临时文件夹
			CString strTempPath = GetTempFilePath();
			CString strErr;
			CommonFunction::MakeSureDirectoryExists(strTempPath, strErr);
			CString strNewFileName = strTempPath;
			strNewFileName.Append(strFileName);
			//创建文件
			CFile file;
			if (file.Open(strNewFileName, CFile::modeCreate | CFile::modeWrite))
			{
				ULONGLONG nBufCount = nFileSize / nBufSize;
				while (nBufCount > 0)
				{
					archive.Read(pBuf, nBufSize);
					file.Write(pBuf, nBufSize);
					nBufCount--;
				}
				if (0 != nFileSize % nBufSize)
				{
					archive.Read(pBuf, nFileSize % nBufSize);
					file.Write(pBuf, nFileSize % nBufSize);
				}
				//往里写
				file.Close();
				bRet = true;
			}
			delete pBuf;
			pBuf = nullptr;
			//读完之后将资源添加进去
			m_mapResource[strResID] = strNewFileName;
		}
		else
		{
			byte * pBuf = new byte[nBufSize];
			//不需要读，则只读不写
			ULONGLONG nBufCount = nFileSize / nBufSize;
			while (nBufCount > 0)
			{
				archive.Read(pBuf, nBufSize);
				nBufCount--;
			}
			if (0 != nFileSize % nBufSize)
			{
				archive.Read(pBuf, nFileSize % nBufSize);
			}
			delete pBuf;
			pBuf = nullptr;
			bRet = true;
		}
	}
	return bRet;
}

CString DataResourceManager::GetTempFilePath()
{
	return m_strTempPath;
}

DataResourceManager & DataResourceManager::Instance()
{
	static DataResourceManager inst;
	return inst;
}

CString DataResourceManager::AddResource(CString strSrc)
{
	CString strID = CommonFunction::CreateGUID();
	m_mapResource[strID] = strSrc;
	return strID;
}

bool DataResourceManager::DelResource(CString strID)
{
	if (m_mapResource.count(strID) > 0)
	{
		m_mapResource.erase(strID);
	}
	return true;
}

CString DataResourceManager::GetResource(CString strID)
{
	if (m_mapResource.count(strID) > 0)
	{
		return m_mapResource[strID];
	}
	return _T("");
}

bool DataResourceManager::Serialize(CArchive & archive, bool bResIncluded)
{
	bool bRet = false;
	if (archive.IsStoring())
	{
		//记下是否存有资源
		archive << bResIncluded;
		if (bResIncluded)
		{
			//如果需要保存资源，则保存
			int nResCount = static_cast<int>(m_mapResource.size());
			archive << nResCount;
			for (auto it : m_mapResource)
			{
				if (!SerializeOneResource(archive, it.first, bResIncluded))
				{
					assert(false);//有资源保存失败，要看看怎么回事，否则结果不可用
				}
			}
		}
	}
	else
	{
		//先确定里边有没有资源，如果没有，直接跳出
		bool bHasRes = false;
		archive >> bHasRes;
		if (bHasRes)
		{
			//如果里边有资源，则无论是否需要读取，都得处理
			int nResCount = 0;
			archive >> nResCount;
			for (int i = 0; i < nResCount; i++)
			{
				if (!SerializeOneResource(archive, _T(""), bResIncluded))//读取的时候ID传空
				{
					assert(false);//有资源读取失败，要看看怎么回事，否则结果不可用
				}
			}
		}
	}
	return bRet;
}
