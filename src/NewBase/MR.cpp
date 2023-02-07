
#include "./MR.h"
#include "../Util/FUtil.hpp"
#include "../Util/StatUtil.h"
#include "../Util/ModelUtil.h"
#include "../SoilData/PlotPF/PFCurve.h"
#include "./Doc.h"
//#include "../NetCDF/NetCDFTransfer.h"
//#include "../NetCDF/ncFile.h"
//#include "../NetCDF/ncDim.h"
//#include "../NetCDF/ncVar.h"
//#include "../external.h"
//using namespace netCDF;
//using namespace netCDF::exceptions;

//#include "../External.h"

#include "codecogs/stats/moments/covariance.h"
#include "codecogs/stats/moments/correlation.h"
#include "codecogs/stats/moments/variance.h"

#include "codecogs/stats/moments/mean.h"

#include "../SoilData/GetSoilStorage.h"
#ifndef COUPSTD
#include "../MainFrm.h"


#endif // !CoupStd
#ifdef COUPSTD
#include <time.h>
#endif



static bool MultiRunChange;

MR::MR()
{
	
	MR_AddExtraMonitor(4);
	m_ResetMonitoring=false;
	m_ResetValMonitoring=false;
	m_pValidIndexSet=nullptr;
	m_ReReadMStorageFile=false;
	for(size_t i=0; i<MAXSIMVAL; i++)
		m_NumberSelectedValidationVar[i]=0;

	m_OutputBinFileExist=true;
	m_OutputValidBinFileExist=true;
	m_OutputValidXBinFileExist=false;
	m_WShed_Active=false;
	Num_Burn=100;
	m_DistDefault.Mean=m_DistDefault.Max=m_DistDefault.Min=m_DistDefault.Std=MISSING;
	m_DistDefault.P5=m_DistDefault.P50=m_DistDefault.P95=MISSING;
	m_DistDefault.NumData=0;
	m_ActualSimplex = Vector(0.5, 0.5);
	m_BayesianStepSize = 0.5;

}
MR::~MR()
{

}
string MR::MR_GetExtraLabel(size_t index)
{
	switch (index) {
							case 0:
								return "Accepted";
								break;
							case 1:
								return "TSV LogLi";
								break;
							case 2:
								return "Sum LogLi";
								break;
							case 3:
								return "Tot LogLi";
								break;
							case 4:
								return "MAP RunNo";
								break;
							case 5:
								return "Acc Sim Time";
								break;
							case 6:
								return "n";
								break;
							case 7:
								return "SSE";
								break;
							case 8:
								return "AIC";
								break;
							case 9:
								return "BIC";
								break;
						}
	return "";

}
void MR::MR_SetResetMonitoring()
{
	m_ResetMonitoring=true;
}
void MR::MR_SetResetValMonitoring()
{
	m_ResetValMonitoring=true;
}
size_t MR::MR_GetNumExtraMonitor()
{
	return m_ExtraMonitor.size();
}
size_t MR::MR_GetExtraMonitor(size_t index) 
{
	if(m_ExtraMonitor.size()>index)
		return m_ExtraMonitor[index];
	else
		return -1;
}
void MR::MR_AddExtraMonitor(size_t index)
{
	
	m_ExtraMonitor.push_back(index);
	m_ResetMonitoring=true;
}
bool MR::MR_RemoveExtraMonitor(size_t index)
{

	for(size_t i=0; i<m_ExtraMonitor.size();i++){
		if(m_ExtraMonitor[i]==index){
			m_ExtraMonitor.erase(m_ExtraMonitor.begin()+i);
			return true;
		}	
	}
	return false;
}
string MR::MR_GetParGroup(size_t index1, size_t index2)
{
	SimB* pPar;
	size_t counter=0;
	size_t nTabIndex, nDim;
	for (size_t i=0;i<m_P_MR_Array.size();i++){
		pPar=m_P_MR_Array[i].pBase;
		nTabIndex=m_P_MR_Array[i].TabIndex;
		if(m_P_MR_Array[i].pBase->Is_DB())
			nDim=((CDB*)pPar)->MR_Get_Dim();
		else if(nTabIndex==string::npos) nDim=pPar->MR_Get_Dim();
		else  nDim=pPar->MR_Get_Dim(nTabIndex);	
		if(nDim==index1) {
			counter++;
			if(counter==index2) {
				if(m_P_MR_Array[i].pBase->Is_DB())
					return "DataBase";
				else
					return pPar->GetGroup();
			}
		}
	}	  
	return " ";

}
string MR::MR_GetParName(size_t index1, size_t index2)
{
	SimB* pPar;
	size_t counter=0;
	size_t nTabIndex, nDim;
	for (size_t i=0;i<m_P_MR_Array.size();i++){
		pPar=(Ps*)m_P_MR_Array[i].pBase;
		nTabIndex=m_P_MR_Array[i].TabIndex;
		if(m_P_MR_Array[i].pBase->Is_DB())
			nDim=((CDB*)pPar)->MR_Get_Dim();
		else if(nTabIndex==string::npos) nDim=pPar->MR_Get_Dim();
		else  nDim=pPar->MR_Get_Dim(nTabIndex);	
		if(nDim==index1) {
			counter++;
			if(counter==index2) {
				if(m_P_MR_Array[i].pBase->Is_DB())
					return ((CDB*)pPar)->GetName();
				else
					return pPar->GetName();
			}
		}
	}	 
	return " ";
}

