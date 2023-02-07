#include "ChangeFromFile.h"
#include"../../std.h"
#include"../../ModelTypes/NewModelType.h"
#include "../ModelBase.h"
#include "../../SoilData/GetSoilStorage.h"
#include "../../Util/FUtil.hpp"
#include "../../Util/ModelUtil.h"
#include "../Doc.h"




ChangeFromFile::ChangeFromFile()
{
}


ChangeFromFile::~ChangeFromFile()
{
}
bool ChangeFromFile::LinkedChangeToParameters(Par *pSimB,  size_t option, size_t index) {
	Func* pFunc;
	SimB *pSimB1;
	switch (option) {
		case 1:

			pSimB1=GetPtr(FUNCTION,"ReCalc Profile Function");
			pFunc=dynamic_cast<Func*>(pSimB1);
			if(pFunc!=nullptr)
				pFunc->MakeOperation();
			pSimB1=GetPtr(FUNCTION,"ReCalc Soil Function");
			pFunc=dynamic_cast<Func*>(pSimB1);
			if(pFunc!=nullptr)
				return pFunc->MakeOperation();
			pSimB1=GetPtr(FUNCTION,"ReCalc Soil Thermal Function");
			pFunc=dynamic_cast<Func*>(pSimB1);
			if(pFunc!=nullptr)
				return pFunc->MakeOperation();

		case 2:


			pSimB1=GetPtr(FUNCTION,"ReCalc Soil Function");
			pFunc=dynamic_cast<Func*>(pSimB1);
			if(pFunc!=nullptr)
				return pFunc->MakeOperation();

		break;
		case 3:


			pSimB1=GetPtr(FUNCTION,"ReCalc Organic Soil Profile");
			pFunc=dynamic_cast<Func*>(pSimB1);
			if(pFunc!=nullptr)
				return pFunc->MakeOperation();

		break;
		case 4:


			pSimB1=GetPtr(FUNCTION,"ReCalc Soil Thermal Function");
			pFunc=dynamic_cast<Func*>(pSimB1);
			if(pFunc!=nullptr)
				return pFunc->MakeOperation();

		break;
		case 5:


			pSimB1=GetPtr(FUNCTION,"ThermalScalingCoefEstimate");
			pFunc=dynamic_cast<Func*>(pSimB1);
			if(pFunc!=nullptr)
				return pFunc->MakeOperation();

		break;
		case 6:


			pSimB1=GetPtr(FUNCTION,"Init Sunshine Function");
			pFunc=dynamic_cast<Func*>(pSimB1);
			if(pFunc!=nullptr)
				return pFunc->MakeOperation();

		break;
		case 7:
			//CalibIndex
			if (index != string::npos) {

				
				Sw *pSw;
				pSw = (Sw*)GetPtr(SWITCH, "CalibrationMode");
				pSw->SetIntValue(index + 1);
				auto value = dynamic_cast<P*>(pSimB)->GetValue(index);
				FCSV *pFCSV;
				string nameofpointer = "ParCalibrationFile";
					nameofpointer += FUtil::STD_ItoAscii(index + 1);
				pFCSV=dynamic_cast<FCSV*>(GetPtr(CSVFILE, nameofpointer));
				ReadDataFromCSVFileandSetDataCorrespondingtoIndexValue(pFCSV, size_t(value));


			}


			break;


		case 10001:


		break;



	};

	return false;
}
void ChangeFromFile::Get_IGBP_Soil()
{
/*	Ps *pP,*pLong,*pLat,*pY, *pC_Humus,*pN_Humus,*pN_Litter, *pC_Litter,*pN_Litter2, *pC_Litter2, *pIndex;
	Sw *pSw;

	pSw=dynamic_cast<Sw*>(GetPtr(SWITCH,"IGBP LandInfo"));
	size_t SwitchValue=pSw->GetIntValue();
	pIndex=dynamic_cast<Ps*>(GetPtr(PAR_SINGLE, "InputFileNameIndex"));

	pP=dynamic_cast<Ps*>(GetPtr(PAR_SINGLE, "LandID"));
	size_t index=pP->GetValue();
	pLong=dynamic_cast<Ps*>(GetPtr(PAR_SINGLE, "X-coordinate"));
	pY=dynamic_cast<Ps*>(GetPtr(PAR_SINGLE, "Y-coordinate"));
	pLat=dynamic_cast<Ps*>(GetPtr(PAR_SINGLE, "Latitude"));
	pC_Humus=dynamic_cast<Ps*>(GetPtr(PAR_SINGLE,"Init H CTot"));
	pN_Humus=dynamic_cast<Ps*>(GetPtr(PAR_SINGLE,"Init H NTot"));
	pC_Litter=dynamic_cast<Ps*>(GetPtr(PAR_SINGLE,"Init L1 CTot"));
	pN_Litter=dynamic_cast<Ps*>(GetPtr(PAR_SINGLE,"Init L1 NTot"));
	pC_Litter2=dynamic_cast<Ps*>(GetPtr(PAR_SINGLE,"Init L2 CTot"));
	pN_Litter2=dynamic_cast<Ps*>(GetPtr(PAR_SINGLE,"Init L2 NTot"));
	string file, var;
	file=m_DataBaseDirectory; file+="Climate\\";
	file+="soil_igbp_bc_watch_0p5deg_capUM6.6.nc";
	var="csoil";
   float fvar[67420];	

   size_t multirun=-1;
   try {
   // Open the file for read access
   NcFile dataFile(file, NcFile::read);

   // Retrieve the variable named "Csoil"
  // if(PARAMETERS_mp_ACTUALNOMULRUN<=0) return;
   NcVar data=dataFile.getVar(var);
   if(data.isNull()) { 

	   return;
   }
   
   float value;
   if(SwitchValue>1) {
	   data.getVar(fvar);

	   value=fvar[index];
	   ((Ps*)pC_Humus)->SetValue(value*1000);
	   //pC_Humus->Apply();
	  ((Ps*)pC_Humus)->MR_Set_TabValue(multirun, value*1000);

	   pN_Humus->SetValue(value*50);
	   //pN_Humus->Apply();
	   pN_Humus->MR_Set_TabValue(multirun, value*50);

	   pC_Litter->SetValue(value*10);
	   pC_Litter->MR_Set_TabValue(multirun, value*10);

	   pN_Litter->SetValue(value*.2);
	   pN_Litter->MR_Set_TabValue(multirun, value*.2);

	   pC_Litter2->SetValue(value*10);
	   pC_Litter2->MR_Set_TabValue(multirun, value*10);

	   pN_Litter2->SetValue(value*.05);
	   pN_Litter2->MR_Set_TabValue(multirun, value*.05);

   }


   data=dataFile.getVar("longitude");   if(data.isNull()) {  return;  };  
		data.getVar(fvar);   pLong->SetValue(fvar[index]); pLong->Apply();
		pLong->MR_Set_TabValue(multirun, fvar[index]);
float value1, value2;
		value1=fvar[index+1];value2=fvar[index-1];
   data=dataFile.getVar("latitude");   if(data.isNull()) {  return;  };   
		data.getVar(fvar);   pLat->SetValue(fvar[index]); pLat->Apply();   pY->SetValue(fvar[index]); pY->Apply(); 
		pY->MR_Set_TabValue(multirun, fvar[index]);	pLat->MR_Set_TabValue(multirun, fvar[index]);
		value1=fvar[index+1];value2=fvar[index-1];
	 if(SwitchValue>2) {
		   data=dataFile.getVar("vwilt");   if(data.isNull()) {  return;  }; 
				P *pPt=dynamic_cast<P*>(GetPtr("Table Parameters","Wilting Point"));
				data.getVar(fvar);  for(size_t ilay=0; ilay< pPt->GetSize(); ilay++) { pPt->SetValue(ilay, 100*fvar[index]);   pPt->MR_Set_TabValue(ilay, multirun, 100*fvar[index]); }; 
  
			data=dataFile.getVar("bexp");   if(data.isNull()) {  return;  }; 
				pPt=dynamic_cast<P*>(GetPtr("Table Parameters","Lambda"));
		
				data.getVar(fvar); value=1./fvar[index]; for(size_t ilay=0; ilay< pPt->GetSize(); ilay++) { pPt->SetValue(ilay, value);   pPt->MR_Set_TabValue(ilay, multirun, value); }; 
 
			data=dataFile.getVar("sathh");   if(data.isNull()) {  return;  };   
				pPt=dynamic_cast<P*>(GetPtr("Table Parameters","Air Entry"));
				data.getVar(fvar);  for(size_t ilay=0; ilay< pPt->GetSize(); ilay++) { pPt->SetValue(ilay, fvar[index]);pPt->MR_Set_TabValue(ilay, multirun, 100*fvar[index]); }; 
		
		   data=dataFile.getVar("vsat");   if(data.isNull()) {  return;  };   
				pPt=dynamic_cast<P*>(GetPtr("Table Parameters","Saturation"));
				data.getVar(fvar);  for(size_t ilay=0; ilay< pPt->GetSize(); ilay++) { pPt->SetValue(ilay, 100*fvar[index]); pPt->MR_Set_TabValue(ilay, multirun, 100*fvar[index]); }; 

			data=dataFile.getVar("satcon");   if(data.isNull()) {  return;  };   
				pPt=dynamic_cast<P*>(GetPtr("Table Parameters","Total Conductivity"));
				data.getVar(fvar);  for(size_t ilay=0; ilay< pPt->GetSize(); ilay++) { pPt->SetValue(ilay, 86400*fvar[index]); pPt->MR_Set_TabValue(ilay, multirun, 86400*fvar[index]); }; 
				pPt=dynamic_cast<P*>(GetPtr("Table Parameters","Matrix Conductivity"));
				data.getVar(fvar);  for(size_t ilay=0; ilay< pPt->GetSize(); ilay++) { pPt->SetValue(ilay, 86400*fvar[index]); pPt->MR_Set_TabValue(ilay, multirun, 86400*fvar[index]); }; 		
	}
   }
   catch(NcException& e)
   {
	   return;
  }



  */


}void ChangeFromFile::Get_Mascur_Soil()
{
#ifndef COUPSTD
	vector<float> values;
	Ps *pP=dynamic_cast<Ps*>(GetPtr(PAR_SINGLE, "LandID"));
	size_t index=size_t(pP->GetValue());

	if(!pGetSoil->IsOpen()) {
			string filename;
			string message;

			Sw *pSw;
			pSw = dynamic_cast<Sw*>(GetPtr(SWITCH, "Mascur soil info file"));
			pSw->SetIntValue(1);




			FCSV *pCSV = dynamic_cast<FCSV*>(GetPtr(CSVFILE, "MASCUR File"));
			if (pCSV != nullptr) {
				filename = pCSV->GetStrValue();
				bool valid = pCSV->CheckIfExistAndOpen();
				if (!valid) {
					message = "Specify the file name for the Mascur soil file";
#ifndef COUPSTD
					MFC_Util::MessageBox(message);
					FILEDLG FileDlg = MFC_Util::GetFileDialog("*.csv", "", "Comma separated file (*.csv)|*.csv|");
					if (FileDlg.opengood != IDOK) {
						return;
					}
					filename = FileDlg.filename;
					cout << message;
#else 
					cout << message+'\n';
					cin >> filename;

#endif
					pCSV->SetValue(filename);
					if (pCSV->CheckFileName())
						pGetSoil->Init(filename);
					else
						return;

				}
			}
			else
				return;

	}


	SOILHEADER header=pGetSoil->GetSoilHeaderByKey(index);
	if (header.RecordPos == string::npos) {
#ifdef COUPSTD
		cout << "Problems when reading MASCUR soil file \n soil key:";
			cout << FUtil::STD_ItoAscii(index)+ '\n';
			while (header.RecordPos == string::npos) {
				header = pGetSoil->GetSoilHeaderByKey(index);


				cout << "Problems when reading MASCUR soil file \n soil key:";
				cout << FUtil::STD_ItoAscii(index) + '\n';
			}



#endif
		
	}
	P *pSand=dynamic_cast<P*>(GetPtr(PAR_TABLE,"SandContent"));
	P *pClay=dynamic_cast<P*>(GetPtr(PAR_TABLE,"ClayContent"));
	P* pUDep=(P*)GetPtr(PAR_TABLE,"Upper Depth");
	P* pLDep=(P*)GetPtr(PAR_TABLE,"Lower Depth");
	P* pDryBulk=(P*)GetPtr(PAR_TABLE,"Init BulkDensity");
	P* pInitC=(P*)GetPtr(PAR_TABLE,"Init C Cont");
	P* pInitN=(P*)GetPtr(PAR_TABLE,"Init N Cont");
	P* pWilt=(P*)GetPtr(PAR_TABLE,"Wilting Point");
	P* pSat=(P*)GetPtr(PAR_TABLE,"Saturation");
	P* pAirEntry=(P*)GetPtr(PAR_TABLE,"Air Entry");
	P* pLambda=(P*)GetPtr(PAR_TABLE,"Lambda");
	P* pRes=(P*)GetPtr(PAR_TABLE,"Residual Water");
	P* pCondTot=(P*)GetPtr(PAR_TABLE,"Total Conductivity");
	P* pCondMat=(P*)GetPtr(PAR_TABLE,"Matrix Conductivity");
	P* pMaxRootDepth=(P*)GetPtr(PAR_TABLE,"Root LowestDepth");
	Ps *pDrainLeveL=dynamic_cast<Ps*>(GetPtr(PAR_SINGLE, "DrainLevel"));
	Ps *pDrainSpacing=dynamic_cast<Ps*>(GetPtr(PAR_SINGLE, "DrainLevel"));



			// for(size_t ilay=0; ilay< pPt->GetSize(); ilay++) { pPt->SetValue(ilay, 100*fvar[index]);   pPt->MR_Set_TabValue(ilay, multirun, 100*fvar[index]); }; 
	if (header.NumLayers <= 0) {
#ifdef COUPSTD
		cout << "Problems when reading MASCUR soil file \n soil key:";
		cout << FUtil::STD_ItoAscii(index) + '\n';
#endif
		return;
}
	Tab* pTab=dynamic_cast<Tab*>(GetPtr(TABLE, "Brooks-Corey, water retention, measured horizons"));
	pTab->SetNumOfRow(header.NumLayers);
	vector<float> previous;
	for(size_t layer=0; layer<header.NumLayers; layer++) {
		values=pGetSoil->GetSoilData(header.KeyProf, header.NumProf, layer);
		if(layer==0) {
			previous.resize(values.size());
			for(size_t ind=0; ind<values.size(); ind++) {
				previous[ind]=values[ind];
			}
		}
		else {
			for(size_t ind=0; ind<values.size(); ind++) {
				if(values[ind]==MISSING) 
					values[ind]=previous[ind];
			}
		}
		pSand->SetValue(layer, values[15]);
		pClay->SetValue(layer, values[13]);
		pUDep->SetValue(layer, values[1]-values[2]);
		pLDep->SetValue(layer, values[1]);
		pDryBulk->SetValue(layer, values[18]);
		pInitC->SetValue(layer, values[20]);
		/*if(values[21]>0) {
			pInitN->SetValue(layer, values[20]/10.);
		}
		else {*/
		pInitN->SetValue(layer, values[20]/10.2);
		
		pWilt->SetValue(layer, values[4]*100);
		pSat->SetValue(layer, values[6]*100);
		pAirEntry->SetValue(layer,FUtil::PedoFunc_AirEntry(values[15],values[13], values[6]));
		pLambda->SetValue(layer,FUtil::PedoFunc_Lambda(values[15],values[13], values[6]));
		pCondTot->SetValue(layer,FUtil::PedoFunc_HydraulicCond(values[15],values[13], values[6]));
		pCondMat->SetValue(layer, pCondTot->GetValue(layer));
		pRes->SetValue(layer,FUtil::PedoFunc_Residual(values[15], values[13], values[6]));

	}
		pMaxRootDepth->SetValue(0,-values[33]);
	/*if(values[10]==67) {
		pDrainLeveL->SetValue(-1.5);
		pDrainSpacing->SetValue(10.);


		}
	else if(values[10]==68) {



		}
	else if(values[10]==69) {



		}*/

	SimB *pSimB1=GetPtr(FUNCTION,"ReCalc Organic Soil Profile");
	Func* pFunc=dynamic_cast<Func*>(pSimB1);
	if(pFunc!=nullptr&&!pFunc->MakeOperation()) return;



	ReCalculateNewSoilProfile();

	Ps *pLong=dynamic_cast<Ps*>(GetPtr(PAR_SINGLE, "X-coordinate"));
	pLong->SetValue(header.Long);

	Ps *pLat=dynamic_cast<Ps*>(GetPtr(PAR_SINGLE, "Y-coordinate"));
	pLat->SetValue(header.Lat);
	pLat=dynamic_cast<Ps*>(GetPtr(PAR_SINGLE, "Latitude"));
	pLat->SetValue(header.Lat);
#endif
	
}



