// PreviewDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Decoder.h"
#include "PreviewDlg.h"
#include "afxdialogex.h"


// PreviewDlg 对话框

IMPLEMENT_DYNAMIC(PreviewDlg, CDialogEx)

PreviewDlg::PreviewDlg(IBaseGraphDecoder * pDecoder, CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DLG_PREVIEW, pParent)
	, m_pDecoder(pDecoder)
	, m_pBuf(nullptr)
{
	m_pBuf = new DWORD[1920 * 1080];
}

PreviewDlg::~PreviewDlg()
{
	delete m_pBuf;
	m_pBuf = nullptr;
}

void PreviewDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

void PreviewDlg::PaintEvent()
{
	if (m_pDecoder)
	{
		m_pDecoder->GetDynamicFrame(0, m_pBuf);

		GetClientRect(m_rtPaintArea);

		int nWidth = 1920;
		int nHeight = 1080;

		BITMAPINFO bitmapinfo;
		bitmapinfo.bmiHeader.biSize = sizeof(bitmapinfo.bmiHeader);
		bitmapinfo.bmiHeader.biCompression = BI_RGB;
		bitmapinfo.bmiHeader.biBitCount = 32;
		bitmapinfo.bmiHeader.biWidth = nWidth;
		bitmapinfo.bmiHeader.biHeight = -nHeight;
		bitmapinfo.bmiHeader.biPlanes = 1;
		CRect rtScene(0, 0, nWidth, nHeight);

		float fAreaWidth = m_rtPaintArea.Width();
		float fAreaHeight = m_rtPaintArea.Height();
		float fPrjWidth = rtScene.Width();
		float fPrjHeight = rtScene.Height();
		float fWindowWidth = fAreaHeight * fPrjWidth / fPrjHeight;
		float fWindowHeight = fAreaWidth * fPrjHeight / fPrjWidth;
		if (fWindowWidth > fAreaWidth)
		{
			fWindowWidth = fWindowHeight * fPrjWidth / fPrjHeight;
		}
		else
		{
			fWindowHeight = fWindowWidth * fPrjHeight / fPrjWidth;
		}

		int nXOffset = (fAreaWidth - fWindowWidth) / 2;
		int nYOffset = (fAreaHeight - fWindowHeight) / 2;

		CRect rtWindow(0, 0, fWindowWidth, fWindowHeight);
		CDC * pDC = GetDC();

		CDC dcMem;
		CBitmap bmpMem;
		dcMem.CreateCompatibleDC(NULL);
		bmpMem.CreateCompatibleBitmap(pDC, m_rtPaintArea.Width(), m_rtPaintArea.Height());
		CBitmap *pOldBmp = dcMem.SelectObject(&bmpMem);
		HGDIOBJ hOldBrush = dcMem.SelectStockObject(NULL_BRUSH);
		HGDIOBJ hOldPen = dcMem.SelectStockObject(WHITE_PEN);
		dcMem.FillRect(CRect(0, 0, m_rtPaintArea.Width(), m_rtPaintArea.Height()), &CBrush(RGB(180, 180, 180)));
		int nOldMode = ::SetStretchBltMode(dcMem.GetSafeHdc(), HALFTONE);
		// 整体拷贝
		StretchDIBits(dcMem.GetSafeHdc(), nXOffset, nYOffset, rtWindow.Width(), rtWindow.Height(), 0, 0, rtScene.Width(), rtScene.Height(), m_pBuf, &bitmapinfo, DIB_RGB_COLORS, SRCCOPY);

		CString strFrame;
		strFrame.Format(_T("Length:%d Frame:%d"), (int)(0), 0);
		dcMem.DrawText(strFrame, m_rtPaintArea, DT_SINGLELINE);
		::SetStretchBltMode(dcMem.GetSafeHdc(), nOldMode);

		pDC->BitBlt(m_rtPaintArea.left, m_rtPaintArea.top, m_rtPaintArea.Width(), m_rtPaintArea.Height(), &dcMem, 0, 0, SRCCOPY);
		dcMem.SelectObject(hOldPen);
		dcMem.SelectObject(hOldBrush);
		dcMem.DeleteDC();

		ReleaseDC(pDC);
	}
}


BEGIN_MESSAGE_MAP(PreviewDlg, CDialogEx)
END_MESSAGE_MAP()


// PreviewDlg 消息处理程序