size_t MR::MR_GetMethod(size_t index1, size_t index2)
{
	SimB* pPar;
	size_t counter=0;
	size_t nTabIndex, nDim;
	for (size_t i=0;i<m_P_MR_Array.size();i++){
		pPar=(Ps*)m_P_MR_Array[i].pBase;
		nTabIndex=m_P_MR_Array[i].TabIndex;
		if(m_P_MR_Array[i].pBase->Is_DB())
			nDim=((CDB*)pPar)->MR_Get_Dim();
		else if(nTabIndex==string::npos) nDim=pPar->MR_Get_Dim();
		else  nDim=pPar->MR_Get_Dim(nTabIndex);	

		if(nDim==index1) {
			counter++;
			if(counter==index2) {
				if(m_P_MR_Array[i].pBase->Is_DB())
					return ((CDB*)pPar)->MR_Get_Method();
				else if(nTabIndex==string::npos)
					return pPar->MR_Get_Method();
				else 
					return pPar->MR_Get_Method(nTabIndex);
			}
		}
	}	
	return 0;
}
string MR::MR_GetMethodName(size_t index1, size_t index2)
{
	SimB* pPar;
	size_t counter=0;
	size_t nTabIndex, nDim,imethod;
	string test;
	for (size_t i=0;i<m_P_MR_Array.size();i++){
		pPar=(Ps*)m_P_MR_Array[i].pBase;
		nTabIndex=m_P_MR_Array[i].TabIndex;
		if(m_P_MR_Array[i].pBase->Is_DB())
			nDim=((CDB*)pPar)->MR_Get_Dim();
		else if(nTabIndex==string::npos) nDim=pPar->MR_Get_Dim();
		else  nDim=pPar->MR_Get_Dim(nTabIndex);	

		if(nDim==index1) {
			counter++;
			if(counter==index2) {
				if(m_P_MR_Array[i].pBase->Is_DB())
					return "DataBase";
				else if(nTabIndex==string::npos)
					imethod=pPar->MR_Get_Method();
				else 
					imethod=pPar->MR_Get_Method(nTabIndex);

				switch (imethod) {
						case 0:
							test="Linear";
							break;
						case 1: test="Logarithmic";
								break;
						case 2: test="Stochastic Linear";
							break;
						case 3: test="Stochastic Log";
							break;
						case 4: test="Table";
							break;
						case 5: test="Database Record";
							break;
						case 6: test="Same As";
							break;
						case 7: test="Relative Change As";
							break;
						case 8: test="Relative Opposite Change As";
							break;
						case 9: if(MR_UseBayesianCoVar)
									test="Apply Bayesian results";
								else
									test="Bayesian Calibration";
							break;
						case 10: test="Connected";
							break;
						case 11: 
						    test="Bayesian Calibration using log distribution\n";
							break;
						case 12:
							test = "Nelder Mead Optimization\n";
							break;
						}


				return test;
			}
		}
	}	
	return "";
}
size_t MR::MR_GetMonitoring(size_t index1, size_t index2)
{
	SimB* pPar;
	CDB* pP;
	size_t counter=0;
	size_t nTabIndex, nDim;
	for (size_t i=0;i<m_P_MR_Array.size();i++){
		pPar=(Ps*)m_P_MR_Array[i].pBase;
		nTabIndex=m_P_MR_Array[i].TabIndex;
		if(m_P_MR_Array[i].pBase->Is_DB()) {
			pP=(CDB*)pPar;
			nDim=pP->MR_Get_Dim();
		}
		else if(nTabIndex==string::npos) nDim=pPar->MR_Get_Dim();
		else  nDim=pPar->MR_Get_Dim(nTabIndex);	
		
		if(nDim==index1) {
			counter++;
			if(counter==index2) {
					if(nTabIndex==string::npos) {
						if(pPar->MR_Get_Method()==10)
							return 0;
						else
							return ((Ps*)pPar)->MR_Get_Monitoring();

					}
					else {
						if(pPar->MR_Get_Method(nTabIndex)==10)
							return 0;
						else
							return ((P*)pPar)->MR_Get_Monitoring(nTabIndex);

					}
			}
		}
	}	
	return -1;

}
void MR::MR_SetMonitoring(size_t index1, size_t index2, size_t value)
{
	SimB* pPar;
	CDB* pP;
	size_t counter=0;
	size_t nTabIndex, nDim;
	for (size_t i=0;i<m_P_MR_Array.size();i++){
		pPar=(Ps*)m_P_MR_Array[i].pBase;
		nTabIndex=m_P_MR_Array[i].TabIndex;
		if(m_P_MR_Array[i].pBase->Is_DB()) {
			pP=(CDB*)pPar;
			nDim=pP->MR_Get_Dim();
		}
		else if(nTabIndex==string::npos) nDim=pPar->MR_Get_Dim();
		else  nDim=pPar->MR_Get_Dim(nTabIndex);	
		
		if(nDim==index1) {
			counter++;
			if(counter==index2) {
					if(nTabIndex==string::npos) {
						if(pPar->MR_Get_Method()!=10)
							((Ps*)pPar)->MR_Set_Monitoring(value);
						return;

					}
					else {
						if(pPar->MR_Get_Method(nTabIndex)!=10)
							return ((P*)pPar)->MR_Set_Monitoring(nTabIndex, value);
						return;

					}
			}
		}
	}	 
	return;

}
double MR::MR_GetMin(size_t index1, size_t index2)
{
	SimB* pPar;
	CDB* pP;
	size_t counter=0;
	size_t nTabIndex, nDim;
	for (size_t i=0;i<m_P_MR_Array.size();i++){
		pPar=(Ps*)m_P_MR_Array[i].pBase;
		nTabIndex=m_P_MR_Array[i].TabIndex;
		if(m_P_MR_Array[i].pBase->Is_DB()) {
			pP=(CDB*)pPar;
			nDim=pP->MR_Get_Dim();
		}
		else if(nTabIndex==string::npos) nDim=pPar->MR_Get_Dim();
		else  nDim=pPar->MR_Get_Dim(nTabIndex);	
		
		if(nDim==index1) {
			counter++;
			if(counter==index2) {
					if(nTabIndex==string::npos) {
						if(pPar->MR_Get_Method()==10)
							return 0;
						else
							return ((Ps*)pPar)->MR_Get_Min();

					}
					else {
						if(pPar->MR_Get_Method(nTabIndex)==10)
							return 0;
						else
							return ((P*)pPar)->MR_Get_Min(nTabIndex);

					}
			}
		}
	}	
	return MISSING;

}
double  MR::MR_GetMin(size_t indexp)
{
	SimB* pPar,*pParDependent;
	CDB* pP;
	size_t counter=0;
	size_t nTabIndex, nDim,nTabIndexDep;
	size_t icount, index1, index2;
	index1=index2=icount=0;
	double Float=0;

	for(size_t i=0;i<MR_GetNumberOfDimensions();i++){
			for(size_t ii=0; ii<MR_Get_NumberOfParametersWithinDim(i+1);ii++){
					if(icount==indexp) {
						index1=i+1;
						index2=ii+1;
					}
					icount++;
			}
	}
	

	for (size_t i=0;i<m_P_MR_Array.size();i++){
		pPar=(Ps*)m_P_MR_Array[i].pBase;
		nTabIndex=m_P_MR_Array[i].TabIndex;
		if(m_P_MR_Array[i].pBase->Is_DB()) {
			pP=(CDB*)pPar;
			nDim=pP->MR_Get_Dim();
		}
		else if(nTabIndex==string::npos) nDim=pPar->MR_Get_Dim();
		else  nDim=pPar->MR_Get_Dim(nTabIndex);	
		
		if(nDim==index1) {
			counter++;
			if(counter==index2) {
					if(nTabIndex==string::npos) {
						if(pPar->MR_Get_Method()==10)
							return 0;
						else if(MR_GetMethod(index1,index2)>=6&&MR_GetMethod(index1,index2)<9) {
								pParDependent=((Ps*)pPar)->Get_MR_ParDependence();
								nTabIndexDep=((Ps*)pPar)->Get_MR_ParDependenceIndex();
								if(nTabIndexDep>=0&& nTabIndexDep!=string::npos)
									Float=((P*)pParDependent)->MR_Get_Min(nTabIndexDep);
							return Float;

						}
						else
							return ((Ps*)pPar)->MR_Get_Min();

					}
					else {
						if(pPar->MR_Get_Method(nTabIndex)==10)
							return 0;
						else if(pPar->MR_Get_Method(nTabIndex)==6&&MR_GetMethod(index1,index2)<9) {
								pParDependent=((P*)pPar)->Get_MR_ParDependence(nTabIndex);
								nTabIndexDep=((P*)pPar)->Get_MR_ParDependenceIndex(nTabIndex);
								if(nTabIndexDep>=0 && nTabIndexDep != string::npos)
									Float=((P*)pParDependent)->MR_Get_Min(nTabIndexDep);

								return Float;

						}
						else
							return ((P*)pPar)->MR_Get_Min(nTabIndex);

					}
			}
		}
	}	 
	return MISSING;

}
bool MR::MR_ResetMonitoring()
{
	if(m_ResetMonitoring) {
		m_ResetMonitoring=false;
		return true;
	}
	return false;
}
bool MR::MR_ResetValMonitoring()
{
	if(m_ResetValMonitoring) {
		m_ResetValMonitoring=false;
		return true;
	}
	return false;
}
double MR::MR_GetStart(size_t index1, size_t index2)
{
	SimB* pPar;
	CDB* pP;
	size_t counter=0;
	size_t nTabIndex, nDim;
	for (size_t i=0;i<m_P_MR_Array.size();i++){
		pPar=(Ps*)m_P_MR_Array[i].pBase;
		nTabIndex=m_P_MR_Array[i].TabIndex;
		if(m_P_MR_Array[i].pBase->Is_DB()) {
			pP=(CDB*)pPar;
			nDim=pP->MR_Get_Dim();
		}
		else if(nTabIndex==string::npos) nDim=pPar->MR_Get_Dim();
		else  nDim=pPar->MR_Get_Dim(nTabIndex);	
		
		if(nDim==index1) {
			counter++;
			if(counter==index2) {
				if(nTabIndex==string::npos)
					return ((Ps*)pPar)->MR_Get_Start();
				else 
					return ((P*)pPar)->MR_Get_Start(nTabIndex);
			}
		}
	}	
	return MISSING;

}
double MR::MR_GetMax(size_t index1, size_t index2)
{
	SimB* pPar;
	CDB* pP;
	size_t counter=0;
	size_t nTabIndex, nDim;
	for (size_t i=0;i<m_P_MR_Array.size();i++){
		pPar=(Ps*)m_P_MR_Array[i].pBase;
		nTabIndex=m_P_MR_Array[i].TabIndex;
		if(m_P_MR_Array[i].pBase->Is_DB()) {
			pP=(CDB*)pPar;
			nDim=pP->MR_Get_Dim();
		}
		else if(nTabIndex==string::npos) nDim=pPar->MR_Get_Dim();
		else  nDim=pPar->MR_Get_Dim(nTabIndex);	
		if(nDim==index1) {
			counter++;
			if(counter==index2) {
				if(nTabIndex==string::npos)
					return ((Ps*)pPar)->MR_Get_Max();
				else 
					return ((P*)pPar)->MR_Get_Max(nTabIndex);
			}
		}
	}		
	return MISSING;
}
double MR::MR_GetMax(size_t indexp)
{
	SimB* pPar,*pParDependent;
	CDB* pP;
	size_t counter=0;
	size_t nTabIndex, nDim,nTabIndexDep;

	size_t icount, index1, index2;
	index1 = index2 = 0;
	double Float;
	icount=0;

	for(size_t i=0;i<MR_GetNumberOfDimensions();i++){
			for(size_t ii=0; ii<MR_Get_NumberOfParametersWithinDim(i+1);ii++){
					if(icount==indexp) {
						index1=i+1;
						index2=ii+1;
					}
					icount++;

			}
	}


	for (size_t i=0;i<m_P_MR_Array.size();i++){
		pPar=(Ps*)m_P_MR_Array[i].pBase;
		nTabIndex=m_P_MR_Array[i].TabIndex;
		if(m_P_MR_Array[i].pBase->Is_DB()) {
			pP=(CDB*)pPar;
			nDim=pP->MR_Get_Dim();
		}
		else if(nTabIndex==string::npos) nDim=pPar->MR_Get_Dim();
		else  nDim=pPar->MR_Get_Dim(nTabIndex);	
		if(nDim==index1) {
			counter++;
			if(counter==index2) {
				if(nTabIndex==string::npos) {
					if(pPar->MR_Get_Method()==10)
							return 0;
					else if(MR_GetMethod(index1,index2)>=6&&MR_GetMethod(index1,index2)<9) {
							pParDependent=((Ps*)pPar)->Get_MR_ParDependence();
							nTabIndexDep=((Ps*)pPar)->Get_MR_ParDependenceIndex();
							if(nTabIndexDep>=0&&nTabIndexDep!=string::npos)
								Float=((P*)pParDependent)->MR_Get_Max(nTabIndexDep);
							else
								Float=((Ps*)pParDependent)->MR_Get_Max();
							return Float;
					}
					else
						return ((Ps*)pPar)->MR_Get_Max();
				}
				else  {
					if(pPar->MR_Get_Method()==10)
							return 0;
					else if(MR_GetMethod(index1,index2)>=6&&MR_GetMethod(index1,index2)<9) {
							pParDependent=((P*)pPar)->Get_MR_ParDependence(nTabIndex);
							nTabIndexDep=((P*)pPar)->Get_MR_ParDependenceIndex(nTabIndex);
							if(nTabIndexDep>=0 && nTabIndexDep != string::npos)
								Float=((P*)pParDependent)->MR_Get_Max(nTabIndexDep);
							else
								Float=((Ps*)pParDependent)->MR_Get_Max();
							return Float;
					}
					else					
					
					
					return ((P*)pPar)->MR_Get_Max(nTabIndex);


				}
			}
		}
	}	 
	return double(MISSING);
}
bool MR::MR_GetLogDist(size_t indexp)
{
	size_t counter=0;

	size_t icount, index1, index2;
	icount=0;

	for(size_t i=0;i<MR_GetNumberOfDimensions();i++){
			for(size_t ii=0; ii<MR_Get_NumberOfParametersWithinDim(i+1);ii++){
					if(icount==indexp) {
						index1=i+1;
						index2=ii+1;
						if(MR_GetMethod(index1, index2)==3||MR_GetMethod(index1, index2)==11)
							return true;
						else
							return false;
					}
					icount++;

			}
	}
	return false;

}
SIMB MR::MR_Get_PAR(size_t index1, size_t index2) {
	SIMB out;
	size_t counter=0;
	size_t nDim;
	for (size_t i=0;i<m_P_MR_Array.size();i++){
		out.pBase=m_P_MR_Array[i].pBase;
		out.TabIndex=m_P_MR_Array[i].TabIndex;
		
		if(m_P_MR_Array[i].pBase->Is_DB())
			nDim=((CDB*)out.pBase)->MR_Get_Dim();
		else if(!m_P_MR_Array[i].pBase->Is_PTable()) 
			nDim=out.pBase->MR_Get_Dim();
		else  
			nDim=out.pBase->MR_Get_Dim(out.TabIndex);	
		if(nDim==index1) {
			counter++;
			if(counter==index2) 
				return out;
		}
	}	
	out.pBase=nullptr;
	return out;
}
SimB* MR::MR_Get_pPar(size_t index1,size_t index2)
{
	SimB* pPar;
	size_t counter=0;
	size_t nTabIndex, nDim;
	for (size_t i=0;i<m_P_MR_Array.size();i++){
		pPar=m_P_MR_Array[i].pBase;
		nTabIndex=m_P_MR_Array[i].TabIndex;
		if(m_P_MR_Array[i].pBase->Is_DB())
			nDim=((CDB*)pPar)->MR_Get_Dim();
		else if(nTabIndex==string::npos) nDim=pPar->MR_Get_Dim();
		else  nDim=pPar->MR_Get_Dim(nTabIndex);	
		if(nDim==index1) {
			counter++;
			if(counter==index2) 
				return pPar;
		}
	}	
	return nullptr;
}
size_t MR:: MR_GetTabIndex(size_t index1,size_t index2)
{ // No error checking is provided here.
	SimB* pPar;
	size_t counter=0;
	size_t nTabIndex, nDim;
	for (size_t i=0;i<m_P_MR_Array.size();i++){
		if (i > 34) {
			auto koll = m_P_MR_Array[i].TabIndex;

		}
		pPar=m_P_MR_Array[i].pBase;
		nTabIndex=m_P_MR_Array[i].TabIndex;
		if(m_P_MR_Array[i].pBase->Is_DB())
			nDim=((CDB*)pPar)->MR_Get_Dim();
		else if(nTabIndex==string::npos) nDim=pPar->MR_Get_Dim();
		else  nDim=pPar->MR_Get_Dim(nTabIndex);	
		if(nDim==index1) {
			counter++;
			if(counter==index2) return nTabIndex;
		}
	}	
	return -1;
}
size_t MR:: MR_GetParType(size_t index1,size_t index2)
{ // No error checking is provided here.
	SimB* pPar;
	size_t counter=0;
	size_t nDim, nTabIndex, ParType;
	for (size_t i=0;i<m_P_MR_Array.size();i++){
		pPar=(Ps*)m_P_MR_Array[i].pBase;
		nTabIndex=m_P_MR_Array[i].TabIndex;
		
		if(m_P_MR_Array[i].pBase->Is_DB()) {
			nDim=((CDB*)pPar)->MR_Get_Dim();
			ParType=2;
		}
		else if(nTabIndex==string::npos) {
			nDim=pPar->MR_Get_Dim();
			ParType=0;
		}
		else  {
			nDim=pPar->MR_Get_Dim(nTabIndex);	
			ParType=1;
		}
		
		if(nDim==index1) {
			counter++;
			if(counter==index2) return ParType;
		}
	}	
	return -1;
}
size_t MR:: MR_Get_NumberOfParametersWithinDim(size_t index)
{  
	size_t counter=0;
	SimB* pPar;

	size_t nTabIndex;
	size_t nDim;
	for (size_t i=0;i<m_P_MR_Array.size();i++){
		pPar=(Ps*)m_P_MR_Array[i].pBase;
		nTabIndex=m_P_MR_Array[i].TabIndex;
		if(m_P_MR_Array[i].pBase->Is_DB())
			nDim=((CDB*)pPar)->MR_Get_Dim();
		else if(nTabIndex==string::npos) nDim=pPar->MR_Get_Dim();
		else  nDim=pPar->MR_Get_Dim(nTabIndex);	
		if(nDim==index) counter++;
	}	
	return counter;
}
double MR::MR_GetTableValue(size_t index1, size_t index2, size_t row)
{
	double tmp=double(MISSING);
	SimB* pPar;
	size_t counter=0;
	size_t nTabIndex, nDim;

	for (size_t i=0;i<m_P_MR_Array.size();i++){
		pPar=(Ps*)m_P_MR_Array[i].pBase;
		nTabIndex=m_P_MR_Array[i].TabIndex;
		if(nTabIndex==string::npos) nDim=pPar->MR_Get_Dim();
		else  nDim=pPar->MR_Get_Dim(nTabIndex);	

		if(nDim==index1) {
			counter++;
			if(counter==index2) {
				if(nTabIndex==string::npos)
					tmp = ((Ps*)pPar)->MR_Get_TabValue(row);
				else 
					tmp = ((P*)pPar)->MR_Get_TabValue(nTabIndex, row);
			}
		}
	}	 
	return tmp;
}
void MR::MR_SetRepeatitionsOfDimension(size_t index, size_t value)
{
   	if(m_MultiRun_Array.size()>index) {
		m_MultiRun_Array[index].NumberOfRepeatitionsWithinDimension=value;
	}
}
void MR::MR_SetNewNumberofTotalRuns(size_t value)
{
	auto idim = MR_GetNumberOfDimensions();
	
	double ratio = double(value) /double( MR_Get_NumberofRuns());
	auto icurrent = MR_GetNumOfRepeatitionsOfDim(idim - 1);
	size_t newrep = max(size_t(1),size_t(double(icurrent)*ratio+0.01));
	MR_SetRepeatitionsOfDimension(idim - 1, newrep);
	MR_Get_NumberofRuns();


}
size_t MR::MR_GetNumOfRepeatitionsOfDim(size_t idim) {
	if(m_MultiRun_Array.size()>idim) 
		return m_MultiRun_Array[idim].NumberOfRepeatitionsWithinDimension;
	else
		return string::npos;


}
void MR::MR_AddNewDimensiomWith11Repeatitions()
{
	MRv s;
	s.NumberOfRepeatitionsWithinDimension=11;
	s.nCount=1;
	m_MultiRun_Array.push_back(s);
}

void MR::MR_DeleteDimValue(size_t index)
{
  if(m_MultiRun_Array.size()>index)
		 m_MultiRun_Array.erase(m_MultiRun_Array.begin()+index);
}

