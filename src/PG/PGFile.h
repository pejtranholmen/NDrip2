#pragma once
struct PGPOS
{
	size_t first;
	size_t second;
	double weight;
	bool shift;
	bool nodiscretehit;
};

#include "./PG_Stream.h"
#include "../ModelTypes/CoupTypes.h"
class CPGFile: public PG_Stream
{
public:
	CPGFile(void);
	~CPGFile(void);

	std::vector <float> GetVarVector(size_t varno);
	std::vector <double> GetVarDVector(size_t varno);
	PGPOS GetPos( size_t minut, size_t varno=-1);
	size_t GetHitPos(size_t ivar, size_t minst, size_t minend);
	size_t GetPrevHitPos(size_t ivar) {return size_t(m_ActHit[ivar]-1);};
	bool Check_Update_ActHit(size_t ivar, size_t minut);
	void ResetPos();
	void* GetRecordPointer(size_t irec, size_t rep=1);
	vector<float> GetRecordDataVector(size_t irec, size_t rep = 1);
	bool SetWritePosition(size_t irec);

	bool SetPointerToAllData(float *pvar);
	void SetRecordPointer(float *pvar);
	void SetDatePointer(float *pvar, size_t ist=0);
	void SetDatePointer(unsigned int *pMinute, size_t ist = 0);

	void SetRecordPointer(float *pvar, size_t ist, size_t num);
	void SetRecordPointer_2Mix(float *pvar1,float *pvar2, size_t ist, size_t num);
	
	void SetVarPointer(float *pvar, size_t ist, size_t num);
	void SetVarPointer_2Mix(float *pvar1, float* pvar2, size_t ist, size_t num);
	void SetRecordPointerInterpolate(float *pvar, size_t ist, size_t num, float weight, const vector<float> &vfloat);
	void SetRecordInterpolated(const vector<float>& vprior, size_t ist, float weight, const vector<float>& vfloat);

	void SetMissingValueRecordPointer(size_t ist, size_t num);



		
	string	ExportToFile(bool secondhalf=false);
	string	ExportToFile(string filename, bool split=false, bool secondhalf=false);




	void	Reset();
	string	GetDate(size_t);



	string GetFileName(bool OnlyTitle=false);
	bool	IsAddInfoSet();


	

	void	CheckEquiDistance(bool MonthDistance=false);
	size_t	GetEquiDistance() {return m_EquiDistance;};
	bool	IsEquiDistance();
	void	CheckIncreaseTime();
	bool	IsTimeIncreasing();
	
	bool	SetRepititionIdNo(size_t, bool resetindex=false);
	bool    CheckMissingNumbers();
	void    SetNormalTimeInterval(unsigned int);
	unsigned int GetNormalTimeInterval();
	size_t	RemoveMissingRecords();
	bool	RemoveRepetition(size_t RepNo);
	bool	AddRepetition(size_t RepNo);
	void	SetCompleteRead(bool);
	void	SetCompleteRepRead(bool);
	bool	GetCompleteRead();
	unsigned int GetLongTimeStart();
	void	SetLongTimeStart(unsigned int min) { pg.status.MinStart = min; };
	bool	SetAllLongTimesFromMinDiff(bool WithMissingAssignment_To_Var=true);

	unsigned int GetLongTimeEnd();
	string	GetVarDateHourMean(size_t recno, size_t hours);
	size_t	GetNumRecordsHour(size_t hours);
	bool	FindKeyValues();

	string	GetVarDes(size_t, size_t repit=1);
	string	GetVarName(size_t, size_t repit=1);
	string	GetVarUnit(size_t, size_t repit=1);
	string	GetVarId(size_t, size_t repit=1);
	string	GetVarPos(size_t, size_t repit=1);
	float	GetVarMin(size_t, size_t repit=1);
	float	GetVarMax(size_t, size_t repit=1);
	float	GetVarMean(size_t, size_t repit=1);

	bool AdjustSize(size_t irecnumber, size_t varnumber, size_t repit=1);
	void	SetDate(size_t, string);

	bool	SetLongTime(size_t, size_t, bool WithinBuffer=false);
	bool	SetRecordMissing(size_t);
	void	SetVarValue(size_t varno, size_t recno, float v);

	void	SetVarName(size_t, string, size_t repit=1);
	void	SetVarUnit(size_t, string, size_t repit=1);
	void	SetVarId(size_t, string, size_t repit=1);
	void	SetVarPos(size_t, string, size_t repit=1);
	void	SetVarMin(size_t, float, size_t repit=1);
	void	SetVarMax(size_t, float, size_t repit=1);

	size_t  GetJumpBacksNum();
	string  GetJumpBackDates(size_t rec);

	size_t	GetNumVariables();
	size_t	GetNumRecords();
	unsigned int GetNumRepititions();
	size_t	GetNumRecordsBufSize();
	size_t	GetNumBuffers();
	size_t	GetPrevBuffer();
	

	bool	SetNumRecords(size_t irec);
	bool	ReAllocate(size_t irec, size_t ivar, size_t repit=1);
	bool    ReCalculateMinMax(size_t repit=1);
		
	float	GetVarValue(size_t varno) {return pg.Var.v[varno];}; // Return the value in Record 1
	float	GetVarValue(size_t varno, size_t recno);
	float	GetVarValueRead(size_t varno, size_t recno);

	void	SetVarKey(size_t, size_t, size_t repit=1);
	void	SetVarLat(size_t, double, size_t repit=1);
	void	SetVarLong(size_t, double, size_t repit=1);
	void	SetVarAlt(size_t, double, size_t repit=1);
	void	SetVarCountry(size_t, string, size_t repit=1);
	void	SetVarStation(size_t, string, size_t repit=1);
	void	SetVarMissing(size_t, size_t value, size_t repit=1);
	void	SetVarSubstituted(size_t, size_t value, size_t repit=1);
	void	SetEndPoints_ValidValue();
	float	GetLastValid(size_t i) {return m_LastValid[i-1];};
	float	GetFirstValid(size_t i) {return m_FirstValid[i-1];};


	unsigned int GetLongTime(size_t, bool WithinBuffer=false);
	size_t	GetCurrentRecords() {return pg.status.b.buf_Size;};
	bool Is_Id_FieldSensitive();
	bool Is_LatLong_FieldSensitive();
	void CheckDescriptors();

protected:
	
	std::vector <float> v_accepted_mean,v_accepted_min,v_accepted_max,v_accepted_sd;
	std::vector <float> v_all_mean,v_all_min,v_all_max,v_all_sd;

	std::vector <DistStat> v_accepted;
	std::vector <DistStat> v_all;
	std::vector <DistStat> v_WithinDay;
	std::vector <DistStat> v_WithinYear;

	bool GapProblemFinded;
	bool m_MissingValueExist, m_MissingTested;
	bool m_IncreaseTimeTest, m_IncreaseTime;

	bool m_IdField_Sensitive, m_LatLong_FieldSensitive;




	size_t m_EquiDistance;









};

