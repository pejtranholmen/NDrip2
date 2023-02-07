#include "NoaaCoord.h"
#include "../codecogs/maths/approximation/regression/linear.h"
#ifndef COUPSTD
#include "../stdafx.h"
#include "../NewBase/FileScanner.h"
#include "../util/MFC_Util.h"
#endif
#include "../Util/FUtil.hpp"
#include "../Util/StatUtil.h"


#include <sstream>
#include <iostream>
NoaaCoord::NoaaCoord(void)
{
	m_pPGSource=nullptr;
	m_dis_threshold=3000;			// in km
	m_Min_Involvedstations=10;			// Number of involved stations
	m_num_used_stations=0;
	m_DestFileName="";
	m_sumweight=0;
	m_pPGSourceReg=nullptr;
}
NoaaCoord::~NoaaCoord(void)
{
}
bool NoaaCoord::ReadStation(string filename, CPG *pPG)
{
	ifstream file;


#ifndef COUPSTD
	CWinApp* pApp = AfxGetApp();
#endif

	file.open(filename,ios::in);

	string line;
	long nrep=pPG->GetNumRepititions();
	if(!pPG->IsAddInfoSet())
		pPG->SetInfoMode();

	map <long, long> IDStatMap;
	map <long, long>::iterator it;
	for(int i=0; i<nrep;i++) {
		string IDS=pPG->GetVarId(1,i+1);
		IDStatMap[FUtil::AtoInt(IDS.substr(0,6))]=i;
	}
	int nvar=pPG->GetNumVariables();

	if(!file.is_open()) return false;

	file.seekg(0,ios::end);
	unsigned long long totsize=file.tellg();
	file.seekg(0,ios::beg);
	while(file.good()) {
		getline(file,line);
		if(line.find("USAF")==0&&line.find("WBAN")==7) {
			getline(file,line);

			while(file.good()) {
				getline(file,line);
				long ID, Lat, Long, Elev;
				double dLat, dLong, dElev;
				ID=FUtil::AtoInt(line.substr(0,6));

				it=IDStatMap.find(ID);
				if(it!=IDStatMap.end()&&line.size()>90) {
					long iv=it->second;	
					string StatName, CountryIDm, Date_St, Date_End;
					StatName=line.substr(13,20);
					FUtil::trim(StatName);
					CountryIDm=line.substr(43,2);
					Lat=FUtil::AtoInt(line.substr(58,6));
					dLat=double(Lat)/1000;
					Long=FUtil::AtoInt(line.substr(65,7));
					dLong=double(Long)/1000;
					Elev=FUtil::AtoInt(line.substr(73,6));
					dElev=double(Elev)/10;
					Date_St=line.substr(83,8);
					Date_End=line.substr(92,8);
					//pPG->CheckMissingNumbers();
					for(int j=0;j<nvar;j++) {
						pPG->SetVarLat(j+1, dLat, iv+1);
						pPG->SetVarLong(j+1, dLong, iv+1);
						pPG->SetVarAlt(j+1, dElev, iv+1);
						pPG->SetVarStation(j+1, StatName, iv+1);
						pPG->SetVarCountry(j+1, CountryIDm, iv+1);
						pPG->SetVarMissing(j+1, 0, iv+1);
						pPG->SetVarSubstituted(j+1, 0, iv+1);
					}
				}
			}
		}
	}
	pPG->SaveAs(pPG->GetFileName(),true);
	pPG->CloseFile();
	file.close();
	string str, str2;
	str=" File have been updated with precise information of lat, long and Altitude";
	str2="Reading file"+filename;
#ifndef COUPSTD
	MFC_Util::MessageBox(str.c_str(),str2.c_str(),IDOK);
#endif


	return true;
}
CPG* NoaaCoord::CreateNewRandomFillingFile(CPG *pPGSource) {
	CPG *pPG_Dest;
	if(pPGSource!=nullptr) {
		m_pPGSource=pPGSource;
	}
	pPG_Dest= new CPG;

	bool fail=true;
	m_pPGSource->CloseFile();

	FUtil::CopyFile_Std(m_pPGSource->GetFileName(), m_DestFileName);
	pPG_Dest->SetCompleteRead(true);
	pPG_Dest->Open(m_DestFileName);
	pPG_Dest->SetInfoMode();


	long nrep=pPG_Dest->GetNumRepititions();
	int nvar=pPG_Dest->GetNumVariables();
	int numrec=pPG_Dest->GetNumRecords();


#ifndef COUPSTD
	CFileScanner ScanMeanProg(nullptr);
	ScanMeanProg.Create(IDD_PROGRESS,nullptr);
	ScanMeanProg.ShowWindow(SW_SHOW);
#endif
	string str="Creating a new file random selected new values";

	str+=" Output File Name: ";
	str+=m_DestFileName;
#ifndef COUPSTD
	ScanMeanProg.SetWindowText(CString(str.c_str()));
	ScanMeanProg.SetLocalText(_T("% made"));
#endif



	//bool weight=EstimateWeightforInterpolation(Lat1, Long1);
	float fmax;
	float fmin;	
	long subn, newsub;
	long nvarsub=0, totsub=0;

	for(int rep=0; rep<nrep;rep++) {
		pPG_Dest->SetRepititionSequence(rep+1);
		for(int j=1;j<=nvar;j++){
			fmax=MISSING;
			fmin=abs(MISSING);
	/*		VALIDATION_mp_ISCA*/
			size_t prel=size_t(100.*(float(j-1)/float(nvar)/float(nrep)+float(rep)/float(nrep)  ));
#ifndef COUPSTD
			ScanMeanProg.Update(prel);	
#endif
			subn=pPG_Dest->GetVarSubstituted(j, rep+1);
			newsub=pPG_Dest->CalcVarMissingByRandomPeriod(j, rep+1);
			pPG_Dest->SetVarSubstituted(j,subn+newsub,rep+1);
			if(newsub>0) {
				nvarsub++;
				totsub+=newsub;
			}
		}	
	}

	pPG_Dest->CheckMissingNumbers();
	if (pPG_Dest->SaveAs(m_DestFileName, true)) fail = false;

	pPG_Dest->CloseFile();
	m_ResRandomSub.clear();
	if(fail)
		return nullptr;
	else {
		//string str="New File succesfylly created, close year approach applied \n";
		//m_ResRandomSub.push_back(str);
		string str2;
		str2=FUtil::STD_ItoAscii(int(totsub));
		str2.append(" substitutions in ");
		str2+=FUtil::STD_ItoAscii(int(nvarsub));	
		str2.append(" variables\n");

		m_ResRandomSub.push_back(str2);
		str2 = pPG_Dest->GetFileName();
		m_ResRandomSub.push_back(str2);

#ifndef COUPSTD
		//MFC_Util::MessageBox(str2.c_str(),str.c_str(),IDOK);
#endif
		return pPG_Dest;
	}
}
bool NoaaCoord::MergeSimilarStation(CPG *pPG)
{	
	pPG->CloseFile();

	pPG->ReOpen(true);
	string str="Scanning the file for similar stations: ";

#ifndef COUPSTD
	CFileScanner ScanMeanProg(nullptr);
	ScanMeanProg.Create(IDD_PROGRESS,nullptr);
	ScanMeanProg.ShowWindow(SW_SHOW);
	str+=pPG->GetFileName();
	ScanMeanProg.SetWindowText(CString(str.c_str()));
	ScanMeanProg.SetLocalText(_T("Scanning Made"));
#endif


	int dis_threshold=2;// in km
	int elev_threshold=3;// in m
	long nrep=pPG->GetNumRepititions();
	int nvar=pPG->GetNumVariables();
	int no_equalstations=0;
	struct Equal // Number to stations that are equal
	{
		int first;
		int second;
		double distance;
		bool removed;
	};
	vector<Equal> my_equalstations;
	Equal Stations;
	for(int i=0;i<nrep;i++){
			double lat1=pPG->GetVarLat(1,i+1);
			double long1=pPG->GetVarLong(1,i+1);
			double Alt1=pPG->GetVarAlt(1,i+1);
			for(int j=i+1;j<nrep;j++) {
				double lat2=pPG->GetVarLat(1,j+1);
				double long2=pPG->GetVarLong(1,j+1);
				double Alt2=pPG->GetVarAlt(1,j+1);
				double dis=StatUtil::DistanceFunc(lat1,lat2,long1,long2);
				double Elev=abs(Alt1-Alt2);
				if (i !=j){
					if (dis < dis_threshold){
						if (Elev < elev_threshold){
							Stations.first=i+1;
							Stations.second=j+1;
							Stations.removed=false;
							Stations.distance=dis;
							my_equalstations.push_back(Stations);
						}
					}
				}			
			}			
	}
	no_equalstations=my_equalstations.size();
	if (no_equalstations > 0){
		int numrec=pPG->GetNumRecords();
		for (int k=0;k<no_equalstations ;k++){
			pPG->SetRepititionSequence(my_equalstations[k].first);
			for(int j=1;j<=nvar;j++){
#ifndef COUPSTD
					size_t prel=size_t(100.*(float(k)/(float(no_equalstations)+float(j-1)/float(nvar)*float(no_equalstations))));
					ScanMeanProg.Update(prel);
#endif
					for (int i=1;i<=numrec;i++){	
						double v=pPG->GetVarValue(j,i);
						if (v<=float(MISSING)){
							pPG->SetRepititionSequence(my_equalstations[k].second);
							double v2=pPG->GetVarValue(j,i);
							pPG->SetRepititionSequence(my_equalstations[k].first);
							if (v2>MISSING){									
								pPG->SetVarValue(j,i,float(v2));					
							
							}						
						}
					}
				}
		}
	}
	for (int k=0;k<no_equalstations ;k++)
			if(pPG->RemoveRepetition(my_equalstations[k].second)) {

				my_equalstations[k].removed=true;
				for(int kk=k+1;kk<no_equalstations; kk++) {
					if(my_equalstations[k].second<my_equalstations[kk].second)
						my_equalstations[kk].second--;
				}


			}
	str=" File have been checked for similar stations closer than 2 km.\n";
	if(no_equalstations>0) {
		str.append(FUtil::STD_ItoAscii(no_equalstations));
		str.append(" equal stations removed");
	}
	else
		str.append(" No equal stations find");

	string str2;
	str2="File Checked:";
	str2.append(pPG->GetFileName());
#ifndef COUPSTD
	MFC_Util::MessageBox(str.c_str(),str2.c_str(),IDOK);
#endif
return true;
}
CPG* NoaaCoord::CreateNewSpatialCorrelationFile(CPG *pPG)
{
	CPG *pPG_Dest;
	pPG_Dest= new CPG;

	pPG->CloseFile();
	FUtil::CopyFile_Std(pPG->GetFileName(), m_DestFileName);
	pPG_Dest->Open(m_DestFileName);
	pPG_Dest->CloseFile();
	pPG_Dest->ReOpen(true);
	pPG->ReOpen();
	m_pPGSourceReg=pPG;
	int nvarsub=0, totsub=0;
	m_statsub.resize(0);


	long nrep=pPG_Dest->GetNumRepititions();
	int nvar=pPG_Dest->GetNumVariables();
	int numrec=pPG_Dest->GetNumRecords();
	string str="Substitution of missing values by using linear regression based on other stations in file: ";
	bool WithoutIntercept;
	auto RegOtherRepetition = [&]() {
		for (int k = 1; k <= nvar; k++) {
			vector<int> MissingIndex;			// Index of records with missing values to be substituted
			string name = pPG_Dest->GetVarName(k);
			int ipos = name.find("Prec");
			if (ipos == -1)
				ipos = name.find("Snow");
			if (ipos == 0)
				WithoutIntercept = true;
			else
				WithoutIntercept = false;
			for (int j = 1; j <= nrep; j++) {
				Maths::Regression::Linear *pLin;
				pLin = nullptr;
				vector<Maths::Regression::Linear *> Lin;				// vector of class Linear that keeps the regressions
				vector<Maths::Regression::Linear *>::iterator it;
				vector<int> Valid_Rep;					// Index of repetitions to which the regressions are find	(
				Lin.resize(0);
				double MaxR;
				MissingIndex.resize(0);
				size_t Num_Substituted = 0;
				Valid_Rep.resize(0);
				pPG_Dest->SetRepititionSequence(j);

				for (int i = 1; i <= numrec; i++) {
					float v = pPG_Dest->GetVarValue(k, i);
					if (v <= float(MISSING)) {
						MissingIndex.push_back(i);
					}
				}
				MaxR = 0;
				for (int jj = 1; jj <= nrep; jj++) { // This loops is scanning all repetition, make the regression, sort the regression in order 
					vector<double> keep1;     // from the lowest r2 to the highest
					vector<double> keep2;	// The missing values
					double sumkeep1, sumkeep2;
					sumkeep1 = sumkeep2 = 0.;
					if (j != jj) {
						pPG->SetRepititionSequence(jj);
						float vmin = -1.*MISSING, vmax = float(MISSING);
						float v2min = -1.*MISSING, v2max = float(MISSING);
						/*	VALIDATION_mp_ISCAN=int(100.*(float(k-1)/float(nvar)+float(j-1)/(float(nrep)*float(nvar))+float(jj-1)/(float(nrep)*float(nrep)*float(nvar))));
						ScanMeanProg.Update();*/
						for (int i = 1; i <= numrec; i++) {

							float v = pPG_Dest->GetVarValue(k, i);
							if (v>MISSING) {
								float v2 = pPG->GetVarValue(k, i);
								if (v2>MISSING) {
									if (WithoutIntercept) {
										sumkeep1 += v;
										sumkeep2 += v2;
										keep1.push_back(sumkeep1);
										keep2.push_back(sumkeep2);
									}
									else {
										keep1.push_back(double(v));
										keep2.push_back(double(v2));
									}
									if (v<vmin) vmin = v;
									if (v>vmax) vmax = v;
									if (v2<v2min) v2min = v2;
									if (v2>v2max) v2max = v2;
								}
							}
						}
						int	num_recdata = keep1.size();
						if (num_recdata > 4 && vmax>vmin&&v2max>v2min) {

							Maths::Regression::Linear *pA = new Maths::Regression::Linear(num_recdata, keep2.data(), keep1.data());//need to check
							int InsertPos = 0;
							for (size_t i = 0; i<Lin.size(); i++) {
								if (pA->getCoefficient()>Lin[i]->getCoefficient())
									InsertPos = i + 1;
							}
							Lin.insert(Lin.begin() + InsertPos, pA);
							Valid_Rep.insert(Valid_Rep.begin() + InsertPos, jj);
						}
					}
				} // End of loop for selection of stations with linear regression
				for (int ireg = Lin.size() - 1; ireg >= 0; ireg--) {
					int indexRep = Valid_Rep[ireg];
					pPG->SetRepititionSequence(indexRep);
					int PriorSubstituted;
					PriorSubstituted = Num_Substituted;
					for (size_t i = 0; i<MissingIndex.size(); i++) {

						float v = pPG->GetVarValue(k, MissingIndex[i]);
						if (v>MISSING) {  // Check if not missing in the variable with the best regression
							double newvalue; // calculate new number
							if (WithoutIntercept) {
								if (v <= 0)
									newvalue = 0;
								else
									newvalue = v*Lin[ireg]->getSlope();
							}
							else
								newvalue = Lin[ireg]->getValue(double(v));


							pPG_Dest->SetVarValue(k, MissingIndex[i], float(newvalue));   // Set Number into PG Struture
							MissingIndex.erase(MissingIndex.begin() + i);  // erase from vector of Missing values	
							Num_Substituted++;
							i--;
							//pPG_Dest->SetRepititionSequence(indexRep);//here added
						}
					}
					if (Num_Substituted != PriorSubstituted) {
						SUBSTAT stat;
						stat.VarSub = k;
						stat.VarSubRep = j;
						stat.VarUsed = k;

						stat.VarUsedRep = indexRep;
						stat.nSub = Num_Substituted - PriorSubstituted;
						stat.R2 = Lin[ireg]->getCoefficient();
						if (WithoutIntercept) {
							stat.A0 = 0;
							stat.A1 = Lin[ireg]->getSlope();
						}
						else {
							stat.A0 = Lin[ireg]->getIntercept();
							stat.A1 = Lin[ireg]->getSlope();
						}
						m_statsub.push_back(stat);
					}
				}
				size_t n;
				n = pPG_Dest->GetVarSubstituted(k, j);
				pPG_Dest->SetVarSubstituted(k, Num_Substituted + n, j);
				pPG_Dest->SetVarMissing(k, MissingIndex.size(), j);
				if (Num_Substituted>0) {
					nvarsub++;
					totsub += Num_Substituted;

				}
				while (Lin.size()>0) {
					delete Lin[0];
					Lin.erase(Lin.begin());
				}
				pPG_Dest->WriteNewBuffer(j);
				//pPG_Dest->WriteNewBuffer(j);
			} // End of loop for repetitions 

		} // End of loop for variables to be substituted


	};
	auto RegSameRepetition = [&]() {
		for (int k = 1; k <= nvar; k++) {
			vector<int> MissingIndex;			// Index of records with missing values to be substituted
			string name = pPG_Dest->GetVarName(k);
			int ipos = name.find("Prec");
			if (ipos == -1)
				ipos = name.find("Snow");
			if (ipos == 0)
				WithoutIntercept = true;
			else
				WithoutIntercept = false;

				Maths::Regression::Linear *pLin;
				pLin = nullptr;
				vector<Maths::Regression::Linear *> Lin;				// vector of class Linear that keeps the regressions
				vector<Maths::Regression::Linear *>::iterator it;
				vector<int> Valid_Rep;					// Index of repetitions to which the regressions are find	(
				Lin.resize(0);
				double MaxR;
				MissingIndex.resize(0);
				size_t Num_Substituted = 0;
				Valid_Rep.resize(0);
				for (int i = 1; i <= numrec; i++) {
					float v = pPG_Dest->GetVarValue(k, i);
					if (v <= float(MISSING)) {
						MissingIndex.push_back(i);
					}
				}
				MaxR = 0;
				for (int jj = 1; jj <= nvar; jj++) { // This loops is scanning all variables, make the regression, sort the regression in order 
					vector<double> keep1;     // from the lowest r2 to the highest
					vector<double> keep2;	// The missing values
					double sumkeep1, sumkeep2;
					sumkeep1 = sumkeep2 = 0.;
					if (k != jj) {
						float vmin = -1.*MISSING, vmax = float(MISSING);
						float v2min = -1.*MISSING, v2max = float(MISSING);
						/*	VALIDATION_mp_ISCAN=int(100.*(float(k-1)/float(nvar)+float(j-1)/(float(nrep)*float(nvar))+float(jj-1)/(float(nrep)*float(nrep)*float(nvar))));
						ScanMeanProg.Update();*/
						for (int i = 1; i <= numrec; i++) {

							float v = pPG->GetVarValue(k, i);
							if (v > MISSING) {
								float v2 = pPG->GetVarValue(jj, i);
								if (v2 > MISSING) {
									if (WithoutIntercept) {
										sumkeep1 += v;
										sumkeep2 += v2;
										keep1.push_back(sumkeep1);
										keep2.push_back(sumkeep2);
									}
									else {
										keep1.push_back(double(v));
										keep2.push_back(double(v2));
									}
									if (v < vmin) vmin = v;
									if (v > vmax) vmax = v;
									if (v2 < v2min) v2min = v2;
									if (v2 > v2max) v2max = v2;
								}
							}
						}
						int	num_recdata = keep1.size();
						if (num_recdata > 4 && vmax > vmin&&v2max > v2min) {

							Maths::Regression::Linear *pA = new Maths::Regression::Linear(num_recdata, keep2.data(), keep1.data());//need to check
							int InsertPos = 0;
							for (size_t i = 0; i < Lin.size(); i++) {
								if (pA->getCoefficient() > Lin[i]->getCoefficient())
									InsertPos = i + 1;
							}
							Lin.insert(Lin.begin() + InsertPos, pA);
							Valid_Rep.insert(Valid_Rep.begin() + InsertPos, jj);
						}
					}
				} // End of loop for selection of stations with linear regression
				for (int ireg = Lin.size() - 1; ireg >= 0; ireg--) {
					int indexRep = Valid_Rep[ireg];
					int PriorSubstituted;
					PriorSubstituted = Num_Substituted;
					for (size_t i = 0; i < MissingIndex.size(); i++) {

						float v = pPG->GetVarValue(indexRep, MissingIndex[i]);
						if (v > MISSING) {  // Check if not missing in the variable with the best regression
							double newvalue; // calculate new number
							if (WithoutIntercept) {
								if (v <= 0)
									newvalue = 0;
								else
									newvalue = v*Lin[ireg]->getSlope();
							}
							else
								newvalue = Lin[ireg]->getValue(double(v));


							pPG_Dest->SetVarValue(k, MissingIndex[i], float(newvalue));   // Set Number into PG Struture
							MissingIndex.erase(MissingIndex.begin() + i);  // erase from vector of Missing values	
							Num_Substituted++;
							i--;
							//pPG_Dest->SetRepititionSequence(indexRep);//here added
						}
					}
					string str;
#ifndef COUPSTD 
					MFC_Util m;
					str = "Substution of Var " + STD_ItoAscii(k) + " by " + STD_ItoAscii(Num_Substituted-PriorSubstituted) + " values from variable " + STD_ItoAscii(indexRep);
						;
					m.SetPaneMessage(str);
#endif
					if (Num_Substituted != PriorSubstituted) {
						SUBSTAT stat;
						stat.VarSub = k;
						stat.VarSubRep = indexRep;
						stat.VarUsed = k;

						stat.VarUsedRep = indexRep;
						stat.nSub = Num_Substituted - PriorSubstituted;
						stat.R2 = Lin[ireg]->getCoefficient();
						if (WithoutIntercept) {
							stat.A0 = 0;
							stat.A1 = Lin[ireg]->getSlope();
						}
						else {
							stat.A0 = Lin[ireg]->getIntercept();
							stat.A1 = Lin[ireg]->getSlope();
						}
						m_statsub.push_back(stat);
					}
				}
				size_t n;
				n = pPG_Dest->GetVarSubstituted(k);
				pPG_Dest->SetVarSubstituted(k, Num_Substituted + n);
				pPG_Dest->SetVarMissing(k, MissingIndex.size());
				if (Num_Substituted > 0) {
					nvarsub++;
					totsub += Num_Substituted;

				}
				while (Lin.size() > 0) {
					delete Lin[0];
					Lin.erase(Lin.begin());
				}

			} 

};


	if (nrep > 1) RegOtherRepetition();
	else RegSameRepetition();
	
	pPG_Dest->WritePGFile();
	pPG_Dest->CloseFile();

	str=" File have been created\n";
	str.append(pPG_Dest->GetFileName());
	string str2="New File succesfully created ! \n";
	str2+=FUtil::STD_ItoAscii(totsub);
	str2.append(" substitutions in ");
	str2+=FUtil::STD_ItoAscii(nvarsub);	
	str2.append(" variables");
#ifndef COUPSTD
	MFC_Util::MessageBox(str.c_str(),str2.c_str(),IDOK);
#endif
	m_pPGSourceReg=pPG;



	return pPG_Dest;
}
bool NoaaCoord::TemporalCorrelation(CPG *pPG)
{
	// filling time gaps with another period originating from same station
	// using a random selected sequence of lenght x days from other years.



return true;
}
float NoaaCoord::NewInterpolatedValue(int varno, long rec_no)
{			
	vector<double> keep;
	//keep.clear();	
		for(int k=0;k<m_num_used_stations;k++){
			m_pPGSource->SetRepititionSequence(m_disindex[k]);
			float v=m_pPGSource->GetVarValue(varno,rec_no);
			if (v>MISSING){
				if (m_distance[k]<m_dis_threshold){
					keep.push_back(double(v));				
				}
			}
		}
		int	num_recdata=keep.size();//number of stations with data at day i
		if (num_recdata>=m_num_used_stations){
			double sum_valueweight=0;
			for (int k=0;k<num_recdata;k++){
				sum_valueweight+=keep[k]*m_weight[k];
			}
			double newvalue=sum_valueweight/m_sumweight;
			return float(newvalue); 
		} 
		else
			return MISSING;
}
bool NoaaCoord::EstimateWeightforInterpolation(double Lat1, double Long1) 
{
	long nrep=m_pPGSource->GetNumRepititions();
	m_distance.resize(0);
	m_weight.resize(0);
	m_disindex.resize(0);
	m_sumweight=0.;
	// Finding the distances to other stations and putting all including the weights into a sorted vector
	for(int k=1;k<=nrep;k++){
		m_pPGSource->SetRepititionSequence(k);
		double lat2=m_pPGSource->GetVarLat(1,k);
		double long2=m_pPGSource->GetVarLong(1,k);				
		double dis=StatUtil::DistanceFunc(Lat1,lat2,Long1,long2);
		if (dis<1)
			dis=1;
		double weigh=1/pow(dis,2);
		int InsertPos=0;
		for(size_t kk=0;kk<m_distance.size();kk++) {
			if(dis>m_distance[kk]) 
			InsertPos=kk+1;
		}
		m_distance.insert(m_distance.begin()+InsertPos,dis);
		m_weight.insert(m_weight.begin()+InsertPos,weigh);
		m_disindex.insert(m_disindex.begin()+InsertPos,k);		
	}
	m_num_used_stations=min(m_Min_Involvedstations, int(nrep));
	for(int k=0;k<m_num_used_stations;k++)
			m_sumweight+=m_weight[k];
		
	return true;
}
CPG* NoaaCoord::CreateNewSinglePosFile(double Lat1, double Long1, CPG *pPGSource) {
	CPG *pPG_Dest;
	if(pPGSource!=nullptr) {
		m_pPGSource=pPGSource;
	}
	if(!EstimateWeightforInterpolation(Lat1, Long1)) return nullptr;
	pPG_Dest= new CPG;

	bool fail=false;
	m_pPGSource->CloseFile();
	m_pPGSource->ReOpen();
	long nrep=m_pPGSource->GetNumRepititions();
	int nvar=m_pPGSource->GetNumVariables();
	int numrec=m_pPGSource->GetNumRecords();
	
	pPG_Dest->SetCompleteRead(true);
	pPG_Dest->CreateNewFile(nvar, numrec);
	pPG_Dest->SetInfoMode();


	string str="Creating a new file representing new position ";
/*
#ifndef COUPSTD
	CFileScanner ScanMeanProg(nullptr);
	ScanMeanProg.Create(IDD_PROGRESS,nullptr);
	ScanMeanProg.ShowWindow(SW_SHOW);


	str+=" Output File Name: ";
	str+=m_DestFileName;
	ScanMeanProg.SetWindowText(str.c_str());
	ScanMeanProg.SetLocalText("% made");
#endif
	*/

	//bool weight=EstimateWeightforInterpolation(Lat1, Long1);
	float fmax;
	float fmin;	
	for(int j=1;j<=nvar;j++){
		fmax=MISSING;
		fmin=abs(MISSING);
/*
#ifndef COUPSTD
		VALIDATION_mp_ISCAN=int(100.*(float(j-1)/float(nvar)  ));
		ScanMeanProg.Update();	
#endif
		*/
		for(int i=0;i<numrec;i++){
			if(j==1) {
			// Read and set the value of date/time
				int time=m_pPGSource->GetLongTime(i+1);
				pPG_Dest->SetLongTime(i+1, time );
			}			
			double newvalue= NewInterpolatedValue(j, i+1);
			if(newvalue>MISSING) {
				if(newvalue>fmax) fmax=newvalue;
				if(newvalue<fmin) fmin=newvalue;
			}
			pPG_Dest->SetVarValue(j,i+1, float(newvalue)); 
		}
		string txt;
		txt=m_pPGSource->GetVarName(j);
		pPG_Dest->SetVarName(j, txt);
		txt=m_pPGSource->GetVarUnit(j);
		pPG_Dest->SetVarUnit(j, txt);
		pPG_Dest->SetVarAlt(j, double(MISSING));
		pPG_Dest->SetVarLong(j, Long1);
		pPG_Dest->SetVarLat(j, Lat1);
		txt="Estimated";
		pPG_Dest->SetVarId(j,txt);
		txt=m_pPGSource->GetVarPos(j);
		pPG_Dest->SetVarPos(j, txt);
		pPG_Dest->SetVarMax(j, fmax);
		pPG_Dest->SetVarMin(j, fmin);
		pPG_Dest->SetVarSubstituted(j,0);

	}	


	pPG_Dest->CheckMissingNumbers();
	pPG_Dest->SaveAs(m_DestFileName,true);
	pPG_Dest->CloseFile();

	if(fail)
		return nullptr;
	else
		return pPG_Dest;
}
bool NoaaCoord::SplitPGFile() 
{
	CPG pPG_Dest;
	bool fail=true;
	if(m_pPGSource==nullptr) return false;
	
	
	long nrep=m_pPGSource->GetNumRepititions();
	int nvar=m_pPGSource->GetNumVariables();
	int numrec=m_pPGSource->GetNumRecords();


	m_pPGSource->WriteSeparateSplitFiles();
	

	return true;

}
void NoaaCoord::SetSourceFile(CPG *pPGSource) {
	m_pPGSource=pPGSource;
}
CPG* NoaaCoord::CreateNewInterPolatedFile(float MaxDaysThreshold, CPG *pPGSource) {
	CPG *pPG_Dest;
	if(pPGSource!=nullptr) {
		m_pPGSource=pPGSource;
	}
	pPG_Dest= new CPG;

	bool fail=true;
	m_pPGSource->CloseFile();
	size_t n;
	bool equalfilename=false;
	if(m_pPGSource->GetFileName()==m_DestFileName) {
		equalfilename=true;
		m_DestFileName+="xx";
	}
	FUtil::CopyFile_Std(m_pPGSource->GetFileName(), m_DestFileName);
	pPG_Dest->Open(m_DestFileName);
	

	pPG_Dest->CloseFile();
	pPG_Dest->ReOpen(true);


	pPG_Dest->SetInfoMode();

	long nrep=pPG_Dest->GetNumRepititions();
	int nvar=pPG_Dest->GetNumVariables();
	int numrec=pPG_Dest->GetNumRecords();
	
#ifndef COUPSTD
	CFileScanner ScanMeanProg(nullptr);
	ScanMeanProg.Create(IDD_PROGRESS,nullptr);
	ScanMeanProg.ShowWindow(SW_SHOW);
#endif

	string str="Creating a new file interpolated values";
	str+=" Output File Name: ";
	str+=m_DestFileName;

#ifndef COUPSTD
	ScanMeanProg.SetWindowText(CString(str.c_str()));
	ScanMeanProg.SetLocalText(_T("% made"));
#endif

	pPG_Dest->SetVarFunction(5);
	int min_gap;
	min_gap=int(1440.*MaxDaysThreshold);
	long subn, newsub;
	long nvarsub=0, totsub=0;
	bool valid;
	if(!pPG_Dest->IsGood()) pPG_Dest->ClearFile();
	for(int rep=0; rep<nrep;rep++) {
		valid=pPG_Dest->SetRepititionSequence(rep+1, true);
		for(int j=1;j<=nvar;j++){
			size_t prel=size_t(100.*(float(j-1)/float(nvar)/float(nrep)+float(rep)/float(nrep)  ));
#ifndef COUPSTD
			ScanMeanProg.Update(prel);
#endif
			pPG_Dest->AddSelection(j);
			subn=pPG_Dest->GetVarSubstituted(j, rep+1);
			newsub=pPG_Dest->CalcVarMissingSubstitute(j,j,false,rep+1, min_gap);
			pPG_Dest->SetVarSubstituted(j,subn+newsub,rep+1);
			if(newsub>0) {
				nvarsub++;
				totsub+=newsub;

			}
		}	
		valid=pPG_Dest->WriteNewBuffer(rep+1);
	}


	pPG_Dest->CheckMissingNumbers();
	pPG_Dest->SaveAs(m_DestFileName,true);
	if(pPG_Dest->IsOpen())
		fail=false;
	pPG_Dest->CloseFile();




	if(fail)
		return nullptr;
	else {
		string str="New File succesfylly created, regression applied \n";
		string str2;
		str2=FUtil::STD_ItoAscii(int(totsub));
		str2.append(" substitutions in ");
		str2+=FUtil::STD_ItoAscii(int(nvarsub));	
		str2.append(" variables\n");
		str2+="FileName: ";
		str2+=pPG_Dest->GetFileName();
#ifndef COUPSTD
		MFC_Util::MessageBox(str2.c_str(),str.c_str(),IDOK);
#endif
		return pPG_Dest;

	}
	return nullptr;
}
CPG* NoaaCoord::CreateNewGridFile(double LatNorth, double LatSouth, double LongEast, double LongWest, int n_Lat, int n_Long, CPG *pPGSource) {
	CPG *pPG_Dest;
	pPG_Dest= new CPG;

	bool fail=true;
	if(pPGSource==nullptr&&m_pPGSource==nullptr) return nullptr;
	else if(pPGSource!=nullptr)
		m_pPGSource=pPGSource;

	m_pPGSource->CloseFile();
	m_pPGSource->ReOpen();

	int nrep_new= (n_Lat)*(n_Long);
	int rep=0;
	vector<double> GridLat;
	vector<double> GridLong;
	GridLat.resize(n_Lat);
	GridLong.resize(n_Long);
	
	double Latstep=(LatNorth-LatSouth)/n_Lat;
	double Longstep=(LongEast-LongWest)/n_Long;
	for (int i=0;i<n_Lat;i++){
		GridLat[i]=LatNorth-Latstep*i-Latstep/2;
	}
	for (int i=0;i<n_Long;i++){
		GridLong[i]=LongEast-Longstep*i-Longstep/2;
	}
	string str="Creating a new file with grid data from ";
/*
#ifndef COUPSTD
	CFileScanner ScanMeanProg(nullptr);
	ScanMeanProg.Create(IDD_PROGRESS,nullptr);
	ScanMeanProg.ShowWindow(SW_SHOW);

	str+=FUtil::STD_ItoAscii(n_Lat);
	str+=" x ";
	str+=FUtil::STD_ItoAscii(n_Long);
	str+=" positions. Output File Name: ";
	str+=m_DestFileName;
	ScanMeanProg.SetWindowText(str.c_str());
	ScanMeanProg.SetLocalText("% made");
#endif
	*/


	long nrep=m_pPGSource->GetNumRepititions();
	int nvar=m_pPGSource->GetNumVariables();
	int numrec=m_pPGSource->GetNumRecords();
	pPG_Dest->SetCompleteRead(true);
	pPG_Dest->CreateNewFile(nvar, numrec,nrep_new);
	pPG_Dest->SetInfoMode();

	
	float fmax;
	float fmin;
								
	for (int Index_lat=0;Index_lat<n_Lat;Index_lat++){		
		for (int Index_long=0;Index_long<n_Long;Index_long++){			
			bool weight=EstimateWeightforInterpolation( GridLat[Index_lat], GridLong[Index_long]);
			rep++;
			pPG_Dest->SetRepititionSequence(rep);
			for(int j=1;j<=nvar;j++){
				fmax=MISSING;
				fmin=abs(MISSING);
				for(int i=0;i<numrec;i++){
					if(j==1) {
						// Read and set the value of date/time
						int time=m_pPGSource->GetLongTime(i+1);
						pPG_Dest->SetLongTime(i+1, time );
					}
					double newvalue= NewInterpolatedValue(j, i+1);
					if(newvalue>MISSING) {
						if(newvalue>fmax) fmax=newvalue;
						if(newvalue<fmin) fmin=newvalue;
					}


					pPG_Dest->SetVarValue(j,i+1, float(newvalue));		
				}
/*
#ifndef COUPSTD
				VALIDATION_mp_ISCAN=int(100.*(float(rep-1)/float(nrep_new)+float(j-1)/(float(nrep_new)*float(nvar))  ));
				ScanMeanProg.Update();		
#endif*/
	//			for(int j=1;j<=nvar;j++){
					string txt;
					txt=m_pPGSource->GetVarName(j);
					pPG_Dest->SetVarName(j, txt, rep);
					txt=m_pPGSource->GetVarUnit(j);
					pPG_Dest->SetVarUnit(j, txt, rep);
					pPG_Dest->SetVarAlt(j, double(MISSING), rep);
					pPG_Dest->SetVarLat(j, GridLat[Index_lat], rep);
					pPG_Dest->SetVarLong(j, GridLong[Index_long], rep);

					pPG_Dest->SetVarMax(j, fmax, rep);
					pPG_Dest->SetVarMin(j, fmin, rep);
					
					txt=FUtil::STD_ItoAscii(Index_lat+1); txt+="("; txt+=FUtil::STD_ItoAscii(n_Lat);txt+=") ";
					txt+=FUtil::STD_ItoAscii(Index_long+1);txt+="("; txt+=FUtil::STD_ItoAscii(n_Long);txt+=") ";
					pPG_Dest->SetVarId(j,txt, rep);
					txt=m_pPGSource->GetVarPos(j);
					pPG_Dest->SetVarPos(j, txt, rep);
					pPG_Dest->SetVarSubstituted(j,0, rep);
				//}
			}
		}
	}

	pPG_Dest->CheckMissingNumbers();
	pPG_Dest->SaveAs(m_DestFileName,true);
	pPG_Dest->CloseFile();

	str=" File have been created\n";
	str.append(pPG_Dest->GetFileName());
	string str2="New File succesfully created !";
#ifndef COUPSTD	
	MFC_Util::MessageBox(str.c_str(),str2.c_str(),IDOK);
#endif



	if(fail)
		return nullptr;
	else
		return pPG_Dest;

}