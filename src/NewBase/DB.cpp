
#include "../std.h"
#include "../Util/FUtil.hpp"
#include "../Util/ModelUtil.h"
#include "../Util/MySingle.h"
#include <time.h>
#include "./DB.h"

#include "../ModelTypes/NewModelType.h"


extern bool Running;
extern bool MultiRunning;
#ifndef COUPSTD
#include "../stdafx.h"
#include "../MainFrm.h"
#endif

#include "./DB/DB_Storage.h"
#include "./DB/DB_Header.h"

DB::DB() {
	m_ShortTimeRead=true;
	m_pSwTimeRes=nullptr;
	m_pSwPlantType=nullptr;
}
bool DB::Read_DB_DocFile(string file, string key, bool ApplyToCurrentSimFile, bool ExportToCSV)
{
	#ifndef COUPSTD
		CMainFrame *p_mainframe;
		p_mainframe = (CMainFrame*) AfxGetMainWnd();
		string txt;
		txt="Reading Data Base Document File:";
		txt+=file;
		p_mainframe->UpdatePaneText(0,txt);
	#endif
	if(file.size()>0) {
		if(file.find("NN3")>=0) {

			string kalle="";
		}
	}
	m_DB_key = key;


	bool good=false;
	m_DocFileIn.close();
	m_DocFileIn.clear();
	m_DocFileIn.open(file, ios::in|ios::binary);
	m_DocFileIn.seekg(0,ios::beg);
	good=m_DocFileIn.good();


	long long totlength=0;
	bool prog;
	if(m_DocFileIn.is_open()) {
		m_DocFileIn.seekg(0,ios::end);
		totlength=m_DocFileIn.tellg();
		good=m_DocFileIn.good();
		if(totlength==0&&!good) {
			m_DocFileIn.close();
			return true;

		}
		else if(totlength==0) {
			m_DocFileIn.close();
			return true;

		};
	}

	vector<Sw*> Applied_Sw; vector<Ps*> Applied_Ps;
	vector<Tab*> Applied_Tab; vector<F*> Applied_Files;


	string ExportFileName;
	ExportFileName = file.substr(0, file.rfind(".Sim"));
	ExportFileName += ".Csv";
	fstream ExportFile;

	string line;
	auto WriteToFile = [&](size_t var) {
		if (var == 0) {
			for (size_t i = 0; i < Applied_Sw.size(); i++) {
				Sw* pSW = Applied_Sw[i];
				line = "Switches,";
				line += pSW->GetGroup() + ",";
				line += pSW->GetName() + ",";
				line += pSW->GetOption(pSW->GetDBIntValue()) + '\n';
				ExportFile.write((char*)&line[0], line.size());
			}
		}
		else if (var == 1) {
			for (size_t i = 0; i < Applied_Ps.size(); i++) {
				Ps* pPs = Applied_Ps[i];
				line = "Parameters,";
				line += pPs->GetGroup() + ",";
				line += pPs->GetName() + ",";
				line += FUtil::STD_DtoAscii(pPs->GetValue()) + '\n';
				ExportFile.write((char*)&line[0], line.size());
			}


		}
		else if (var == 2) {
			for (size_t i = 0; i < Applied_Tab.size(); i++) {
				Tab* pTab = Applied_Tab[i];
				line = "Tables,";
				line += pTab->GetGroup() + ",";
				line += pTab->GetName() + ",";
				line += FUtil::STD_ItoAscii(pTab->GetDB_Row()) + '\n';
				ExportFile.write((char*)&line[0], line.size());

				line = "NumberofCol,";
				line+=FUtil::STD_ItoAscii(pTab->GetNumOfCol()) + '\n';
				ExportFile.write((char*)&line[0], line.size());
				for (size_t nCol = 0; nCol < pTab->GetNumOfCol(); nCol++) {				
					line = "Title:,";
					line += pTab->GetTitle(nCol)+'\n';
					ExportFile.write((char*)&line[0], line.size());
				}


				for (size_t nRow = 0; nRow<pTab->GetDB_Row(); nRow++) {
					for (size_t nCol = 0; nCol<pTab->GetNumOfCol(); nCol++) {
						P *pP = pTab->GetP_Pointer(nCol);
						line = "Table Parameters,";
						line += pP->GetGroup()+',';
						line += pP->GetName() + "("+FUtil::STD_ItoAscii(nRow+1)+"),";
						line += FUtil::STD_DtoAscii(pP->GetDBValue(nRow))+"\n";
						ExportFile.write((char*)&line[0], line.size());
					}
				}


			}

		}
		else if (var == 3)
		{
			for (size_t i = 0; i < Applied_Files.size(); i++) {
				F* pF = Applied_Files[i];
				line = "Files,";
				line += pF->GetGroup() + ",";
				line += pF->GetName() + ",";
				line += pF->GetDBStrValue() + '\n';
				ExportFile.write((char*)&line[0], line.size());
			}


		}

	};
	auto WriteHeaderToFile = [&]() {

		CDB* pDB=DB_GetPointer(m_ID_DB_Group);
		DB_OldHeader header;
		DB_NewHeader headernew;
		DB_Header *pHeader= MySingle::instance().GetDB_HeaderPointer();

		size_t keyindex = pHeader->GetCurrentKeyIndex();

		header = pHeader->GetHeader(pHeader->GetCurrentDBIndex(true), keyindex);
		headernew = pHeader->GetNewHeader(pHeader->GetCurrentDBIndex(true), keyindex);
		line = "Data Base Group,";	line += m_ID_DB_Group; line += "\n";
		ExportFile.write((char*)&line[0], line.size());
		line = "Key,";	line += header.IdUser; line += "\n";
		ExportFile.write((char*)&line[0], line.size());
		line = "Type,";
		line += pHeader->GetTypeString(pHeader->GetCurrentDBIndex(true), header.Type)+"\n";
		ExportFile.write((char*)&line[0], line.size());
		line = "Name,";	line += header.Name; line +="\n";
		ExportFile.write((char*)&line[0], line.size());
		line = "Site Name,";	line += header.SiteName; line += "\n";
		ExportFile.write((char*)&line[0], line.size());	
		line = "Latitude,";	line += header.Latitude; line += "\n";
		ExportFile.write((char*)&line[0], line.size());
		line = "Longitude,";	line += header.Longitude; line += "\n";
		ExportFile.write((char*)&line[0], line.size());
		line = "Country,";	line += header.Country; line += "\n";
		ExportFile.write((char*)&line[0], line.size());
		line = "Created,";	line += headernew.CreateDate; line += "\n";
		ExportFile.write((char*)&line[0], line.size());
		
		size_t nlines = 0;
		vector<string> newlines;
		string Details = header.Details;
		while (Details.find('\r')!=string::npos) {
			newlines.push_back(Details.substr(0, Details.find('\r')));
			Details=Details.substr(Details.find('\r') + 2);
		}
		if (Details.size() > 0) {
			newlines.push_back(Details);
		}

		line = "Num of Lines with Details,";	
		
		line += FUtil::STD_ItoAscii(newlines.size()); line += "\n";
		ExportFile.write((char*)&line[0], line.size());
		for (size_t i = 0; i < newlines.size(); i++) {
			line = "Details (" + FUtil::STD_ItoAscii(i + 1) + "),";
			line += newlines[i] + '\n';
			ExportFile.write((char*)&line[0], line.size());
		}


	};
	if (ExportToCSV) {
		ExportFile.open(ExportFileName, ios::out);
		WriteHeaderToFile();
	}

	if(m_DocFileIn.is_open()&&totlength>0) {

		m_DocFileIn.seekg(0,ios::beg);
		good=m_DocFileIn.good();
		string problem;
		string str;

		rString(&str);
		line = str;
		line += "\n";
		if (ExportToCSV) ExportFile.write((char*)&line[0], line.size());
		int irep;
		m_Read_DB_Version=0;




		int ReadSectionCount=0;
		while(good&&str!="End") {	
			rInt(&irep);
			line = FUtil::STD_ItoAscii(irep);
			line += "\n";
			if(ExportToCSV) ExportFile.write((char*)&line[0], line.size());
			if(str=="Start") {
				m_Read_DB_Version=irep;
			}
			else if(str.find("Switches")!=string::npos) {
				Applied_Sw=ReadWrite_DB_Switches(true, irep);
				if (ExportToCSV) WriteToFile(0);
			}
			else if(str=="Parameters") {
				Applied_Ps=ReadWrite_DB_Parameters(true, irep);
				if (ExportToCSV) WriteToFile(1);
			}
			else if(str=="Tables") {
				Applied_Tab = ReadWrite_DB_ParTables(true, irep);
				if (ExportToCSV) WriteToFile(2);
			}
			else if(str=="Files") {
				Applied_Files = ReadWrite_DB_Files(true, irep);
				if (ExportToCSV) WriteToFile(3);
			}
			rString(&str);
			line = str + '\n';
			if(ExportToCSV) ExportFile.write((char*)&line[0], line.size());
			good=m_DocFileIn.good();
		}

		if(m_DocFileIn.good()) {
			m_DocFileIn.close();
		}
		if (ApplyToCurrentSimFile) {
			Apply_DB_DocFile(Applied_Sw, Applied_Ps, Applied_Tab, Applied_Files);
		}
		if (ExportToCSV) {
			ExportFile.close();
		}
		return true;
	}
	else {
		return false;
	}


}
void DB::SetGroupConnections(string m_ID_DB_Group)
{
	string Type, Group, Name;
	Name="Nitrogen and Carbon";
	Type="Switches"; Group="Model Structure";
	m_pSw_NC=(Sw*)GetPtr(Type, Group, Name);
	m_arrGroups.clear();
	
	if(m_ID_DB_Group=="Soil Mineral N Processes")
		m_arrGroups.push_back("Soil mineral N processes");
	else if(m_ID_DB_Group=="Soil Organic Processes")
		m_arrGroups.push_back("Soil organic processes");
	else if(m_ID_DB_Group=="Snow")
		m_arrGroups.push_back("SnowPack");
	else
		m_arrGroups.push_back(m_ID_DB_Group);
	
	if(m_ID_DB_Group==DB_GetNames(0)) { //Plant
		m_arrGroups.push_back("Potential Transpiration");
		m_arrGroups.push_back("Water uptake");
		m_arrGroups.push_back("Interception");
	}
	else if(m_ID_DB_Group==DB_GetNames(3)) {  // Snow

	}
	else if(m_ID_DB_Group==DB_GetNames(4))	 { // Soil Frost
	
	}
	else if(m_ID_DB_Group==DB_GetNames(5)) {		// Soil Water Flow
		m_arrGroups.push_back("Drainage and deep percolation");
		m_arrGroups.push_back("Surface water");
	}
	else if(m_ID_DB_Group==DB_GetNames(2)) {  // Soil Evaporation
		m_arrGroups.push_back("Soil water flows");
	}
	else if(m_ID_DB_Group==DB_GetNames(7)) {
		m_arrGroups.push_back("External N inputs");
		m_arrGroups.push_back("Meteorological Data");
		m_arrGroups.push_back("Radiation properties");
		m_arrGroups.push_back("Soil Management");

	}
	else if(m_ID_DB_Group==DB_GetNames(8)) {
		m_arrGroups.push_back("Common abiotic responses");


	}
	else if(m_ID_DB_Group==DB_GetNames(9)) {
		m_arrGroups.push_back("Gas processes");


	}
	else if(m_ID_DB_Group==DB_GetNames(6)) {
		m_arrGroups.push_back("Multiple Plant");
		m_arrGroups.push_back("Potential Transpiration");
//		m_pSw_NC->SetDBValue(1);
	}
	m_PlantTypes.clear();
	if(m_ID_DB_Group==DB_GetNames(0)||m_ID_DB_Group==DB_GetNames(6)) {
	 	m_PlantTypes.push_back("Coniferous Forest");
		m_PlantTypes.push_back("Decidous Forest");
		m_PlantTypes.push_back("Mixed Forest");
		m_PlantTypes.push_back("Natural Grassland");
		m_PlantTypes.push_back("Crops");
		m_PlantTypes.push_back("Undefined");
	//	m_arrGroups.push_back("Multiple Plant");
	//	m_arrGroups.push_back("Potential Transpiration");

	}
	else if(m_ID_DB_Group==DB_GetNames(3)) {
		m_PlantTypes.push_back("Seasonal snow");
		m_PlantTypes.push_back("Permanent snow");
		m_PlantTypes.push_back("Undefined");
	}
	else if(m_ID_DB_Group==DB_GetNames(5)) {
		m_PlantTypes.push_back("Natural drainage");
		m_PlantTypes.push_back("Tile drainage");
		m_PlantTypes.push_back("Ditches or canals");
		m_PlantTypes.push_back("Undefined");
	}
	else {
		m_PlantTypes.push_back("Clayey soil");
		m_PlantTypes.push_back("Sandy soil");
		m_PlantTypes.push_back("Silty soil");
		m_PlantTypes.push_back("Loamy soil");
		m_PlantTypes.push_back("Organic soil");
		m_PlantTypes.push_back("Undefined");
	}



}

