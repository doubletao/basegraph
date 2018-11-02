
// DecoderDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Decoder.h"
#include "DecoderDlg.h"
#include "afxdialogex.h"
#include <locale.h>
#include "PreviewDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CDecoderDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BTN_OPEN, &CDecoderDlg::OnBnClickedBtnOpen)
	ON_BN_CLICKED(IDC_BTN_LOAD, &CDecoderDlg::OnBnClickedBtnLoad)
	ON_BN_CLICKED(IDC_BTN_UPDATE, &CDecoderDlg::OnBnClickedBtnUpdate)
	ON_BN_CLICKED(IDC_BTN_PREVIEW, &CDecoderDlg::OnBnClickedBtnPreview)
END_MESSAGE_MAP()

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CDecoderDlg 对话框



CDecoderDlg::CDecoderDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DECODER_DIALOG, pParent)
	, m_pDecoder(nullptr)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	Interface_InitialEnv();
	Interface_CreateDecoder(m_pDecoder);
	_tsetlocale(LC_CTYPE, _T("chs"));
}

CDecoderDlg::~CDecoderDlg()
{
	Interface_ReleaseDecoder(m_pDecoder);
	m_pDecoder = nullptr;
	Interface_ReleaseEnv();
}

void CDecoderDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

void CDecoderDlg::LayoutCtrl()
{
	CWnd * pEditXML = GetDlgItem(IDC_EDIT_XML);
	CWnd * pEditPath = GetDlgItem(IDC_EDIT_PATH);
	CWnd * pBtnOpen = GetDlgItem(IDC_BTN_OPEN);
	CWnd * pBtnLoad = GetDlgItem(IDC_BTN_LOAD);
	CWnd * pBtnUpdate = GetDlgItem(IDC_BTN_UPDATE);
	CWnd * pBtnPreview = GetDlgItem(IDC_BTN_PREVIEW);
	if (pEditXML && ::IsWindow(pEditXML->GetSafeHwnd())
		&& pEditPath && ::IsWindow(pEditPath->GetSafeHwnd())
		&& pBtnOpen && ::IsWindow(pBtnOpen->GetSafeHwnd())
		&& pBtnLoad && ::IsWindow(pBtnLoad->GetSafeHwnd())
		&& pBtnUpdate && ::IsWindow(pBtnUpdate->GetSafeHwnd())
		&& pBtnPreview && ::IsWindow(pBtnPreview->GetSafeHwnd()))
	{
		CRect rtClient;
		GetClientRect(rtClient);
		CRect rtBtn;
		pBtnOpen->GetWindowRect(rtBtn);
		int nGap = 3;

		pEditXML->MoveWindow(0, 0, rtClient.Width(), rtClient.Height() - rtBtn.Height() - nGap);
		pEditPath->MoveWindow(0, rtClient.Height() - rtBtn.Height(), rtBtn.Width() * 2, rtBtn.Height());
		pBtnOpen->MoveWindow(rtBtn.Width() * 2 + nGap * 1 + rtBtn.Width() * 0, rtClient.Height() - rtBtn.Height(), rtBtn.Width(), rtBtn.Height());
		pBtnLoad->MoveWindow(rtBtn.Width() * 2 + nGap * 2 + rtBtn.Width() * 1, rtClient.Height() - rtBtn.Height(), rtBtn.Width(), rtBtn.Height());
		pBtnUpdate->MoveWindow(rtBtn.Width() * 2 + nGap * 3 + rtBtn.Width() * 2, rtClient.Height() - rtBtn.Height(), rtBtn.Width(), rtBtn.Height());
		pBtnPreview->MoveWindow(rtBtn.Width() * 2 + nGap * 4 + rtBtn.Width() * 3, rtClient.Height() - rtBtn.Height(), rtBtn.Width(), rtBtn.Height());
	}
}

// CDecoderDlg 消息处理程序

BOOL CDecoderDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	LayoutCtrl();
	CEdit * pEdit = (CEdit *)GetDlgItem(IDC_EDIT_XML);
	if (pEdit && ::IsWindow(pEdit->GetSafeHwnd()))
	{
		pEdit->SetLimitText(UINT_MAX);
	}

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CDecoderDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CDecoderDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

void CDecoderDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
	LayoutCtrl();
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CDecoderDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CDecoderDlg::OnBnClickedBtnOpen()
{
	CFileDialog dialog(TRUE);
	if (dialog.DoModal() == IDOK)
	{
		CString strFileNamePath = dialog.GetPathName();
		GetDlgItem(IDC_EDIT_PATH)->SetWindowText(strFileNamePath);
		if (m_pDecoder->OpenProject(strFileNamePath))
		{
			CString strContent = m_pDecoder->GetContent();
			strContent.Replace(_T("\n"), _T("\r\n"));
			GetDlgItem(IDC_EDIT_XML)->SetWindowText(strContent);
		}
	}
}

void CDecoderDlg::OnBnClickedBtnLoad()
{
	int nFrame = m_pDecoder->GetFrameLength();
	if (m_pDecoder->LoadProject())
	{
		CString strMsg;
		strMsg.Format(_T("load success : %d frame"), nFrame);
		AfxMessageBox(strMsg);
	}
}

void CDecoderDlg::OnBnClickedBtnUpdate()
{
}

void CDecoderDlg::OnBnClickedBtnPreview()
{
	PreviewDlg dlg(m_pDecoder);
	dlg.DoModal();
}