bool MR::MR_UpdateDimCounter()
{	
	m_DocFile.m_SimulationRunNo++;

	size_t test = MR_Get_ActualRunNumber();

	vector<size_t> modulus;
	size_t number=1;
	for (size_t i = 0; i<m_MultiRun_Array.size()-1; i++) {
		number *= m_MultiRun_Array[i].NumberOfRepeatitionsWithinDimension;
		modulus.push_back(number);
	}
	size_t idim = 0;
	bool find = true;
	if (modulus.size() > 0) {
		find = false;
		idim = modulus.size() - 1;
	}
	while(!find) {
		if ((test-1)%modulus[idim] == modulus[idim]-1) {
			find = true;
			idim++;
		}
		else if (idim > 0)
			idim--;
		else
			find = true;
	}
	if (idim > m_ActualDimension) {

		if ((m_MultiRun_Array[m_ActualDimension].nCount)%m_MultiRun_Array[m_ActualDimension].NumberOfRepeatitionsWithinDimension == 0) m_MultiRun_Array[m_ActualDimension].nCount = 1;
	}
	m_ActualDimension = idim;
	m_MultiRun_Array[m_ActualDimension].nCount++;
	
	return true;

}
bool MR::WSHED_UpdateCounter()
{
	m_DocFile.m_SimulationRunNo++;
	if(m_WShed_ActiveIndex+1<m_WShed_ID.size()){
			m_WShed_ActiveIndex++;
			return true;
	}
	return false;
}
void MR::WSHED_ResetCounter()
{
		m_WShed_ActiveIndex=0;
}
void MR::WSHED_UpdateValues()
{
/*	Ps *pPar;
	string Name,Group, Type;
	Type="Parameters";

	PARAMETERS_mp_WATERSHED_RUN=m_WShed_ActiveIndex;
	if(m_WShed_ActiveIndex<m_WShed_ID.size()) {	
		PARAMETERS_mp_LATERAL_OUTPUT_ID=size_t(m_WShed_ID_Receiver[m_WShed_ActiveIndex]);
		PARAMETERS_mp_LATERAL_INPUT_ID=size_t(m_WShed_ID[m_WShed_ActiveIndex]);

//   Selection of parameters to each run could be made below:

		Group="Drainage and deep percolation";
		Name="DrainSpacing";

//		Name="DrainLevel";
		float SlopeValue, DrainSpacingValue;
		SlopeValue=m_WShed_Slope[m_WShed_ActiveIndex]; 	
		if(SlopeValue<1.E-10f) SlopeValue=1.E-10f;
		DrainSpacingValue=1/SlopeValue;

		pPar=(Ps*)GetPtr(Type, Group,Name);
		if(pPar!=nullptr) {
			pPar->SetValue(DrainSpacingValue);
			pPar->Apply();
		}
		/*
//		Coordinates
		Group="Additional Variables";
//		Name="X-coordinate";
		pParSw=m_pDoc->GetPtr(Name, type, Group);*/


	//}
	//}
	  
}
bool MR::MR_UpdateValuesFromMBin(size_t runno)
{  	

	SimB *pPar;
	P* pCPt;

	float Float;
	size_t	  nTabIndex;


	size_t startRunNo=m_DocFile.m_SimulationRunNo-MR_Get_ActualRunNumber();
	size_t ActualRunNo=runno-startRunNo-1;
	size_t icount;
		


	icount=0;
	for(size_t i=0;i<MR_GetNumberOfDimensions();i++){
			for(size_t ii=0; ii<MR_Get_NumberOfParametersWithinDim(i+1);ii++){
					pPar=MR_Get_pPar(i+1,ii+1);
					nTabIndex=MR_GetTabIndex(i+1,ii+1);
				
					Float=m_MStorage.GetMultiP(ActualRunNo,icount);

					if(nTabIndex==string::npos) {
						((Ps*)pPar)->SetValue(Float);
						//pPar->Apply();
						m_DocFile.m_TimeModified=time(nullptr);
						History_Add(pPar,-1,time(nullptr),Float);
					}
					else if(nTabIndex>=0) {
						pCPt=(P*)pPar;
						pCPt->SetValue(nTabIndex, Float);
						//pCPt->Apply(nTabIndex);
						m_DocFile.m_TimeModified=time(nullptr);
						History_Add(pPar,nTabIndex,time(nullptr),Float);
					}	
					icount++;
			}
	}	 
	return true;
}
void MR::MR_UpdateValues()
{
#ifndef COUPSTD
	//::StartMode=false;
	p_ModelInfo->SetStartMode(false);
#endif
	Ps* pPar, * pParDependent;
	P* pCPt;
	bool recalculate_profile = false;
	string strname, strkey;
	double Ratio, Float, min, max;
	size_t	ii, irep, icount, nTabIndex, nTabIndexDep, icountBayes, icountNelderMead{ 0 };
	//PARAMETERS_mp_NODIMCOUNTERS=
		MR_GetNumberOfDimensions();
	//PARAMETERS_mp_ACTUALNOMULRUN=
		MR_Get_ActualRunNumber();
	//PARAMETERS_mp_TOTALNOMULRUN=
		MR_Get_TotalNumberofRuns();
	//PARAMETERS_mp_WATERSHED_RUN=
		auto kollNo=MR_Get_ActualRunNumber();


		bool NewRandomEstimate = true;

		pPar = GetPsPointer("Aggregate Calib Number");
	//if(PARAMETERS_mp_ACTUALNOMULRUN==1)PARAMETERS_mp_RAND_INIT(); 
		if (m_IsMakingBayesianCalib) {

			if (pPar != nullptr) {
				size_t n = size_t(pPar->GetValue() + 0.001);
				if ((MR_Get_ActualRunNumber() - 1) % n == 0)
					UpdateNewPValues();
			}
		}
		else {
			NewRandomEstimate = false;
			if (pPar != nullptr) {
				size_t n = size_t(pPar->GetValue() + 0.001);
				if ((MR_Get_ActualRunNumber() - 1) % n == 0) {
					NewRandomEstimate = true;
				}
			}




		}





	size_t recalc;
	if (m_MultiRun_Array.size() == 0) return;

	//for(size_t i=0;i<MR_GetNumberOfDimensions();i++){
		size_t i = 0;
		for (size_t i = 0; i < m_ActualDimension + 1; i++) {
			irep = m_MultiRun_Array[i].nCount;
			irep = m_MultiRun_Array[i].NumberOfRepeatitionsWithinDimension;
			if (irep < 2) irep = 2;
			//i = m_ActualDimension;

			icount = MR_GetDimCounter(i);
			icountBayes = 0;
			//PARAMETERS_mp_DIMCOUNTERS[i]=icount;
			for (ii = 0; ii < MR_Get_NumberOfParametersWithinDim(i + 1); ii++) {
				pPar = (Ps*)MR_Get_pPar(i + 1, ii + 1);
				nTabIndex = MR_GetTabIndex(i + 1, ii + 1);
				//if(MR_GetMethod(i+1,ii+1)<=2) {
				min = MR_GetMin(i + 1, ii + 1); // This variable does not always have realistic values.
				max = MR_GetMax(i + 1, ii + 1); // " "


			//}
				switch (MR_GetMethod(i + 1, ii + 1)) {
				case 0: Ratio = (float)(icount - 1) / (float)(irep - 1);// linear
					Float = Ratio * max + (1 - Ratio)*min;
					if (nTabIndex == string::npos) {
						recalc = pPar->SetValue(Float);
						pPar->Apply();
					}
					else {
						pCPt = (P*)pPar;
						//pCPt->SetValue(nTabIndex, Float);
						recalc = pCPt->SetValue(nTabIndex, Float);

						pCPt->Apply(nTabIndex);
					}
					if (recalc == 1 || recalc == 2) recalculate_profile = true; // Threshold to indicate change of soil profile					
					break;
				case 1: //Logarithmic
					Ratio = (float)(icount - 1) / (float)(irep - 1);
					Float = exp(Ratio*log(max) + (1 - Ratio)*log(min));
					if (nTabIndex == string::npos) {
						recalc = pPar->SetValue(Float);
						LinkedChangeToDB_Parameters(pPar, recalc);
						pPar->Apply();
					}
					else {
						pCPt = (P*)pPar;
						pCPt->SetValue(nTabIndex, Float);
						recalc = pCPt->SetValue(nTabIndex, Float);
						pCPt->Apply(nTabIndex);
					}
					if (recalc == 1 || recalc == 2) recalculate_profile = true; // Threshold to indicate change of soil profile

					break;
				case 2:	//PARAMETERS_mp_RAND_LOCAL(); // (Uniform) randomly distributed variables
					if (NewRandomEstimate) {
						Ratio = StatUtil::frand();//://PARAMETERS_mp_RANDOMVALUE;					
						Float = Ratio * max + (1 - Ratio)*min;
						if (nTabIndex != string::npos)
							((P*)pPar)->MR_Set_RelChange(nTabIndex, Ratio);
						else
							pPar->MR_Set_RelChange(Ratio);

						if (nTabIndex == string::npos) {
							recalc = pPar->SetValue(Float);
							pPar->Apply();
						}
						else {
							pCPt = (P*)pPar;
							recalc = pCPt->SetValue(nTabIndex, Float);
							pCPt->Apply(nTabIndex);
						}
						if (recalc == 1 || recalc == 2) { // Threshold to indicate change of soil profile
							recalculate_profile = true;

						};
					}
					break;
				case 3:	//PARAMETERS_mp_RAND_LOCAL();
					if (NewRandomEstimate) {
						Ratio = StatUtil::frand();//PARAMETERS_mp_RANDOMVALUE;
						Float = exp(Ratio*log(max) + (1 - Ratio)*log(min));
						if (nTabIndex != string::npos)
							((P*)pPar)->MR_Set_RelChange(nTabIndex, Ratio);
						else
							pPar->MR_Set_RelChange(Ratio);

						if (nTabIndex == string::npos) {
							recalc = pPar->SetValue(Float);
							pPar->Apply();
						}
						else {
							pCPt = (P*)pPar;
							recalc = pCPt->SetValue(nTabIndex, Float);



							pCPt->Apply(nTabIndex);
						}
						if (recalc == 1 || recalc == 2) { // Threshold to indicate change of soil profile
							recalculate_profile = true;

						};
					}
					break;
					// MR_Extract_MidpointValue expects the final argument to be
				case 4: // Table  
					if (nTabIndex == string::npos) // Table, Note that ii is not incremented during MRs!!
						Float = pPar->MR_Get_TabValue(icount - 1);  // Ps case
					else
						Float = ((P*)pPar)->MR_Get_TabValue(nTabIndex, icount - 1);  // Ps case
					break;
					// 	Float=MR_GetTableValue(i+1,ii+1,icount-1);				 	    break;
				case 5: // Database Records
					strname = ((CDB*)pPar)->GetName();
					strkey = ((CDB*)pPar)->MR_GetKeySelection(icount - 1);
					break;
				case 6: // Same as another parameter
					if (nTabIndex != string::npos) {
						pParDependent = (Ps*)((P*)pPar)->Get_MR_ParDependence(nTabIndex);
						nTabIndexDep = ((P*)pPar)->Get_MR_ParDependenceIndex(nTabIndex);
						if (nTabIndexDep >= 0)
							Float = ((P*)pParDependent)->GetValue(nTabIndexDep);
						else
							Float = pParDependent->GetValue();
					}
					else {
						pParDependent = (Ps*)pPar->Get_MR_ParDependence();
						nTabIndexDep = pPar->Get_MR_ParDependenceIndex();
						if (nTabIndexDep != string::npos)
							Float = ((P*)pParDependent)->GetValue(nTabIndexDep);
						else
							Float = pParDependent->GetValue();
					}
					break;
				case 7: // Same Relative another parameter
					if (nTabIndex != string::npos) {
						pParDependent = (Ps*)((P*)pPar)->Get_MR_ParDependence(nTabIndex);
						nTabIndexDep = ((P*)pPar)->Get_MR_ParDependenceIndex(nTabIndex);
						if (nTabIndexDep != string::npos)
							Ratio = ((P*)pParDependent)->MR_Get_RelChange(nTabIndexDep);
						else
							Ratio = pParDependent->MR_Get_RelChange();
					}
					else {
						pParDependent = (Ps*)pPar->Get_MR_ParDependence();
						nTabIndexDep = pPar->Get_MR_ParDependenceIndex();
						if (nTabIndexDep != string::npos)
							Ratio = ((P*)pParDependent)->MR_Get_RelChange(nTabIndexDep);
						else
							Ratio = pParDependent->MR_Get_RelChange();
					}
					Float = Ratio * max + (1 - Ratio)*min;
					break;
				case 8: // Same Opposite Relative another parameter
					if (nTabIndex != string::npos) {
						pParDependent = (Ps*)((P*)pPar)->Get_MR_ParDependence(nTabIndex);
						nTabIndexDep = ((P*)pPar)->Get_MR_ParDependenceIndex(nTabIndex);
						if (nTabIndexDep != string::npos)
							Ratio = ((P*)pParDependent)->MR_Get_RelChange(nTabIndexDep);
						else
							Ratio = pParDependent->MR_Get_RelChange();
					}
					else {
						pParDependent = (Ps*)pPar->Get_MR_ParDependence();
						nTabIndexDep = pParDependent->Get_MR_ParDependenceIndex();
						if (nTabIndexDep != string::npos)
							Ratio = ((P*)pParDependent)->MR_Get_RelChange(nTabIndexDep);
						else
							Ratio = pParDependent->MR_Get_RelChange();
					}
					Float = Ratio * min + (1 - Ratio)*max;
					break;
				case 9: case 11: // Bayesian calibration

					Ratio = GetBayes_PCandidateValue(icountBayes);
					//Float=0.5; // Waiting for new implementation
					Float = min + Ratio * (max - min);
					if (MR_GetMethod(i + 1, ii + 1) == 11 && min > 0) {

						//Ratio=(Float-log(min))/(log(max)-log(min));
						Float = exp(Ratio*log(max) + (1 - Ratio)*log(min));

					}

					if (nTabIndex != string::npos)
						((P*)pPar)->MR_Set_RelChange(nTabIndex, Ratio);
					else
						pPar->MR_Set_RelChange(Ratio);

					icountBayes++;
					break;
				case 10:
					break;	// No Action
				case NELDER_MEAD:
				//	m_ActualSimplex[icountNelderMead] = max(min, max(min, m_ActualSimplex[icountNelderMead]));
					Float = m_ActualSimplex[icountNelderMead];

					if (nTabIndex == string::npos) {
						recalc = pPar->SetValue(Float);
						pPar->Apply();
					}
					else {
						pCPt = (P*)pPar;
						//pCPt->SetValue(nTabIndex, Float);
						recalc = pCPt->SetValue(nTabIndex, Float);

						pCPt->Apply(nTabIndex);
					} 
					icountNelderMead++;
					break;

				};
				
				if (MR_GetMethod(i + 1, ii + 1) != 5 && MR_GetMethod(i + 1, ii + 1) <= 9 || MR_GetMethod(i + 1, ii + 1) == 11 || MR_GetMethod(i + 1, ii + 1) == NELDER_MEAD) {

					if (nTabIndex == string::npos) {
						recalc = pPar->SetValue(Float);
						if (recalc > 10000)
							LinkedChangeToDB_Parameters(pPar, recalc);

					}
					else {
						pCPt = (P*)pPar;
						recalc = pCPt->SetValue(nTabIndex, Float);

						pCPt->Apply(nTabIndex);
					}
					if (recalc == 1 || recalc == 2) recalculate_profile = true;
					//PARAMETERS_mp_DIMPARVALUES[i]=Float;
				}
				else if (MR_GetParType(i + 1, ii + 1) == 2) {
					// Retrieve and apply
					string name = ((CDB*)pPar)->GetName();
					if (name.find("Soil Properties") == 0) {
						MultiRunChange = true;
						GetProfileFromPlotPF(strkey);



					}
					else {
						Reset();
						ReadFrom_DB_Object(strname, strkey, false);
					}

				}
				if (MR_GetMethod(i + 1, ii + 1) == 6) {






				}
				//Check validation Disabled error in check June 13
				if (m_pValidIndexSet == nullptr) return;
				if (m_pValidIndexSet->GetIntValue() == 1) {
					size_t numindex = size_t(m_pValidIndex->GetValue());
					size_t numrep = size_t(m_pValidIndexRep->GetValue());
					size_t nummax = size_t(m_pValidIndexLoopSize->GetValue());
					size_t val;

					for (size_t j2 = 0; j2 < numrep; j2++) {
						val = j2 * nummax + numindex - 1;
						//ValidationFilePointer()->SetValNumber(j2,val);
						//SetValFileIndex(j2, val);

					}


				}

			}
		}
		if(recalculate_profile) ReCalculateNewSoilProfile(); // Make conditional with soil properties changed

	
}
bool MR::SetParametersRandomForMultiRun(vector<SIMB> simv, FCSV * pCSVFile)
{
	vector<double> values, min_values, max_values;
	size_t Dimension = MR_GetNumberOfDimensions();

			min_values = pCSVFile->GetMinVector();
			max_values = pCSVFile->GetMaxVector();
			for (size_t i = 0; i < simv.size(); i++) {
				if (simv[i].pBase->Is_PTable()) {
					dynamic_cast<P*>(simv[i].pBase)->MR_Set_Min(simv[i].TabIndex, min_values[i]);
					dynamic_cast<P*>(simv[i].pBase)->MR_Set_Max(simv[i].TabIndex, max_values[i]);
					dynamic_cast<P*>(simv[i].pBase)->MR_Set_Start(simv[i].TabIndex, (min_values[i] + max_values[i]) / 2.);

					dynamic_cast<P*>(simv[i].pBase)->MR_Set_Method(simv[i].TabIndex, STOCHASTIC_LIN_CHANGE);
					dynamic_cast<P*>(simv[i].pBase)->MR_Set_Dim(simv[i].TabIndex, Dimension);
					dynamic_cast<MR*>(this)->MR_CP_Set(simv[i].pBase, simv[i].TabIndex);
				}
				else {
					dynamic_cast<Ps*>(simv[i].pBase)->MR_Set_Min(min_values[i]);
					dynamic_cast<Ps*>(simv[i].pBase)->MR_Set_Max(max_values[i]);
					dynamic_cast<Ps*>(simv[i].pBase)->MR_Set_Start((min_values[i] + max_values[i]) / 2.);
					dynamic_cast<Ps*>(simv[i].pBase)->MR_Set_Method(STOCHASTIC_LIN_CHANGE);
					dynamic_cast<Ps*>(simv[i].pBase)->MR_Set_Dim(Dimension);
					dynamic_cast<MR*>(this)->MR_CP_Set(simv[i].pBase, string::npos);

				}
			}



	return true;
}
size_t MR::MR_Get_NumBayesPar()
{
size_t icount=0;
	for(size_t i=0;i<MR_GetNumberOfDimensions();i++){
			for(size_t ii=0; ii<MR_Get_NumberOfParametersWithinDim(i+1);ii++){
				if(MR_GetMethod(i+1,ii+1)<4|| MR_GetMethod(i + 1, ii + 1) > 8) {
					icount++;
				}
			}
	}
return icount;
}
size_t MR::MR_Get_NumNotTablePar()
{
	size_t icount = 0;
	size_t count_all = 0;
	m_NoTableParIndex.clear();
	for (size_t i = 0; i < MR_GetNumberOfDimensions(); i++) {
		for (size_t ii = 0; ii < MR_Get_NumberOfParametersWithinDim(i + 1); ii++) {
			if (MR_GetMethod(i + 1, ii + 1) != 4) {
				icount++;
				m_NoTableParIndex.push_back(count_all);
			}
			count_all++;
		}
	}
	return icount;
}
size_t MR::MR_Get_NumTotalPar()
{
size_t icount=0;
	for(size_t i=0;i<MR_GetNumberOfDimensions();i++){
			icount+=MR_Get_NumberOfParametersWithinDim(i+1);
	}

return icount;

}
string MR::MR_Get_BayesGroup(size_t index)
{
size_t icount;
string name;
//P* pCPt;

	icount=0;

	for(size_t i=0;i<MR_GetNumberOfDimensions();i++){
			for(size_t ii=0; ii<MR_Get_NumberOfParametersWithinDim(i+1);ii++){
				if(MR_GetMethod(i+1,ii+1)!=5) {
					if(icount==index) {
						name=MR_GetParGroup(i+1,ii+1);
						return name;
					}
					icount++;
					
				}
			}
	}
	return "-";

}
size_t MR::MR_Get_BayesMethod(size_t index)
{
size_t icount;
string name;
//P* pCPt;

	icount=0;

	for(size_t i=0;i<MR_GetNumberOfDimensions();i++){
			for(size_t ii=0; ii<MR_Get_NumberOfParametersWithinDim(i+1);ii++){
				if(MR_GetMethod(i+1,ii+1)!=5) {
					if(icount==index) {
						return MR_GetMethod(i+1,ii+1);
					}
					icount++;
					
				}
			}
	}
	return -1;

}
string MR::MR_Get_CalibParName(size_t index)
{
string name;
size_t icount,nTabIndex;

SimB *pPar;
//P* pCPt;

	icount=0;

	for(size_t i=0;i<MR_GetNumberOfDimensions();i++){
			for(size_t ii=0; ii<MR_Get_NumberOfParametersWithinDim(i+1);ii++){
				if(MR_GetMethod(i+1,ii+1)!=5) {
					if(icount==index) {
						pPar=MR_Get_pPar(i+1,ii+1);
						nTabIndex=MR_GetTabIndex(i+1,ii+1);
						name=pPar->GetName().c_str();
						if(pPar->Is_Vector())				
							name+="("+FUtil::STD_ItoAscii(nTabIndex+1)+")";
						return name;
					}
					icount++;
					
				}
			}
	}  
	return "-";

}
void MR::MR_CHECK_Bayesian(bool UpdateExt)
{
 
	if(!IsMultiSimulation()) return;
	if(!m_IsMakingBayesianCalib&&!m_IsMakingNelderMeadCalib) return;
	
	if(!UpdateExt) {


		Set_TotLogLi(m_DocFile.m_TsVLogLi+m_DocFile.m_TsVLogLiSum);
		Ps* pPar = GetPsPointer("Aggregate Calib Number");
		if (pPar != nullptr) {
			size_t n = size_t(pPar->GetValue() + 0.001);
			if ((MR_Get_ActualRunNumber()) % n == 0) {
				if(m_IsMakingBayesianCalib) Bayes_Check();
				if (m_IsMakingNelderMeadCalib) {
					m_ActualNumberNelderMeadSteps++;
					auto ndim = m_ActualSimplex.dimension();
					int icount_NelderMead = 0;
					vector<float> newvalues;
					auto actualobjectivefunction = m_InformCriteria.SSE;
					if (NelderMeadOjective(m_NelderMeadObjective) == NelderMeadOjective::AIC)
						actualobjectivefunction = m_InformCriteria.AIC;
					else if (NelderMeadOjective(m_NelderMeadObjective) == NelderMeadOjective::BIC)
						actualobjectivefunction = m_InformCriteria.BIC;
					if (m_ActualNumberNelderMeadSteps <= ndim) {
						m_NelderMead.insert(m_ActualSimplex, -actualobjectivefunction);
						for (size_t i = 0; i < MR_GetNumberOfDimensions(); i++) {
							for (size_t ii = 0; ii < MR_Get_NumberOfParametersWithinDim(i + 1); ii++) {
								if (MR_GetMethod(i + 1, ii + 1) == NELDER_MEAD) {
									icount_NelderMead++;
									auto max = MR_GetMax(i + 1, ii + 1);
									auto min = MR_GetMin(i + 1, ii + 1);
									auto range = max - min;
									float scalingfactor = 0.2+float(m_ActualNumberNelderMeadSteps) / float(ndim);
									if ((m_ActualNumberNelderMeadSteps - 1) % ndim == 0)
										newvalues.push_back(max);
									else   if ((m_ActualNumberNelderMeadSteps - 1) % ndim ==1)
										newvalues.push_back(min);
									else   if ((m_ActualNumberNelderMeadSteps - 1) % ndim == 2)
										newvalues.push_back(max -float(scalingfactor * range));
									else   if ((m_ActualNumberNelderMeadSteps - 1) % ndim == 3)
										newvalues.push_back(min + float(scalingfactor * range));
									else if((m_ActualNumberNelderMeadSteps-1)%2==0)
										newvalues.push_back(max - float(scalingfactor * range));
									else 
										newvalues.push_back(min + float(scalingfactor * range));
								}
							}
						}
						m_ActualSimplex=Vector(newvalues);
					}
					else {
						if (actualobjectivefunction < m_NelderMead_BestScore) {
							SetAccepted(true);
							m_NelderMead_BestScore = actualobjectivefunction;
						}
						else
							SetAccepted(false);
						m_ActualSimplex = m_NelderMead.step(m_ActualSimplex, -actualobjectivefunction);
					}

				}
			}				
		}
	}
	else {
		//When all have runs have been made
		MR_Init_BayesCocor(Num_BayesPar);
		MR_Init_BayesCovar(Num_BayesPar);
	}
	return;
}
float MR::MR_Get_BayesMean(size_t indexp)
{
	if(m_PBayesMean.size()>indexp)
		return m_PBayesMean[indexp];
	else
		return 0.;

}
float MR::MR_Get_BayesCoVar(size_t indexp, size_t indexp2)
{
	auto ind=indexp*Num_BayesPar+indexp2;
	if(m_PBayesCovar.size()>ind) 
		return m_PBayesCovar[ind];
	else
		return 0.;

}
float MR::MR_Get_BayesCoCor(size_t indexp, size_t indexp2)
{
	auto ind=indexp*Num_BayesPar+indexp2;
/*	if(m_PBayesCocor.size()>ind) 
		return m_PBayesCocor[ind];
	else*/
		return m_MStorage.GetCoCorValid(indexp,indexp2);
}
void MR::MR_Set_BayesMean(size_t indexp, float v)
{
	if(indexp<m_PBayesMean.size())
		m_PBayesMean[indexp]=v;
	else 
		m_PBayesMean.push_back(v);


}
void MR::MR_Set_BayesCoVar(size_t indexp, size_t indexp2, float v)
{
	auto ind=indexp*Num_BayesPar+indexp2;
	if(ind<m_PBayesCovar.size()) {
		m_PBayesCovar[ind]=v;
	}
}
void MR::MR_Set_BayesCoCor(size_t indexp, size_t indexp2, float v)
{
	auto ind=indexp*Num_BayesPar+indexp2;
	if(ind<m_PBayesCocor.size()) {
		m_PBayesCocor[ind]=v;
	}
}
void MR::MR_RESET_Bayesian()
{
//	BAYESIANCALIB_mp_CALIB_RESET();

}
void MR::MR_CheckCalibMethod() {
	size_t icount=0;
	m_IsMakingNelderMeadCalib = false;
	for(size_t i=0;i<MR_GetNumberOfDimensions();i++){
			for(size_t ii=0; ii<MR_Get_NumberOfParametersWithinDim(i+1);ii++){
				if(MR_GetMethod(i+1,ii+1)==9||MR_GetMethod(i+1,ii+1)==11) {
					icount++;
				}
				if (MR_GetMethod(i + 1, ii + 1) == NELDER_MEAD)  m_IsMakingNelderMeadCalib = true;
			}
	}

	if(icount==0) 
		m_IsMakingBayesianCalib=false;
	else
		m_IsMakingBayesianCalib=true;

	SetNum_BayesPar(icount);
	SetNum_BayesChain(MR_Get_TotalNumberofRuns());

}
void MR::MR_INIT_Bayesian_Nelder_Mead()
{  
	SetMC_Step(m_BayesianStepSize);
	MR_CheckCalibMethod();

	
	SetNum_BayesChain(MR_Get_TotalNumberofRuns());

	m_ActualNumberNelderMeadSteps = 0;
	m_NelderMead_BestScore = 1.E38;

	size_t icount=0;
	size_t icount_NelderMead=0;
	SimB* pPar;
	size_t nTabIndex;


	vector<float> startNelderMeadvalues, maxNelder, MinNelder;
	for(size_t i=0;i<MR_GetNumberOfDimensions();i++){
			for(size_t ii=0; ii<MR_Get_NumberOfParametersWithinDim(i+1);ii++){
				pPar = MR_Get_pPar(i + 1, ii + 1);
				nTabIndex = MR_GetTabIndex(i + 1, ii + 1);


				if(MR_GetMethod(i+1,ii+1)==BAYESIAN_CAL) {
					SetStart_PValues(icount, (MR_GetStart(i+1,ii+1)-MR_GetMin(i + 1, ii + 1)) /(MR_GetMax(i+1,ii+1)-MR_GetMin(i+1,ii+1)));
					icount++;
				}
				else if(MR_GetMethod(i+1,ii+1)==BAYESIAN_CAL_LOG) {
					double max, min, rel;
					max=abs(MR_GetMax(i+1,ii+1));
					min=abs(MR_GetMin(i+1, ii+1));
					if(max>min)
						rel=abs(MR_GetStart(i+1,ii+1)- MR_GetMin(i + 1, ii + 1))/(max-min);
					else
						rel=0.5;
					SetStart_PValues(icount,rel);
					icount++;
				}
				else if (MR_GetMethod(i + 1, ii + 1) == NELDER_MEAD) {
					icount_NelderMead++;
					auto max = MR_GetMax(i + 1, ii + 1);
					auto min = MR_GetMin(i + 1, ii + 1);
					auto start = MR_GetStart(i + 1, ii + 1);
					auto act_start = start;
					if (start < min || start > max) act_start=(max + min) / 2.;
					startNelderMeadvalues.push_back(act_start);
					
				
					if (nTabIndex == string::npos) {
						auto pP = dynamic_cast<Ps*>(pPar);
						pP->SetValue(act_start);
						pP->Apply();
					}
					else {
						auto pCPt = dynamic_cast<P*>(pPar);
						pCPt->SetValue(nTabIndex, act_start);
						pCPt->Apply(nTabIndex);
					}
				
				}
			}

	}
	
	Set_BayesInitCalib(true);
	//BAYESIANCALIB_mp_CALIB_INI();
	if(MR_UseBayesianCoVar) {
// Apply the covariance matrice as estimated from previous run
		Num_BayesPar=MR_Get_NumBayesPar()-icount_NelderMead;

	}
	else {
		Num_BayesPar=MR_Get_NumBayesPar() - icount_NelderMead;
		if(m_PBayesMean.size()>0) m_PBayesMean.clear();
		if(m_PBayesCovar.size()>0) m_PBayesCovar.clear();
		m_PBayesMean.resize(Num_BayesPar);
		m_PBayesCovar.resize(Num_BayesPar*Num_BayesPar);
		for(size_t i=0; i<Num_BayesPar;i++) {
			m_PBayesMean[i]=0.;
			for(size_t ii=0;ii<Num_BayesPar;ii++) {
				size_t ind;
				ind=Num_BayesPar*i+ii;
				m_PBayesCovar[ind]=0.;
			}
		}
	}
	if (icount_NelderMead > 0) {
		m_NelderMead.ResetNelderMead();

		m_NelderMead.SetNumDimensions(icount_NelderMead);	
		m_ActualSimplex = Vector(startNelderMeadvalues);
		auto pSw = GetPtrByName("Nelder-Mead Objective");
		m_NelderMeadObjective = ((Sw*)pSw)->GetIntValue();
	
	}

}
bool MR::UpdateCoVar()
{
/*	size_t icount=0;

	for(size_t i=0;i<MR_GetNumberOfDimensions();i++){
		for(size_t ii=0; ii<MR_Get_NumberOfParametersWithinDim(i+1);ii++){
			if(MR_GetMethod(i+1,ii+1)==9||MR_GetMethod(i+1,ii+1)==11||
				MR_GetMethod(i+1,ii+1)==3||MR_GetMethod(i+1,ii+1)==4) {
				icount++;
			}
		}
	}*/

#ifndef COUPSTD
		CMainFrame *p_mainframe;
		p_mainframe =MySingle::instance().GetFramePointer() ;
		string txt;
		txt="Updating CoVariance Matrix Started:";
		p_mainframe->UpdatePaneText(0,txt);

#endif

	if(!m_DocFile.m_MultiRunning) return false;

	size_t numpar=MR_Get_NumTotalPar();
	size_t numall=numpar+m_ValidationData.GetNumberSelectedTSV_Val()*6;
	size_t numaccept=m_MStorage.GetNumAccepted();
	//BAYESIANCALIB_mp_CALIB_RESET();
	//BAYESIANCALIB_mp_CALIB_DEF();


	if(m_PBayesMean.size()>0) m_PBayesMean.clear();
	if(m_PBayesCovar.size()>0) m_PBayesCovar.clear();
	m_PBayesMean.resize(numall);
	m_PBayesCovar.resize(numall*numall);
	m_PValues.resize(numall*numaccept);

	size_t naccepted=m_MStorage.GetNumAccepted();

	for(size_t ii=0;ii<naccepted;ii++) {
				size_t irun=m_MStorage.Get_AcceptedRuns(ii);
				for(size_t i=0;i<numpar;i++) {
					//BAYESIANCALIB_mp_PCHAINVALUES[i]=m_MStorage.GetMultiP(irun,i);
					m_PValues[i*naccepted+ii]=m_MStorage.GetMultiP(irun,i);
				}
				
				for(size_t i=0;i<m_ValidationData.GetNumberSelectedTSV_Val()*6;i++) {
				//	BAYESIANCALIB_mp_PCHAINVALUES[i+numpar]=m_MStorage.GetValid(irun,i);
					m_PValues[i*naccepted+naccepted*numpar+ii]=m_MStorage.GetValid(irun,i);
				}
				size_t index=ii+1;
			//	BAYESIANCALIB_mp_SETPCHAINVALUES(&index);
#ifndef COUPSTD
				if(ii==naccepted/2) {
					txt="50 % of values loaded File";
					p_mainframe->UpdatePaneText(0,txt);

				}
#endif
	}


#ifndef COUPSTD
		txt="Values loaded from File to estimated covariance:";
		p_mainframe->UpdatePaneText(0,txt);
#endif

	if(m_MStorage.GetNumAccepted()>3) {
		//BAYESIANCALIB_mp_NUMBURNPERIOD=0;
		//BAYESIANCALIB_mp_COVARIANCEUPDATE();

		m_MStorage.SetCoCorValid_Init(numall);
	

		float v;
		for(size_t i=0;i<numall;i++){
				float vvv=Stats::Moments::mean(numaccept,&m_PValues[i*numaccept]);
				MR_Set_BayesMean(i, vvv);

				for(size_t ii=0;ii<numall;ii++){
						//icount++;
						//BAYESIANCALIB_mp_INDEXP=i+1;
						//BAYESIANCALIB_mp_INDEXP2=ii+1;
						//float v=BAYESIANCALIB_mp_GETCOCOR();
						if(i==ii) {

							v=Stats::Moments::variance(numaccept,&m_PValues[i*numaccept],false);
						}
						else {

							v=Stats::Moments::correlation(numaccept,&m_PValues[i*numaccept],&m_PValues[ii*numaccept]);

						}
						
						m_MStorage.SetCoCorValid(i,ii,v);
					//	MR_Set_BayesCoCor(i,ii,v);
				}
		}

	}
#ifndef COUPSTD
	txt="Updating CoVariance Matrix Completed:";
	p_mainframe->UpdatePaneText(0,txt);
#endif
	m_MStorage.SetCoParUpdated(true);
return true;

}
void MR::MR_Init_BayesCovar(size_t ncount)
{

	if(m_PBayesMean.size()>0) m_PBayesMean.clear();
	if(m_PBayesCovar.size()>0) m_PBayesCovar.clear();
	m_PBayesMean.resize(ncount);
	m_PBayesCovar.resize(ncount*ncount);

		for (size_t i=0; i<ncount;i++) {
			m_PBayesMean[i]=0.;
			for(size_t ii=0;ii<ncount;ii++) {
				size_t ind;
				ind=ncount*i+ii;
				m_PBayesCovar[ind]=0.;
			}
		}
	
}
void MR::MR_Init_BayesCocor(size_t ncount)
{
	if(m_PBayesCocor.size()>0) m_PBayesCocor.clear();
	m_PBayesCocor.resize(ncount*ncount);

		for (size_t i=0; i<ncount;i++) {
			for(size_t ii=0;ii<ncount;ii++) {
				size_t ind;
				ind=ncount*i+ii;
				m_PBayesCocor[ind]=0.;
			}
		}
}
bool MR::MR_SetDimCounter(size_t index, size_t count)
{
	if(m_MultiRun_Array.size()>index){
		m_MultiRun_Array[index].nCount=count;
		return true;
	}
	return false;
}
bool MR::MR_IsEmpty()
{
	if(m_MultiRun_Array.size()>0) 
		return false;
	else
		return true;
}
size_t MR::MR_ReadData() {
 // This is for reading of MR parameters immediately prior to starting MRs.

  size_t status = 0;
  return status;
}
void MR::MR_Reset()
{
	if(m_MultiRun_Array.size()>0) 
		m_MultiRun_Array.clear();
}
void MR::MR_ResetAll()
{
	if(m_MultiRun_Array.size()>0) 
		m_MultiRun_Array.clear();
	m_P_MR_Array.clear();
}
void MR::AcceptAll()
{
	if(m_MStorage.IsOpen())
		m_MStorage.UpdateAcceptedRuns(true, true);

}
size_t MR::MR_GetNumberOfDimensions()
{
   return m_MultiRun_Array.size();
}
bool MR::MR_CP_Set(SimB *pPar, size_t nTabIndex, bool database)
{
	for(size_t i=0;i<m_P_MR_Array.size();i++){
		if(pPar==m_P_MR_Array[i].pBase&&nTabIndex==m_P_MR_Array[i].TabIndex) return false;
	}
	SIMB val;
	val.pBase=pPar;
	val.TabIndex=nTabIndex;

	m_P_MR_Array.push_back(val);
	return true;
}
bool MR::SetParametersTableForMultiRun(vector<SIMB> simv, FCSV *pCSVFile)
{			//TABLE Values to include
	vector<double> values, min_values, max_values;
	size_t Dimension = MR_GetNumberOfDimensions();
	min_values = pCSVFile->GetMinVector();
	max_values = pCSVFile->GetMaxVector();
	vector<size_t> keys;
	keys = pCSVFile->GetKeyVector();
	for (size_t i = 0; i < simv.size(); i++) {
		if (simv[i].pBase->Is_DB()) {
			dynamic_cast<CDB*>(simv[i].pBase)->MR_SetSelection(0, "Soil", "le");
		}
		else if (simv[i].pBase->Is_PTable()) {
			dynamic_cast<P*>(simv[i].pBase)->MR_Set_Min(simv[i].TabIndex, min_values[i]);
			dynamic_cast<P*>(simv[i].pBase)->MR_Set_Max(simv[i].TabIndex, max_values[i]);
			dynamic_cast<P*>(simv[i].pBase)->MR_Set_Start(simv[i].TabIndex, (min_values[i] + max_values[i]) / 2.);

			dynamic_cast<P*>(simv[i].pBase)->MR_Set_Method(simv[i].TabIndex, TABLE_VAL);
			dynamic_cast<P*>(simv[i].pBase)->MR_Set_Dim(simv[i].TabIndex, Dimension);
			MR_CP_Set(simv[i].pBase, simv[i].TabIndex);
		}
		else {
			dynamic_cast<Ps*>(simv[i].pBase)->MR_Set_Min(min_values[i]);
			dynamic_cast<Ps*>(simv[i].pBase)->MR_Set_Max(max_values[i]);
			dynamic_cast<Ps*>(simv[i].pBase)->MR_Set_Start((min_values[i] + max_values[i]) / 2.);
			dynamic_cast<Ps*>(simv[i].pBase)->MR_Set_Method(TABLE_VAL);
			dynamic_cast<Ps*>(simv[i].pBase)->MR_Set_Dim(Dimension);
			MR_CP_Set(simv[i].pBase, string::npos);

		}
		dynamic_cast<MR*>(this)->MR_SetRepeatitionsOfDimension(Dimension, keys.size());
		if (simv[i].pBase->Is_DB()) {
			vector<string> cdb_Keyvalues;
			cdb_Keyvalues.resize(2);
			for (size_t ii = 0; ii < keys.size(); ii++) {

				cdb_Keyvalues = pCSVFile->GetCDB_Value(keys[ii]);
				dynamic_cast<CDB*>(simv[i].pBase)->MR_SetSelection(ii, dynamic_cast<CDB*>(simv[i].pBase)->GetName(), cdb_Keyvalues[1]);
			}

		}
		else {
			if (simv[i].pBase->Is_PTable())	dynamic_cast<P*>(simv[i].pBase)->MR_Reset_Table(simv[i].TabIndex);
			else dynamic_cast<Ps*>(simv[i].pBase)->MR_Reset_Table();

			for (size_t ii = 0; ii < keys.size(); ii++) {

				values = pCSVFile->GetParValue(keys[ii]);
				if (simv[i].pBase->Is_PTable()) {
					dynamic_cast<P*>(simv[i].pBase)->MR_Append_TableValue(simv[i].TabIndex, values[i + 1]);
				}
				else {
					dynamic_cast<Ps*>(simv[i].pBase)->MR_Append_TableValue(values[i + 1]);
				}
			}
		}
	}
	if (values.size() > 0) return true;
	else return false;






	return false;
}
void MR::MR_CP_Reset(SimB *pPar)
{
	for(size_t i=0;i<m_P_MR_Array.size();i++){
		if(pPar==(Ps*)m_P_MR_Array[i].pBase) {
			m_P_MR_Array.erase(m_P_MR_Array.begin()+i);
		}
	}
	return;
}
void MR::MR_CP_Reset(SimB *pPar, size_t nTabIndex)
{
	for(size_t i=0;i<m_P_MR_Array.size();i++){
		if(pPar==(Ps*)m_P_MR_Array[i].pBase&&nTabIndex==m_P_MR_Array[i].TabIndex) {
			m_P_MR_Array.erase(m_P_MR_Array.begin()+i);
		}
	}
	return;
}
void MR::MR_ResetDimCounter()
{
	for(size_t i=0;i<m_MultiRun_Array.size();i++) 
		m_MultiRun_Array[i].nCount=1;
	m_ActualDimension = 0;

}