bool DB::ReadFrom_DB_Object(string ID_DB_Group, string Key, bool deletefile, bool NotApply, bool ExportToCSV)
{
			m_ID_DB_Group = ID_DB_Group;
			string strApp=m_AppString;
			m_SimFileName="DB_Plant_";
#ifdef COUPSTD
			m_DataBaseDirectory = FUtil::GetProfileStringStd("DataBaseDirectory", m_DataBaseDirectory);
#else
			if (m_DataBaseDirectory.size() == 0) m_DataBaseDirectory = MFC_Util::GetProfileStringStd("DataBaseDirectory", m_DataBaseDirectory);

#endif
			m_SimFileName=m_DataBaseDirectory;//pApp->GetProfileString(strApp,_T("DataBaseDirectory"),m_SimFileName);
			m_SimFileName+=m_ID_DB_Group+"\\DB_"+m_ID_DB_Group+"_"+Key;
			m_SimFileName+=".Sim";

			fstream file;
			string Error;
			Error="Could not find data base object \nMake sure that the correct data base is assigned !\n";
			if(!FUtil::IsFileExisting(string(m_SimFileName))) {
				if(m_CountError<1) {

							Error+="your simulation will not account for the information in data base ! \nFile missing: ";
							Error+=m_SimFileName;
		#ifndef COUPSTD
							MFC_Util::MessageBox( Error,"File Open Error", MB_OK|MB_ICONEXCLAMATION);
		#endif
							m_CountError=1;
						}
						return false;
			}
			SetGroupConnections(m_ID_DB_Group);

			if(deletefile) {
				file.open(m_SimFileName, ios::in|ios::binary);

				if(!file.is_open()) {
					Error="Could not find data base object \nMake sure that the correct data base is assigned !\n";
#ifndef COUPSTD
					MFC_Util::MessageBox( Error,"File Open Error", MB_OK|MB_ICONEXCLAMATION);
#endif
					return false;
				}
				file.close();
				remove(m_SimFileName.data());
				return true;
			}

			bool prog;
			prog=Read_DB_DocFile(m_SimFileName, Key, !NotApply, ExportToCSV);

			return prog;
}
void DB::SaveTo_DB_Object(string m_ID_DB_Group, string key)
{
			string FileName;
			string strApp=m_AppString;

			FileName=m_DataBaseDirectory;
			FileName+=m_ID_DB_Group+"\\DB_"+m_ID_DB_Group+"_"+key;
			FileName+=".Sim";

			bool prog;
			SetGroupConnections(m_ID_DB_Group);
			prog=Write_DB_DocFile(FileName);



}

