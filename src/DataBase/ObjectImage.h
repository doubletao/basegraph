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
