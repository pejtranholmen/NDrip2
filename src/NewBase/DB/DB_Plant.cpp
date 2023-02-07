// DB_Plant.cpp : implementation file
//

#include "../stdafx.h"
#include "../std.h"
#include "../Util.h"
#include "./DB_Plant.h"

#ifndef COUPSTD
#include "../NewBase\Doc.h"
#endif






DB_Plant::DB_Plant(string pszCaption,bool readonly, void *pDoc)
{
		m_ReadOnly=readonly;
		m_DuringRun=false;
	
		m_pDoc=pDoc;
		string Type, Name, Group;
		
		Name="TimeResolution";
		Type="Switches";
		Group="Hidden";

		m_pSwTimeRes=(Sw*)((NewMap*)m_pDoc)->GetPtr(Type, Group, Name);

		Group="Model Structure";
		Name="PlantType";
		m_pSwPlantType=(Sw*)((NewMap*)m_pDoc)->GetPtr(Type, Group, Name);
		Name="WaterEq";
		Sw *pSw;
		pSw=(Sw*)((NewMap*)m_pDoc)->GetPtr(Type, Group,Name);
		pSw->SetDBIntValue(1);


		Name="SnowPack";
		m_Group=pszCaption;
 
		if(m_Group=="Snow") {
			pSw=(Sw*)((NewMap*)m_pDoc)->GetPtr(Type, Group,Name);
			pSw->SetDBIntValue(1);
			pszCaption="SnowPack";
		}
		if(m_Group=="Soil frost") {
			Name="HeatEq";
			pSw=(Sw*)((NewMap*)m_pDoc)->GetPtr(Type, Group,Name);
			pSw->SetDBIntValue(1);
		}
		if(m_Group=="Soil evaporation") {
			Name="Evaporation Method";
			Group=m_Group;
		}
		if(m_Group=="Meteorological Data") {
			Name="RadiationInput";
			Group="Hidden";
			pSw=(Sw*)((NewMap*)m_pDoc)->GetPtr(Type, Group,Name);
			pSw->SetDBIntValue(1);
			Name="TypeOfDrivingFile";
			pSw=(Sw*)((NewMap*)m_pDoc)->GetPtr(Type, Group,Name);
			pSw->SetDBIntValue(1);


		}
		
		Group=pszCaption;
		if(m_Group=="Multiple Plant") {
			Group="Plant";
			m_Group=Group;
		}
	

}
DB_Plant::DB_Plant(string pszCaption, void *pDoc,  string DB_key)
{
		m_DuringRun=false;

		Sw *pSw;

		m_pDoc=pDoc;
		string Type, Name, Group;
		
		Name="TimeResolution";
		Type="Switches";
		Group="Hidden";

		m_DB_key=DB_key;

		m_pSwTimeRes=(Sw*)((ModelBase*)m_pDoc)->GetPtr(Type, Group, Name);
		Group="Model Structure";
		Name="PlantType";
		m_pSwPlantType=(Sw*)((NewMap*)m_pDoc)->GetPtr(Type, Group, Name);

		Group="Model Structure";

		Name="WaterEq";
		pSw=(Sw*)((NewMap*)m_pDoc)->GetPtr(Type, Group,Name);
		pSw->SetDBIntValue(1);



		Name="SnowPack";
		m_Group=pszCaption;
	  
		if(m_Group=="Snow") {
			pSw=(Sw*)((NewMap*)m_pDoc)->GetPtr(Type, Group,Name);
			pSw->SetDBIntValue(1);
			pszCaption="SnowPack";
		}
		if(m_Group=="Soil frost") {
			Name="HeatEq";
			pSw=(Sw*)((NewMap*)m_pDoc)->GetPtr(Type, Group,Name);
			pSw->SetDBIntValue(1);
		}
		if(m_Group=="Soil evaporation") {
			Name="Evaporation Method";
			Group=m_Group;
		//	pSw=(Sw*)((NewMap*)m_pDoc)->GetPtr(Type, Group,Name);
	//		pSw->SetDBValue(1);
		}


		m_Group=pszCaption;
		if(m_Group=="Multiple Plant") {
			Group="Plant";
			m_Group=Group;
		}


	
}


