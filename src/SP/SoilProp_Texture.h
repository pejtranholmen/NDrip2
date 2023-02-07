#if !defined(AFX_SOILPROP_TEXTURE_H__82B185F3_0A64_11D2_AE34_00C04F95A11E__INCLUDED_)
#define AFX_SOILPROP_TEXTURE_H__82B185F3_0A64_11D2_AE34_00C04F95A11E__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// SoilProp_Texture.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// SoilProp_Texture recordset

class SoilProp_Texture : public CRecordset
{
public:
	SoilProp_Texture(CDatabase* pDatabase = NULL);
	DECLARE_DYNAMIC(SoilProp_Texture)

// Field/Param Data
	//{{AFX_FIELD(SoilProp_Texture, CRecordset)
	//}}AFX_FIELD


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SoilProp_Texture)
	public:
	virtual CString GetDefaultConnect();    // Default connection string
	virtual CString GetDefaultSQL();    // Default SQL for Recordset
	virtual void DoFieldExchange(CFieldExchange* pFX);  // RFX support
	//}}AFX_VIRTUAL

// Implementation
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SOILPROP_TEXTURE_H__82B185F3_0A64_11D2_AE34_00C04F95A11E__INCLUDED_)
