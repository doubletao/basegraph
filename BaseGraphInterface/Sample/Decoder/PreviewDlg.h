#pragma once


// PreviewDlg 对话框

class PreviewDlg : public CDialogEx
{
	DECLARE_DYNAMIC(PreviewDlg)

public:
	PreviewDlg(IBaseGraphDecoder * pDecoder, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~PreviewDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_PREVIEW };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

protected:
	IBaseGraphDecoder * m_pDecoder;
	DWORD * m_pBuf;
	CRect m_rtPaintArea;
protected:
	void PaintEvent();
};