bool DB::Write_DB_DocFile(string file)
{

	m_DocFileOut.open(file,ios_base::binary|ios_base::out);

	string Type, str, Group, Name, FileName;
	int Version=2;
	Version=m_DocFile.m_FileVersionNumber;

	bool good, prog;

	good=m_DocFileOut.good();

	Type="Start";
	wString(&Type);
	wInt(&Version);
	
	
			
	//	Type="Switches";			
		prog=Info_DB_Switches(false);
		if(!prog) return false;
		
		prog=Info_DB_Parameters(false);
		if(!prog) return false;
		
		prog=Info_DB_ParTables(false);
		if(!prog) return false;
		
		prog=Info_DB_Files(false);
		if(!prog) return false;
		

	Type="End";
	wString(&Type);

	if(m_DocFileOut.good()) {
		m_DocFileOut.close();

	}
	return true;

}
void DB::Reset()
{
	string str, Group, Type, Name;
	vector<SimB*> vvp;
	Ps *pP;
	Sw* pSw;
	P *pCPt;

	for(size_t i=0;i<m_arrGroups.size();i++) {
			Group=m_arrGroups[i];
			if(Group=="Multiple Plant") Group="Plant";
			Type="Switches";
			vvp=GetPtrVector(Type, Group, true);

				for(size_t ii=0;ii<vvp.size();ii++) {
					pSw=(Sw*)vvp[ii];
					size_t i1=pSw->GetOriginalIntValue();
					pSw->SetDBIntValue(string::npos);
				}

			Type="Parameters";
			vvp=GetPtrVector(Type, Group, true);
			for(size_t ii=0;ii<vvp.size();ii++) {
				pP=(Ps*)vvp[ii];
				pP->SetDBValue(MISSING);
			}

			Type="Table Parameters";
			vvp=GetPtrVector(Type, Group, true);
			for(size_t ii=0;ii<vvp.size();ii++) {
				pCPt=(P*)vvp[ii];
				int numn=pCPt->GetSize();
				for (int i3=0; i3<numn;i3++) {
						double val=pCPt->GetOriginalValue(i3);
						pCPt->SetDBValue(i3, MISSING);
				}
			}
			F *pF=nullptr;
			Type="Files";
			vvp=GetPtrVector(Type, Group, true);
			for(size_t ii=0;ii<vvp.size();ii++) {
				pF=(F*)vvp[ii];					
				string val=pF->GetOriginalStrValue();
				pF->SetDBValue(val);						
			}


	}
   
}
bool DB::Retrieve(string group, string key, bool NotApply) 
{
	if (key == "-10")
		return false;
	else
		return ReadFrom_DB_Object(group, key,false, NotApply);



}
bool DB::Info_DB_Switches(bool reading, int iv)
{
	SimB *pSw=nullptr;
	string type,group,name;
	int  value;
	type="Switches";

	if(reading) {

			for(int i=0; i<iv;i++) {
				rString(&group);
				if(group=="Driving variables") group="Meteorological Data";
				if(group=="Soil mineral nitrogen processes") group="Soil mineral N processes";
				rString(&name);
				rInt(&value);
				if(group=="Model Structure"&&name=="TimeResolution") group="Technical";
 				if(group=="Hidden"&&name=="TimeResolution") group="Technical";
 				if(group=="Irrigation"&&name=="IndexForValidation") group="Technical";
				if(m_FileVersionNumberRead<=24&&group=="Potential Transpiration"&&name=="Aerodyn. Resistance") name="Aerodynamic Resistance";
				pSw=GetPtr(type,group,name);
				if(pSw!=nullptr) {
					if(name=="SweClimScenarious") {

						int i=0;
					}
					pSw->SetDBIntValue(value);
				}
			}

	}
	else {
		vector <int> index_Sw;
		index_Sw.resize(0);
			string str, Group, Type, Name;
			vector<SimB*> vvp;

			Sw* pSw;

			for(size_t i=0;i<m_arrGroups.size();i++) {
					Group=m_arrGroups[i];
					Type="Switches";
					if(Group=="Multiple Plant") Group="Plant";

					Actual_DB(Group);

					vvp=GetPtrVector(Type, Group, true);
					
					for(size_t jj=0; jj<vvp.size(); jj++) {
						for( size_t jj2=0; jj2<m_Sw_Array.size(); jj2++) {
							pSw=(Sw*)m_Sw_Array[jj2];
							if(vvp[jj]==m_Sw_Array[jj2]&&pSw->GetDBIntValue()!=pSw->GetOriginalIntValue()) {
								index_Sw.push_back(jj2);
								break;
							}
						}
					}





			}


			iv=index_Sw.size();


		type="Switches";
		wString(&type);
// Allways add TimeResolution and PlantType
		size_t n_add = 2;
		iv+=n_add;
		group = "Technical";
		name = "TimeResolution";
		pSw =(Sw*)GetPtr(type, group, name);

		wInt(&iv);

        string group_psw = pSw->GetGroup();
        string name_psw = pSw->GetName();
        
		wString(&group_psw);
		wString(&name_psw);
		int value = pSw->GetDBIntValue();
		wInt(&value);
		
		group = "Model Structure";
		name = "PlantType";
		pSw = (Sw*)GetPtr(type, group, name);

        group_psw = pSw->GetGroup();
        name_psw = pSw->GetName();
        
		wString(&group_psw);
		wString(&name_psw);
		value = pSw->GetDBIntValue();
		wInt(&value);

		for(int i=0;i<iv-n_add;i++) {
			pSw=(Sw*)m_Sw_Array[index_Sw[i]];
            
            group_psw = pSw->GetGroup();
            name_psw = pSw->GetName();
            
			wString(&group_psw);
			wString(&name_psw);
			value=pSw->GetDBIntValue();
			wInt(&value);
		}
	//	m_DocFileOut.write(&iv ,4);


	}


  
return true;
}
vector<Sw*> DB::ReadWrite_DB_Switches(bool reading, int iv) {
	vector<Sw*> Applied_Sw;
	SimB *pSw = nullptr;
	string type, group, name;
	int  value;
	type = "Switches";

	if (reading) {

		for (int i = 0; i<iv; i++) {
			rString(&group);
			if (group == "Driving variables") group = "Meteorological Data";
			if (group == "Soil mineral nitrogen processes") group = "Soil mineral N processes";
			rString(&name);
			rInt(&value);
			if (group == "Model Structure"&&name == "TimeResolution") group = "Technical";
			if (group == "Hidden"&&name == "TimeResolution") group = "Technical";
			if (group == "Irrigation"&&name == "IndexForValidation") group = "Technical";
			if (m_FileVersionNumberRead <= 24 && group == "Potential Transpiration"&&name == "Aerodyn. Resistance") name = "Aerodynamic Resistance";
			pSw = GetPtr(type, group, name);
			if (pSw != nullptr) {
				pSw->SetDBIntValue(value);
				Applied_Sw.push_back((Sw*)pSw);
			}
		}

	}
	else {
		vector <int> index_Sw;
		index_Sw.resize(0);
		string str, Group, Type, Name;
		vector<SimB*> vvp;

		Sw* pSw;

		for (size_t i = 0; i<m_arrGroups.size(); i++) {
			Group = m_arrGroups[i];
			Type = "Switches";
			if (Group == "Multiple Plant") Group = "Plant";

			Actual_DB(Group);

			vvp = GetPtrVector(Type, Group, true);

			for (size_t jj = 0; jj<vvp.size(); jj++) {
				for (size_t jj2 = 0; jj2<m_Sw_Array.size(); jj2++) {
					pSw = (Sw*)m_Sw_Array[jj2];
					if (vvp[jj] == m_Sw_Array[jj2] && pSw->GetDBIntValue() != pSw->GetOriginalIntValue()) {
						index_Sw.push_back(jj2);
						break;
					}
				}
			}





		}


		iv = index_Sw.size();


		type = "Switches";
		wString(&type);
		// Allways add TimeResolution and PlantType
		size_t n_add = 2;
		iv += n_add;
		group = "Technical";
		name = "TimeResolution";
		pSw = (Sw*)GetPtr(type, group, name);

		wInt(&iv);
        
        string group_psw = pSw->GetGroup();
        string name_psw = pSw->GetName();
        
		wString(&group_psw);
		wString(&name_psw);
		int value = pSw->GetDBIntValue();
		wInt(&value);

		group = "Model Structure";
		name = "PlantType";
		pSw = (Sw*)GetPtr(type, group, name);

        group_psw = pSw->GetGroup();
        name_psw = pSw->GetName();
        
		wString(&group_psw);
		wString(&name_psw);
		value = pSw->GetDBIntValue();
		wInt(&value);

		for (int i = 0; i<iv - n_add; i++) {
			pSw = (Sw*)m_Sw_Array[index_Sw[i]];
            
            group_psw = pSw->GetGroup();
            name_psw = pSw->GetName();
            
			wString(&group_psw);
			wString(&name_psw);
			value = pSw->GetDBIntValue();
			wInt(&value);
		}
		//	m_DocFileOut.write(&iv ,4);


	}


	return Applied_Sw;


}
bool DB::Info_DB_Parameters(bool reading, int iv)
{
	Ps *pP=nullptr;
	string type,group,name;
	float fvalue;
	type="Parameters";

	if(reading) {

		for(int i=0; i<iv;i++) {
			rString(&group);
			if(group=="Driving variables") group="Meteorological Data";
			if(group=="Soil mineral nitrogen processes") group="Soil mineral N processes";
		
			rString(&name);
			rFloat(&fvalue);
			if(name=="RateCoefHumus"&&m_FileVersionNumberRead<=21) {
				fvalue=fvalue*2;
			}
			pP=(Ps*)GetPtr(type,group,name);
			if(pP!=nullptr) {
				((Ps*)pP)->SetDBValue(fvalue);
			}
			else {

			}
		}

	}
	else {
		if(iv==0) iv=m_P_Array.size();

		vector <int> index_P;
		index_P.resize(0);
		string str, Group, Type, Name;
		vector<SimB*> vvp;
		Ps *pP;

		for(size_t i=0;i<m_arrGroups.size();i++) {
				Group=m_arrGroups[i];
				Type="Parameters";
				if(Group=="Multiple Plant") Group="Plant";
				vvp=GetPtrVector(Type, Group, true);

				for(size_t jj=0; jj<vvp.size(); jj++) {
					for(size_t jj2=0; jj2<m_P_Array.size(); jj2++) {
						pP=(Ps*)m_P_Array[jj2];
						if(pP->GetDBValue()!=pP->GetOriginalValue()&&vvp[jj]==m_P_Array[jj2]) // Exist in the Document Array and value different from default
							index_P.push_back(jj2);
					}

				}
		}
		iv=index_P.size();
		type="Parameters";
		wString(&type);
		wInt(&iv);
		float value;
        
        string group_psw;
        string name_psw;
        
		for(int i=0;i<iv;i++) {
			pP=(Ps*)m_P_Array[index_P[i]];
            
            group_psw = pP->GetGroup();
            name_psw = pP->GetName();
            
			wString(&group_psw);
			wString(&name_psw);
            
			value=float(pP->GetDBValue());
			wFloat(&value);
		}
	//	m_DocFileOut.write(&iv ,4);


	}



return true;
}
vector<Ps*> DB::ReadWrite_DB_Parameters(bool reading, int iv)
{
	vector<Ps*> Applied_Parameters;
	Ps *pP = nullptr;
	string type, group, name;
	float fvalue;
	type = "Parameters";

	if (reading) {

		for (int i = 0; i<iv; i++) {
			rString(&group);
			if (group == "Driving variables") group = "Meteorological Data";
			if (group == "Soil mineral nitrogen processes") group = "Soil mineral N processes";

			rString(&name);
			rFloat(&fvalue);
			if (name == "RateCoefHumus"&&m_FileVersionNumberRead <= 21) {
				fvalue = fvalue * 2;
			}
			pP = (Ps*)GetPtr(type, group, name);
			if (pP != nullptr) {
				((Ps*)pP)->SetDBValue(fvalue);
				Applied_Parameters.push_back(pP);
			}
			else {

			}
		}

	}
	else {
		if (iv == 0) iv = m_P_Array.size();

		vector <int> index_P;
		index_P.resize(0);
		string str, Group, Type, Name;
		vector<SimB*> vvp;
		Ps *pP;

		for (size_t i = 0; i<m_arrGroups.size(); i++) {
			Group = m_arrGroups[i];
			Type = "Parameters";
			if (Group == "Multiple Plant") Group = "Plant";
			vvp = GetPtrVector(Type, Group, true);

			for (size_t jj = 0; jj<vvp.size(); jj++) {
				for (size_t jj2 = 0; jj2<m_P_Array.size(); jj2++) {
					pP = (Ps*)m_P_Array[jj2];
					if (pP->GetDBValue() != pP->GetOriginalValue() && vvp[jj] == m_P_Array[jj2]) // Exist in the Document Array and value different from default
						index_P.push_back(jj2);
				}

			}
		}
		iv = index_P.size();
		type = "Parameters";
		wString(&type);
		wInt(&iv);
		float value;
        string group_psw;
        string name_psw;
		for (int i = 0; i<iv; i++) {
			pP = (Ps*)m_P_Array[index_P[i]];
            
            group_psw = pP->GetGroup();
            name_psw = pP->GetName();
            
			wString(&group_psw);
			wString(&name_psw);
			value = float(pP->GetDBValue());
			wFloat(&value);
			Applied_Parameters.push_back(pP);
		}
		//	m_DocFileOut.write(&iv ,4);


	}




	return Applied_Parameters;
}
bool DB::Info_DB_ParTables(bool reading, int iv)
{
	P *pPt=nullptr;
	Tab* pTab=nullptr;
	string type,group,name;
	int  nColSize, aNE;
	float fvalue;
	string str, comp_str;
	type="Tables";
	if(reading) {
		for(int i=0; i<iv;i++) {
				rString(&group);
				if(group=="Driving variables") group="Meteorological Data";
				if(group=="Soil mineral nitrogen processes") group="Soil mineral N processes";	
				rString(&name);
				if(m_Read_DB_Version>20) {

					rInt(&nColSize);
					for(int nCol=0;nCol<nColSize;nCol++) {
						rString(&str);
						if(pTab!=nullptr) {
							comp_str=string(pTab->GetTitle(nCol));
						}
					}
				}
				rInt(&aNE);
				if(aNE!=1) {
					int na=i;
				}
				if(name=="Above ground crop characteristics") name="Above ground characteristics with time";
				if(name=="Evapotranspiration parameters") name="Evapotranspiration - single canopy";	
				if(name=="Scaling of Litter fall ") name="Scaling of Litter fall"; 
				if(name=="Plant Behavior") name="Plant Behaviour"; 
				pTab=(Tab*)GetPtr(type,group,name);

				if(pTab!=nullptr) {
					int oldNE=pTab->GetNumOfRow();
					pTab->SetDB_Row(aNE);
					if(aNE!=oldNE) {
						pTab->SetDB_Row(aNE);
					}
					else {
						int ii=2;

					}
					pTab->SetNotOriginalValue();
				}
				else {
					string mes="Table name :";
					mes.append(name);
					mes.append(" from file:");
					mes.append(m_SimFileName);
					#ifndef COUPSTD
							CMainFrame *p_mainframe;
							p_mainframe = (CMainFrame*) AfxGetMainWnd();
							p_mainframe->UpdatePaneText(0,mes.c_str());
					//	MFC_Util::MessageBox(_T(mes.c_str()),_T("Fatal Input Error"),	MB_OK|MB_ICONEXCLAMATION);
					#endif
					if(aNE>100||aNE<1) return false;
				  for(int nRow=0;nRow<aNE;nRow++) {
					   for (int nCol=0; nCol<nColSize;nCol++)
						   rFloat(&fvalue);
					}
					
				}
				if(pTab!=nullptr) {
					if(m_Read_DB_Version<21) {
						nColSize=0;
					  	//while(pTab->GetDBCell(nColSize, 0, &fvalue)){
				  		//  	  nColSize++;
		  			//	}
						nColSize=pTab->GetNumOfCol();
						if(m_Read_DB_Version==0&&name=="Root depths development with time") nColSize=2;
						if(m_Read_DB_Version<=9&&name=="Size and shape of growing plant") nColSize=7; 
						if(m_Read_DB_Version<=2&&name=="Size and shape of growing plant") nColSize=5; 
						if(m_Read_DB_Version<=9&&name=="Above ground characteristics with time") nColSize=4; 
						if(m_Read_DB_Version<=14&&name=="Size and shape of growing plant") nColSize=7; 
						if(m_Read_DB_Version==1&&name=="Hydraulic conductivity, model boundaries") nColSize=11;
						if(m_Read_DB_Version==15&&name=="Harvest of Plants") nColSize=8;
						if(name=="Harvest of Plants") nColSize=9;
						if(name=="Initial conditions of plants") nColSize=8;
						if(name=="Minimum CN Ratios of plants") nColSize=3;
						if(name=="Scaling of Litter fall") nColSize=6;

						if(m_Read_DB_Version<=2&&name=="Start of growth") nColSize=4;
						if(m_Read_DB_Version<=2&&name=="Evapotranspiration - multiple canopies") nColSize=7;
						if(m_Read_DB_Version<=2&&name=="Allocation parameters") nColSize=12;
					}
					if(name=="Growth stage") {
						int ijj=pTab->GetDB_Row();

					}
					for(int nRow=0; nRow<aNE ; nRow++){
	  		  			for (int nCol=0; nCol<nColSize; nCol++){
	  						rFloat(&fvalue);
							P *pP=pTab->GetP_Pointer(nCol);
							if(pP!=nullptr)
							pP->SetDBValue(nRow, fvalue);
							//pP->GetDBValue()
	  					//	pTab->SetDBCell(nCol, nRow, fvalue);
	  		  			}
	  				}
	
				}

		}
}
	else {


		vector <int> index_Tab;
		index_Tab.resize(0);
			string str, Group, Type, Name;
			vector<SimB*> vvp;
			for(size_t i=0;i<m_arrGroups.size();i++) {
					Group=m_arrGroups[i];
					Type="Tables";
					if(Group=="Multiple Plant") Group="Plant";
					vvp=GetPtrVector(Type, Group,true);

					for(size_t jj=0; jj<vvp.size(); jj++) {
						for(size_t jj2=0; jj2<m_Tab_Array.size(); jj2++) 
							if(vvp[jj]==m_Tab_Array[jj2]) {
								index_Tab.push_back(jj2);
								break;
							}
					}
			}


		iv=index_Tab.size();
		type="Tables";

		wString(&type);
		wInt(&iv);
        string group_psw;
        string name_psw;
        
		for(int i=0;i<iv;i++) {
			pTab=(Tab*)m_Tab_Array[index_Tab[i]];
            group_psw = pTab->GetGroup();
            name_psw = pTab->GetName();
            
			wString(&group_psw);
			wString(&name_psw);
            
			size_t v=pTab->GetNumOfCol();
			if(pTab->GetName()=="Growth stage") {

					v =pTab->GetNumOfRow();
					v=pTab->GetNumOfCol();
			}
			v=pTab->GetNumOfCol();
			wSize_t(&v);
			for(size_t j=0; j<v;j++) {
				str=pTab->GetTitle(j);
				wString(&str);
			}
			v =pTab->GetNumOfRow();
			wSize_t(&v);
			for(size_t nRow=0; nRow<pTab->GetNumOfRow(); nRow++) {
				for(size_t nCol=0;nCol<pTab->GetNumOfCol(); nCol++) {
						P *pP=pTab->GetP_Pointer(nCol);
						fvalue=float(pP->GetDBValue(nRow));
						wFloat(&fvalue);
				}
			}
		}
}	
	
	return true;
}
vector<Tab*> DB::ReadWrite_DB_ParTables(bool reading, int iv)
{
	vector<Tab*> Applied_Tab;
	P *pPt = nullptr;
	Tab* pTab = nullptr;
	string type, group, name;
	int  nColSize, aNE;
	float fvalue;
	string str, comp_str;
	type = "Tables";
	if (reading) {
		for (int i = 0; i<iv; i++) {
			rString(&group);
			if (group == "Driving variables") group = "Meteorological Data";
			if (group == "Soil mineral nitrogen processes") group = "Soil mineral N processes";
			rString(&name);
			if (m_Read_DB_Version>20) {

				rInt(&nColSize);
				for (int nCol = 0; nCol<nColSize; nCol++) {
					rString(&str);
					if (pTab != nullptr) {
						comp_str = string(pTab->GetTitle(nCol));
					}
				}
			}
			rInt(&aNE);
			if (aNE != 1) {
				int na = i;
			}
			if (name == "Above ground crop characteristics") name = "Above ground characteristics with time";
			if (name == "Evapotranspiration parameters") name = "Evapotranspiration - single canopy";
			if (name == "Scaling of Litter fall ") name = "Scaling of Litter fall";
			if (name == "Plant Behavior") name = "Plant Behaviour";
			pTab = (Tab*)GetPtr(type, group, name);

			if (pTab != nullptr) {
				int oldNE = pTab->GetNumOfRow();
				pTab->SetDB_Row(aNE);
				if (aNE != oldNE) {
					pTab->SetDB_Row(aNE);
				}

				pTab->SetNotOriginalValue();
			}
			else {
				string mes = "Table name :";
				mes.append(name);
				mes.append(" from file:");
				mes.append(m_SimFileName);
#ifndef COUPSTD
				CMainFrame *p_mainframe;
				p_mainframe = (CMainFrame*)AfxGetMainWnd();
				p_mainframe->UpdatePaneText(0, mes.c_str());
				//	MFC_Util::MessageBox(_T(mes.c_str()),_T("Fatal Input Error"),	MB_OK|MB_ICONEXCLAMATION);
#endif
				if (aNE>100 || aNE<1) return Applied_Tab;
				for (int nRow = 0; nRow<aNE; nRow++) {
					for (int nCol = 0; nCol<nColSize; nCol++)
						rFloat(&fvalue);
				}

			}
			if (pTab != nullptr) {
				if (m_Read_DB_Version<21) {
					nColSize = 0;
					//while(pTab->GetDBCell(nColSize, 0, &fvalue)){
					//  	  nColSize++;
					//	}
					nColSize = pTab->GetNumOfCol();
					if (m_Read_DB_Version == 0 && name == "Root depths development with time") nColSize = 2;
					if (m_Read_DB_Version <= 9 && name == "Size and shape of growing plant") nColSize = 7;
					if (m_Read_DB_Version <= 2 && name == "Size and shape of growing plant") nColSize = 5;
					if (m_Read_DB_Version <= 9 && name == "Above ground characteristics with time") nColSize = 4;
					if (m_Read_DB_Version <= 14 && name == "Size and shape of growing plant") nColSize = 7;
					if (m_Read_DB_Version == 1 && name == "Hydraulic conductivity, model boundaries") nColSize = 11;
					if (m_Read_DB_Version == 15 && name == "Harvest of Plants") nColSize = 8;
					if (name == "Harvest of Plants") nColSize = 9;
					if (name == "Initial conditions of plants") nColSize = 8;
					if (name == "Minimum CN Ratios of plants") nColSize = 3;
					if (name == "Scaling of Litter fall") nColSize = 6;

					if (m_Read_DB_Version <= 2 && name == "Start of growth") nColSize = 4;
					if (m_Read_DB_Version <= 2 && name == "Evapotranspiration - multiple canopies") nColSize = 7;
					if (m_Read_DB_Version <= 2 && name == "Allocation parameters") nColSize = 12;
				}
				if (name == "Growth stage") {
					int ijj = pTab->GetDB_Row();

				}
				for (int nRow = 0; nRow<aNE; nRow++) {
					for (int nCol = 0; nCol<nColSize; nCol++) {
						rFloat(&fvalue);
						P *pP = pTab->GetP_Pointer(nCol);
						if (pP != nullptr)
							pP->SetDBValue(nRow, fvalue);
						//pP->GetDBValue()
						//	pTab->SetDBCell(nCol, nRow, fvalue);
					}
				}
				Applied_Tab.push_back(pTab);

			}

		}
	}
	else {


		vector <int> index_Tab;
		index_Tab.resize(0);
		string str, Group, Type, Name;
		vector<SimB*> vvp;
		for (size_t i = 0; i<m_arrGroups.size(); i++) {
			Group = m_arrGroups[i];
			Type = "Tables";
			if (Group == "Multiple Plant") Group = "Plant";
			vvp = GetPtrVector(Type, Group, true);

			for (size_t jj = 0; jj<vvp.size(); jj++) {
				for (size_t jj2 = 0; jj2<m_Tab_Array.size(); jj2++)
					if (vvp[jj] == m_Tab_Array[jj2]) {
						index_Tab.push_back(jj2);
						break;
					}
			}
		}


		iv = index_Tab.size();
		type = "Tables";

		wString(&type);
		wInt(&iv);
        string group_psw;
        string name_psw;
        
		for (int i = 0; i<iv; i++) {
			pTab = (Tab*)m_Tab_Array[index_Tab[i]];
            
            group_psw = pTab->GetGroup();
            name_psw = pTab->GetName();
            
			wString(&group_psw);
			wString(&name_psw);
			size_t v = pTab->GetNumOfCol();
			if (pTab->GetName() == "Growth stage") {

				v = pTab->GetNumOfRow();
				v = pTab->GetNumOfCol();
			}
			v = pTab->GetNumOfCol();
			wSize_t(&v);
			for (size_t j = 0; j<v; j++) {
				str = pTab->GetTitle(j);
				wString(&str);
			}
			v = pTab->GetNumOfRow();
			wSize_t(&v);
			for (size_t nRow = 0; nRow<pTab->GetNumOfRow(); nRow++) {
				for (size_t nCol = 0; nCol<pTab->GetNumOfCol(); nCol++) {
					P *pP = pTab->GetP_Pointer(nCol);
					fvalue = float(pP->GetDBValue(nRow));
					wFloat(&fvalue);
				}
			}
		}
	}

	return Applied_Tab;
}
bool DB::Info_DB_Files(bool reading, int iv)
{
	F *pF=nullptr;
	string type,group,name, FileName;
	type="Files";

	if(reading) {
		if(type=="Files") {
			for(int i=0; i<iv;i++) {
				rString(&group);
				if(group=="Driving variables") group="Meteorological Data";
				if(group=="Soil mineral nitrogen processes") group="Soil mineral N processes";
		
				rString(&name);

				rString(&FileName);
				pF=(F*)GetPtr(type,group,name);
				if(pF!=nullptr&&group!="Hidden") {
					pF->SetDBValue(FileName);
				}

			}
		}

	}
	else {


		vector <int> index_F;
		index_F.resize(0);
			string str, Group, Type, Name;
			vector<SimB*> vvp;
			for(size_t i=0;i<m_arrGroups.size();i++) {
					Group=m_arrGroups[i];
					Type="Files";
					if(Group=="Multiple Plant") Group="Plant";
					vvp=GetPtrVector(Type, Group);

					for(size_t jj=0; jj<vvp.size(); jj++) {
						for( size_t jj2=0; jj2<m_F_Array.size(); jj2++) 
							if(vvp[jj]==m_F_Array[jj2]) {
								index_F.push_back(jj2);
								break;
							}
					}
			}


		iv=index_F.size();


		type="Files";

		wString(&type);
		wInt(&iv);
        string group_psw;
        string name_psw;
        string dbstringval_psw;
        
		for(size_t i=0; i<size_t(iv);i++) {
			pF=(F*)m_F_Array[index_F[i]];
            
            group_psw = pF->GetGroup();
            name_psw = pF->GetName();
            dbstringval_psw = pF->GetDBStrValue();
			wString(&group_psw);
			wString(&name_psw);
			wString(&dbstringval_psw);
		}
	}

return true;
}
vector<F*> DB::ReadWrite_DB_Files(bool reading, int iv)
{
	vector<F*> Applied_Files;
	F *pF = nullptr;
	string type, group, name, FileName;
	type = "Files";

	if (reading) {
		if (type == "Files") {
			for (int i = 0; i<iv; i++) {
				rString(&group);
				if (group == "Driving variables") group = "Meteorological Data";
				if (group == "Soil mineral nitrogen processes") group = "Soil mineral N processes";

				rString(&name);

				rString(&FileName);
				pF = (F*)GetPtr(type, group, name);
				if (pF != nullptr&&group != "Hidden") {
					pF->SetDBValue(FileName);
					Applied_Files.push_back(pF);
				}

			}
		}

	}
	else {


		vector <int> index_F;
		index_F.resize(0);
		string str, Group, Type, Name;
		vector<SimB*> vvp;
		for (size_t i = 0; i<m_arrGroups.size(); i++) {
			Group = m_arrGroups[i];
			Type = "Files";
			if (Group == "Multiple Plant") Group = "Plant";
			vvp = GetPtrVector(Type, Group);

			for (size_t jj = 0; jj<vvp.size(); jj++) {
				for (size_t jj2 = 0; jj2<m_F_Array.size(); jj2++)
					if (vvp[jj] == m_F_Array[jj2]) {
						index_F.push_back(jj2);
						break;
					}
			}
		}


		iv = index_F.size();


		type = "Files";

		wString(&type);
		wInt(&iv);
        string group_psw;
        string name_psw;
        string dbstringval_psw;
		for (size_t i = 0; i<size_t(iv); i++) {
			pF = (F*)m_F_Array[index_F[i]];
            
            group_psw = pF->GetGroup();
            name_psw = pF->GetName();
            dbstringval_psw = pF->GetDBStrValue();
            
			wString(&group_psw);
			wString(&name_psw);
			wString(&dbstringval_psw);
		}
	}

	return Applied_Files;
}
bool DB::Apply_DB_Struc(string Name)
{
	Sw *pSw;
	time_t sec;
	sec=time(nullptr);


	string Type, Group;

	Type="Switches";
	if(Name=="TimeResolution") 
		Group="Hidden";
	else
		Group=m_Group;

	pSw=(Sw*)GetPtr(Type, Group, Name);
	if(pSw!=nullptr) {
		int ivalue=pSw->GetIntValue();
		if(ivalue!=pSw->GetDBIntValue()) {
			pSw->SetIntValue(pSw->GetDBIntValue());
			int ityp=p_ModelInfo->GetTypeIndex(Type);
			History_Add(pSw,ityp,-1,sec,pSw->GetOption(pSw->GetDBIntValue()),"DB: "+m_DB_key);
		}
		return true;
	}
	else  
		return false;


}
bool DB::Apply_DB_DocFile(string group,   string DB_key, bool during)
{
		m_DuringRun=during;
		Sw *pSw;

		string Type, Name, Group;
		
		Name="TimeResolution";
		Type="Switches";
		Group="Technical";

		m_DB_key=DB_key;

		m_pSwTimeRes=(Sw*)GetPtr(Type, Group, Name);
		Group="Model Structure";
		Name="PlantType";
		m_pSwPlantType=(Sw*)GetPtr(Type, Group, Name);

		Group="Model Structure";

		Name="WaterEq";
		pSw=(Sw*)GetPtr(Type, Group,Name);
		pSw->SetDBIntValue(1);



		Name="SnowPack";
		m_Group=group;
	  
		if(m_Group=="Snow") {
			pSw=(Sw*)GetPtr(Type, Group,Name);
			pSw->SetDBIntValue(1);
			group="SnowPack";
		}
		if(m_Group=="Soil frost") {
			Name="HeatEq";
			pSw=(Sw*)GetPtr(Type, Group,Name);
			pSw->SetDBIntValue(1);
		}
		if(m_Group=="Soil evaporation") {
			Name="Evaporation Method";
			Group=m_Group;
		}

		m_Group=group;
		if(m_Group=="Multiple Plant") {
			Group="Plant";
			m_Group=Group;
		}

// init defined above
// Apply below

	SimB *pBase;
	F *pCF;
	vector<SimB*> vptr;

	float fvalue, f_org, f_db;

	int ivalue;
	int ityp;
	time_t sec;
	sec=time(nullptr);
	int numchange[5];
	numchange[0] = numchange[1] = numchange[2] = numchange[3] = numchange[4] = 0;

	if(m_Group=="Plant") {
		ityp=p_ModelInfo->GetTypeIndex(string("Switches"));
		ivalue=m_pSwTimeRes->GetIntValue();
		if(m_pSwTimeRes->GetDBIntValue()!=ivalue) {
			m_pSwTimeRes->SetIntValue(m_pSwTimeRes->GetDBIntValue());	
			if (m_DB_key.size() > 0) {
				History_Add(m_pSwTimeRes, -1, sec, m_pSwTimeRes->GetOption(m_pSwTimeRes->GetDBIntValue()), "DB: " + m_DB_key);
				numchange[0]++;
			}
		}
		ivalue=m_pSwPlantType->GetIntValue();
		if(m_pSwPlantType->GetDBValue()!=ivalue) {
			m_pSwPlantType->SetIntValue(m_pSwPlantType->GetDBIntValue());
			if (m_DB_key.size() > 0) {
				History_Add(m_pSwPlantType, -1, sec, m_pSwPlantType->GetOption(m_pSwPlantType->GetDBIntValue()), "DB: " + m_DB_key);
				numchange[0]++;
			}
		}
	}

	Type="Switches";
	vptr=GetPtrVector(Type,m_Group, true);
	if(vptr.size()>0) {
		ityp=p_ModelInfo->GetTypeIndex(Type);
		for(size_t i=0;i<vptr.size();i++) {
			pSw=(Sw*)vptr[i];
			ivalue=pSw->GetIntValue();
			if(ivalue!=pSw->GetDBIntValue()) {


				pSw->SetIntValue(pSw->GetDBIntValue());
				if (m_DB_key.size() > 0) {
					History_Add(pSw, ityp, int(-1), sec, string(pSw->GetOption(pSw->GetDBIntValue())), "DB: " + m_DB_key);
					numchange[0]++;
				}
			}
		}
	}
	Type="Parameters";
	vptr=GetPtrVector(Type,m_Group);
	if(vptr.size()>0) {
		ityp=p_ModelInfo->GetTypeIndex(Type);
		for(size_t i=0;i<vptr.size();i++) {
			pBase=(SimB*)vptr[i];
			Ps *pPs;
			pPs= dynamic_cast<Ps*>(vptr[i]);
			fvalue=float(pPs->GetValue());
			f_db=float(pPs->GetDBValue());
			if(fvalue!=f_db&&f_db<=MISSING) {
				f_org=float(pPs->GetOriginalValue());
				if(f_org!=f_db) {
					pPs->SetValue(f_db);
					if (m_DB_key.size() > 0) {
						History_Add(pPs, ityp, -1, sec, float(pPs->GetDBValue()), "DB: " + m_DB_key);
						numchange[1]++;

					}
				}
			}
		}
	}
//Parameter Tables
	Type="Tables";

	vptr=GetPtrVector(Type,m_Group);
	if(vptr.size()>0) {
		ityp=p_ModelInfo->GetTypeIndex(Type);
		for(size_t i=0;i<vptr.size();i++) {
			Tab* pTab=dynamic_cast<Tab*>(vptr[i]);
		 // ((Tab*)pBase)->GetDBCell(0, 0,&fvalue);
		  string Name=pTab->GetName();
		  if(Name!="Root distribution with depth") {
			auto oldvalue=pTab->GetNumOfRow();
			auto newvalue=pTab->GetDB_Row();
			if(!m_DuringRun) 
			  pTab->SetNumOfRow(newvalue);
			if(oldvalue!=newvalue) {
				string str=FUtil::STD_ItoAscii(newvalue);
				if (m_DB_key.size() > 0) {
					History_Add(pTab, ityp, -1, sec, str, "DB: " + m_DB_key);
					numchange[2]++;
				}
			}
		  
			for(size_t nRow=0; nRow<pTab->GetNumOfRow(); nRow++){
				for (size_t nCol=0;nCol<pTab->GetNumOfCol(); nCol++){
					if(!m_DuringRun||nRow<oldvalue) {//((Tab*)pBase)->SetCell(nCol, nRow,fvalue);
						P* pCPt;
						pCPt=pTab->GetP_Pointer(nCol);
						ityp=p_ModelInfo->GetTypeIndex(string("Table Parameters"));
						if(pCPt->GetValue(nRow)!=pCPt->GetDBValue(nRow)) {
							fvalue=float(pCPt->GetDBValue(nRow));
							f_org=float(pCPt->GetOriginalValue(nRow));
							if(fvalue!=f_org&&fvalue>0.9*MISSING) {
								pCPt->SetValue(nRow, fvalue);
								if (m_DB_key.size() > 0) {
									History_Add(pCPt, nRow, sec, fvalue, "DB: " + m_DB_key);
									numchange[3]++;
								}
							}
						}
					}

				}
			}
//			((Tab*)pBase)->Apply_Cells();
		  }
		}
	}
	Type="Files";
	vptr=GetPtrVector(Type,m_Group);
	if(vptr.size()>0) {
		ityp=p_ModelInfo->GetTypeIndex(Type);
		for(size_t i=0;i<vptr.size();i++) {
			pCF=(F*)vptr[i];
			if(pCF->GetStrValue()!=pCF->GetDBStrValue()&&pCF->GetDBStrValue()!="") {
				pCF->SetValue(pCF->GetDBStrValue());
				if(m_DB_key.size()>0) {
					History_Add(pCF,-1,sec,pCF->GetDBStrValue(),"DB: "+m_DB_key);
					numchange[4]++;
				}
			}
		}
	}
#ifndef COUPSTD
	MFC_Util::MessageBoxReturn(FUtil::STD_ItoAscii(numchange[0]) + " Switches Changed \n" +
		FUtil::STD_ItoAscii(numchange[1]) + " Parameter Changed \n" +
		FUtil::STD_ItoAscii(numchange[2]) + " Parameter Table elements Changed \n" +
		FUtil::STD_ItoAscii(numchange[3]) + " Parameter values in Tables Changed \n" +
		FUtil::STD_ItoAscii(numchange[4]) + " Model Files Changed \n", "From Object :" + m_DB_key+
		"Connected to group :"+ group, 0);
#endif
	 
	return true;


}
bool DB::Apply_DB_DocFile(vector<Sw*> AppliedSw, vector<Ps*> AppliedPs, vector<Tab*> Applied_Tab, vector<F*> Applied_Files)
{
	time_t sec;
	sec = time(nullptr);
	m_DuringRun = false;
	int numchange[5];
	numchange[0] = numchange[1] = numchange[2] = numchange[3] = numchange[4] = 0;

	size_t ityp;
	auto checkswitch = [&](Sw* pSw) {
		if (pSw->GetIntValue() != pSw->GetDBIntValue()) {
			string m;
			m = pSw->GetName() + " From: ";
			m += pSw->GetOption(pSw->GetIntValue()) + "To: ";
			m += pSw->GetOption(pSw->GetDBIntValue());
#ifndef COUPSTD
			return MFC_Util::MessageBoxReturn(m, "Change of Switch", MB_YESNO);
#endif
		}
		return 7;
	};
	auto checkpar = [&](Ps* pPs) {
		if (pPs->GetValue() != pPs->GetDBValue()) {
			string m;
			m = pPs->GetName() + " From: ";
			string before, after;
			before = FUtil::STD_DtoAscii(pPs->GetValue());
			after = FUtil::STD_DtoAscii(pPs->GetDBValue());
#ifndef COUPSTD
			m +=  before+ " To: ";
			m += after +" ?";
			if (before != after)
				return MFC_Util::MessageBoxReturn(m, "Change of Parameter", MB_YESNO);
			else
				return 7;
#endif
			return 6;
		}
		return 7;
	};
	auto checkpartable = [&](P* pP, size_t index) {
		if (pP->GetValue(index) != pP->GetDBValue(index)) {
			string m;
			m = pP->GetName() + "Layer " + FUtil::STD_ItoAscii(index) + " From: ";

			string before, after;
			before = FUtil::STD_DtoAscii(pP->GetValue(index));
			after = FUtil::STD_DtoAscii(pP->GetDBValue(index));
#ifndef COUPSTD
			m += before + " To: ";
			m += after  + " ?";
			if (before != after)
				return MFC_Util::MessageBoxReturn(m, "Change of Table Parameter", MB_YESNO);
			else
				return 7;
#endif
			return 6;
		}
		return 7;
	};
	auto checkTable = [&](Tab* pTab) {
		if (pTab->GetNumOfRow() != pTab->GetDB_Row()) {
			string m;
			m = pTab->GetName() + " From: ";
			m += FUtil::STD_ItoAscii(pTab->GetNumOfRow()) + " To: ";
			m += FUtil::STD_ItoAscii(pTab->GetDB_Row()) + " ?";
#ifndef COUPSTD
			return int(MFC_Util::MessageBoxReturn(m, "Change of Elements in Table", MB_YESNO));
#endif
			return 6;
		}
		return 7;
	};

	auto checkFileName = [&](F* pF) {
		if (pF->GetStrValue() != pF->GetDBStrValue()) {
			string m;
			m = pF->GetName() + " From: ";
			m += pF->GetStrValue() + " To: ";
			m += pF->GetDBStrValue()+ " ?";
#ifndef COUPSTD
			return MFC_Util::MessageBoxReturn(m, "Change of PG File name", MB_YESNO);
#endif
			return 6;
		}
		return 7;
	};



	for (size_t i = 0; i<AppliedSw.size(); i++) {
		Sw* pSw = AppliedSw[i];
		
		auto ans=checkswitch(pSw);
		if (ans==6) {
			pSw->SetIntValue(pSw->GetDBIntValue());
			History_Add(pSw, int(-1), sec, string(pSw->GetOption(pSw->GetDBIntValue())), "DB: " + m_DB_key);
			numchange[0]++;
		}

	}
	//ityp = p_ModelInfo->GetTypeIndex("Parameters");
	for (size_t i = 0; i<AppliedPs.size(); i++) {
		Ps* pPs = AppliedPs[i];		
		if (checkpar(pPs) == 6) {
			pPs->SetValue(pPs->GetDBValue());
			History_Add(pPs, int(-1), sec, pPs->GetDBValue(), "DB: " + m_DB_key);
			numchange[1]++;
		}
	}
	//Parameter Tables
		//ityp = p_ModelInfo->GetTypeIndex("Tables");
		for (size_t i = 0; i<Applied_Tab.size(); i++) {
			Tab* pTab = dynamic_cast<Tab*>(Applied_Tab[i]);
			// ((Tab*)pBase)->GetDBCell(0, 0,&fvalue);
			string Name = pTab->GetName();
			if (Name != "Root distribution with depth") {
				auto oldvalue = pTab->GetNumOfRow();
				auto newvalue = pTab->GetDB_Row();


				auto ans = checkTable(pTab);
				if (ans==6) {
					if (!m_DuringRun)
						pTab->SetNumOfRow(newvalue);
					string str = FUtil::STD_ItoAscii(newvalue);
					if (m_DB_key.size() > 0) {
						History_Add(pTab, -1, sec, str, "DB: " + m_DB_key);
						numchange[2]++;
					}
				}

				for (size_t nRow = 0; nRow<pTab->GetNumOfRow(); nRow++) {
					for (size_t nCol = 0; nCol<pTab->GetNumOfCol(); nCol++) {
						
						if (!m_DuringRun || nRow<oldvalue||ans==6) {//((Tab*)pBase)->SetCell(nCol, nRow,fvalue);
							P* pCPt;
							pCPt = pTab->GetP_Pointer(nCol);
							ans = checkpartable(pCPt, nRow);
							if (ans==6) {
								float fvalue = float(pCPt->GetDBValue(nRow));
								float f_org = float(pCPt->GetOriginalValue(nRow));
								if (fvalue != f_org&&fvalue>0.9*MISSING) {
									pCPt->SetValue(nRow, fvalue);
									if (m_DB_key.size() > 0) {
										History_Add(pCPt, nRow, sec, fvalue, "DB: " + m_DB_key);
										numchange[3]++;
									}
								}
							}
						}

					}
				}
				
			}
		}

		ityp = p_ModelInfo->GetTypeIndex("Files");
		for (size_t i = 0; i<Applied_Files.size(); i++) {
			F* pCF = (F*)Applied_Files[i];
			if (pCF->GetStrValue() != pCF->GetDBStrValue() && pCF->GetDBStrValue() != "") {

				auto ans = checkFileName(pCF);
				if (ans == 6) {
					pCF->SetValue(pCF->GetDBStrValue());
					if (m_DB_key.size() > 0) {
						History_Add(pCF, -1, sec, pCF->GetDBStrValue(), "DB: " + m_DB_key);
						numchange[4]++;
					}
				}
			}
		}
#ifndef COUPSTD
	MFC_Util::MessageBoxReturn(FUtil::STD_ItoAscii(numchange[0]) + " Switches Changed \n" +
		FUtil::STD_ItoAscii(numchange[1]) + " Parameter Changed \n" +
		FUtil::STD_ItoAscii(numchange[2]) + " Parameter Table elements Changed \n" +
		FUtil::STD_ItoAscii(numchange[3]) + " Parameter values in Tables Changed \n" +
		FUtil::STD_ItoAscii(numchange[4]) + " Model Files Changed \n", "From Object :" + m_DB_key, 0);
#endif
	return false;
}
bool DB::Export_DB_CSVFile(vector<Sw*> AppliedSw, vector<Ps*> AppliedPs, vector<Tab*> Applied_Tab, vector<F*> Applied_Files)
{
	time_t sec;
	sec = time(nullptr);
	int numchange[5];
	numchange[0] = numchange[1] = numchange[2] = numchange[3] = numchange[4] = 0;

	

	int ityp = p_ModelInfo->GetTypeIndex("Switches");
	for (size_t i = 0; i<AppliedSw.size(); i++) {
		Sw* pSw = AppliedSw[i];
		pSw->SetIntValue(pSw->GetDBIntValue());
		numchange[0]++;

	}
	ityp = p_ModelInfo->GetTypeIndex("Parameters");
	for (size_t i = 0; i<AppliedPs.size(); i++) {
		Ps* pPs = AppliedPs[i];
		pPs->SetValue(pPs->GetDBValue());
	
		numchange[1]++;
	}
	//Parameter Tables
	ityp = p_ModelInfo->GetTypeIndex("Tables");
	for (size_t i = 0; i<Applied_Tab.size(); i++) {
		Tab* pTab = dynamic_cast<Tab*>(Applied_Tab[i]);
		// ((Tab*)pBase)->GetDBCell(0, 0,&fvalue);
		string Name = pTab->GetName();
		if (Name != "Root distribution with depth") {
			auto oldvalue = pTab->GetNumOfRow();
			auto newvalue = pTab->GetDB_Row();
			if (!m_DuringRun)
				pTab->SetNumOfRow(newvalue);
			if (oldvalue != newvalue) {
				string str = FUtil::STD_ItoAscii(newvalue);

			}

			for (size_t nRow = 0; nRow<pTab->GetNumOfRow(); nRow++) {
				for (size_t nCol = 0; nCol<pTab->GetNumOfCol(); nCol++) {
					if (!m_DuringRun || nRow<oldvalue) {//((Tab*)pBase)->SetCell(nCol, nRow,fvalue);
						P* pCPt;
						pCPt = pTab->GetP_Pointer(nCol);
						ityp = p_ModelInfo->GetTypeIndex(string("Table Parameters"));
						if (pCPt->GetValue(nRow) != pCPt->GetDBValue(nRow)) {
							float fvalue = float(pCPt->GetDBValue(nRow));
							float f_org = float(pCPt->GetOriginalValue(nRow));
							if (fvalue != f_org&&fvalue>0.9*MISSING) {
								pCPt->SetValue(nRow, fvalue);

							}
						}
					}

				}
			}

		}
	}

	ityp = p_ModelInfo->GetTypeIndex("Files");
	for (size_t i = 0; i<Applied_Files.size(); i++) {
		F* pCF = (F*)Applied_Files[i];
		if (pCF->GetStrValue() != pCF->GetDBStrValue() && pCF->GetDBStrValue() != "") {
			pCF->SetValue(pCF->GetDBStrValue());

				numchange[4]++;

		}
	}
#ifndef COUPSTD
	MFC_Util::MessageBoxReturn(FUtil::STD_ItoAscii(numchange[0]) + " Switches Changed \n" +
		FUtil::STD_ItoAscii(numchange[1]) + " Parameter Changed \n" +
		FUtil::STD_ItoAscii(numchange[2]) + " Parameter Table elements Changed \n" +
		FUtil::STD_ItoAscii(numchange[3]) + " Parameter values in Tables Changed \n" +
		FUtil::STD_ItoAscii(numchange[4]) + " Model Files Changed \n", "From Object :" + m_DB_key+" to CSV File", 0);
#endif


	return false;
}
void DB::Actual_DB(string group)
{

	SimB *pBase;
	Sw *pSw;
	F *pCF;
	if(group!="") m_Group=group;
	vector<SimB*> vptr;
	string Type, str, Group, Name;
	if(m_pSwTimeRes==nullptr) {
		m_pSwTimeRes=(Sw*)GetPtr(SWITCH,"TimeResolution");
		m_pSwPlantType=(Sw*)GetPtr(SWITCH,"PlantType");
	}

	m_pSwTimeRes->SetDBIntValue(m_pSwTimeRes->GetIntValue());
	m_pSwPlantType->SetDBIntValue(m_pSwPlantType->GetIntValue());

	Type="Switches";

	vptr=GetPtrVector(Type,m_Group);
	if(vptr.size()>0) {
		for(size_t i=0;i<vptr.size();i++) {
			pSw=(Sw*)vptr[i];
			pSw->SetDBIntValue(pSw->GetIntValue());
			if(pSw->GetName()=="Growth") {
				int i=pSw->GetDBIntValue();

			}
		}
	}
	Type="Parameters";
	vptr=GetPtrVector(Type,m_Group);
	if(vptr.size()>0) {
		for(size_t i=0;i<vptr.size();i++) {
			pBase=(SimB*)vptr[i];
			((Ps*)pBase)->SetDBValue(((Ps*)pBase)->GetValue());
		}
	}
//Parameter Tables
	Type="Tables";
	vptr=GetPtrVector(Type,m_Group);
	if(vptr.size()>0) {
		for(size_t i=0;i<vptr.size();i++) {
			pBase=(SimB*)vptr[i];
			((Tab*)pBase)->SetDBIntValue(((Tab*)pBase)->GetIntValue());
			for(size_t nRow=0; nRow<((Tab*)pBase)->GetNumOfRow(); nRow++){
				for (size_t nCol=0;nCol<((Tab*)pBase)->GetNumOfCol(); nCol++){
					P *pP=((Tab*)pBase)->GetP_Pointer(nCol);
					auto fvalue=float(pP->GetValue(nRow));
					pP->SetDBValue(nRow, fvalue);
				}
			}
		}
	}
	Type="Files";
	vptr=GetPtrVector(Type,m_Group);
	if(vptr.size()>0) {
		for(size_t i=0;i<vptr.size();i++) {
			pCF=(F*)vptr[i];
			pCF->SetDBValue(pCF->GetStrValue());
		}
	}  

}

