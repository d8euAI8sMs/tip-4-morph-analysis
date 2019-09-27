// morph-analysisDlg.cpp : implementation file
//

#include "stdafx.h"
#include "morph-analysis.h"
#include "morph-analysisDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMorphAnalysisDlg dialog

CMorphAnalysisDlg::CMorphAnalysisDlg(CWnd* pParent /*=NULL*/)
    : CSimulationDialog(CMorphAnalysisDlg::IDD, pParent)
    , m_bRandSize(FALSE)
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMorphAnalysisDlg::DoDataExchange(CDataExchange* pDX)
{
    CSimulationDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_IMG, m_imgCtrl);
    DDX_Control(pDX, IDC_IMG2, m_pattCtrls[0]);
    DDX_Control(pDX, IDC_IMG3, m_pattCtrls[1]);
    DDX_Control(pDX, IDC_IMG4, m_pattCtrls[2]);
    DDX_Text(pDX, IDC_EDIT1, m_data.params.count);
    DDX_Check(pDX, IDC_CHECK1, m_bRandSize);
}

BEGIN_MESSAGE_MAP(CMorphAnalysisDlg, CSimulationDialog)
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(IDC_BUTTON1, &CMorphAnalysisDlg::OnBnClickedButton1)
    ON_BN_CLICKED(IDC_BUTTON4, &CMorphAnalysisDlg::OnBnClickedButton4)
    ON_BN_CLICKED(IDC_BUTTON2, &CMorphAnalysisDlg::OnBnClickedButton2)
    ON_BN_CLICKED(IDC_BUTTON3, &CMorphAnalysisDlg::OnBnClickedButton3)
    ON_BN_CLICKED(IDC_BUTTON6, &CMorphAnalysisDlg::OnBnClickedButton6)
    ON_BN_CLICKED(IDC_BUTTON7, &CMorphAnalysisDlg::OnBnClickedButton7)
    ON_BN_CLICKED(IDC_BUTTON5, &CMorphAnalysisDlg::OnBnClickedButton5)
END_MESSAGE_MAP()

// CMorphAnalysisDlg message handlers

BOOL CMorphAnalysisDlg::OnInitDialog()
{
    CSimulationDialog::OnInitDialog();

    // Set the icon for this dialog.  The framework does this automatically
    //  when the application's main window is not a dialog
    SetIcon(m_hIcon, TRUE);            // Set big icon
    SetIcon(m_hIcon, FALSE);        // Set small icon

    m_imgCtrl.plot_layer.with(model::make_bmp_plot(m_data.csource));
    m_imgCtrl.plot_layer.with(model::make_bmp_plot(m_data.cmask));

    m_pattCtrls[0].plot_layer.with(model::make_bmp_plot(m_data.cpatterns[0]));
    m_pattCtrls[1].plot_layer.with(model::make_bmp_plot(m_data.cpatterns[1]));
    m_pattCtrls[2].plot_layer.with(model::make_bmp_plot(m_data.cpatterns[2]));

    // TODO: Add extra initialization here

    return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CMorphAnalysisDlg::OnPaint()
{
    if (IsIconic())
    {
        CPaintDC dc(this); // device context for painting

        SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

        // Center icon in client rectangle
        int cxIcon = GetSystemMetrics(SM_CXICON);
        int cyIcon = GetSystemMetrics(SM_CYICON);
        CRect rect;
        GetClientRect(&rect);
        int x = (rect.Width() - cxIcon + 1) / 2;
        int y = (rect.Height() - cyIcon + 1) / 2;

        // Draw the icon
        dc.DrawIcon(x, y, m_hIcon);
    }
    else
    {
        CSimulationDialog::OnPaint();
    }
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CMorphAnalysisDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}


bool CMorphAnalysisDlg::LoadImage(model::bitmap & dst, CBitmap & cdst)
{
    CFileDialog fd(TRUE, TEXT("bmp"));
    if (fd.DoModal() == IDOK)
    {
        std::wstring path(fd.GetPathName().GetBuffer());
        std::string asciipath(path.begin(), path.end());
        dst.from_file(asciipath);
        dst.to_cbitmap(cdst);
        return true;
    }
    return false;
}


bool CMorphAnalysisDlg::SaveImage(model::bitmap & src)
{
    CFileDialog fd(TRUE, TEXT("bmp"));
    if (fd.DoModal() == IDOK)
    {
        std::wstring path(fd.GetPathName().GetBuffer());
        std::string asciipath(path.begin(), path.end());
        src.to_file(asciipath);
        return true;
    }
    return false;
}


void CMorphAnalysisDlg::OnBnClickedButton1()
{
    if (LoadImage(m_data.patterns[0], m_data.cpatterns[0])) {
        m_pattCtrls[0].RedrawWindow();
    }
}


void CMorphAnalysisDlg::OnBnClickedButton2()
{
    if (LoadImage(m_data.patterns[1], m_data.cpatterns[1])) {
        m_pattCtrls[1].RedrawWindow();
    }
}


void CMorphAnalysisDlg::OnBnClickedButton3()
{
    if (LoadImage(m_data.patterns[2], m_data.cpatterns[2])) {
        m_pattCtrls[2].RedrawWindow();
    }
}


void CMorphAnalysisDlg::OnBnClickedButton4()
{
    if (LoadImage(m_data.source, m_data.csource)) {
        m_imgCtrl.plot_layer.layers[1]->visible = false;
        m_imgCtrl.RedrawWindow();
    }
}


void CMorphAnalysisDlg::OnBnClickedButton6()
{
    SaveImage(m_data.source);
}


void CMorphAnalysisDlg::OnBnClickedButton7()
{
    model::segmentation_helper h(m_data.params);
    h.process(m_data, m_cfg);
    m_imgCtrl.plot_layer.layers[1]->visible = true;
    m_imgCtrl.RedrawWindow();
}


void CMorphAnalysisDlg::OnBnClickedButton5()
{
    UpdateData(TRUE);
    m_data.params.random_size = (m_bRandSize == TRUE);
    model::segmentation_helper h(m_data.params);
    h.generate(m_data, m_cfg);
    m_imgCtrl.plot_layer.layers[1]->visible = false;
    m_imgCtrl.RedrawWindow();
    m_pattCtrls[0].RedrawWindow();
    m_pattCtrls[1].RedrawWindow();
    m_pattCtrls[2].RedrawWindow();
}
