#if !defined(AFX_SP_PF_POINTS_H__8C656344_0AB0_11D2_AE35_00C04F95A11E__INCLUDED_)
#define AFX_SP_PF_POINTS_H__8C656344_0AB0_11D2_AE35_00C04F95A11E__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// SP_PF_POINTS.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// SP_PF_POINTS DAO recordset

class SP_PF_POINTS : public CDaoRecordset
{
public:
	SP_PF_POINTS(CDaoDatabase* pDatabase = NULL);
	DECLARE_DYNAMIC(SP_PF_POINTS)

// Field/Param Data
	//{{AFX_FIELD(SP_PF_POINTS, CDaoRecordset)
	short	m_SiteNumber;
	short	m_ReplicateNumber;
	short	m_Layer;
	float	m_Psi1;
	float	m_Theta1;
	float	m_Psi2;
	float	m_Theta2;
	float	m_Psi3;
	float	m_Theta3;
	float	m_Psi4;
	float	m_Theta4;
	float	m_Psi5;
	float	m_Theta5;
	float	m_Psi6;
	float	m_Theta6;
	float	m_Psi7;
	float	m_Theta7;
	float	m_Psi8;
	float	m_Theta8;
	float	m_Psi9;
	float	m_Theta9;
	float	m_Psi10;
	float	m_Theta10;
	float	m_Psi11;
	float	m_Theta11;
	float	m_Psi12;
	float	m_Theta12;
	float	m_Psi13;
	float	m_Theta13;
	float	m_Psi14;
	float	m_Theta14;
	float	m_Psi15;
	float	m_Theta15;
	//}}AFX_FIELD

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SP_PF_POINTS)
	public:
	virtual CString GetDefaultDBName();		// Default database name
	virtual CString GetDefaultSQL();		// Default SQL for Recordset
	virtual void DoFieldExchange(CDaoFieldExchange* pFX);  // RFX support
	//}}AFX_VIRTUAL

// Implementation
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SP_PF_POINTS_H__8C656344_0AB0_11D2_AE35_00C04F95A11E__INCLUDED_)
