#pragma once
class BASEGRAPH_COMMON_API CommonFunction
{
private:
	CommonFunction();
	~CommonFunction();
public:
	static CString CreateGUID();//生成新的GUID
	static HRESULT GuidToCString(const GUID& guid, CString& str);//转换GUID到字符串
	static CString GetBinPath();
	static void GetPathAndName(const CString &strSrcFilePath, CString &strPath, CString &strName);//根据文件全名获取文件路径及文件名
	static CString ConverFrameToText(int nFrame, float fFPS = 25.0);
	static BOOL IsFileExists(const CString &strSrcFilePath);//判定文件是否存在
};
