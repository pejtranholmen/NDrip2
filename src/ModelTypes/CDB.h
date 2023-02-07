#include "./SimB.h"
class CDB : public SimB {

public:
	void MR_AddSelection(string des, string key);
	void MR_ResetSelection();
	void MR_SetSelection(size_t index,string des, string key);
	string MR_GetKeySelection(size_t index);
	string MR_GetSelection(size_t index);
	size_t Get_DB_Index() { return DB_Index; };
	size_t MR_GetNumberOfSelections();
	void MR_SetNextRepetition(size_t newvalue) { m_LastSelectedRepetition=newvalue;};
	string GetStringValue();
	string GetName();
	void SetName(string);
	int MR_Get_Dim();
	void MR_Set_Dim(int);
	MR_METHOD MR_Get_Method();
	void MR_Set_Method(MR_METHOD );
	bool SetValue(string str);
	size_t GetLastSelectedRepetition() {return m_LastSelectedRepetition;};
	~CDB();
	CDB();						// Constructor
	CDB(string, string);						// Constructor
	CDB(DB_types);						// Constructor
	CDB(int);						// Constructor
	CDB(string);						// Constructor
	bool SetStringValue(string);		// Set temporary value to value. DSM
	//bool Apply();				// set values to tmpvalues
	long GetChaDate(size_t);
	string GetChaParValue(size_t);
	string GetChaParKey(size_t);
	int GetChaParIndex(size_t);
	size_t GetNumChaDates();
	bool SetChaDateAndValues(long, string, string);
	bool DeleteChaDate(size_t);
	bool ResetChaDates();
protected:
	int m_MR_Dimension;
	size_t DB_Index;
	size_t m_LastSelectedRepetition;
	MR_METHOD m_MR_Method;
	string	ID_Group;					// variable value
	string ID_Selection;
	vector<long> ChaDatelong;
	vector<int> ChaParIndex;
	vector<string> ChaParSelection, ChaParKeySelection;
	vector<string> MR_ParSelection, MR_KeySelection;
	vector<int> m_MR_MethodArray, m_MR_DimensionArray;
};
