#pragma once
#include "./GetPFCurve.h"
class PFCurve :
	public GetPFCurve
{
public:
	PFCurve(void);
	~PFCurve(void);
	bool SetPF_Pressure(std::vector<float> pressure, bool extra=false);
	bool Set_Header(NEWHEADER head, bool extra=false);
	bool SetPF_Theta(size_t row, std::vector<float> theta, bool extra = false);
	bool SetPF_Coef(size_t row, PFCOEF pfcoef, bool extra=false);
	bool SetPF_Texture(size_t row, std::vector<float> texture, bool extra=false);
	bool AddNewProfile(bool Extra=false);
	bool DeleteProfile(size_t row, bool Extra=false);
	bool CleanMainDataBase();
	bool UpdateActualProfile();
	NEWHEADER GetNewHeader();
	bool Export_Import_ActualProfile_CSVFile(size_t keyno, bool Export = true);

private:
	std::vector<float> m_IniPressure;
	std::vector<float> m_theta_ini;
	std::vector<float> m_texture_ini;
	PFCOEF m_pfcoef_ini;

};

