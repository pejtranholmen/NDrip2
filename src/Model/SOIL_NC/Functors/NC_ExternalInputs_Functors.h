#pragma once
#include "../../../std.h"
enum class N_Fertilization_Sw { off,Parameters,PG_file,Plant_Stage_Table,Year_Based_Table_1_app,Year_Based_Table_3_app };
enum class FertlizationIndexFileName_Sw{ off,Using_Single_PG_file,Using_Multi_PG_file };
enum class DepositionIndexFileName_Sw{ off,Using_Single_PG_file,Using_Multi_PG_file };
enum class N_Irrigation_Sw{ off,on };
enum class N_Deposition_Sw { off, Parameters,PG_file };
enum class ManureInput_Sw{ Parameters, PG_file };
enum class ReadYearBasedManagementTable_Sw{off,on};

class Day_FertRates {
public:
	Day_FertRates(vector<double> FertDayNo, vector<double> FertRates) : FertDayNo(FertDayNo), FertRates(FertRates)
	{};
	double operator() (double DayNum) {
		for (size_t i = 0; i < FertDayNo.size(); i++)
			if (size_t(DayNum+0.01) == size_t(FertDayNo[i] + 0.01)) return FertRates[i];
		return 0.;
	};
private:
	vector<double> FertDayNo, FertRates;
};

class YearStage_FertRates {
public:
	YearStage_FertRates(vector<double> YearStage, vector<double> YearRate, vector<double> YearFrequency, vector<double> FertDayNo) :
		YearStage(YearStage), YearRate(YearRate), YearFrequency(YearFrequency)
		
	{vector<double> FertDay; FertDay.push_back(FertDayNo.front()); f_YearApp = Day_FertRates(FertDay, YearRate); };
	double operator() (double Year, double DayNum) {
		double N_Fert_App = 0.;
		for (size_t jj = 0; jj < YearStage.size(); jj++) {
			if (Year <= YearStage[jj]) {
				if (int(Year) % int(YearFrequency[jj]) == 0) 
					N_Fert_App = f_YearApp(DayNum);
			}
		}
		return N_Fert_App;
}

private:
	vector<double> YearStage, YearRate, YearFrequency;
	function<double(double)> f_YearApp;

};
class YearTable_FertRates {
public:
	YearTable_FertRates(vector<double> FertYear, vector<double> FertDayApp1, vector<double> FertRateApp1):FertRateApp1(FertRateApp1)
		{
		for (size_t i = 0; i < FertDayApp1.size(); ++i) FertDay1.push_back(int(FertDayApp1[i] + 0.001));
		m_CountYear = 0; for (size_t i = 0; i < FertYear.size(); i++) IntFertYear.push_back(int(FertYear[i]));
		yearapp3 = false;
	};

	YearTable_FertRates(vector<double> FertYear, vector<double> FertDayApp1, vector<double> FertRateApp1,
		 vector<double> FertDayApp2, vector<double> FertRateApp2,
		 vector<double> FertDayApp3, vector<double> FertRateApp3) : FertRateApp1(FertRateApp1),
		FertRateApp2(FertRateApp2), FertRateApp3(FertRateApp3)
	{
		for (size_t i = 0; i < FertDayApp1.size(); ++i) FertDay1.push_back(int(FertDayApp1[i] + 0.001));
		for (size_t i = 0; i < FertDayApp2.size(); ++i) FertDay2.push_back(int(FertDayApp2[i] + 0.001));
		for (size_t i = 0; i < FertDayApp3.size(); ++i) FertDay3.push_back(int(FertDayApp3[i] + 0.001));
		m_CountYear = 0; for (size_t i = 0; i < FertYear.size(); i++) IntFertYear.push_back(int(FertYear[i]));
		yearapp3 = true;
	};

	double operator() (double Year, double DayNum) {
		double N_Fert_App = 0.;
		
		UpdateYearCount(int(Year));
		if (int(Year) == IntFertYear[m_CountYear]) {
			if (int(DayNum) == FertDay1[m_CountYear]) return FertRateApp1[m_CountYear];
			if (yearapp3) {
				if (int(DayNum) == FertDay2[m_CountYear]) return FertRateApp2[m_CountYear];
				if (int(DayNum) == FertDay3[m_CountYear]) return FertRateApp3[m_CountYear];
			}
		}

		return N_Fert_App;
	};

private:
	void UpdateYearCount(int Year) {
		while (Year > IntFertYear[m_CountYear] && IntFertYear.size() > m_CountYear + 1) m_CountYear++;
	};
	vector<int> IntFertYear;
	vector<int> FertDay1, FertDay2, FertDay3;
	vector<double> FertRateApp1, FertRateApp2, FertRateApp3;
	size_t m_CountYear;
	bool yearapp3;
	

};
