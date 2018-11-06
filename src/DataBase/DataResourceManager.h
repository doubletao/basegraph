#pragma once
#include <map>

/*
资源管理器
图文中所有使用到的外部资源（图片，视频等）都被本类管理
刚进来的资源不会被拷贝，仅仅做一个映射（源路径与GUID的映射）
序列化入时会将资源全部打到存储文件，序列化出时会将资源读取到本地临时路径，此时资源的源路径已经指向了本地临时路径
本管理器希望实现上层逻辑对外部资源的无关性，拿着ID就能用，不必纠结到底在哪
*/
class DataResourceManager
{
private:
	DataResourceManager();
	~DataResourceManager();
private:
	std::map<CString, CString> m_mapResource;
protected:
	bool SerializeOneResource(CArchive & archive, CString strResID, bool bResIncluded);
	CString GetTempFilePath();
public:
	static DataResourceManager & Instance();
	CString AddResource(CString strSrc);//添加一个资源，返回资源标识
	bool DelResource(CString strID);//删除指定ID的资源
	CString GetResource(CString strID);//给ID返回源路径
	bool Serialize(CArchive & archive, bool bResIncluded);//将所有资源文件序列化（串入过程会对所有文件名重新进行guid编码，串出时按照guid将文件串出到临时文件夹中）,bResIncluded为false时如果时存储则不会写入，如果是读取会跳过buf
};