void DB_Plant::Init(string pszCaption,bool readonly, void *pDoc)
{

		m_ReadOnly=readonly;
		m_DuringRun=false;
		Sw *pSw;

		m_pDoc=pDoc;
		string Type, Name, Group;
		
		Name="TimeResolution";
		Type="Switches";
		Group="Hidden";

		m_pSwTimeRes=(Sw*)((NewMap*)m_pDoc)->GetPtr(Type, Group, Name);

		Group="Model Structure";
		Name="PlantType";
		m_pSwPlantType=(Sw*)((NewMap*)m_pDoc)->GetPtr(Type, Group, Name);
		Name="WaterEq";
		pSw=(Sw*)((NewMap*)m_pDoc)->GetPtr(Type, Group,Name);
		pSw->SetDBIntValue(1);


		Name="SnowPack";
		m_Group=pszCaption;
 
		if(m_Group=="Snow") {
			pSw=(Sw*)((NewMap*)m_pDoc)->GetPtr(Type, Group,Name);
			pSw->SetDBIntValue(1);
			pszCaption="SnowPack";
		}
		if(m_Group=="Soil frost") {
			Name="HeatEq";
			pSw=(Sw*)((NewMap*)m_pDoc)->GetPtr(Type, Group,Name);
			pSw->SetDBIntValue(1);
		}
		if(m_Group=="Soil evaporation") {
			Name="Evaporation Method";
			Group=m_Group;
		}
		if(m_Group=="Meteorological Data") {
			Name="RadiationInput";
			Group="Hidden";
			pSw=(Sw*)((NewMap*)m_pDoc)->GetPtr(Type, Group,Name);
			pSw->SetDBIntValue(1);
			Name="TypeOfDrivingFile";
			pSw=(Sw*)((NewMap*)m_pDoc)->GetPtr(Type, Group,Name);
			pSw->SetDBIntValue(1);


		}
		
		Group=pszCaption;
		if(m_Group=="Multiple Plant") {
			Group="Plant";
			m_Group=Group;
		}

}

void DB_Plant::Init(string pszCaption,  string DB_key)
{
		m_DuringRun=false;
		Sw *pSw;

		string Type, Name, Group;
		
		Name="TimeResolution";
		Type="Switches";
		Group="Hidden";

		m_DB_key=DB_key;

		m_pSwTimeRes=(Sw*)((NewMap*)m_pDoc)->GetPtr(Type, Group, Name);
		Group="Model Structure";
		Name="PlantType";
		m_pSwPlantType=(Sw*)((NewMap*)m_pDoc)->GetPtr(Type, Group, Name);

		Group="Model Structure";

		Name="WaterEq";
		pSw=(Sw*)((NewMap*)m_pDoc)->GetPtr(Type, Group,Name);
		pSw->SetDBIntValue(1);



		Name="SnowPack";
		m_Group=pszCaption;
	  
		if(m_Group=="Snow") {
			pSw=(Sw*)((NewMap*)m_pDoc)->GetPtr(Type, Group,Name);
			pSw->SetDBIntValue(1);
			pszCaption="SnowPack";
		}
		if(m_Group=="Soil frost") {
			Name="HeatEq";
			pSw=(Sw*)((NewMap*)m_pDoc)->GetPtr(Type, Group,Name);
			pSw->SetDBIntValue(1);
		}
		if(m_Group=="Soil evaporation") {
			Name="Evaporation Method";
			Group=m_Group;
		}


		m_Group=pszCaption;
		if(m_Group=="Multiple Plant") {
			Group="Plant";
			m_Group=Group;
		}


	
}

DB_Plant::~DB_Plant()
{
}

