#pragma once
#include "BaseObject.h"
using BaseGraph_DataBase::BaseObject;

class ObjectImage :
	public BaseObject
{
public:
	ObjectImage();
	virtual ~ObjectImage();
public:
	virtual void InitialObjectParam();
	virtual std::map<BaseGraph_DataBase::ParamValueDef, Value> GetProperty();
	virtual Value GetValue(BaseGraph_DataBase::ParamValueDef emValueDef);
	virtual void SetValue(BaseGraph_DataBase::ParamValueDef emValueDef, Value stuValue);

	CString GetImagePath();
	void SetImagePath(CString strPath);
	int GetSrcWidth();
	void SetSrcWidth(int nValue);
	int GetSrcHeight();
	void SetSrcHeight(int nValue);
	int GetDispWidth();
	void SetDispWidth(int nValue);
	int GetDispHeight();
	void SetDispHeight(int nValue);
};
