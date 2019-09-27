// morph-analysisDlg.h : header file
//

#pragma once

#include <util/common/gui/SimulationDialog.h>
#include <util/common/gui/PlotControl.h>

#include "model.h"

// CMorphAnalysisDlg dialog
class CMorphAnalysisDlg : public CSimulationDialog
{
// Construction
public:
    CMorphAnalysisDlg(CWnd* pParent = NULL);    // standard constructor

// Dialog Data
    enum { IDD = IDD_MORPHANALYSIS_DIALOG };

    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
    HICON m_hIcon;

    CPlotControl m_imgCtrl;
    CPlotControl m_pattCtrls[3];
    model::model_data m_data;
    model::segmentation_helper::autoconfig m_cfg;

    // Generated message map functions
    virtual BOOL OnInitDialog();
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedButton1();
    bool LoadImage(model::bitmap & dst, CBitmap & cdst);
    bool SaveImage(model::bitmap & src);
    afx_msg void OnBnClickedButton4();
    afx_msg void OnBnClickedButton2();
    afx_msg void OnBnClickedButton3();
    afx_msg void OnBnClickedButton6();
    afx_msg void OnBnClickedButton7();
    afx_msg void OnBnClickedButton5();
    BOOL m_bRandSize;
};
