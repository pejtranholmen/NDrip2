#include "../ModelTypes/FixedInteger_PlantComp.h"
#include "../NewBase/Doc.h"
#include "ModelUtil.h"
#include <time.h>
#include "./FUtil.hpp"
#include "../NewBase/Doc.h"
#include "./SoilData/GetSoilStorage.h"




ModelUtil::ModelUtil(void)
{
}

ModelUtil::~ModelUtil(void)
{
}

bool ModelUtil::SetParTableValue(Doc* pDoc,P * pP, size_t index, double newvalue)
{
#ifndef NO_MS_GUI
	double orgvalue;
	orgvalue = pP->GetOriginalValue(index);
	if (orgvalue < -1.E37) {
		double value_org = pP->GetOriginalValue(0);
		double value_min = pP->GetMinValue(0);
		double value_max = pP->GetMaxValue(0);
		vector<double> newv;
		newv.resize(pP->GetSize());
		for (size_t i = 0; i < pP->GetSize(); i++) {
			newv[i] = value_org;
		}
		pP->SetOriginalValue(newv);
		pP->SetMinValue(value_min);
		pP->SetMaxValue(value_max);
		orgvalue = pP->GetOriginalValue(index);
	}
	double relorg = orgvalue / (pP->GetMaxValue(index) - pP->GetMinValue(index));
	double newrel = newvalue / (pP->GetMaxValue(index) - pP->GetMinValue(index));
	double oldrel = pP->GetValue(index) / (pP->GetMaxValue(index) - pP->GetMinValue(index));
	size_t recalc=0;
	bool change = true;
	if (abs(newrel - relorg)>1.E-8) {
		recalc = pP->SetValue(index, newvalue);
		pP->SetNotOriginalIndicator(index, true);
	}
	else if (abs(oldrel - newrel)>1.E-8) {
		recalc = pP->SetValue(index, newvalue);
		pP->SetNotOldValue(index);
	}
	else {
		change = false;
	}
	if (change) {
		if (recalc>0) {
			pDoc->LinkedChangeToParameters(pP, recalc, index);

		}
		pDoc->TimeModified();
		pDoc->History_Add(pP, index, time(nullptr), newvalue);
	}

	return change;
#else
	return false;
#endif
}

bool ModelUtil::SetParValue(Doc *pDoc, Ps * pPs, double newvalue)
{
#ifndef NO_MS_GUI
	size_t recalc = pPs->SetValue(newvalue);
	if (recalc>10000) {
		pDoc->LinkedChangeToDB_Parameters(pPs, recalc);


	}

	//((Ps*)pSw)->EditApply();
	pDoc->TimeModified();
	pDoc->History_Add(pPs, string::npos,time(nullptr), newvalue);


#endif

	return true;
}