bool DB::rString(string *str)
{
	unsigned char il;
	size_t ic;
	char cc;
		long long s=m_DocFileIn.tellg();
		if(s<0) return false;
		m_DocFileIn.read((char*)(&il),1);
		cc=il;
		if(il==255) {
			char s1,s2;
			m_DocFileIn.read((char*)(&s1), 1);
			m_DocFileIn.read((char*)(&s2), 1);
			ic=255+s2;
		}
		else if(cc<0xFF) {
			ic=il;

		}
		else if(cc==0xFF) {
			char s1,s2;
			m_DocFileIn.read((char*)(&s1),1);
			m_DocFileIn.read((char*)(&s2), 1);
			ic=255+s2;

		}
		else {
			m_DocFileIn.read((char*)&ic, 2);


		}
		
		//m_DocFileIn.read((char*)(&ic),sizeof(ic));
		if(il== 0xfffe) {
			int ivv;
			m_DocFileIn.read((char*)&ivv, 2);

			str->clear();
			long long s=m_DocFileIn.tellg();
			if(s<0) {
				return false;

			}
		}
		else if(il== 0xffff) {
			int ivv;
			m_DocFileIn.read((char*)&ivv, 4);


		}
		else {
			m_DocFileIn.read(m_cbuf, ic);
			str->clear();
			if(ic>0)
				str->assign(&m_cbuf[0],ic);
		}
		return true;

}
bool DB::rInt(int *iv)
{   int i;
	m_DocFileIn.read((char*)(&i) ,4);
	*iv=i;
	return true;
}
bool DB::rUint(unsigned int * iv)
{   unsigned int i = 0;
	m_DocFileIn.read((char*)(&i) ,4);
	*iv=i;
	return true;
}
bool DB::rSize_t(size_t * iv)
{
	unsigned int i = 0;
	m_DocFileIn.read((char*)(&i), 4);
	*iv = i;
	return true;
}
bool DB::rBOOL(bool *iv)
{
	int iBOOL;
	m_DocFileIn.read((char*)(&iBOOL) ,4);
	if(iBOOL!=0)
		*iv=true;
	else
		*iv=false;
	return true;
}
bool DB::rbool(bool *iv)

