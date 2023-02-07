#pragma once

struct PFCOEF
{
	float Lambda;
	float AirEntry;
	float Saturation;
	float Residual;
	float MatConductivity;
	float TotConductivity;
	float Wilting;
	float UpperDepth;
	float LowerDepth;
	float Gen_Alfa;
	float Gen_N;
	float Gen_M;
	float Mualem_n;
	float UpperBoundary;
	float LowerBoundary;
	float N_SrCoef;
	float N_SECoef;
};
struct A_TEXTURE
{
	float clay;
	float f_mjla;
	float g_mjla;
	float f_mo;
	float g_mo;
	float f_sand;
	float g_sand;
};
struct OLDHEADER
{
	char numP[3];
	char repP[2];
	char place[16];
	char country[6];
	char koord[16];
	char comfil[8];
};

struct BC_ESTIMATE
{
	bool good;
	float Residual;
	float Lambda;
	float AirEntry;
	float R2;
	size_t n;
	size_t Layer;
};

#include "PFStorage.h"
class GaussKreuger;
class GetPFCurve :
	public PFStorage
{
public:
	GetPFCurve(void);
	~GetPFCurve(void);
	void ReadHeaderFile();
	void ReadLevelFile();
	bool SaveNewFormat();
	bool SaveRecord(bool NewRecord=false);
	bool InitSaveRecord(size_t numprofile);
	std::vector<float> GetPF_Pressure(size_t record);
	std::vector<float> GetPF_Theta(size_t layer);
	PFCOEF GetPF_Coef(size_t layer);
	std::vector<float> GetTexture(size_t layer);
	const size_t GetKey(int nump, int numr);
	const size_t GetKey() { return m_ActProfile.KeyProf; };
	const std::string GetCountry() { return m_ActProfile.County; };
	const size_t GetRecLayers() { return m_ActProfile.RecLayers; };

	const size_t GetNewKey(std::string oldkey);
	const size_t GetRecord(size_t key);
	const size_t GetHeaderRecord(size_t key);
	const size_t GetNumLayers() {return m_ActProfile.NumLayers;};
	const size_t GetNumPF_Pressures() {return m_NumMeasured_PFSteps;}; 
	const std::streampos GetInsertRecord(size_t rec);
	bool IsProfile() {if(m_ActProfile.KeyProf>0) return true; else return false;};
	bool InitNewScan(bool Extra=false);
	bool ScanNewHeaders(NEWHEADER *head, bool Extra=false);
	NEWHEADER GetHeader(std::streampos rec, bool Extra=false);
	NEWHEADER GetHeader(size_t rec, bool Extra = false);
	NEWHEADER GetHeader(int nump, int numr) {if(m_AllProfiles.size()>GetHeaderRecord(GetKey(nump, numr))) 
		return m_AllProfiles[GetHeaderRecord(GetKey(nump,numr))];else return m_IniHeader;};
	bool SetHeader(size_t rec) {if(rec<m_AllProfiles.size()) {
		m_ActProfile=m_AllProfiles[rec]; return true;}
	else
		return false;
	};
	
	bool SetHeader(int nump, int numr) {if(m_AllProfiles.size()>GetHeaderRecord(GetKey(nump, numr))) { 
		m_ActProfile=m_AllProfiles[GetHeaderRecord(GetKey(nump,numr))];
		return true;
		}
		else return false;};
	void SetTopRow(int toprow) {m_TopRow=toprow;};
	int GetTopRow() {return m_TopRow;};
	BC_ESTIMATE Estimate_BC(std::vector<double>* Theta,std::vector<double>* Psi,  double ThetaMax, double FixResidual);
	BC_ESTIMATE Estimate_BC_Texture(double sand, double clay, double poro);
	double Estimate_HydraulicCond_Texture(double sand, double clay, double poro);
	size_t SetComments(std::string kalle);
	std::string GetComments();
	std::vector<std::string> GetCommentsLines();
	std::string GetComFileName(); 
	bool DeleteActHeader();
	bool ExpandToNewLayerActHeader(size_t newnum_layers);
	size_t GetActHeaderRecord() {
		return GetHeaderRecord(m_ActProfile.KeyProf);
	};




	bool UpdateHeaderFile();
	bool ExportMainDataBasetoCSVFile();


	
	size_t m_NumMeasured_PFSteps;
protected:
	double m_ConvCondUnit;

private:
	GaussKreuger *m_Convertor;
	std::vector<PFCOEF> m_MeanLayers;
	std::vector<PFCOEF> m_BoundaryLayers;
	std::vector<A_TEXTURE> m_TextureLayers;
	std::vector<float> m_PFSteps;
	std::vector<float> m_OrgLayers;
	int m_TopRow;

};

