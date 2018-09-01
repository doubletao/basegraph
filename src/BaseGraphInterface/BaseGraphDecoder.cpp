#include "stdafx.h"
#include "BaseGraphDecoder.h"

BaseGraphDecoder::BaseGraphDecoder()
{
}


BaseGraphDecoder::~BaseGraphDecoder()
{
}

bool BaseGraphDecoder::OpenProject(CString strProjFile)
{
	return false;
}

bool BaseGraphDecoder::LoadProject()
{
	return false;
}

CString BaseGraphDecoder::GetProjectInfo()
{
	return CString();
}

bool BaseGraphDecoder::ChangeProjectCfg(CString strProInfo)
{
	return false;
}

CString BaseGraphDecoder::GetContent()
{
	return CString();
}

bool BaseGraphDecoder::ChangeContent(CString strContent)
{
	return false;
}