size_t MR::MR_Get_TotalNumberofAddedRuns(size_t index)
{	
	size_t count;

	if(index==0) {
		if(m_MultiRunsOriginal==0) {
			count=1;
			for(size_t i=0;i<MR_GetNumberOfDimensions();i++)
				count*=MR_GetNumberOfRepeationsForThisDimension(i);
			m_MultiRunsOriginal=count;
		}

		return m_MultiRunsOriginal;
	}
	else {
		count=m_MultiRunsOriginal;
		for(size_t i=0;i<index;i++) 
			count+=m_MStorage.GetNumberOfAddedRuns(i);
		return count;
	}

}
bool MR::MR_Set_TotalNumberofRuns(size_t NewNumber)
{	
	m_MultiRun_TotalNumber=1;
	for(size_t i=0;i<MR_GetNumberOfDimensions();i++)
		m_MultiRun_TotalNumber*=MR_GetNumberOfRepeationsForThisDimension(i);

	if(m_MultiRun_TotalNumber>NewNumber&&MR_GetNumberOfDimensions()==1) {
		MR_SetRepeatitionsOfDimension(0, NewNumber);
		return true;
	}
	return false;
}

void MR::MR_SetSimTimeUsed()
{
	//m_TimeOneRun=GENERAL_mp_IHOUR*3600+GENERAL_mp_IMINUT*60+GENERAL_mp_ISEC;
}
size_t MR::MR_GetSimTimeLeft(size_t n)
{
		return n*m_TimeOneRun;	
}
void MR::Set_MultiStopped()
{
		m_MultiStopped=true;
}
bool MR::IsMultiStopped()
{
		return m_MultiStopped;
}
void MR::SetMBinFileName(string value)
{

	m_DocFile2.m_Multi_MBin_File=value;
}
void MR::MR_Storage_Init()
{
	m_PreviousMultiMBinFile=m_DocFile2.m_Multi_MBin_File;
	string	pzPathName=GetCurrentSimFile();
	size_t ipos_sim;
	if (m_xmlFileToUse) {
		ipos_sim = pzPathName.find(".xml");


	}
	else {
		ipos_sim = pzPathName.find(".Sim");
		if (ipos_sim < 0)
			ipos_sim = pzPathName.find(".SI");
		if (ipos_sim < 0)
			ipos_sim = pzPathName.find(".S");
		if (ipos_sim < 0)
			ipos_sim = pzPathName.find(".s");
		if (ipos_sim < 0)
	 			ipos_sim = pzPathName.size();
	}
	m_DocFile2.m_Multi_MBin_File=pzPathName.substr(0,ipos_sim)+".MBin";
	auto ncountval=m_ValidationData.GetNumberSelectedTSV_Val();
	m_ValidationData.SetNumberSelectedTSV_Val();
	m_MStorage.Init(this);
	m_MStorage.InitData(m_DocFile2.m_Multi_MBin_File.c_str(), MR_Get_TotalNumberofRuns(),MR_Get_NumTotalPar(),
		ncountval, m_ValidationData.GetNumSumVarVariables(), GetNumOutPuts(), this) ;
	m_MStorage.SaveBase();
	m_MStorage.Close();
}
void MR::MR_Storage_Add()
{
	size_t irep, icount,ii, nTabIndex,indexp;
	size_t irec=MR_Get_ActualRunNumber()-1;
	Ps* pPar;
	P* pCPt;
	float Float, Ratio,min,max;
	size_t ParType;
	
		//MultiParameter Values
	indexp=0;
	for(size_t i=0;i<MR_GetNumberOfDimensions();i++){
		irep=m_MultiRun_Array[i].NumberOfRepeatitionsWithinDimension;
		icount=MR_GetDimCounter(i);
		//PARAMETERS_mp_DIMCOUNTERS[i]=icount;
		for(ii=0; ii<MR_Get_NumberOfParametersWithinDim(i+1);ii++){
				pPar=(Ps*)MR_Get_pPar(i+1,ii+1);
				string namep=string(pPar->GetName());
				nTabIndex=MR_GetTabIndex(i+1,ii+1);
				ParType=MR_GetParType(i+1,ii+1);
				min=MR_GetMin(i+1,ii+1); // This variable does not always have realistic values.
				max=MR_GetMax(i+1,ii+1); // " "
				switch (MR_GetMethod(i+1,ii+1)) {
				case 0: Ratio=(float)(icount-1)/(float)(irep-1);// linear
						Float=Ratio*max+(1-Ratio)*min;
						break;
				case 1: //Logarithmic
					Ratio=(float)(icount-1)/(float)(irep-1);	
						Float=exp(Ratio*log(max)+(1-Ratio)*log(min));
						break;
				case 2:
				case 3:
				case 6:
				case 7:
				case 8:
				case 9:
				case 11:
				case 12:

						if(ParType==0) {
							Float=pPar->GetValue();
						}
							else {
								pCPt=(P*)pPar;
								Float=pCPt->GetValue(nTabIndex);
						}
						break; 
                case 4: // Table  
					    if (nTabIndex == -1) // Table, Note that ii is not incremented during MRs!!
					      Float = ((Ps*)pPar)->MR_Get_TabValue(icount - 1);  // Ps case
						else 
						  Float = ((P*)pPar)->MR_Get_TabValue(nTabIndex, icount - 1);  // Ps case
				       	break;
						// 	Float=MR_GetTableValue(i+1,ii+1,icount-1);				 	    break;
				case 5: // Database Records
					if (pPar != nullptr) {
						string strname = ((CDB*)pPar)->GetName();
						string str = ((CDB*)pPar)->MR_GetKeySelection(icount-1);
						size_t nump = 1, numr = 1;
						auto pos = str.find(":");
						if (pos != string::npos) {
							nump = FUtil::AtoInt(str.substr(0, pos));
							str = str.substr(pos + 1);
							numr = FUtil::AtoInt(str);
						}
						Float = float(nump * 100 + numr);
					}
					else
						Float = -100.;
					break;
				case 10:

						if(ParType==0) {
							Float=pPar->GetValue();
						}
							else {
								pCPt=(P*)pPar;
								Float=pCPt->GetValue(nTabIndex);
						}
						break;

				};
			//Set Value		
			m_MStorage.AddMultiP(irec,indexp, Float);
			indexp++;
		}
	}

	if(!IsRunAccepted()&&IsBayesianCalibration())
		Float=0;
	else
		Float=1;
	
	m_MStorage.AddMultiP(irec,indexp, Float);
	
	indexp++;
	m_MStorage.AddMultiP(irec,indexp, m_DocFile.m_TsVLogLi);
	indexp++;
	m_MStorage.AddMultiP(irec,indexp, m_DocFile.m_TsVLogLiSum);
	indexp++;
	m_MStorage.AddMultiP(irec,indexp, m_DocFile.m_TsVLogLi+m_DocFile.m_TsVLogLiSum);
	indexp++;
	m_MStorage.AddMultiP(irec,indexp, GetBestRun());
	indexp++; //+6
	m_MStorage.AddMultiP(irec,indexp, m_DocFile.m_AccSimTime);
		indexp++; //+7
	m_MStorage.AddMultiP(irec,indexp, float(m_InformCriteria.n_points));
		indexp++; //+8

	if (irec%m_InformCriteria.n_aggregateNumber == m_InformCriteria.n_aggregateNumber - 1 && m_InformCriteria.n_aggregateNumber > 1) {
		for (size_t rec = irec - m_InformCriteria.n_aggregateNumber+1; rec < irec; rec++) {
			m_MStorage.AddMultiP(rec, indexp, float(m_InformCriteria.SSE));
			m_MStorage.AddMultiP(rec, indexp+1, float(m_InformCriteria.AIC));
			m_MStorage.AddMultiP(rec, indexp+2, float(m_InformCriteria.BIC));
		}
	}
	m_MStorage.AddMultiP(irec,indexp, float(m_InformCriteria.SSE));
		indexp++; //+9
	m_MStorage.AddMultiP(irec, indexp, float(m_InformCriteria.AIC));	
		indexp++; //+10
	m_MStorage.AddMultiP(irec,indexp, float(m_InformCriteria.BIC));


// Set value of Bayesian Results




	// Validation Value
	indexp=0;

	for (size_t i6 = 0; i6 < 6; ++i6) {
		for (size_t i = 0; i < m_ValidationData.GetNumberSelectedTSV_Val(); i++) {
			if (i6 == 0) m_MStorage.AddValid(irec, indexp, m_ValidationData.GetValR2(i));
			else if (i6 == 1)	m_MStorage.AddValid(irec, indexp, m_ValidationData.GetValA0(i));
			else if (i6 == 2)	m_MStorage.AddValid(irec, indexp, m_ValidationData.GetValA1(i));
			else if (i6 == 3)	m_MStorage.AddValid(irec, indexp, m_ValidationData.GetValME(i));
			else if (i6 == 4)  m_MStorage.AddValid(irec, indexp, m_ValidationData.GetValRMSE(i));
			else if (i6 == 5)	m_MStorage.AddValid(irec, indexp, float(m_ValidationData.GetValLogLi(i)));
			indexp++;
		}
	}



	
	for(size_t i=0;i<m_ValidationData.GetNumSumVarVariables();i++) {
		indexp=i*3;
		m_MStorage.AddValidSum(irec,indexp+0, m_ValidationData.GetValSumVarME(i));
		m_MStorage.AddValidSum(irec,indexp+1, m_ValidationData.GetValSumVarRMSE(i));
		m_MStorage.AddValidSum(irec,indexp+2, float(m_ValidationData.GetValSumVarLogLi(i)));
	}
	SimB* Sw;
	indexp=0;
	size_t indpg=0;
	size_t  lin;
	//vector<OUTv> vv;
	vector<SIMB> pvv;
	pvv=GetAllSelectedOutputs();

		for(size_t i=0;i<pvv.size();i++) {

		  Sw = (SimB*)pvv[i].pBase;	
		  OutSingle *pSingle;
		  OutVector *pVector;
		  pVector=dynamic_cast<OutVector*>(pvv[i].pBase);
		  if(pVector!=nullptr){
				lin=pvv[i].TabIndex;
				m_MStorage.AddSumV(irec,indexp ,pVector->GetInitial(lin)); indexp++;
		     	m_MStorage.AddSumV(irec,indexp,pVector->GetFinal(lin)); indexp++;
		     	m_MStorage.AddSumV(irec,indexp,pVector->GetMin(lin)); indexp++;
		     	m_MStorage.AddSumV(irec,indexp,pVector->GetMax(lin)); indexp++;
		     	m_MStorage.AddSumV(irec,indexp,pVector->GetMean(lin)); indexp++;
		     	m_MStorage.AddSumV(irec,indexp,pVector->GetAccumulated(lin)); indexp++;
		  }
		  else {
			pSingle=dynamic_cast<OutSingle*>(pvv[i].pBase);
			if(pSingle!=nullptr) {
			//indpg=(pSingle->GetPgFileIndex()-1)*6;
				//if(indpg>=0) {
					m_MStorage.AddSumV(irec,indexp,pSingle->GetInitial()); indexp++;
		     		m_MStorage.AddSumV(irec,indexp,pSingle->GetFinal()); indexp++;
		     		m_MStorage.AddSumV(irec,indexp,pSingle->GetMin()); indexp++;
		     		m_MStorage.AddSumV(irec,indexp,pSingle->GetMax()); indexp++;
		     		m_MStorage.AddSumV(irec,indexp,pSingle->GetMean()); indexp++;
		     		m_MStorage.AddSumV(irec,indexp,pSingle->GetAccumulated()); indexp++;
				//}
			}
		  }
	
		//}
	}
	//	m_MStorage.SaveRecord(irec);

   
}
void MR::MR_Storage_Close()
{
		
		m_MStorage.SaveBase();
		m_MStorage.Close();
}
bool MR::MR_Storage_InitEnsembles() {

	m_MStorage.EvaluateFile(false);
	return m_MStorage.CompleteProcessingOfInputFiles_MultiStorage();
}
bool MR::MR_Storage_InitEvaluateStat() {

	Ps* pPar = GetPsPointer("Aggregate Calib Number");
	

	if (m_IsMakingBayesianCalib) {
	
		if (pPar != nullptr) {
			size_t n = size_t(pPar->GetValue() + 0.001);
			m_MStorage.ReAssignPerformanceandAcceptanceFromAggregation(n);
		}

		return m_MStorage.UpdateAcceptedRuns(false, false);

	}
	else {

		if (pPar != nullptr) {
			size_t n = size_t(pPar->GetValue() + 0.001);
			m_MStorage.CommonPerformanceFromAggregation(n);
		}
		return m_MStorage.UpdateAcceptedRuns(true, false);

		
	}
	
}
size_t MR::MR_GetValidIndex_IndexInValFile_InDim1()
{
	SimB *pPar;
	size_t count = 0;
	for (int i = 0; i < MR_GetNumberOfDimensions(); i++) {
		for (int ii = 0; ii < MR_Get_NumberOfParametersWithinDim(i + 1); ii++) {
			pPar = MR_Get_pPar(i + 1, ii + 1);
			string namep = pPar->GetName();
			if ((namep == "Index for selection of File (first val file)")||(namep == "Index in val file")&&i==0) {
				return count;
			}
			count++;
		}
	}
	return string::npos;
}
bool MR::MR_Storage_Reset()
{

	return m_MStorage.Reset();
}
void MR::MR_Reset_MeanResiduals()
{
	for (size_t ifile = 0; ifile < m_ResidualFileVector.size(); ifile++)
	for(size_t i=0; i<=m_MStorage.GetNumberOfAddedFiles(m_MStorage.GetNameBaseFile());i++) {
		m_ResidualFileVector[ifile][i]->m_IsUpdated=false;
	}
}
bool MR::MR_Storage_OpenPrevious()
{
		return m_MStorage.OpenFile(m_PreviousMultiMBinFile.c_str(), false);
		return true;
}
bool MR::MR_Storage_Open()
{
	if(!IsMultiSimulation()) return false;
	if(!m_MStorage.IsOpen()&&m_MultiStorageBinFileExist){
		if(!m_MStorage.OpenFile(m_DocFile2.m_Multi_MBin_File.c_str())){
			SetMultiStorageBinFileMissing();
			return false;
		}
		m_DocFile2.m_Multi_MBin_File=m_MStorage.GetNameBaseFile();
	}
	return true;

}
bool MR::MR_Storage_IsOpen()
{
	return m_MStorage.IsOpen();
}
void MR::MR_SetBayesianStepSize(float value)
{
	m_BayesianStepSize=value;

}
float MR::MR_GetBayesianStepSize()
{
	return m_BayesianStepSize;
}