bool ChangeFromFile::ReCalculateNewSoilProfile()
{
	Func* pFunc;
	SimB *pSimB1;
	bool good=true;


	pSimB1=GetPtr(FUNCTION,"ReCalc Profile Function");
	pFunc=dynamic_cast<Func*>(pSimB1);
	if(pFunc!=nullptr)
		good=pFunc->MakeOperation();
	else
		return false;
	pSimB1=GetPtr(FUNCTION,"ReCalc Soil Function");
	pFunc=dynamic_cast<Func*>(pSimB1);
	if(pFunc!=nullptr&&good)
			return pFunc->MakeOperation();
	else
		return false;

}

size_t ChangeFromFile::SaveSelectedParamtersToCSVFile(size_t options)
{
	vector<Ps*> par_vector;
	vector<SIMB> parv_vector;
	SIMB simb;
	enum SelectionOptions {AllEnabled, NonDefault, Selected};

	switch (options) {

	case AllEnabled:
		dynamic_cast<NewMap*>(this)->RemoveOriginalValues("Parameters", "ALL", true);

		for (auto it = m_P_Array.begin(); it != m_P_Array.end(); it++) {
			simb.pBase = *it;
			simb.TabIndex = 0;
			parv_vector.push_back(simb);
		}

		dynamic_cast<NewMap*>(this)->RemoveOriginalValues("Tables", "ALL", true);
		for (auto it = m_Tab_Array.begin(); it != m_Tab_Array.end(); it++) {
			Tab* pTab = *it;
			auto nrow = pTab->GetNumOfRow();
			auto icol = pTab->GetNumOfCol();
			for (size_t i = 0; i < icol; i++) {
				P* pCPt = pTab->GetP_Pointer(i);
				simb.pBase = pCPt;
				if (pCPt->IsEnabled()&&!pCPt->IsReadOnly()) {
					for (size_t ii = 0; ii < nrow; ii++) {
						simb.TabIndex = ii;
						parv_vector.push_back(simb);
					}
				}
			}
		}


		break;
	case NonDefault:
		dynamic_cast<NewMap*>(this)->RemoveOriginalValues("Parameters", "ALL", false);
		for (auto it = m_P_Array.begin(); it != m_P_Array.end(); it++) {
			simb.pBase = dynamic_cast<SimB*>(*it);
			simb.TabIndex = 0;
			parv_vector.push_back(simb);
		}
		dynamic_cast<NewMap*>(this)->RemoveOriginalValues("Tables", "ALL", false);
		for (auto it = m_Tab_Array.begin(); it != m_Tab_Array.end(); it++) {
			Tab* pTab = *it;
			auto nrow = pTab->GetNumOfRow();
			auto icol = pTab->GetNumOfCol();
			for (size_t i = 0; i < icol; i++) {
				P* pCPt = pTab->GetP_Pointer(i);
				simb.pBase = pCPt;
				if (pCPt->IsEnabled() && !pCPt->IsReadOnly()) {
					for (size_t ii = 0; ii < nrow; ii++) {
						if (pCPt->IsNotOriginalValue(ii)) {
							simb.TabIndex = ii;
							parv_vector.push_back(simb);
						}
					}
				}
			}
		}
		break;
	case Selected:
		for (int i = 0; i < m_MultiRun_Array.size(); i++) {
			for (int ii = 0; ii < dynamic_cast<MR*>(this)->MR_Get_NumberOfParametersWithinDim(i + 1); ii++) {
				string test = dynamic_cast<MR*>(this)->MR_GetParName(i + 1, ii + 1).c_str();
				simb.pBase = dynamic_cast<MR*>(this)->MR_Get_pPar(i + 1, ii + 1);
				if (simb.pBase->Is_Vector()) {
					simb.TabIndex = dynamic_cast<MR*>(this)->MR_GetTabIndex(i + 1, ii + 1);
				}
				else {
					simb.TabIndex = 0;
				}
				parv_vector.push_back(simb);
			}
		}
	}
#ifndef COUPSTD
	string filename = MFC_Util::GetCSVFileName();
	fstream _fstream;
	filename = FUtil::check_open_newcsvfile(&_fstream, filename);

	enum choice { Name, Group, Unit, Default, MinValue, MaxValue, Current };

	string line;
	
	auto AddSubstrings= [&](SIMB simb, enum choice var)
	{
		string name = simb.pBase->GetName();
		if (!simb.pBase->Is_P()) {
			name += "(";
			name += FUtil::STD_ItoAscii(simb.TabIndex+1);
			name += ")";
		}
		switch (var) {
			case Name:
			line += name;
			break;
			case Group:
				if (simb.pBase->Is_DB())
					line += "DataBase";
				else
					line += simb.pBase->GetGroup() ;
				break;
			case Unit:
				if (simb.pBase->Is_DB())
					line += "-";
				else
					line += simb.pBase->GetUnit();
				break;
			case Default:
				if (simb.pBase->Is_P()) {
					line += FUtil::STD_DtoAscii(((Ps*)simb.pBase)->GetOriginalValue()) ;
				}
				else {
					line += FUtil::STD_DtoAscii(simb.pBase->GetOriginalValue(0));
				}
				break;
			case MinValue:
				if (simb.pBase->Is_P()) {
					line += FUtil::STD_DtoAscii(((Ps*)simb.pBase)->GetMinValue());
				}
				else
					line += FUtil::STD_DtoAscii(simb.pBase->GetMinValue(simb.TabIndex)) ;
				break;
			case MaxValue:
				if (simb.pBase->Is_P())
					line += FUtil::STD_DtoAscii(((Ps*)simb.pBase)->GetMaxValue());
				else
					line += FUtil::STD_DtoAscii(simb.pBase->GetMaxValue(simb.TabIndex));
				break;
			case Current:
				if (simb.pBase->Is_P())
					line += FUtil::STD_DtoAscii(((Ps*)simb.pBase)->GetValue());
				else
					line += FUtil::STD_DtoAscii(simb.pBase->GetValue(simb.TabIndex));
				break;
		}
		line+=FUtil::GetDelimChar();
	};

	auto AddKeystring = [&](SIMB simb, size_t NRep) {
		for (size_t i = 0; i < NRep; i++) {
			line = FUtil::STD_ItoAscii(i + 1);
			line += FUtil::GetDelimChar();
			CDB *pCDB;
			pCDB = dynamic_cast<CDB*>(simb.pBase);
			if (pCDB != nullptr) {
				line += pCDB->MR_GetKeySelection(i);
			}
			else
				line += "-";
			line += FUtil::GetDelimChar();
			line = line.substr(0, line.size() - 1);
			line += "\n";
			_fstream.write((char*)&line[0], line.size());
			line.clear();


		}
	

	};
	auto AddValues = [&](double value)
	{
		line += FUtil::STD_DtoAscii(value);	
		line+=FUtil::GetDelimChar();
	};

	auto writetostream=[&]() {
		line = line.substr(0, line.size() - 1); 
		line += "\n";
		_fstream.write((char*)&line[0], line.size()); 
		line.clear();
	};

	line = "Name;";
	for (auto it = parv_vector.begin(); it != parv_vector.end(); AddSubstrings(*it, Name), it++);
	writetostream();

	if (parv_vector.begin()->pBase->Is_DB()) {
		auto pNewMap = dynamic_cast<NewMap*>(this);
		size_t nRep = pNewMap->MR_GetNumberOfRepeationsForThisDimension(0);

		// Case corresponds to Table
		for (auto it = parv_vector.begin(); it != parv_vector.end(); AddKeystring(*it, nRep), it++);

	}
	else {

		line = "Group;";
		for (auto it = parv_vector.begin(); it != parv_vector.end(); AddSubstrings(*it, Group), it++);
		writetostream();

		line = "Unit;";
		for (auto it = parv_vector.begin(); it != parv_vector.end(); AddSubstrings(*it, Unit), it++);
		writetostream();

		line = "Default;";
		for (auto it = parv_vector.begin(); it != parv_vector.end(); AddSubstrings(*it, Default), it++);
		writetostream();

		line = "Min;";
		for (auto it = parv_vector.begin(); it != parv_vector.end(); AddSubstrings(*it, MinValue), it++);
		writetostream();

		line = "Max;";
		for (auto it = parv_vector.begin(); it != parv_vector.end(); AddSubstrings(*it, MaxValue), it++);
		writetostream();

		line = "Current;";
		for (auto it = parv_vector.begin(); it != parv_vector.end(); AddSubstrings(*it, Current), it++);
		writetostream();
	}
#else
	options = 0;
#endif

	return size_t(options);
}

