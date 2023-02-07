#if !defined(AFX_SP_TEXTURE_H__8C656343_0AB0_11D2_AE35_00C04F95A11E__INCLUDED_)
#define AFX_SP_TEXTURE_H__8C656343_0AB0_11D2_AE35_00C04F95A11E__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// SP_Texture.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// SP_Texture DAO recordset

class SP_Texture : public CDaoRecordset
{
public:
	SP_Texture(CDaoDatabase* pDatabase = NULL);
	DECLARE_DYNAMIC(SP_Texture)

// Field/Param Data
	//{{AFX_FIELD(SP_Texture, CDaoRecordset)
	short	m_SiteNumber;
	short	m_ReplicateNumber;
	short	m_Layer;
	float	m_Clay;
	float	m_Sand;
	float	m_Porosity;
	//}}AFX_FIELD

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SP_Texture)
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

#endif // !defined(AFX_SP_TEXTURE_H__8C656343_0AB0_11D2_AE35_00C04F95A11E__INCLUDED_)