void MR::GetProfileFromPlotPF(string test)
{
			bool Testkey;
			Testkey=true;
			size_t layer,prof,repNumber,index;
			repNumber = 1;
			prof = 1;
			index=test.find(":");
			if(index>0) {
				if(index>2)
					prof=FUtil::AtoInt(test.substr(index-3,3));
				else if(index>1)
					prof=FUtil::AtoInt(test.substr(index-2,2));
				else
					prof = FUtil::AtoInt(test.substr(index - 1, 1));
				repNumber=FUtil::AtoInt(test.substr(index+1));
			}
			else
				Testkey=false;
			layer=1;
			if (!p_PFCurve->IsOpen()) {
				p_PFCurve->ReOpenLevelFile();
				p_PFCurve->OpenEditMainDataBase();

			}
			//size_t newkey = p_PFCurve->GetKey(prof, repNumber);
			//size_t rec = p_PFCurve->GetHeaderRecord(newkey);
			//NEWHEADER head=p_PFCurve->GetHeader(rec);
			p_PFCurve->SetHeader(prof, repNumber);

			GetProfileFromPlotPF( false, false);

	
			return;
}
void MR::GetProfileFromPlotPF( bool Add_To_Multi, bool Message)
{

// if(PLOTPF_mp_FINDPROFILE&&PLOTPF_mp_TOBEDOWNLOADED) {
//	PLOTPF_mp_RETRIEVESOILPROFILE();


	string Name, Group, Type;
	P *pClayContent, *pSandContent, *pProf, *pRepProf,*pProfLayer,
		*pSaturation, *pTotalCond, *pMatCond, *pLambda, *pAirEntry,
		*pWilt, *pResidual, *pUDep, *pLDep, *pLBound, *pUBound,
		*p_nVal, *pGA, *pGN, *pGM,*p_nSR, *p_nSE;

	P *pUValid,*pLValid,*pLinR2,*pLinN;
	Sw *pPedo, *pEquation;

		Tab* pCTab;
		pCTab=(Tab*)GetPtr(TABLE,"Hydraulic conductivity, measured horizons");
		pCTab->SetNumOfRow(p_PFCurve->GetNumLayers());
				


	Name="ClayContent";
	Group="Soil Hydraulic";
	Type="Table Parameters";
    pClayContent=(P*)GetPtr(Type,Group, Name);
	Name="SandContent";
    pSandContent=(P*)GetPtr(Type,Group,Name);

	Name="Upper Valid";
	pUValid=(P*)GetPtr(Type,Group,Name);

	Name="Lower Valid";
	pLValid=(P*)GetPtr(Type,Group,Name);

	Name="Regline R2";
	pLinR2=(P*)GetPtr(Type,Group,Name);

	Name="Regline N";
	pLinN=(P*)GetPtr(Type,Group,Name);

	Name="Pedo Functions";
	Type="Switches";
	pPedo=(Sw*)GetPtr(Type,Group,Name);
	auto PedoFunction=pPedo->GetIntValue();


	Name="Hydraulic Functions";
	
	pEquation=(Sw*)GetPtr(Type,Group,Name);
	auto Equation=pEquation->GetIntValue();
	Type="Table Parameters";

	Name="ProfileLayer";
    pProfLayer=(P*)GetPtr(Type,Group,Name);
	Name="Upper Depth";
    pUDep=(P*)GetPtr(PAR_TABLE,Name);
	Name="Lower Depth";
    pLDep=(P*)GetPtr(PAR_TABLE,Name);
	Name="Upper Boundary";
	pUBound=(P*)GetPtr(Type,Group,Name);
	Name="Macro Pore";
    pLBound=(P*)GetPtr(Type,Group,Name);
	Name="n Tortuosity";
    p_nVal=(P*)GetPtr(Type,Group,Name);
	Name="n Power (SatRel)";
	p_nSR=(P*)GetPtr(Type,Group,Name);
	Name="n Power (SatEffective)";
	p_nSE=(P*)GetPtr(Type,Group,Name);
	Name="Alpha";
    pGA=(P*)GetPtr(Type,Group,Name);
	Name="n-value";
    pGN=(P*)GetPtr(Type,Group,Name);
	Name="m-value";
	pGM=(P*)GetPtr(Type,Group,Name);

	Name="Saturation";
    pSaturation=(P*)GetPtr(Type,Group,Name);
	Name="Total Conductivity";
    pTotalCond=(P*)GetPtr(Type,Group,Name);
	Name="Matrix Conductivity";
    pMatCond=(P*)GetPtr(Type,Group,Name);

	Name="Lambda";
    pLambda=(P*)GetPtr(Type,Group,Name);
	Name="Air Entry";
    pAirEntry=(P*)GetPtr(Type,Group,Name);
	Name="Wilting Point";
    pWilt=(P*)GetPtr(Type,Group,Name);
	Name="Residual Water";
    pResidual=(P*)GetPtr(Type,Group,Name);

	Name = "ProfileNo";
	pProf = (P*)GetPtr(Type, Group, Name);
	Name = "ProfileRepNo";
	pRepProf = (P*)GetPtr(Type, Group, Name);


	float EstimateMin,EstimateMax, Residual,Porosity, Wilt, R2;
	size_t icount;
	vector<double> Theta, Psi;
	//float Theta[25],Psi[25];
	
	auto nl=pWilt->GetSize();


	vector<float> pressure_head, theta;
	pressure_head=p_PFCurve->GetPF_Pressure(p_PFCurve->GetRecLayers());
	

	for(size_t i=0;i<p_PFCurve->GetNumLayers();i++) {
		pProf->		SetValue(i,	   float(p_PFCurve->GetKey()/100)	);
		pRepProf->	SetValue(i,	   float(p_PFCurve->GetKey()%100) 	);
		pProfLayer->SetValue(i,	   float(i+1)	);

		PFCOEF coef=p_PFCurve->GetPF_Coef(i);

		pWilt->		SetValue(i,	coef.Wilting);

 		pUDep->		SetValue(i,	coef.UpperDepth );
		pLDep->		SetValue(i, coef.LowerDepth	);
		icount=0;
		if(PedoFunction>2) {
			EstimateMin=pLValid->GetValue(i);
			EstimateMax=pUValid->GetValue(i);


			//PLOTPF_mp_ACTUALLEVEL=i+1;
			//PLOTPF_mp_RETRIEVEPF_POINTS();
			Theta.resize(25); Psi.resize(25);
			
			for(size_t jj=0;jj<pressure_head.size();jj++) {
				theta=p_PFCurve->GetPF_Theta(jj);
				if(pressure_head[jj]>=EstimateMin&&
					pressure_head[jj]<=EstimateMax&&
					theta[jj]>0.001) {
						Theta[icount]=theta[jj];
						Psi[icount]=pressure_head[jj];
						if(Psi[icount]>14900.&&Psi[icount]<15100.)
							Wilt=Theta[icount];
						icount++;
				}
			}
			Theta.resize(icount); Psi.resize(icount);
			Residual=0.;
	
			Porosity=coef.Saturation; //  PLOTPF_mp_APORO[i];

			if(icount>1) {
				Residual=0.;
				BC_ESTIMATE out=p_PFCurve->Estimate_BC(&Theta, &Psi, Porosity, Residual);
//			size_t jjj=HYDRAULIC_mp_ESTIMATEBC(&Theta, &Psi, &icount, &Porosity, &Residual);
				coef.Lambda=out.Lambda;
				coef.AirEntry=out.AirEntry;
				coef.Residual=out.Residual;
				//PLOTPF_mp_ALAMDA[i]=HYDRAULIC_mp_LAMBDA_EST;
				//PLOTPF_mp_APSIE[i]=HYDRAULIC_mp_AIRENTRY_EST;
				//PLOTPF_mp_ATETRE[i]=HYDRAULIC_mp_RESIDUAL_EST;
				R2=out.R2;
				pLinR2->SetValue(i, R2);	

				coef.Gen_Alfa=1/out.AirEntry;
				coef.Gen_N=1+out.Lambda;
				coef.Gen_M=1-1./coef.Gen_N;
				//PLOTPF_mp_BGA[i]=float(1.)/PLOTPF_mp_APSIE[i];
				//PLOTPF_mp_BGN[i]=float(1.)+PLOTPF_mp_ALAMDA[i];
			//	PLOTPF_mp_BGM[i]=float(1.)-1/PLOTPF_mp_BGN[i];
			}
			else
				pLinR2->SetValue(i, 0.);
		}

		pLinN->SetValue(i,float(icount));
		pLambda->	SetValue(i, coef.Lambda); //   PLOTPF_mp_ALAMDA[i]	);
		pAirEntry->	SetValue(i, coef.AirEntry);//   PLOTPF_mp_APSIE[i] 	);
		pResidual->	SetValue(i,  coef.Residual); // PLOTPF_mp_ATETRE[i]	);
		pSaturation->SetValue(i,  coef.Saturation);//  PLOTPF_mp_APORO[i] 	);

		pLBound->	SetValue(i, coef.LowerBoundary);//   PLOTPF_mp_BLB[i]   	);
		pUBound->	SetValue(i, coef.UpperBoundary);///   PLOTPF_mp_BUB[i]   	);
		pMatCond->	SetValue(i, coef.MatConductivity);//   PLOTPF_mp_BSATC[i]);

		pTotalCond->SetValue(i, coef.TotConductivity);//   PLOTPF_mp_BBSAT[i]);

		p_nVal->	SetValue(i, coef.Mualem_n);//   PLOTPF_mp_BNVAR[i] 	);
		p_nSR->		SetValue(i, coef.N_SrCoef);  // PLOTPF_mp_BNSR[i] 	);
		p_nSE->		SetValue(i, coef.N_SECoef);//   PLOTPF_mp_BNSE[i] 	);
		if(coef.Gen_Alfa<-1.E37) coef.Gen_Alfa=1/coef.AirEntry;
		pGA->		SetValue(i, coef.Gen_Alfa);//   PLOTPF_mp_BGA[i]   	);
		if(coef.Gen_N<-1.E37) coef.Gen_N=1+coef.Lambda;
		pGN->		SetValue(i, coef.Gen_N);//   PLOTPF_mp_BGN[i]   	);
		if(coef.Gen_M<-1.E37) coef.Gen_M=1-1/coef.Gen_N;
		pGM->		SetValue(i, coef.Gen_M);//   PLOTPF_mp_BGM[i]   	);

	}

	
	float value;
	if (p_PFCurve->GetNumLayers() < nl) {
		for (size_t i = p_PFCurve->GetNumLayers(); i < nl; i++) {
			pWilt->SetValue(i, pWilt->GetValue(i - 1));
			pUDep->SetValue(i, pLDep->GetValue(i - 1));
			pLDep->SetValue(i, pLDep->GetValue(i - 1) + float(0.1));
			value = pLambda->GetValue(i - 1);
			value = pLambda->GetValue(i);
			pLambda->SetValue(i, pLambda->GetValue(i - 1));
			value = pLambda->GetValue(i);
			pAirEntry->SetValue(i, pAirEntry->GetValue(i - 1));
			pSaturation->SetValue(i, pSaturation->GetValue(i - 1));
			pResidual->SetValue(i, pResidual->GetValue(i - 1));
			pMatCond->SetValue(i, pMatCond->GetValue(i - 1));
			pTotalCond->SetValue(i, pTotalCond->GetValue(i - 1));
			p_nVal->SetValue(i, p_nVal->GetValue(i - 1));
			p_nSR->SetValue(i, p_nSR->GetValue(i - 1));
			p_nSE->SetValue(i, p_nSE->GetValue(i - 1));
			pLinR2->SetValue(i, pLinR2->GetValue(i - 1));
			pLinN->SetValue(i, pLinN->GetValue(i - 1));
			pClayContent->SetValue(i, pClayContent->GetValue(i - 1));
			pSandContent->SetValue(i, pSandContent->GetValue(i - 1));
			pGA->SetValue(i, pGA->GetValue(i - 1));
			pGN->SetValue(i, pGN->GetValue(i - 1));
			pGM->SetValue(i, pGM->GetValue(i - 1));

			pLBound->SetValue(i, pLBound->GetValue(i - 1));
			pUBound->SetValue(i, pUBound->GetValue(i - 1));
		}
		pWilt->Apply(nl);
		pUDep->Apply(nl);
		pLDep->Apply(nl);
		pLambda->Apply(nl);
		pAirEntry->Apply(nl);
		pSaturation->Apply(nl);
		pResidual->Apply(nl);
		pLBound->Apply(nl);
		pUBound->Apply(nl);
		pMatCond->Apply(nl);
		pTotalCond->Apply(nl);
		p_nVal->Apply(nl);
		p_nSR->Apply(nl);
		p_nSE->Apply(nl);
		pGA->Apply(nl);
		pGN->Apply(nl);
		pGM->Apply(nl);
		pSandContent->Apply(nl);
		pClayContent->Apply(nl);
		pLinN->Apply(nl);
		pLinR2->Apply(nl);
		pProf->Apply(nl);
		pRepProf->Apply(nl);
	}

	

	if(Message) {
		string string="Profile :";
		string+=FUtil::STD_ItoAscii(p_PFCurve->GetKey() /100);
		string+=" :"; string+=FUtil::STD_ItoAscii(p_PFCurve->GetKey()%100);
		string+= p_PFCurve->GetKey();
		string+="\nfrom county : ";
		string+= p_PFCurve->GetCountry();
	
		string+="\nincluding ";
		string+=FUtil::STD_ItoAscii(p_PFCurve->GetNumLayers());
		string+= " layers has now been linked with your document";
		string+="\n Use \"Edit - Parameter Tables - Soil hydraulics\" menu to view and/or modify the selected properties"; 
	//MessageBox(nullptr,"Model Running - Not possible to open Documents","File Open Error",MB_OK|MB_ICONEXCLAMATION);

	#ifndef COUPSTD
		if(!MultiRunChange) MFC_Util::MessageBox(string,"Soil Profile downloaded from databas",MB_OK|MB_ICONEXCLAMATION);
	#endif	
	}

	ReCalculateNewSoilProfile();


	if (Add_To_Multi) ModelUtil::SetMRSoilPropConnections(dynamic_cast<Doc*>(this));


}	 



