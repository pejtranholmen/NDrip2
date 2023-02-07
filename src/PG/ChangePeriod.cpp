#include "./ChangePeriod.h"
#include "../stdafx.h"
#include "../MainFrm.h"
bool ChangePeriod::CreateNewFile(CPG *pPG, int Option)
{
	CPG *pPG_Dest;
	pPG_Dest= new CPG;

	bool fail=true;
	if(pPG==nullptr&&m_pPGSource==nullptr) return nullptr;
	else if(pPG!=nullptr)
		m_pPGSource=pPG;

	m_pPGSource->CloseFile();
	m_pPGSource->SetCompleteRead(false);
	m_pPGSource->Open(m_pPGSource->GetFileName());
	m_pPGSource->ReOpen();

	auto nrep=m_pPGSource->GetNumRepititions();
	auto nvar=m_pPGSource->GetNumVariables();
	auto numrec=m_pPGSource->GetNumRecords();
	m_pPGSource->ReadNewBuffer(1, 1);
	size_t NewNumRec=0;
	switch(Option) {
		case 0:
			NewNumRec=numrec/2;		
			break;
		case 1:
			NewNumRec=m_EndOut-m_StartOut;
	}

	if(NewNumRec==0) {
		delete pPG_Dest;
		return false;
	}

	pPG_Dest->SetCompleteRead(false);
	pPG_Dest->SetInfoMode();
	pPG_Dest->CreateNewFile(nvar, NewNumRec,nrep);


	for(size_t rep=0; rep<nrep; rep++) {
		string txt;
			for(size_t i=1; i<=nvar; i++) {
							txt=m_pPGSource->GetVarName(i,rep+1);
							pPG_Dest->SetVarName(i, txt, rep+1);
							txt=m_pPGSource->GetVarUnit(i, rep+1);
							pPG_Dest->SetVarUnit(i, txt, rep+1);
							txt=m_pPGSource->GetVarId(i, rep+1);
							pPG_Dest->SetVarId(i, txt, rep+1);
							txt=m_pPGSource->GetVarPos(i, rep+1);
							pPG_Dest->SetVarPos(i, txt, rep+1);
							double Lat=m_pPGSource->GetVarLat(i, rep+1);
							pPG_Dest->SetVarLat(i, Lat, rep+1);
							double Long=m_pPGSource->GetVarLong(i, rep+1);
							pPG_Dest->SetVarLong(i, Long, rep+1);
							double Alt=m_pPGSource->GetVarAlt(i, rep+1);
							pPG_Dest->SetVarAlt(i, Alt, rep+1);
							string txt=m_pPGSource->GetVarStation(i, rep+1);
							pPG_Dest->SetVarStation(i, txt, rep+1);
							txt=m_pPGSource->GetVarCountry(i, rep+1);
							pPG_Dest->SetVarCountry(i, txt, rep+1);
			}
	}
	if(!pPG_Dest->SaveAs(m_DestFileName, true)) {
		pPG_Dest->ReOpen(true);
	}
	//if(!pPG_Dest->ReOpen(true)) return false;


		

	/*if(!pPG_Dest->WritePGFileStartHeader(NewNumRec, nrep)) {
		string txt="Calculation failed no file created";
		m_ResultSum.push_back(txt);
		
		
	};*/
	CWinApp* pApp = AfxGetApp();
	CCoupModelApp *pCoupModel=(CCoupModelApp*)pApp;
	CMainFrame *p_mainframe;
 
	p_mainframe=(CMainFrame*)pCoupModel->m_pMainWnd;



	for(size_t rep=0; rep<nrep; rep++) {
		float rel=float(rep)/float(nrep);
		p_mainframe->UpdateProgress(0, rel);
		//m_pPGSource->ReadNewBuffer(1, rep+1);
		//m_pPGSource->SetRepititionSequence(rep+1, true);
				vector<float> min_v, max_v;
				min_v.assign(nvar, abs(MISSING)); max_v.assign(nvar,MISSING);
				for(size_t rec=0; rec<NewNumRec; rec++) {
					void *pFloat;void *pDate;
					if(Option==0) {
						pFloat=m_pPGSource->GetRecordPointer(NewNumRec+rec+1, rep+1);
						pDate=m_pPGSource->GetRecordPointer(rec+1, rep+1);		
					}
					else {
						pFloat=m_pPGSource->GetRecordPointer(m_StartOut+rec, rep+1);
						pDate=m_pPGSource->GetRecordPointer(m_StartOut+rec, rep+1);	
					}

					pPG_Dest->SetDatePointer((float*)pDate);
					pPG_Dest->SetVarPointer((float*)pFloat+1,0, nvar);
					for(size_t i=0; i<nvar;i++) {
						float value=pPG_Dest->GetVarValue(i+1);
						if(min_v[i]>value) min_v[i]=value;
						if(max_v[i]<value) max_v[i]=value;
					}
					//if(pPG_Dest->SetWritePosition(rep*NewNumRec+rec+1))
					if(!pPG_Dest->WritePGFileRecord(rec+1, rep+1)) return false;
				;
					//pPG_Dest->WriteNewPGFileRecord();
				}
				for(size_t i=0; i<nvar;i++) {
					pPG_Dest->SetVarMin(i+1, min_v[i], rep+1);
					pPG_Dest->SetVarMax(i+1, max_v[i], rep+1);	
				}

	}

	string txt;
	if(pPG_Dest->WritePGFileEnd(NewNumRec, nrep)) {
		if(Option==0) txt="Second half has now moved to be the only part of file created :";
		else if(Option==1) txt="New subperiod in Output file :";
		m_ResultSum.push_back(txt);
		m_ResultSum.push_back(pPG_Dest->GetFileName());
		txt="Click on file name to open file";
		m_ResultSum.push_back(txt);
		
	}
	else {
		txt="Calculation failed no file created";
		m_ResultSum.push_back(txt);
	}
	pPG_Dest->CloseFile();
	delete pPG_Dest;

	return true;
}
