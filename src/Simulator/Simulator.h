#pragma once
#include "../Model/HBV/HBVModel.h"

#include "../NewBase/ModelBase.h"
#include "../Model/SOIL/SoilModel.h"
#include "../Model/PLANT/PlantModel.h"
#include "../Model/Atmosphere/AtmModel.h"
#include "../Model/Structure/StructureModel.h"
#include "../Model/PLANT_NC/PlantOrgModel.h"
#include "../Model/SOIL_NC/SoilOrgModel.h" 

struct VALID_PGFILE
{
	CPG* file;
	CPG* simvalfile;
	vector<size_t> varindex;
	size_t	ActPos;
	vector<float> simvalues;
	vector<float> accsimvalues;
	double accduration;
	double prevSimTime;
	size_t count;
	vector<size_t> val_listindex;
	bool MeanValueOut;
	long HalfDuration;
	size_t PrevPos;
	CResiduals *pRes;
	size_t FirstValidRecord;
};
struct VALID_RESULT
{
	vector<float> residual; 
};
class ModelInfo;
class Simulator	
	{
	public:
		Simulator(void);
		void SetSimPointer(ModelBase *);
		bool Start(bool Multi=false, bool First=false);
		bool Run(bool Multi=false);
		bool End(bool Multi=false, bool Final=false, bool First=false);
		Structure* GetSimStructurePointer();
		
		~Simulator(void);
		
		HBVModel m_HBVModel;
		SoilModel m_SoilModel;
		PlantModel m_PlantModel;
		AtmModel m_AtmModel;
		StructureModel m_StructureModel;
		PlantOrgModel m_PlantOrgModel;
		SoilOrgModel m_SoilOrgModel;
		ModelBase *m_pSim;
		ModelInfo* m_pModelInfo;
		ModelMap* m_pModelMap;
		Structure* m_pModelStructure;

	protected:

		double AccPeriod;

	private:
		void Stat_Ini();
		void Stat_Calc();
		void Stat_Reset();

		void Store_Outputs(bool MultiFlag=false, size_t num_timescaling=0);
		bool Store_Write(bool MultiFlag=false, bool Final=false);
		
		bool Valid_Check();
		bool Valid_Check_Precise(size_t i);
		bool Valid_Check_Interval(size_t i);
		bool Valid_Ini(bool Multi=false, bool First=true);
		bool Valid_End(bool Multi=false, bool Final=false, bool First=false);
		bool Valid_Evaluation(bool MultiFlag=false, bool Final=false);
		
		void Reset_DrivingFiles();
		
		bool WShed_Ini();
		bool WShed_Store();
		bool WShed_StoreWrite();

		double GetSimValValue(size_t index)
		{if(m_pValVar[index].pBase->Is_Vector()) return ((OutVector*)m_pValVar[index].pBase)->GetValue(m_pValVar[index].TabIndex);
		else return ((OutSingle*)m_pValVar[index].pBase)->GetValue(); }	;
	
		vector<SIMB> m_Outputs, m_MultiOutputs, m_WshedOutputs;	// The two possible time serie outputs for single runs and Multi Runs	
		vector<OUTPUT_SUMMARY_RESULTS> Stat;
		vector<float> m_OutFloat, m_OutFloatWShed;
		vector<size_t> m_ShiftSign;
		SimB *m_ShiftPointer;
		vector<VALID_TEST_RESULTS> m_ValTest;
		vector<VALID_PGFILE> m_ValPGFile;
		vector<VALID_RESULT> m_ValRes;
		vector<SIMB> m_pValVar;
		double m_DayError;
		
	};