void MR::LockMultiStorageFile()
{
	m_MultiStoragelocked=true;
	m_MStorage.m_Lock=true;
}
void MR::UnLockMultiStorageFile()
{
	m_MultiStoragelocked=false;
	m_MStorage.m_Lock=false;
}
size_t MR::MR_GetAddedFiles(size_t RunNo)
{
	size_t files=1;
	while(RunNo>=MR_Get_TotalNumberofAddedRuns(files-1)) {
		files++;
	}
	return files-1;
}
size_t MR::MR_GetInternalRunNo(size_t RunNo)
{
	size_t files=1;
	while(RunNo>=MR_Get_TotalNumberofAddedRuns(files-1)) {
		files++;
	}
	if(files>1)
		return RunNo-MR_Get_TotalNumberofAddedRuns(files-2);
	else
		return RunNo;

}
void MR::Set_ReReadMStorageFile()
{
	m_ReReadMStorageFile=true;
}
void MR::MR_ReCalculatePostDist(bool BayesMeanMethod)
{
	size_t naccepted=m_MStorage.GetNumAccepted();
	if(naccepted==0) return;
	m_AcceptedParDist.resize(MR_Get_NumNotTablePar());
	for(size_t i=0;i<MR_Get_NumNotTablePar();i++) {
		auto ipar = MR_GetNoTableParIndex(i);
		vector<float> values;
		values.resize(size_t(m_MStorage.GetNumberOfRuns()));
		size_t icount=0;
		for(size_t ii=0;ii<naccepted;ii++) {
				size_t irun=m_MStorage.Get_AcceptedRuns(ii);
				if(irun>GetNumBurnIn()&&BayesMeanMethod) {
					if(icount==0) icount=GetNumBurnIn();
					size_t irun_next;
					if(ii>=naccepted-1)
						irun_next=m_MStorage.GetNumberOfRuns();
					else
						irun_next=m_MStorage.Get_AcceptedRuns(ii+1);
					auto fval=m_MStorage.GetMultiP(irun,ipar);
					while(icount<irun_next) {
						values[icount-GetNumBurnIn()]=fval;
						icount++;
					}
				}
				else if(!BayesMeanMethod) {
					double fval=m_MStorage.GetMultiP(irun,ipar);
					values[icount]=fval;
					icount++;
				}
		}
		if(GetNumBurnIn()>=icount&&BayesMeanMethod)  {
			m_AcceptedParDist[i]=m_DistDefault;

		}
		else {
			if(BayesMeanMethod) icount-=GetNumBurnIn();
			StatUtil::SortArray(&values[0],icount );
			size_t ncheck;
			DistStat stat;
			MeanStat(&values[0], icount, &ncheck, &stat);
			m_AcceptedParDist[i]=stat;
		}
	}
			
// Store the result in m_Valid_TSV.ParDist
	m_MStorage.UpdateParDist(m_AcceptedParDist);

}

