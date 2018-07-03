#include "stdafx.h"
#include "UIControl\UIEditPath.h"
#include "UIControl\ThemeResoueceID.h"
#include "ThemeManager.h"
#include <assert.h>

UIEditPath::UIEditPath()
	: m_pUIEdit(nullptr)
	, m_pUIStatic(nullptr)
	, m_pUIButton(nullptr)
{
}

UIEditPath::~UIEditPath()
{
}

bool UIEditPath::Create(CString strTitle, int nTitleWidth, int nID, DWORD dwStyleEs, HWND hParentWnd, UIElement * pParent)
{
	if (!m_bCreated)
	{
		Initialize(0, 0, 100, 100, nID, hParentWnd, pParent);
		m_pUIEdit = new UIEdit;
		m_pUIEdit->Create(0, 0, 100, 100, nID, dwStyleEs | ES_READONLY, hParentWnd, this);
		m_vecChildElement.push_back(m_pUIEdit);

		m_pUIStatic = new UIStatic;
		m_pUIStatic->Create(0, 0, nTitleWidth, 10, 0, nullptr, this, strTitle, ThemeFont_CustomEditCtr, ThemeManager::Instance().GetColor(ThemeColor_CustomStatic));
		m_vecChildElement.push_back(m_pUIStatic);

		m_pUIButton = new UIButton;
		m_pUIButton->Create(0, 0, 10, 10, 0, nullptr, ThemeImage_BtnPathSel_N, ThemeImage_BtnPathSel_F, ThemeImage_BtnPathSel_D, ThemeImage_BtnPathSel_X, this);
		m_vecChildElement.push_back(m_pUIButton);

		m_setMsgFilter.insert(WM_LBUTTONDOWN);
		m_setMsgFilter.insert(WM_LBUTTONUP);
		m_setMsgFilter.insert(WM_MOUSEMOVE);
		m_bCreated = true;
		return true;
	}
	return false;
}

bool UIEditPath::OnChildMsg(EmUIElementMsg emMsg, WPARAM wParam, LPARAM lParam)
{
	if (emMsg == EmUIElementMsg_BtnClick)
	{
		CString strDefPathFile = m_value.ToCString();
		UIButton * pButton = (UIButton *)lParam;
		if (m_pUIButton == pButton)
		{
			CString strFilter(_T("png Files (*.png)|*.png|bmp Files (*.bmp)|*.bmp|jpg Files (*.jpg)|*.jpg;|All Files (*.*)|*.*||"));
			CFileDialog selectFile(TRUE, NULL, strDefPathFile, 0, strFilter);
			if (selectFile.DoModal() == IDOK)
			{
				m_value = selectFile.GetPathName();
				UpdateValueToUI();
				NotifyValueChg();
			}
		}
		return true;
	}
	else if (emMsg == EmUIElementMsg_OnEditEnded)
	{
		UpdateUIToValue();
		NotifyValueChg();
		return true;
	}
	return UICombinedCtrl::OnChildMsg(emMsg, wParam, lParam);
}

EmUIElementMsgRet UIEditPath::HandleMsg(UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	if (m_setMsgFilter.count(nMsg) > 0)
	{
		switch (nMsg)
		{
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
			ChildHandleMsg(nMsg, wParam, lParam);
			break;
		case WM_MOUSEMOVE:
			EnterLeaveChild(nMsg, lParam, wParam);
			break;
		default:
			break;
		}
		return UIElementMsgSuccBreak;
	}
	return UIElementMsgSuccContinue;
}

void UIEditPath::UpdateValueToUI()
{
	if (m_pUIEdit)
	{
		CString strDefPathFile = m_value.ToCString();
		m_pUIEdit->SetText(strDefPathFile);
	}
}

void UIEditPath::UpdateUIToValue()
{
	if (m_pUIEdit)
	{
		m_value = m_pUIEdit->GetText();
	}
}

void UIEditPath::RearrangeChildPos()
{
	if (m_pUIButton && m_pUIEdit && m_pUIStatic)
	{
		CImage * pImg = ThemeManager::Instance().GetImage(ThemeImage_BtnPathSel_N);
		if (pImg)
		{
			int nBtnWidth = pImg->GetWidth();
			int nBtnHeight = pImg->GetHeight();
			int nTitleWidth = m_pUIStatic->GetWidth();
			int nCtrGap = ThemeManager::Instance().GetIntValue(ThemeInt_PropertyViewCtrlGap);

			m_nHeight = nBtnHeight;
			m_pUIStatic->MoveTo(0, 0, nTitleWidth, m_nHeight);
			m_pUIEdit->MoveTo(nTitleWidth + nCtrGap, 0, m_nWidth - nTitleWidth - nCtrGap * 2 - nBtnWidth, m_nHeight);
			m_pUIButton->MoveTo(m_nWidth - nCtrGap - nBtnWidth, 0, nBtnWidth, m_nHeight);
		}
	}
}