{ 
	m_DocFileIn.read((char*)(&*iv) ,1);
	return true;
}
bool DB::rFloat(float *f)
{   float v;
	m_DocFileIn.read((char*)(&v) ,4);
	*f=v;
	return true;
}
bool DB::rDouble(double *f)
{   double v;
	m_DocFileIn.read((char*)(&v) ,8);
	*f=v;
	return true;
}

bool DB::wString(string *str) 
{
	//char il;
	long long a=m_DocFileOut.tellp();
	auto il=str->size();
	if(il>255) il=0;
	m_DocFileOut.write((char*)(&il),1);
	if(il>0)
		m_DocFileOut.write((char*)str->data(), il);
	a=m_DocFileOut.tellp();

		return true;

}
bool DB::wInt(int *iv)		{  m_DocFileOut.write((char*)(&*iv) ,4);	return true; }
bool DB::wSize_t(size_t *iv) {m_DocFileOut.write((char*)(&*iv), 4); return true;}
bool DB::wUint(unsigned int *iv) {m_DocFileOut.write((char*)(&*iv) ,4);	if(m_DocFileOut.good())	return true;else return false;}

bool DB::wbool(bool *iv)
{ 
	m_DocFileOut.write((char*)(&*iv) ,1);
	//*iv=i;
	return true;
}
bool DB::wFloat(float *f)
{   
	m_DocFileOut.write((char*)(&*f) ,4);
	return true;
}
bool DB::wDouble(double *f)
{   
	m_DocFileOut.write((char*)(&*f) ,8);
	return true;
}
bool DB::wTime(time_t *time)
{

	m_DocFileOut.write((char*)&*time,8);
	return true;
}
bool DB::rTime(time_t *time)
{
	char buffer[8];
	char* buf;
	buf=&buffer[0];
	if(m_FileVersionNumberRead>=50) 
		m_DocFileIn.read((char*)&*time,8);
	else if(m_FileVersionNumberRead<48||m_ShortTimeRead) {
		//m_DocFileIn.read((char*)&*time,8);
		m_DocFileIn.read(m_cbuf,4);
		for(int i=4;i<8;i++) 
			*(buf+i)=0;
		for(int i=0;i<4;i++) 
			*(buf+i)=m_cbuf[i];
		*time=*(time_t*)buf;
	}
	else  {
		m_DocFileIn.read(m_cbuf,12);
		for(int i=4;i<8;i++) 
			*(buf+i)=0;
		for(int i=0;i<4;i++) 
			*(buf+i)=m_cbuf[4+i];
		*time=*(time_t*)buf;
	}

		
	//if(m_FileVersionNumberRead<50)
	//	*time=0;

	return true;
}
string DB::CreateFlagString(SimB* currentParameter, bool GDType, int FlagValue)
{
	string outString="";
/*	int selected;
//	DataTrans converter;
	int updateSequenceStart=-1;
	if (GDType)
	{
		outString+="\""+STD_ItoAscii(((G*)currentParameter)->GetNumberOfFlags())+"\"   ";
		//Special case: only one flag
		if (((G*)currentParameter)->GetNumberOfFlags()==1)
		{
			((G*)currentParameter)->GetFlag(0,&selected);
			if (selected==FlagValue)
				outString+="\"1";
			else
				outString+="\"\"";
		}
		else
		//Loop through all the flags
		{
		outString+="\"";
		for (size_t i=0;i<((G*)currentParameter)->GetNumberOfFlags();i++)		
		{
			//Get the state of the i:th flag
			((G*)currentParameter)->GetFlag(i,&selected);
			//If this flag is selected and updateSequenceStart isn´t set then
			if (selected==FlagValue && (updateSequenceStart==-1))
			{
				//If this is the last item then it should be added to outString
				if (i==(((X*)currentParameter)->GetNumberOfFlags()-1))
					outString+=STD_ItoAscii(((X*)currentParameter)->GetNumberOfFlags());
				else
					updateSequenceStart=i;
			}
			else
			{
				//else if it isn´t selected and updateSequenceStart is set
				//or this is the last item in the flaglist then update outString
				if (((selected!=FlagValue)&&(updateSequenceStart!=-1))||(((i==(((X*)currentParameter)->GetNumberOfFlags()-1))&&selected==FlagValue)))
				{
					//Update outString
					//If this is the last flag in the list and it is selected
					if ((i==(((G*)currentParameter)->GetNumberOfFlags()-1))&&selected==FlagValue)
					{
						outString+=STD_ItoAscii(updateSequenceStart+1)+"-"+STD_ItoAscii(((G*)currentParameter)->GetNumberOfFlags());
					}
					else
					{
						if ((i-updateSequenceStart)==1)
							outString+=STD_ItoAscii(updateSequenceStart+1)+",";
						else
							outString+=STD_ItoAscii(updateSequenceStart+1)+"-"+STD_ItoAscii(i)+",";
						//Reset updateSequenceStart
						updateSequenceStart=-1;
					}
				}
			}
		}
		}
	}
	else
	{
		outString+="\""+STD_ItoAscii(((X*)currentParameter)->GetNumberOfFlags())+"\"   ";
		//Special case: only one flag
		if (((X*)currentParameter)->GetNumberOfFlags()==1)
		{
			((X*)currentParameter)->GetFlag(0,&selected);
			if (selected==FlagValue)
				outString+="\"1";
			else
				outString+="\"\"";
		}
		else
		//Loop through all the flags
		{
		outString+="\"";
		for (size_t i=0;i<((X*)currentParameter)->GetNumberOfFlags();i++)		
		{
			//Get the state of the i:th flag
			((X*)currentParameter)->GetFlag(i,&selected);
			//If this flag is selected and updateSequenceStart isn´t set then
			if (selected==FlagValue && (updateSequenceStart==-1))
			{
				//If this is the last item then it should be added to outString
				if (i==(((X*)currentParameter)->GetNumberOfFlags()-1))
					outString+=STD_ItoAscii(((X*)currentParameter)->GetNumberOfFlags());
				else
					updateSequenceStart=i;
			}
			else
			{
				//else if it isn´t selected and updateSequenceStart is set
				//or this is the last item in the flaglist then update outString
				if (((!selected)&&(updateSequenceStart!=-1))||(((i==(((X*)currentParameter)->GetNumberOfFlags()-1))&&selected==FlagValue)))
				{
					//Update outString
					//If this is the last flag in the list and it is selected
					if ((i==(((X*)currentParameter)->GetNumberOfFlags()-1))&&selected==FlagValue)
					{
						outString+=STD_ItoAscii(updateSequenceStart+1)+"-"+STD_ItoAscii(((X*)currentParameter)->GetNumberOfFlags());
					}
					else
					{
						if ((i-updateSequenceStart)==1)
							outString+=STD_ItoAscii(updateSequenceStart+1)+",";
						else
							outString+=STD_ItoAscii(updateSequenceStart+1)+"-"+STD_ItoAscii(i)+",";
						//Reset updateSequenceStart
						updateSequenceStart=-1;
					}
				}
			}
		}
		}
	}
	//If the text ends with a , then remove the , from the string
	if ((outString.size()!=0)&&(outString[outString.size()-1]==","))
		outString=outString.substr(0, outString.size()-1);
	//If the string doesnt allready end with two "" add a "
	if (!((outString[outString.size()-1]=="\"")&&((outString.size()>2)&&(outString[outString.size()-2]=="\""))))
			outString+="\"";   */
	return outString;
}
