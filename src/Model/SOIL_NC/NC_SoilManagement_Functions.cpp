#include "NC_SoilManagement_Functions.h"
#include "../PLANT_NC/PlantOrgModel.h"
#include "../SOIL/SoilModel.h"
#include "../SOIL/Soil_Frost.h"
#include "../PLANT/PlantModel.h"


NC_SoilManagement_Functions::NC_SoilManagement_Functions(void)
{


}
NC_SoilManagement_Functions::~NC_SoilManagement_Functions(void)
{
}


bool NC_SoilManagement_Functions::Def()
{
	NC_GasFlux::Def();
	ModuleNo=30;
	Sw* pSw; Ps *pPs;	
	pSw= new Sw(&SurfaceCultivation ,  SIMPROFILE, ORGANIC, TRANSPORT,SOILMIXING, NORMAL);

	Define(pSw, "Surface cultivation",0,"off|on","Growth > 0");
	pSw= new Sw(&Ploughing,  SIMPROFILE, ORGANIC, TRANSPORT,SOILMIXING, NORMAL);
	Define(pSw ,"Deep ploughing",0,"off|on","Growth > 0");
	pPs= new Ps(&SurfaceCultDay ,  SIMPROFILE, ORGANIC, TRANSPORT,SOILMIXING, NORMAL);
   
	Define(pPs,	"SurfaceCultDay", 0.,"#","Surface cultivation >0",0.,365.);
//!	Call P(Group_30, SurfaceCultDepth,	"SurfaceCultDepth", -0.1,"m","Surface cultivation >0",-.2,0.);
	pPs= new Ps(&PloughingDay ,  SIMPROFILE, ORGANIC, TRANSPORT,SOILMIXING, NORMAL);
	Define(pPs,	"PloughingDay", 0.,"#","Deep ploughing >0",0.,365.);
	pPs= new Ps(&PloughingDepth ,  SIMPROFILE, ORGANIC, TRANSPORT,SOILMIXING, NORMAL);
	Define(pPs,	"PloughingDepth", -0.3,"m","Deep ploughing >0",-.7,0.);

return true;
}

bool NC_SoilManagement_Functions::MixFrac(vector<double> *Substrate, size_t Num) {
	//Use Profile
//	Integer*4 Num
	SoilModel* p_Soil=(SoilModel*)m_pModelInfo->GetModelPtr("Soil Model");
	Soil_Frost* pSoil=&p_Soil->m_SoilFrost;


	double Sum1, Remain, Frac;
	double MassCheck, Prior;

	Sum1=0;
	Prior=sum(*Substrate);
	
	if(Num>1) 
		Frac=1.-(pSoil->DepthLayers[Num-1]+PloughingDepth)/(pSoil->DepthLayers[Num-1]-pSoil->DepthLayers[Num-2]);
	else
		Frac=-PloughingDepth/pSoil->DepthLayers.front();
	
	for( size_t i=0; i<Num-1; i++) 
	   Sum1+=Substrate->at(i);
	Sum1+=Frac*Substrate->at(Num-1);
	Remain=(1-Frac)*Substrate->at(Num-1);
	
	for( size_t i=0; i<Num; i++) 
		Substrate->at(i)=DistFrac[i]*Sum1;

	Substrate->at(Num-1)+=Remain;
	MassCheck=sum(*Substrate)-Prior;
	
	if(abs(MassCheck)>0.001) 
	    return false;    
	else
		return true;
}