bool ModelUtil::SetMRSoilPropConnections(Doc *pDoc)
{
#ifndef NO_MS_GUI
	if (pDoc == nullptr) return false;
	string Name, Group, Type;

	Group = "Soil Hydraulic";
	Type = "Table Parameters";

	P *pClayContent, *pSandContent, *pProf, *pRepProf, *pProfLayer,
		*pSaturation, *pTotalCond, *pMatCond, *pLambda, *pAirEntry,
		*pWilt, *pResidual, *pUDep, *pLDep, *pLBound, *pUBound,
		*p_nVal, *pGA, *pGN, *pGM, *p_nSR, *p_nSE;
	Sw *pPedo, *pEquation;

	pEquation = (Sw*)pDoc->GetPtr(Type, Group, Name);
	auto Equation = pEquation->GetIntValue();

	Name = "n Tortuosity";
	p_nVal = (P*)pDoc->GetPtr(Type, Group, Name);
	Name = "m_n Power (SatRel)";
	p_nSR = (P*)pDoc->GetPtr(Type, Group, Name);
	Name = "m_n Power (SatEffective)";
	p_nSE = (P*)pDoc->GetPtr(Type, Group, Name);
	Name = "mAlpha";
	pGA = (P*)pDoc->GetPtr(Type, Group, Name);
	Name = "m_n-value";
	pGN = (P*)pDoc->GetPtr(Type, Group, Name);
	Name = "m_m-value";
	pGM = (P*)pDoc->GetPtr(Type, Group, Name);

	Name = "mSaturation";
	pSaturation = (P*)pDoc->GetPtr(Type, Group, Name);
	Name = "bTotal Conductivity";
	pTotalCond = (P*)pDoc->GetPtr(Type, Group, Name);
	Name = "bMatrix Conductivity";
	pMatCond = (P*)pDoc->GetPtr(Type, Group, Name);

	Name = "mLambda";
	pLambda = (P*)pDoc->GetPtr(Type, Group, Name);
	Name = "mAir Entry";
	pAirEntry = (P*)pDoc->GetPtr(Type, Group, Name);
	Name = "mWilting Point";
	pWilt = (P*)pDoc->GetPtr(Type, Group, Name);
	Name = "mResidual Water";
	pResidual = (P*)pDoc->GetPtr(Type, Group, Name);

	auto nl = pWilt->GetSize();

	if (Equation == 1) {
		for (size_t i = 0; i<nl; i++) {
			pDoc->MR_CP_Set(pGA, i);
			pGA->MR_Set_Dim(i, 1);
			pGA->MR_Set_Method(i, MR_METHOD(10));
		}

		for (size_t i = 0; i<nl; i++) {
			pDoc->MR_CP_Set(pGN, i);
			pGN->MR_Set_Dim(i, 1);
			pGN->MR_Set_Method(i, MR_METHOD(10));
		}
		for (size_t i = 0; i<nl; i++) {
			pDoc->MR_CP_Set(pGM, i);
			pGM->MR_Set_Dim(i, 1);
			pGM->MR_Set_Method(i, MR_METHOD(10));
		}

	}



	for (size_t i = 0; i<nl; i++) {
		pDoc->MR_CP_Set(pSaturation, i);
		pSaturation->MR_Set_Dim(i, 1);
		pSaturation->MR_Set_Method(i, MR_METHOD(10));
	}

	for (size_t i = 0; i<nl; i++) {
		pDoc->MR_CP_Set(pWilt, i);
		pWilt->MR_Set_Dim(i, 1);
		pWilt->MR_Set_Method(i, MR_METHOD(10));
	}
	for (size_t i = 0; i<nl; i++) {
		pDoc->MR_CP_Set(pResidual, i);
		pResidual->MR_Set_Dim(i, 1);
		pResidual->MR_Set_Method(i, MR_METHOD(10));
	}

	for (size_t i = 0; i<nl; i++) {
		pDoc->MR_CP_Set(pLambda, i);
		pLambda->MR_Set_Dim(i, 1);
		pLambda->MR_Set_Method(i, MR_METHOD(10));
	}

	for (size_t i = 0; i<nl; i++) {
		pDoc->MR_CP_Set(pAirEntry, i);
		pAirEntry->MR_Set_Dim(i, 1);
		pAirEntry->MR_Set_Method(i, MR_METHOD(10));
	}

	for (size_t i = 0; i<nl; i++) {
		pDoc->MR_CP_Set(pTotalCond, i);
		pTotalCond->MR_Set_Dim(i, 1);
		pTotalCond->MR_Set_Method(i, MR_METHOD(10));
	}
	for (size_t i = 0; i<nl; i++) {
		pDoc->MR_CP_Set(pMatCond, i);
		pMatCond->MR_Set_Dim(i, 1);
		pMatCond->MR_Set_Method(i, MR_METHOD(10));
	}

	return true;
#else
	return false;
#endif
}

bool ModelUtil::SetMRSoilPropConnectionsProfNumberOnly(Doc *pDoc)
{
#ifndef NO_MS_GUI
	if(pDoc==nullptr)return false;
	string Name, Group, Type;
	P *pProf, *pRepProf;
	Group = "Soil Hydraulic";
	Type = "Table Parameters";
	Name = "ProfileNo";
	pProf = (P*)pDoc->GetPtr(Type, Group, Name);
	Name = "ProfileRepNo";
	pRepProf = (P*)pDoc->GetPtr(Type, Group, Name);

	pDoc->MR_CP_Set(pProf, 0);
	pProf->MR_Set_Dim(0, 1);
	pProf->MR_Set_Method(0, MR_METHOD(10));
	pDoc->MR_CP_Set(pRepProf, 0);
	pRepProf->MR_Set_Dim(0, 1);
	pRepProf->MR_Set_Method(0, MR_METHOD(10));

	return true;
#else
	return false;
#endif
}