bool MR::LinkedChangeToSwitches(Sw *pSw1, size_t option) {
	Func* pFunc;
	Sw *pTarget1;Sw *pSw2,*pSw3, *pSw4;
	SimB *pSimB1;
	switch (option) {
	case 4:
			pSimB1=GetPtr(FUNCTION,"ReCalc Soil Thermal Function");
			pFunc=dynamic_cast<Func*>(pSimB1);
			if(pFunc!=nullptr)
				return pFunc->MakeOperation();


		break;


	case 100:	// WaterEq
	case 101:   // HeatEq
		if(option==101) pSw1=(Sw*)GetPtr(SWITCH,"WaterEq");

		pSw2=(Sw*)GetPtr(SWITCH,"HeatEq");
		pSw3=(Sw*)GetPtr(SWITCH,"Growth");
		pSw4=(Sw*)GetPtr(SWITCH,"Nitrogen and Carbon");
		(Sw*)GetPtr(SWITCH,"RadiationInput");
		if(pSw3->GetIntValue()<=2&&pSw1->GetIntValue()==0&&pSw2->GetIntValue()==0) {
			GetSw("TAirGlobRad")->SetIntValue(0);
			GetSw("RadiationInput")->SetIntValue(0);
		}
		else if(pSw4->GetIntValue()>0) {
			GetSw("TAirGlobRad")->SetIntValue(1);
			GetSw("RadiationInput")->SetIntValue(1);
		}
		if(pSw4->GetIntValue()>0&&pSw1->GetIntValue()==0&&pSw2->GetIntValue()==0) 
			GetSw("OnlyNC")->SetIntValue(1);
		else
			GetSw("OnlyNC")->SetIntValue(0);

		if(pSw4->GetIntValue()>0&&pSw1->GetIntValue()==0&&pSw2->GetIntValue()==0&&pSw3->GetIntValue()>2)
			GetSw("LaiInput")->SetIntValue(2);
		

		break;

		case 102: //Evaporation
			if(pSw1->GetIntValue()>1) {
				pTarget1=(Sw*)GetPtr(SWITCH,"RadiationInput");
				if(pTarget1!=nullptr)
					pTarget1->SetIntValue(1);
			}
			else if(pSw1->GetIntValue()==0) {
				pTarget1=(Sw*)GetPtr(SWITCH,"PlantType");
				if(pTarget1!=nullptr)
					pTarget1->SetIntValue(0);
			}
			else if(pSw1->GetIntValue()==1) {
				pTarget1=(Sw*)GetPtr(SWITCH,"PlantType");
				if(pTarget1!=nullptr)
					pTarget1->SetIntValue(1);
			}
			return true;
			break;
		case 103: // Plant Type
		case 200: // Driving Variable Switch
			pSw2=GetSw("LaiInput");
			pSw3=GetSw("AlbedoVeg");
			if(pSw1->GetIntValue()==1)
				//GetSw("Evaporation")->SetIntValue(1);

			if(pSw1->GetIntValue()>=2)
				GetSw("RadiationInput")->SetIntValue(1);
			
			if((pSw2->GetIntValue()==0&&pSw1->GetIntValue()<3)||(pSw3->GetIntValue()==1&&pSw1->GetIntValue()<3))
				GetSw("AboveTable")->SetIntValue(1);
			else 
				GetSw("AboveTable")->SetIntValue(0);
			if(GetSw("PrecInput")->GetIntValue()>0||				
				GetSwValue("TempAirInput")>0||
				GetSwValue("RadInLongInput")>0||
				(GetSwValue("HumRelInput")>0&&GetSwValue("Evaporation")>1)||
				(GetSwValue("WSpeedInput")>0&&GetSwValue("Evaporation")>1)||
				(GetSwValue("RadNetInput")>0&&GetSwValue("Evaporation")>1)||
				(GetSwValue("RadGlobInput")>0&&GetSwValue("TAirGlobRad")>1)||
				(GetSwValue("CloudInput")>0&&GetSwValue("TAirGlobRad")>1)||
				(GetSwValue("DBSunInput")>0&&GetSwValue("TAirGlobRad")>1)||
				(GetSwValue("PotTranspInput")>0&&pSw1->GetIntValue()>1)||
				(GetSwValue("TempSurfInput")>0&&GetSwValue("HeatEq")>0)
			)
				SetSwValue("TypeOfDrivingFile",1);
			else
				SetSwValue("TypeOfDrivingFile",0);
			if(GetSwValue("LaiInput")==1||GetSwValue("RootInput")==1||GetSwValue("CanopyHeightInput")==1||GetSwValue("LaiInput")==3)
				SetSwValue("TypeOfDrivingFile",2);
			if((GetSwValue("LaiInput")==0&&GetSwValue("PlantType")<3)||(GetSwValue("AlbedoVeg")==1&&GetSwValue("PlantType")<3))
				SetSwValue("AboveTable",1);
			else
				SetSwValue("AboveTable",0);

			if(GetSwValue("TempAirInput")==0&&GetSwValue("OnlyNC")==1)
				SetSwValue("TempCycleInput",1);
			else if(GetSwValue("OnlyNC")==1)
				SetSwValue("TempCycleInput",0);

			return true;
			break;

		case 201: // Evaporation Method (SoilEvap)
			if(GetSwValue("Evaporation Method")==4&&GetSwValue("Surface Temperature")==2)
				SetSwValue("Surface Temperature",1);
			else if(GetSwValue("Evaporation Method")==3)
				SetSwValue("Surface Temperature",2);
			if((GetSwValue("Surface Temperature")==2&&GetSwValue("Evaporation Method")==2)||
				(GetSwValue("Surface Temperature")==2&&GetSwValue("Evaporation Method")==1))
				SetSwValue("Surface Temperature",1);
			
			return true;
			break;
		case 202: // Canopy Shade
			if(pSw1->GetIntValue()==1)
				SetSwValue("SplitGlobal",1);
			return true;
			break;
		case 203: // HBV_Soil
			if(pSw1->GetIntValue()==2)
				SetSwValue("WaterEq",2);
			else if(GetSwValue("WaterEq")==2) 
				SetSwValue("WaterEq",1);			return true;
			break;
		case 204: // DriveDrainLevel
			if(pSw1->GetIntValue()==1)
				SetSwValue("DrainFileUse",1);
			else
				SetSwValue("DrainFileUse",0);
			return true;
			break;
		case 205: // Lower Bound Saturated
			if(pSw1->GetIntValue()==4)
				SetSwValue("DrainFileUse",1);

			return true;
			break;
		case 206: // Driving Variable Switch Canopy Height

			if(GetSwValue("CanopyHeightInput")==2) SetSwValue("Roughness",2);
			break;
		case 104: // Water Uptake Switch

			if(GetSwValue("Salt influence")>0||GetSwValue("PlantResistance")>0||GetSwValue("Salinity stress")>0)
				SetSwValue("SaltCoefValid",1);
			else
				SetSwValue("SaltCoefValid",0);
			if(GetSwValue("Basic equation")==0)
				SetSwValue("PlantWaterStorage",0);
			return true;
			break;
		case 108: // SnowPack Switch
			if(pSw1->GetIntValue()>0&&GetSwValue("HeatEq")==1)
				SetSwValue("RadiationInput",1);
			return true;
			break;
		case 109: // Nitrogen and Carbon Switch
			if(GetSwValue("Nitrogen and Carbon")==2) {
				SetSwValue("WaterEq",0);
				SetSwValue("HeatEq",0);
				SetSwValue("SnowPack",0);
				SetSwValue("Evaporation",0);
				SetSwValue("GroundWaterFlow",0);
				if(GetSwValue("Growth")>2) {
					SetSwValue("TAirGlobRad",1);
					SetSwValue("RadiationInput",1);
				}
				else if(GetSwValue("WaterEq")==0&&GetSwValue("HeatEq")==0) {
					SetSwValue("TAirGlobRad",0);
					SetSwValue("RadiationInput",0);
				}
			}
			else if(GetSwValue("Nitrogen and Carbon")==1) {
				SetSwValue("WaterEq",1);
				SetSwValue("HeatEq",1);
			}
			if(pSw1->GetIntValue()>0&&pSw1->GetName()!="Nitrogen and Carbon") {
				if(GetSwValue("SoilTempInput")==2||
				   GetSwValue("SoilInfilInput")==2||
				   GetSwValue("SoilDrainageInput")==3||
				   GetSwValue("SoilWaterInput")==2||
				   GetSwValue("SoilWaterFlowInput")==2||
				   GetSwValue("WaterStressInput")==2||
				   GetSwValue("DeepPercolationInput")==3)
						SetSwValue("Nitrogen and Carbon", 1);
			}
			if(GetSwValue("SoilTempInput")==1||
				   GetSwValue("SoilInfilInput")==1||
				   GetSwValue("SoilDrainageInput")==1||
				   GetSwValue("SoilWaterInput")==1||
				   GetSwValue("SoilWaterFlowInput")==1||
				   GetSwValue("WaterStressInput")==1)
					SetSwValue("TypeOfNDrivingFile",1);
			else
				SetSwValue("TypeOfNDrivingFile",0);

			if(GetSwValue("WaterEq")==0&&GetSwValue("HeatEq")==0&&GetSwValue("Nitrogen and Carbon")>0)
				SetSwValue("OnlyNC", 1);
			else
				SetSwValue("OnlyNC",0);

			return true;
			break;
		case 110: // Lateral Input
			if(pSw1->GetIntValue()==2) {
				SetSwValue("Lateral GW Source",1);
	        /*I=SetFlagsT('Salt Tracer'C,'SaltDrainFlow'C,'1-100'C, 1)
	        I=SetFlagsT('Drainage and deep percolation'C,'DeepPerc'C,'1'C, 1)
	        I=SetFlagsT('Surface Water'C,'SpoolRunoff'C,'1'C, 1)   */


			}
			else if (pSw1->GetIntValue()==1) {
				SetSwValue("Lateral GW Source",0);
	        /*I=SetFlagsT('Salt Tracer'C,'SaltDrainFlow'C,'1-100'C, 0)
	        I=SetFlagsT('Drainage and deep percolation'C,'DeepPerc'C,'1'C, 0)
	        I=SetFlagsT('Surface Water'C,'SpoolRunoff'C,'1'C, 0)   */



			}
		/*	if(pSw1->GetIntValue()==2) {
				SetSwValue("Dripper",1);
				SetSwValue("Irrigation",1);


			}*/
			return true;
			break;
		case 111: // RSMethod
			if(pSw1->GetIntValue()==3)
				SetSwValue("PlantDevelopment",1);
			return true;
			break;
		case 114: // Dripper Switch
			if(pSw1->GetIntValue()>0)
				SetSwValue("EnableSaltSource",1);
			else if(GetSwValue("Salt GW Source")==1)
				SetSwValue("EnableSaltSource",1);
			else
				SetSwValue("EnableSaltSource",0);
			return true;
			break;
		case 116: // Road Salt Application
			SetSwValue("EnableInputPGSalt",0);
			if(pSw1->GetIntValue()==1)
				SetSwValue("EnableInputPGSalt",1);
			else if(GetSwValue("SaltDeposition")>0)
				SetSwValue("EnableInputPGSalt",1);
			return true;
			break;
		case 117: // N Fertilization Switch
			SetSwValue("EnableInputN",0);
			if(pSw1->GetIntValue()==2||GetSwValue("Manure Input")==1)
				SetSwValue("EnableInputN",1);
			return true;
			break;
		case 701: // StartTable for growing season
			if(GetSwValue("Sowing")==3||GetSwValue("Emergence")==3)
				SetSwValue("StartSeasonTableExist",1);
			else
				SetSwValue("StartSeasonTableExist",0);
			break;

		case 3101: // Trace Gas Emissions
			if(pSw1->GetIntValue()>0)
				SetSwValue("Denitrification", 2);
			else
				SetSwValue("Denitrification", 1);
			return true;
			break;
		case 3102: // 
			if(pSw1->GetIntValue()>1&&GetSwValue("Trace Gas Emissions")==0)
				SetSwValue("Trace Gas Emissions", 1);
			else if(pSw1->GetIntValue()==0&&GetSwValue("Trace Gas Emissions")>0)
				SetSwValue("Trace Gas Emissions", 0);
			return true;
			break;
		case 12000:
			PAR_OPTIONS k;
			k.pSimB = nullptr;
			k.options = option;
			m_OptionsForReading.push_back(k);
			break;


	};
	return false;


}
bool MR::LinkedChangeToDB_Parameters(Par *pSimB, size_t option) {
	
	
	if(option>10000&&option<12000) {

		Get_Mascur_Soil();




	}
	else if (option== 12000) {




	}
	else {
		Get_IGBP_Soil();



	}
	return true;
}


