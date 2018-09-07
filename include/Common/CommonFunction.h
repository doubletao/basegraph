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
	static CString GetFilePath(LPCTSTR szFullPathFileName);
	static CString ConvertNumToCString(DWORD dwValue);//转换DWORD到csing
	static CString ConvertNumToCString(int nValue);//转换整数到csing
	static CString ConvertNumToCString(float fValue, int nPrecision = 2);//转换浮点数到csing
	static CString ConvertNumToCString(double dValue, int nPrecision = 2);//转换双精度小数到csing
	static int ConvertCStringToInt(CString strValue);//转换cstring到整数
	static float ConvertCStringToFloat(CString strValue);//转换cstring到浮点数
	static double ConvertCStringToDouble(CString strValue);//转换cstring到双精度小数
	static std::string ConverCStringToStdString(CString cstrValue);//CString与string之间的转换
	static std::string ConverCStringToStdString(CString cstrValue, DWORD dwFormatW);//CString与string之间的转换
	static CString ConvertStdStringToCString(const std::string & strValue);//CString与string之间的转换
};
