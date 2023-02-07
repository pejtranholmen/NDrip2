#if !defined(AFX_SP_ESTIMATE_H__42A05B83_0755_11D2_AE2F_00C04F95A11E__INCLUDED_)
#define AFX_SP_ESTIMATE_H__42A05B83_0755_11D2_AE2F_00C04F95A11E__INCLUDED_
#include "..\CoupModel.h"
#include "..\ModelTypes\SimB.h"	// Added by ClassView
#include "..\CoupModelDoc.h"
#include "..\PlotPF\PlotPF\PFCurve.h"

#include "afxcmn.h"
#include "afxwin.h"
#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// SP_Estimate.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// SP_Estimate dialog
//typedef CTypedPtrArray<CPtrArray,float*> CFloatArray;


class SP_Estimate : public CDialog 
{
// Construction
public:
	SP_Estimate(Tab *PointerToTable, int layer, CCoupModelDoc* pDoc,CWnd* pParent = NULL, BOOL m_AllLayers=FALSE);   // standard constructor
//Destructor
	~SP_Estimate();
	float m_SatCond,m_AirEntry,m_Lambda,m_Residual,m_Wilt;
	vector<float>	m_arrPressure, m_arrTheta;
	int m_Num_pF_curve;

// Dialog Data
	//{{AFX_DATA(SP_Estimate)
	enum { IDD = IDD_DIALOG_ESTIMATE_TEX };
	CListBox	m_Measured_pF;
	CListBox	m_Estimated_Coef;
	float	m_Clay;
	float	m_Silt;
	float	m_Sand;
	float	m_Porosity;
	float	m_Est_R2;
	CString	m_Static1;
	CString	m_Static2;
	CString	m_Static3;
	CString	m_Static4;
	float	m_EstimateMin;
	float	m_EstimateMax;
	CString	m_charProfNumber;
	CString	m_charProfName;
	CString	m_charProfLayer;
	float	m_tex2;
	float	m_tex3;
	float	m_tex4;
	float	m_tex5;
	float	m_tex6;
	float	m_tex7;
	float	m_org;
	CString	m_charCUM1;
	CString	m_charCUM2;
	CString	m_charCUM3;
	CString	m_charCUM4;
	CString	m_charCUM5;
	CString	m_charCUM6;
	CString	m_charCUM7;
	CString	m_charCUM8;
	CString	m_charClay;
//	float	m_Residual;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SP_Estimate)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CCoupModelDoc *m_pDoc;
	PFCurve *m_pPFCurve;
	int m_NumEstimated;
	BOOL m_Estimated_pF;
	BOOL m_NewEstimate;
	BOOL m_Retention;
	BOOL m_InitTexture;
	BOOL m_DetailedSandEstimate;
	int m_Layer;
	P *pClayContent, *pSandContent,*pSaturation;
	P *pTotalCond, *pMatCond;
	P *pLambda,*pAirEntry,*pWilt,*pResidual, *pMacro;
	P *pProf, *pRepProf,*pProfLayer;
	int m_Prof,m_RepProf,m_ProfLayer;
	int m_Estimated_Count;
	int m_Listed_Count;
	CObject* m_pView;
	int m_PreviousViewFunction;
	int m_ViewCounter;
	Tab *m_pCTab;
	//ParameterPlot *m_pParChart;
	BOOL m_AllLayers, m_PlotCurve;
	int m_NumberOfLayers;
	void PedoTexture();
	void PedoRetension();
	void Retrieve();

	// Generated message map functions
	//{{AFX_MSG(SP_Estimate)
	afx_msg void OnEstimate();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnEstimatePf();
	afx_msg void OnEditRetention();
	afx_msg void OnEditRetentionEdit();
	afx_msg void OnEditRetentionReset();
	afx_msg void OnSave();
	afx_msg void OnRetrieve();
	afx_msg void OnApply2();
	afx_msg void OnEstimatePf2();
	afx_msg void OnEstimate2();
	afx_msg void OnPlotCurve();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	CSpinButtonCtrl m_LayersChange;
	afx_msg void OnBnClickedButtonAllLayers();
	afx_msg void OnDeltaposSpin2(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedRetrieve();

	afx_msg void OnBnClickedRadio1Sand();
	CButton m_DetailedSand;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SP_ESTIMATE_H__42A05B83_0755_11D2_AE2F_00C04F95A11E__INCLUDED_)