bool ChangeFromFile::ReadDataFromCSVFileandSetDataCorrespondingtoIndexValue(FCSV *pCSVFile, size_t keyindex)
{

	// Check if file is defined

	// Check value of Index
	if (pCSVFile->CheckFileName(this)) {

		pCSVFile->AssignSIMB_PointersFromHeader();
		pCSVFile->AssignKey_WithValues();

		vector<size_t> keys;
		keys = pCSVFile->GetKeyVector();
		vector<SIMB> simv = pCSVFile->GetSIMB_vector();

		vector<double> values = pCSVFile->GetParValue(keyindex);
		size_t count_changed = 0;

			for (size_t i = 0; i < simv.size(); i++) {
				if (simv[i].pBase->Is_PTable()) {
					if(ModelUtil::SetParTableValue(dynamic_cast<Doc*>(this),dynamic_cast<P*>(simv[i].pBase), simv[i].TabIndex, values[i + 1])) count_changed++;
				}
				else {
					if (ModelUtil::SetParValue(dynamic_cast<Doc*>(this), dynamic_cast<Ps*>(simv[i].pBase), values[i + 1])) count_changed++;
					dynamic_cast<Ps*>(simv[i].pBase)->SetValue(values[i+1]);
				}

			}
			string str;
			str=FUtil::STD_ItoAscii(count_changed);
			str += " Parameters have been assigned from new values using key index ";

			str += FUtil::STD_ItoAscii(keyindex);
			str += "\nto identify the values from CSV file : ";
			str += pCSVFile->GetStrValue();
			str += "\nThe changes are recorded in the history list";
#ifndef COUPSTD
			MFC_Util::MessageBox(str);
#endif

	}

	// Read the data with data corresponding to the Index

	// Apply the values find that matches the data to the Current values within the actual document


	return false;
}
