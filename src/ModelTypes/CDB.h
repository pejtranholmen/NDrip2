#include "./SimB.h"
class CDB : public SimB {

public:
	void MR_AddSelection(std::string des, std::string key);
	void MR_ResetSelection();
	void MR_SetSelection(size_t index,std::string des, std::string key);
	std::string MR_GetKeySelection(size_t index);
	std::string MR_GetSelection(size_t index);
	size_t Get_DB_Index() { return DB_Index; };
	size_t MR_GetNumberOfSelections();
	void MR_SetNextRepetition(size_t newvalue) { m_LastSelectedRepetition=newvalue;};
	std::string GetStringValue();
	std::string GetName();
	void SetName(std::string);
	int MR_Get_Dim();
	void MR_Set_Dim(int);
	MR_METHOD MR_Get_Method();
	void MR_Set_Method(MR_METHOD );
	bool SetValue(std::string str);
	size_t GetLastSelectedRepetition() {return m_LastSelectedRepetition;};
	~CDB();
	CDB();						// Constructor
	CDB(std::string, std::string);						// Constructor
	CDB(DB_types);						// Constructor
	CDB(int);						// Constructor
	CDB(std::string);						// Constructor
	bool SetStringValue(std::string);		// Set temporary value to value. DSM
	//bool Apply();				// set values to tmpvalues
	long GetChaDate(size_t);
	std::string GetChaParValue(size_t);
	std::string GetChaParKey(size_t);
	int GetChaParIndex(size_t);
	size_t GetNumChaDates();
	bool SetChaDateAndValues(long, std::string, std::string);
	bool DeleteChaDate(size_t);
	bool ResetChaDates();
protected:
	int m_MR_Dimension;
	size_t DB_Index;
	size_t m_LastSelectedRepetition;
	MR_METHOD m_MR_Method;
	std::string	ID_Group;					// variable value
	std::string ID_Selection;
	std::vector<long> ChaDatelong;
	std::vector<int> ChaParIndex;
	std::vector<std::string> ChaParSelection, ChaParKeySelection;
	std::vector<std::string> MR_ParSelection, MR_KeySelection;
	std::vector<int> m_MR_MethodArray, m_MR_DimensionArray;
};