void DB_Plant::DB_Actual()
{

	SimB *pBase;
	Sw *pSw;
	F *pCF;
	vector<SimB*> vptr;

	string Type, str, Group, Name;
	double fvalue, f_org, f_db;
	int i ;
	int ivalue;
	time_t sec;
	sec=time(nullptr);


	if(m_Group=="Plant") {
		ivalue=m_pSwTimeRes->GetIntValue();
		if(m_pSwTimeRes->GetDBIntValue()!=ivalue) {
			m_pSwTimeRes->SetIntValue(m_pSwTimeRes->GetDBIntValue());	
			//m_pSwTimeRes->Apply();
			((NewMap*)m_pDoc)->History_Add(m_pSwTimeRes,-1,sec,m_pSwTimeRes->GetOption(m_pSwTimeRes->GetDBIntValue()),"DB: "+m_DB_key);
		}
		ivalue=m_pSwPlantType->GetIntValue();
		if(m_pSwPlantType->GetDBIntValue()!=ivalue) {
			m_pSwPlantType->SetIntValue(m_pSwPlantType->GetDBIntValue());
			//m_pSwPlantType->Apply();
			((NewMap*)m_pDoc)->History_Add(m_pSwPlantType,-1,sec,m_pSwPlantType->GetOption(m_pSwPlantType->GetDBIntValue()),"DB: "+m_DB_key);
		}
	}

	Type="Switches";
	vptr=((NewMap*)m_pDoc)->GetPtrVector(Type,m_Group);
	if(vptr.size()>0) {
		for(size_t i=0;i<vptr.size();i++) {
			pSw=(Sw*)vptr[i];
			ivalue=pSw->GetIntValue();
			if(ivalue!=pSw->GetDBIntValue()) {
				pSw->SetIntValue(pSw->GetDBIntValue());
				//pSw->Apply();
				((NewMap*)m_pDoc)->History_Add(pSw,-1,sec,pSw->GetOption(pSw->GetDBIntValue()),"DB: "+m_DB_key);
			}
		}
	}
	Type="Parameters";
	vptr=((NewMap*)m_pDoc)->GetPtrVector(Type,m_Group);
	if(vptr.size()>0) {
		for(size_t i=0;i<vptr.size();i++) {
			pBase=(SimB*)vptr[i];
			fvalue=((Ps*)pBase)->GetValue();
			f_db=pBase->GetDBValue();
			if(fvalue!=f_db) {
				f_org=((Ps*)pBase)->GetOriginalValue();
				if(f_org!=f_db) {
					((Ps*)pBase)->SetValue(f_db);
					((Ps*)pBase)->EditApply();
					((NewMap*)m_pDoc)->History_Add(pBase,-1,sec,pBase->GetDBValue(),"DB: "+m_DB_key);
				}
			}
		}
	}
//Parameter Tables
	Type="Tables";

	vptr=((NewMap*)m_pDoc)->GetPtrVector(Type,m_Group);
	if(vptr.size()>0) {
		for(size_t i=0;i<vptr.size();i++) {
		  pBase=vptr[i];
		  //((Tab*)pBase)->GetDBCell(0, 0,&fvalue);
		  string Name=((Tab*)pBase)->GetName();
		  if(Name!="Root distribution with depth") {
			size_t oldvalue=((Tab*)pBase)->GetNumOfRow();
			size_t newvalue=((Tab*)pBase)->GetDB_Row();
			if(!m_DuringRun) 
			  ((Tab*)pBase)->SetNumOfRow(newvalue);
			if(oldvalue!=newvalue) {
				//((Tab*)pBase)->Apply_CE_only();
				string str=STD_ItoAscii(newvalue);
				((NewMap*)m_pDoc)->History_Add(pBase,-1,sec,str,"DB: "+m_DB_key);
			}
		  
			for(size_t nRow=0; nRow<((Tab*)pBase)->GetNumOfRow(); nRow++){
				for (size_t nCol=0;nCol<((Tab*)pBase)->GetNumOfCol(); nCol++){
					if(!m_DuringRun||nRow<oldvalue) {//((Tab*)pBase)->SetCell(nCol, nRow,fvalue);
						P* pCPt;
						pCPt=((Tab*)pBase)->GetP_Pointer(nCol);
						if(pCPt->GetValue(nRow)!=pCPt->GetDBValue(nRow)) {
							fvalue=pCPt->GetDBValue(nRow);
							f_org=pCPt->GetOriginalValue(nRow);
							if(fvalue!=f_org) {
								pCPt->SetValue(nRow, fvalue);
								pCPt->EditApply(nRow);
								((NewMap*)m_pDoc)->History_Add(pCPt,nRow,sec,fvalue,"DB: "+m_DB_key);
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
	vptr=((NewMap*)m_pDoc)->GetPtrVector(Type,m_Group);
	if(vptr.size()>0) {
		for(size_t i=0;i<vptr.size();i++) {
			pCF=(F*)vptr[i];
			if(pCF->GetStrValue()!=pCF->GetDBStrValue()) {
				pCF->SetValue(pCF->GetDBStrValue());
				pCF->Apply(); // Changed form PushedApply to Apply to avoid check of file name
				((NewMap*)m_pDoc)->History_Add(pCF,-1,sec,pCF->GetValue(),"DB: "+m_DB_key);
			}
		}
	}


	Type="Switches";
	string group="Model Structure";
	string name="GroundWaterFlow";
	pSw=(Sw*)((NewMap*)m_pDoc)->GetPtr(Type, Group, Name);
	i=pSw->GetIntValue();


}
bool DB_Plant::DB_Struc_Actual(string Name)
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

	pSw=(Sw*)((NewMap*)m_pDoc)->GetPtr(Type, Group, Name);
	if(pSw!=nullptr) {
		int ivalue=pSw->GetIntValue();
		if(ivalue!=pSw->GetDBIntValue()) {
			pSw->SetIntValue(pSw->GetDBIntValue());
			//pSw->Apply();
			((NewMap*)m_pDoc)->History_Add(pSw,-1,sec,pSw->GetOption(pSw->GetDBIntValue()),"DB: "+m_DB_key);
		}
		return true;
	}
	else
		return false;
}
void DB_Plant::Actual_DB()
{

	SimB *pBase;
	Sw *pSw;
	F *pCF;
	vector<SimB*> vptr;
	string Type, str, Group, Name;
	double fvalue;


	m_pSwTimeRes->SetDBIntValue(m_pSwTimeRes->GetIntValue());
	m_pSwPlantType->SetDBIntValue(m_pSwPlantType->GetIntValue());

	Type="Switches";

	vptr=((NewMap*)m_pDoc)->GetPtrVector(Type,m_Group);
	if(vptr.size()>0) {
		for(size_t i=0;i<vptr.size();i++) {
			pSw=(Sw*)vptr[i];
			pSw->SetDBIntValue(pSw->GetIntValue());
		}
	}
	Type="Parameters";
	vptr=((NewMap*)m_pDoc)->GetPtrVector(Type,m_Group);
	if(vptr.size()>0) {
		for(size_t i=0;i<vptr.size();i++) {
			pBase=(SimB*)vptr[i];
			((Ps*)pBase)->SetDBValue(pBase->GetValue());
		}
	}
//Parameter Tables
	Type="Tables";
	vptr=((NewMap*)m_pDoc)->GetPtrVector(Type,m_Group);
	if(vptr.size()>0) {
		for(size_t i=0;i<vptr.size();i++) {
			pBase=(SimB*)vptr[i];
		  ((Tab*)pBase)->SetDB_Row(((Tab*)pBase)->GetNumOfRow());
		  for(size_t nRow=0; nRow<((Tab*)pBase)->GetNumOfRow(); nRow++){
			
			for (size_t nCol=0;nCol<((Tab*)pBase)->GetNumOfCol(); nCol++){
				P* pP=((Tab*)pBase)->GetP_Pointer(nCol);
				fvalue=pP->GetValue(nCol);
				pP->SetDBValue(nCol, fvalue);
			}
				//((Tab*)pBase)->SetDBCell(nCol, nRow,fvalue);}
		  }
		}
	}
	Type="Files";
	vptr=((NewMap*)m_pDoc)->GetPtrVector(Type,m_Group);
	if(vptr.size()>0) {
		for(size_t i=0;i<vptr.size();i++) {
			pCF=(F*)vptr[i];
			pCF->SetDBValue(pCF->GetStrValue());
		}
	}



}

bool DB_Plant::ReadWrite(string FileName, bool writing)
{	
//REMAIN
/*	CBasePtrArray *arr;
	SimB *pBase;
	Sw *pSw;
	Ps *pCP;
	NewMap *p_fmap;
	p_fmap=m_pDoc->GetFortranMap();
	string Type, str, Group, Name, FileName;
	float fvalue;
	int Version=2, ReadVersion;
	Version=((NewMap*)m_pDoc)->m_DocFile.m_FileVersionNumber;


	if(writing) {
		Type="Start";
		ar<<Type;
		ar<<Version;

		Type="Switches";
		if(m_pDoc->GetArray(Type,m_Group,arr)) {
			ivalue=arr->GetSize()+2;
			ar<<Type;
			ar<<ivalue;
				str="Hidden"; ar<<str;
				str="TimeResolution"; ar<<str;
				ar<<m_pSwTimeRes->GetDBValue();
				str="Model Structure"; ar<<str;
				str="PlantType"; ar<<str;
				ar<<m_pSwPlantType->GetDBValue();

			for(int i=0;i<arr->GetSize();i++) {
				pSw=(Sw*)arr->GetAt(i);
				ar<<pSw->GetGroup();
				ar<<pSw->GetName();
				ar<<pSw->GetDBValue();
			}
		}
		Type="Parameters";
		if(m_pDoc->GetArray(Type,m_Group,arr)) {
			ivalue=arr->GetSize();
			ar<<Type;
			ar<<ivalue;
			for(int i=0;i<arr->GetSize();i++) {
				pBase=arr->GetAt(i);
				ar<<pBase->GetGroup();
				ar<<pBase->GetName();
				fvalue=*(float*)pBase->GetDBValue();
				ar<<fvalue;
			}
		}
//Parameter Tables
		Type="Tables";
		if(m_pDoc->GetArray(Type,m_Group,arr)) {
			ivalue=arr->GetSize();
			int nCol;
			ar<<Type;
			ar<<ivalue;
			for(int i=0; i<arr->GetSize(); i++) {
			  pBase = (SimB*)arr->GetAt(i);	
			  ar<<pBase->GetGroup();
			  ar<<pBase->GetName();
			  nCol=((Tab*)pBase)->GetNumberOfColumns();
			  ar<<nCol;
			  for(int i=0;i<nCol;i++) {
				  ar<<((Tab*)pBase)->GetTitle(i);
			  }
			  ar<<((Tab*)pBase)->GetDBValue();// Number of rows in table
			  for(int nRow=0; ((Tab*)pBase)->GetDBCell(0, nRow,&fvalue); nRow++){
				for (int nCol=0;((Tab*)pBase)->GetDBCell(nCol, nRow, &fvalue); nCol++){
					ar<<fvalue;}
			  }
			}
		}
		Type="Files";
		if(m_pDoc->GetArray(Type,m_Group,arr)) {
			ivalue=arr->GetSize();
			ar<<Type;
			ar<<ivalue;
			for(int i=0;i<arr->GetSize();i++) {
				pSw=(Sw*)arr->GetAt(i);
				ar<<pSw->GetGroup();
				ar<<pSw->GetName();
				F *node=(F*)pSw;
				ar<<node->GetDBValue();
			}
		}


	}
	else {
// Retrieve values
		//if(ar.IsBufferEmpty())
		//	return;
		ReadVersion=0;
		ar>>str;
		while(str!="End"&&!ar.IsBufferEmpty()) {
			Type=str;
		    ar>>irep;
			if(str=="Start") {
				ReadVersion=irep;
			}  
			else if(str=="Switches") {
			   for(int i=0;i<irep;i++) {
					ar>>Group;
					ar>>Name;
					ar>>ivalue;
					pSw=(Sw*)((NewMap*)m_pDoc)->GetPtr(Type, Group,Name);
					if(pSw!=nullptr) pSw->SetDBValue(ivalue);
			   }
			}
			else if(str=="Parameters") {
				for(int i=0;i<irep;i++) {
					ar>>Group;
					ar>>Name;
					ar>>fvalue;
					pCP=(Ps*)((NewMap*)m_pDoc)->GetPtr(Type, Group,Name);
					if(pCP!=nullptr) pCP->SetDBValue(fvalue);
					else {
					SimB *Sw;
					if(Group=="Plant Growth") {
					if(Group=="Radiation properties") {
						Type="Tables";
						if(Name=="RntLAI") {
						  Name="Beers law - multipe plants";
						  Sw=((NewMap*)m_pDoc)->GetPtr(Type, Group,Name);
						  int nrow=((Tab*)Sw)->GetValue();
						  for(int jj=0;jj<nrow;jj++)
							  ((Tab*)Sw)->SetDBCell(0, jj, fvalue);
						}
					}
					else if(Group=="Plant Growth") {
						Type="Tables";
						if(Name=="PhoTempResMin") {
						  Name="Photosynthesis Response";
						  Sw=((NewMap*)m_pDoc)->GetPtr(Type, Group,Name);
						  int nrow=((Tab*)Sw)->GetValue();
						  for(int jj=0;jj<nrow;jj++)
							  ((Tab*)Sw)->SetDBCell(2, jj, fvalue);
						}
						else if(Name=="PhoTempResOpt1") {
							Name="Photosynthesis Response";
							Sw=((NewMap*)m_pDoc)->GetPtr(Type, Group,Name);
						  int nrow=((Tab*)Sw)->GetValue();
						  for(int jj=0;jj<nrow;jj++)
							  ((Tab*)Sw)->SetDBCell(3, jj, fvalue);
						}

						else if(Name=="PhoTempResOpt2") {
							Name="Photosynthesis Response";
							Sw=((NewMap*)m_pDoc)->GetPtr(Type, Group,Name);
						  int nrow=((Tab*)Sw)->GetValue();
						  for(int jj=0;jj<nrow;jj++)
							  ((Tab*)Sw)->SetDBCell(4, jj, fvalue);
						}

						else if(Name=="PhoTempResMax") {
							Name="Photosynthesis Response";
							Sw=((NewMap*)m_pDoc)->GetPtr(Type, Group,Name);
						  int nrow=((Tab*)Sw)->GetValue();
						  for(int jj=0;jj<nrow;jj++)
							  ((Tab*)Sw)->SetDBCell(5, jj, fvalue);
						}

						else if(Name=="PhoCNLeafOpt") {
							Name="Photosynthesis Response";
							Sw=((NewMap*)m_pDoc)->GetPtr(Type, Group,Name);
						  int nrow=((Tab*)Sw)->GetValue();
						  for(int jj=0;jj<nrow;jj++)
							  ((Tab*)Sw)->SetDBCell(0, jj, fvalue);
						}

						else if(Name=="PhoCNLeafThres") {
							Name="Photosynthesis Response";
							Sw=((NewMap*)m_pDoc)->GetPtr(Type, Group,Name);
						  int nrow=((Tab*)Sw)->GetValue();
						  for(int jj=0;jj<nrow;jj++)
							  ((Tab*)Sw)->SetDBCell(1, jj, fvalue);
						}

						else if(Name=="PhoRadEfficiency") {
							Name="Radiation use efficiency";
							Sw=((NewMap*)m_pDoc)->GetPtr(Type, Group,Name);
						  int nrow=((Tab*)Sw)->GetValue();
						  for(int jj=0;jj<nrow;jj++)
							  ((Tab*)Sw)->SetDBCell(0, jj, fvalue);
						}
 
						else if(Name=="PhoRadEff_Reduc") {
							Name="Radiation use efficiency";
							Sw=((NewMap*)m_pDoc)->GetPtr(Type, Group,Name);
						  int nrow=((Tab*)Sw)->GetValue();
						  for(int jj=0;jj<nrow;jj++)
							  ((Tab*)Sw)->SetDBCell(1, jj, fvalue);
						}

						else if(Name=="Pmax") {
							Name="Radiation use efficiency";
							Sw=((NewMap*)m_pDoc)->GetPtr(Type, Group,Name);
						  int nrow=((Tab*)Sw)->GetValue();
						  for(int jj=0;jj<nrow;jj++)
							  ((Tab*)Sw)->SetDBCell(2, jj, fvalue);
						}

						else if(Name=="FixedNdemand") {
							Name="Radiation use efficiency";
							Sw=((NewMap*)m_pDoc)->GetPtr(Type, Group,Name);
						  int nrow=((Tab*)Sw)->GetValue();
						  for(int jj=0;jj<nrow;jj++)
							  ((Tab*)Sw)->SetDBCell(3, jj, fvalue);
						}
 
 						else if(Name=="T Sum Sowing") {
 							Name="Growth stage";
 							Sw=((NewMap*)m_pDoc)->GetPtr(Type, Group,Name);
						  int nrow=((Tab*)Sw)->GetValue();
						  for(int jj=0;jj<nrow;jj++)
							  ((Tab*)Sw)->SetDBCell(0, jj, fvalue);
						}
						else if(Name=="T Thres Sowing") {
							Name="Growth stage";
							Sw=((NewMap*)m_pDoc)->GetPtr(Type, Group,Name);
						  int nrow=((Tab*)Sw)->GetValue();
						  for(int jj=0;jj<nrow;jj++)
							  ((Tab*)Sw)->SetDBCell(1, jj, fvalue);
						}
						else if(Name=="T Sum Emerg") {
							Name="Growth stage";
 							Sw=((NewMap*)m_pDoc)->GetPtr(Type, Group,Name);
						  int nrow=((Tab*)Sw)->GetValue();
						  for(int jj=0;jj<nrow;jj++)
							  ((Tab*)Sw)->SetDBCell(2, jj, fvalue);
 						}
 						else if(Name=="T Thres Emerg") {
 							Name="Growth stage";
							Sw=((NewMap*)m_pDoc)->GetPtr(Type, Group,Name);
						  int nrow=((Tab*)Sw)->GetValue();
						  for(int jj=0;jj<nrow;jj++)
							  ((Tab*)Sw)->SetDBCell(3, jj, fvalue);
						}
						else if(Name=="GrainSI_Step") {
							Name="Growth stage";
							Sw=((NewMap*)m_pDoc)->GetPtr(Type, Group,Name);
						  int nrow=((Tab*)Sw)->GetValue();
						  for(int jj=0;jj<nrow;jj++)
							  ((Tab*)Sw)->SetDBCell(4, jj, fvalue);
						}
						else if(Name=="GrainSI_StepDay") {
							Name="Growth stage";
							Sw=((NewMap*)m_pDoc)->GetPtr(Type, Group,Name);
						  int nrow=((Tab*)Sw)->GetValue();
						  for(int jj=0;jj<nrow;jj++)
							  ((Tab*)Sw)->SetDBCell(5, jj, fvalue);
						}
						else if(Name=="GrainSI_ThresDay") {
							Name="Growth stage";
							Sw=((NewMap*)m_pDoc)->GetPtr(Type, Group,Name);
						  int nrow=((Tab*)Sw)->GetValue();
						  for(int jj=0;jj<nrow;jj++)
							  ((Tab*)Sw)->SetDBCell(6, jj, fvalue);
						}
						else if(Name=="GrainSI_StepTemp") {
							Name="Growth stage";
							Sw=((NewMap*)m_pDoc)->GetPtr(Type, Group,Name);
						  int nrow=((Tab*)Sw)->GetValue();
						  for(int jj=0;jj<nrow;jj++)
							  ((Tab*)Sw)->SetDBCell(7, jj, fvalue);
						}
						else if(Name=="GrainSI_ThresTemp") {
							Name="Growth stage";
							Sw=((NewMap*)m_pDoc)->GetPtr(Type, Group,Name);
						  int nrow=((Tab*)Sw)->GetValue();
						  for(int jj=0;jj<nrow;jj++)
							  ((Tab*)Sw)->SetDBCell(8, jj, fvalue);
						}
						else if(Name=="T Sum Grainfill") {
							Name="Growth stage";
							Sw=((NewMap*)m_pDoc)->GetPtr(Type, Group,Name);
						  int nrow=((Tab*)Sw)->GetValue();
						  for(int jj=0;jj<nrow;jj++)
							  ((Tab*)Sw)->SetDBCell(9, jj, fvalue);
						}
						else if(Name=="T Thres Grainfill") {
							Name="Growth stage";
							Sw=((NewMap*)m_pDoc)->GetPtr(Type, Group,Name);
						  int nrow=((Tab*)Sw)->GetValue();
						  for(int jj=0;jj<nrow;jj++)
							  ((Tab*)Sw)->SetDBCell(10, jj, fvalue);
						}
						else if(Name=="LeafLitterRate c1") {
							Name="Litter fall Above Ground";
							Sw=((NewMap*)m_pDoc)->GetPtr(Type, Group,Name);
						  int nrow=((Tab*)Sw)->GetValue();
						  for(int jj=0;jj<nrow;jj++)
							  ((Tab*)Sw)->SetDBCell(0, jj, fvalue);
						}
						else if(Name=="LeafLitterRate c2") {
							Name="Litter fall Above Ground";
							Sw=((NewMap*)m_pDoc)->GetPtr(Type, Group,Name);
						  int nrow=((Tab*)Sw)->GetValue();
						  for(int jj=0;jj<nrow;jj++)
							  ((Tab*)Sw)->SetDBCell(1, jj, fvalue);
						}
						else if(Name=="LeafLitterTsum1") {
							Name="Litter fall Above Ground";
							Sw=((NewMap*)m_pDoc)->GetPtr(Type, Group,Name);
						  int nrow=((Tab*)Sw)->GetValue();
						  for(int jj=0;jj<nrow;jj++)
							  ((Tab*)Sw)->SetDBCell(2, jj, fvalue);
						}
						else if(Name=="LeafLitterTsum2") {
							Name="Litter fall Above Ground";
							Sw=((NewMap*)m_pDoc)->GetPtr(Type, Group,Name);
						  int nrow=((Tab*)Sw)->GetValue();
						  for(int jj=0;jj<nrow;jj++)
							  ((Tab*)Sw)->SetDBCell(3, jj, fvalue);
						}
						else if(Name=="StemLitterRate c1") {
							Name="Litter fall Above Ground";
							Sw=((NewMap*)m_pDoc)->GetPtr(Type, Group,Name);
						  int nrow=((Tab*)Sw)->GetValue();
						  for(int jj=0;jj<nrow;jj++)
							  ((Tab*)Sw)->SetDBCell(4, jj, fvalue);
						}
						else if(Name=="StemLitterRate c2") {
							Name="Litter fall Above Ground";
							Sw=((NewMap*)m_pDoc)->GetPtr(Type, Group,Name);
						  int nrow=((Tab*)Sw)->GetValue();
						  for(int jj=0;jj<nrow;jj++)
							  ((Tab*)Sw)->SetDBCell(5, jj, fvalue);
						}
						else if(Name=="StemLitterTsum1") {
							Name="Litter fall Above Ground";
							Sw=((NewMap*)m_pDoc)->GetPtr(Type, Group,Name);
						  int nrow=((Tab*)Sw)->GetValue();
						  for(int jj=0;jj<nrow;jj++)
							  ((Tab*)Sw)->SetDBCell(6, jj, fvalue);
						}
						else if(Name=="StemLitterTsum2") {
							Name="Litter fall Above Ground";
							Sw=((NewMap*)m_pDoc)->GetPtr(Type, Group,Name);
						  int nrow=((Tab*)Sw)->GetValue();
						  for(int jj=0;jj<nrow;jj++)
							  ((Tab*)Sw)->SetDBCell(7, jj, fvalue);
						}
						else if(Name=="GrainLitterRate c1") {
							Name="Litter fall Above Ground";
							Sw=((NewMap*)m_pDoc)->GetPtr(Type, Group,Name);
						  int nrow=((Tab*)Sw)->GetValue();
						  for(int jj=0;jj<nrow;jj++)
							  ((Tab*)Sw)->SetDBCell(8, jj, fvalue);
						}
						else if(Name=="GrainLitterRate c2") {
							Name="Litter fall Above Ground";
							Sw=((NewMap*)m_pDoc)->GetPtr(Type, Group,Name);
						  int nrow=((Tab*)Sw)->GetValue();
						  for(int jj=0;jj<nrow;jj++)
							  ((Tab*)Sw)->SetDBCell(9, jj, fvalue);
						}
						else if(Name=="GrainLitterTsum1") {
							Name="Litter fall Above Ground";
							Sw=((NewMap*)m_pDoc)->GetPtr(Type, Group,Name);
						  int nrow=((Tab*)Sw)->GetValue();
						  for(int jj=0;jj<nrow;jj++)
							  ((Tab*)Sw)->SetDBCell(10, jj, fvalue);
						}
  						else if(Name=="GrainLitterTsum2") {
							Name="Litter fall Above Ground";
							Sw=((NewMap*)m_pDoc)->GetPtr(Type, Group,Name);
						  int nrow=((Tab*)Sw)->GetValue();
						  for(int jj=0;jj<nrow;jj++)
							  ((Tab*)Sw)->SetDBCell(11, jj, fvalue);
						}
						else if(Name=="RootLitterRate c1") {
							Name="Litter fall Below Ground";
							Sw=((NewMap*)m_pDoc)->GetPtr(Type, Group,Name);
						  int nrow=((Tab*)Sw)->GetValue();
						  for(int jj=0;jj<nrow;jj++)
							  ((Tab*)Sw)->SetDBCell(0, jj, fvalue);
						}
						else if(Name=="RootLitterRate c2") {
							Name="Litter fall Below Ground";
							Sw=((NewMap*)m_pDoc)->GetPtr(Type, Group,Name);
						  int nrow=((Tab*)Sw)->GetValue();
						  for(int jj=0;jj<nrow;jj++)
							  ((Tab*)Sw)->SetDBCell(1, jj, fvalue);
						}
						else if(Name=="RootLitterTsum1") {
							Name="Litter fall Below Ground";
							Sw=((NewMap*)m_pDoc)->GetPtr(Type, Group,Name);
						  int nrow=((Tab*)Sw)->GetValue();
						  for(int jj=0;jj<nrow;jj++)
							  ((Tab*)Sw)->SetDBCell(2, jj, fvalue);
						}
						else if(Name=="RootLitterTsum2") {
							Name="Litter fall Below Ground";
							Sw=((NewMap*)m_pDoc)->GetPtr(Type, Group,Name);
						  int nrow=((Tab*)Sw)->GetValue();
						  for(int jj=0;jj<nrow;jj++)
							  ((Tab*)Sw)->SetDBCell(3, jj, fvalue);
						}
						else if(Name=="RadEffReduction") {
							Name="Farquhar parameters";
							Sw=((NewMap*)m_pDoc)->GetPtr(Type, Group,Name);
						  int nrow=((Tab*)Sw)->GetValue();
						  for(int jj=0;jj<nrow;jj++)
							  ((Tab*)Sw)->SetDBCell(5, jj, fvalue);
						}
						else if(Name=="FixedNdemand") {
							Name="Farquhar parameters";
							Sw=((NewMap*)m_pDoc)->GetPtr(Type, Group,Name);
						  int nrow=((Tab*)Sw)->GetValue();
						  for(int jj=0;jj<nrow;jj++)
							  ((Tab*)Sw)->SetDBCell(6, jj, fvalue);
						}
//						else {
//			   	MessageBox(nullptr,"Non Identified option from previous version disregarded - "+Name,"Input Error",MB_OK|MB_ICONEXCLAMATION);
						}
// 				}
// 					else
//						MessageBox(nullptr,"Non Identified option from previous version disregarded - "+Name,"Input Error",MB_OK|MB_ICONEXCLAMATION);

			}

			}



			}
			}	
			else if(str=="Tables") {
				int nColSize;
				for(int i=0;i<irep;i++) {
					ar>>Group;
					ar>>Name;	  
					mm_name=Name;
					if(ReadVersion>20) {
						string strtest;
						ar>>nColSize;
						for(int i=0;i<nColSize;i++){
						   ar>>strtest;
						}
					}
					//Number of Columns
					ar>>ivalue;	 // Number of rows
					  if(Name=="Above ground crop characteristics") Name="Above ground characteristics with time";
					  if(Name=="Evapotranspiration parameters") Name="Evapotranspiration - single canopy";	
					  if(Name=="Scaling of Litter fall ") Name="Scaling of Litter fall"; 
					  if(Name=="Plant Behavior") Name="Plant Behaviour"; 

str==
		  			  if(pBase!=nullptr) pBase=((NewMap*)m_pDoc)->GetPtr(Type, Group,Name);
					  else { 
						  if(ReadVersion<40)
						  return;

					  }
					  if(ReadVersion<21&&pBase!=nullptr) {
						nColSize=0;
					  	while(((Tab*)pBase)->GetDBCell(nColSize, 0, &fvalue)){
				  		  	  nColSize++;
		  				}
						if(ReadVersion==0&&Name=="Root depths development with time") nColSize=2;
//						if(ReadVersion<=8&&Name=="Scaling of Litter fall") nColSize=4; 
						if(ReadVersion<=9&&Name=="Size and shape of growing plant") nColSize=7; 
						if(ReadVersion<=9&&Name=="Above ground characteristics with time") nColSize=4; 
						if(ReadVersion<=14&&Name=="Size and shape of growing plant") nColSize=7; 
						if(ReadVersion==1&&Name=="Hydraulic conductivity, model boundaries") nColSize=11;
						if(ReadVersion==15&&Name=="Harvest of Plants") nColSize=8;
						if(ReadVersion<=2&&Name=="Start of growth") nColSize=4;
						if(ReadVersion<=2&&Name=="Evapotranspiration - multiple canopies") nColSize=7;
					  }
				
					
					
					if(pBase!=nullptr){
					((Tab*)pBase)->SetDBValue(ivalue);
				  	  // if(intValue!=nRow) 
		  				for(int nRow=0; nRow<ivalue ; nRow++){
	  		  				for (int nCol=0; nCol<nColSize; nCol++){
	  							ar>>fvalue;
	  							if(pBase!=nullptr) ((Tab*)pBase)->SetDBCell(nCol, nRow, fvalue);
	  		  				}
	  					}
					}

				}
			}	
			else if(str=="Files") {
			   for(int i=0;i<irep;i++) {
					ar>>Group;
					ar>>Name;
					ar>>FileName;
			        if(Group!="Hidden") pBase=((NewMap*)m_pDoc)->GetPtr(Type, Group,Name);
			        if(pBase!=nullptr&&Group!="Hidden") {
						F *node=(F*)pBase;
						node->SetDBValue(FileName);
					}
			   }
			}
			ar>>str;
		}

	}
	*/
return true;
}



/////////////////////////////////////////////////////////////////////////////
// DB_Plant message handlers
