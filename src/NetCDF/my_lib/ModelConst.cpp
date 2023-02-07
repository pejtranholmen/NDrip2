#include "ModelConst.h"
ModelConst::ModelConst()
{

    PI=3.1415927;
	RAD=0.01745329;	
	SecPerDay =86400. ;//   ! Converstion for heat flux


	JDayNum = 0.;
	DayLength = 0.;
	JYearNum =1;
}
ModelConst::~ModelConst()
{

}