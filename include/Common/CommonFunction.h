#pragma once
class BASEGRAPH_COMMON_API CommonFunction
{
private:
	CommonFunction();
	~CommonFunction();
public:
	static CString CreateGUID();//生成新的GUID
	static HRESULT GuidToCString(const GUID& guid, CString& str);//转换GUID到字符串
};
