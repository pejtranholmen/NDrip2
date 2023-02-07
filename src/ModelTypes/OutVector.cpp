//#include "../stdafx.h"
#include "../std.h"
#include "../Util/FUtil.hpp"
#include "OutVector.h"
#include "NE.h"




OutVector::OutVector(enum datatype data, enum elements elem, enum fysprocess fproc, enum bioprocess bproc, enum userlevel ulev)
	:OutBase(data,elem, fproc, bproc, ulev) 
{


}
OutVector::~OutVector(void)
{
}
bool OutVector::SetOptInitialValue(size_t index, float initial)
{
	if(SumList.size()>index){		
		SumList[index].OptInitial=initial;
		return true;
	}
	return false;

}
double OutVector::GetOptInitialValue(size_t index) 
{
	if(SumList.size()>index){

		return double(SumList[index].OptInitial);
	}
	return MISSING;

}
size_t OutVector::GetSumVarIndex(size_t index, size_t col) const
{
	if(SumList.size()>index)
		return SumList[index].PgFileIndex*6+col- FUtil::GetPositionSumInitialFromOutSheet(); 
	return string::npos;
}
size_t OutVector::GetPgFileIndex(size_t index) const
{


	if(SumList.size()>index){
		if(SumList[index].StoreFlag==1)
			return SumList[index].PgFileIndex;
		else if(SumList[index].StoreFlag==2)
			return SumList[index].PgMultiFileIndex;
	}
	return string::npos;
}
size_t OutVector::GetPgSingleFileIndex(size_t index) const
{
	if(SumList.size()>index)
			return SumList[index].PgFileIndex;

	return string::npos;
}
size_t OutVector::GetPgMultiFileIndex(size_t index) const
{


	if(SumList.size()>index){
		return SumList[index].PgMultiFileIndex;
	}
	return string::npos;
}
bool OutVector::SetPgFileIndex(size_t index, size_t value)
{

	if(SumList.size()>index){
		SumList[index].PgFileIndex=value;
		return true;
	}
	return false;
}
float OutVector::GetInitial(size_t index) const
{ 

	if(SumList.size()>index)
		return SumList[index].Initial;

	return float(MISSING);
}
float OutVector::GetFinal(size_t index) const
{
	if(SumList.size()>index)
		return SumList[index].Final;
	return float(MISSING);
   
}
float OutVector::GetMin(size_t index) const
{
	if(SumList.size()>index)
		return SumList[index].Min;

	return (float)MISSING;
}
float OutVector::GetMax(size_t index) const
{
	if(SumList.size()>index)
		return SumList[index].Max;
	return float(MISSING);
}
float OutVector::GetMean(size_t index) const
{
	if(SumList.size()>index)
		return SumList[index].Mean;
	return float(MISSING);
}
float OutVector::GetAccumulated(size_t index) const
{
	if(SumList.size()>index)
		return SumList[index].Accumulated;
	return float(MISSING);
}
bool OutVector::ApplyInitialValue(size_t index)
{

	if(SumList.size()>index){

		SetValue(index,double(SumList[index].OptInitial));
		return true;
	}
	return false;
}
void OutVector::CleanSumVarList()
{
	SumList.clear();
}
bool OutVector::SetValidFlagIndex(size_t index, size_t value)
{ 

	if(SumList.size()>index){
		SumList[index].ValidFlagIndex=value;

		return true;
	}
	return false;
}
size_t OutVector::GetLocalIndex(size_t index) const
{ 
	if(SumList.size()>index)
	{
		return SumList[index].LocalIndex;
	}
	return string::npos;
}
vector<size_t> OutVector::GetAllSelectedLocalIndex() {
	vector<size_t> outselected;
	for (size_t ii = 0; ii < GetNumberOfFlags(); ii++) {
		if (GetStoreFlag(ii) > 0) {		
			outselected.push_back(ii+1);
		}
	}
	return outselected;
}
size_t OutVector::GetSelectedLocalIndex(size_t index, bool multi) const
{ 
	int icount=0;
	int selected;
	for(size_t ii=0;ii<GetNumberOfFlags();ii++){
			selected=GetStoreFlag(ii);
			if(selected>0) {			 
					if((multi&&selected>1)||(!multi&&selected>0)) {
						if(index==icount)
							return ii;
						icount++;
					}
			}
	}
	return string::npos;

}
size_t OutVector::GetValidFlagIndex(size_t index) const
{ 

	if(SumList.size()>index)
		return SumList[index].ValidFlagIndex;
	return string::npos;
}
int OutVector::GetStoreFlag(size_t index) const
{ 

	if(SumList.size()>index)
		return SumList[index].StoreFlag;
	return -1;
}
bool OutVector::AddSumVar(size_t index, float initial, float final,float  min,float  max, float mean,float accumulated, int pgindex, int pgMultiIndex, int StoreFlag)
{
	if(SumList.size()>index) {
		SumList[index].LocalIndex=index;
		SumList[index].Initial=initial;
		SumList[index].Final=final;
		SumList[index].Min=min;
		SumList[index].Max=max;
		SumList[index].Mean=mean;
		SumList[index].Accumulated=accumulated;
		SumList[index].PgFileIndex=pgindex;
		SumList[index].PgMultiFileIndex=pgMultiIndex;
		SumList[index].ValidFlagIndex=0;
		if(StoreFlag>0)
			SumList[index].StoreFlag=StoreFlag;
		return true;
	}
	return false;
}
bool OutVector::AddSumVar(size_t index, double initial, double final,double  min,double  max, double mean,double accumulated, int pgindex, int pgMultiIndex, int StoreFlag)
{
	if(SumList.size()>index) {
		SumList[index].LocalIndex=index;
		SumList[index].Initial=float(initial);
		SumList[index].Final=float(final);
		SumList[index].Min=float(min);
		SumList[index].Max=float(max);
		SumList[index].Mean=float(mean);
		SumList[index].Accumulated=float(accumulated);
		SumList[index].PgFileIndex=pgindex;
		SumList[index].PgMultiFileIndex=pgMultiIndex;
		SumList[index].ValidFlagIndex=0;
		if(StoreFlag>0)
			SumList[index].StoreFlag=StoreFlag;
		return true;
	}
	return false;
}
bool OutVector::AddOutPutVar(size_t index, OUTPUT out) {

	if (SumList.size() > index) {
		SumList[index] = out;
		return true;
	}
	return false;

}

//Checks whether all flags are set or not
bool OutVector::AreAllFlagsSet()
{
	if(GetNumberOfSelectedFlags()==GetNumberOfFlags())
		return true;
	else
		return false;
}


size_t OutVector::GetNumberOfFlags() const
{
	//return numFlags;
	return size_t(pNE->GetNEValue());
}



//Find the number of selected flags
size_t OutVector::GetNumberOfSelectedFlags(bool multi) 
{
	int number=0;
	int selected;
	//Loop through all the flags and if the flag is set increase number
	for (size_t i=0;i<GetNumberOfFlags();i++)
	{
		selected=GetStoreFlag(i);
		if ((multi&&selected==2)||(!multi&&selected>0))
			number++;
	}
	return number;

}
bool OutVector::IsFlagsSet(bool multi) {
	if(GetNumberOfSelectedFlags(multi)>0) 
		return true; 
	else 
		return false;
};
bool OutVector::SetFlags(string flagString, int value)
{
	size_t citationPlace,commaPlace,minusPlace;
	size_t numFlags;
	vector <int>  flags;
	size_t flagPosition=0;
	int maxPos;
	string workingString;
//	DataTrans converter;
	//Get rid of annoying spaces 
	flagString.erase(flagString.find(' '),flagString.find_first_not_of(' ')); 
	flagString.erase(flagString.find_last_not_of(' ')+1,flagString.rfind(' ')); 
//	flagString.TrimLeft();
//	flagString.TrimRight();
	//Observe that the first " has already been removed in the calling function
	citationPlace=flagString.find('"');
	if(citationPlace==0) {
		flagString=flagString.substr(1,flagString.size()-1);
		citationPlace=flagString.find('"');
	}
	//Get the number of flags
	numFlags=FUtil::AtoInt(flagString.substr(0,citationPlace));
	//Get rid of the number of flags
	flagString=flagString.substr(citationPlace+1);
	//Get rid of annoying spaces 
		flagString.erase(flagString.find(' '),flagString.find_first_not_of(' ')); 
	//flagString.TrimLeft();
	//Read and store the values for the flags.
	//Observe that if there are more flag values in the string than there are flags then the last flags in the string are discarded.
		//Zero all the flags in the beginning
	flags.resize(GetNumberOfFlags());
	for (size_t k=0;k<GetNumberOfFlags();k++)
				flags[k]=0;

		//Remove the first citationmark
	flagString=flagString.substr(1);
	while ((flagPosition<numFlags)&&(flagPosition<GetNumberOfFlags()))
	{
			//If the rest of the flagstring is empty then exit the while loop
		if ((flagString=="\"\"")||(flagString=="\"")||(flagString.size()==0))
				break;
			//Find the firts comma and minus sign
		commaPlace=flagString.find(',');
		minusPlace=flagString.find('-');
			
			//If there is only pone number left in the string
		if ((minusPlace==-1)&&(commaPlace==-1))
		{
				//Get the position
			flagPosition=FUtil::AtoInt(flagString.substr(0,flagString.size()-1));
				//Store the new value
			if (flagPosition<=flags.size())
				flags[flagPosition-1]=value;

				//End the while loop
			break;
		}
			//If there is no more - in flagString
		if (minusPlace==-1)
		{
				//Get next position
			flagPosition=FUtil::AtoInt(flagString.substr(0,commaPlace));
				//Remove the read position and the comma from the string
			flagString=flagString.substr(commaPlace+1);
				//Store the new value
			if (flagPosition<=flags.size())
					flags[flagPosition-1]=value;
			//Continue the while loop
			continue;
		}
			
		//If there is no more , in flagString
		if (commaPlace==-1)
		{
			//If the - comes first
			if (minusPlace!=0)
			{
				//Get the first position in the serie
				flagPosition=FUtil::AtoInt(flagString.substr(0,minusPlace));
				//Remove the read position but leave the - sign in the string
				flagString=flagString.substr(minusPlace);
				//Store the new value
				if (flagPosition<=flags.size())
						flags[flagPosition-1]=value;
			}
			else
			{
				//Increase flagposition and store the current flag
				//Observe that there are no commas in the string
				flagPosition++;
				//Retrieve the maximum value that is to be incremented into
				workingString=flagString.substr(1);
				minusPlace=workingString.find('-');
				if (minusPlace==-1)
				{
					maxPos=FUtil::AtoInt(workingString.substr(0,workingString.size()-1));
					//If the maximum number has been reached then clear the string
					if (flagPosition==maxPos)
						flagString="";
				}
				else
				{
					maxPos=FUtil::AtoInt(workingString.substr(0,minusPlace));
					//If the maximum number has been reached then remove this part
					if (flagPosition==maxPos)
						flagString=flagString.substr(minusPlace+1);
				}
				//Store the new value
				if (flagPosition<=flags.size())
						flags[flagPosition-1]=value;

			}
			//Continue the while loop
			continue;
		}

		//If the , comes before - in flagString
		if (commaPlace<minusPlace)
		{
			//Get next position
			flagPosition=FUtil::AtoInt(flagString.substr(0,commaPlace));
			//Remove the read position and the comma from the string
			flagString=flagString.substr(commaPlace+1);
			//Store the new value
			if (flagPosition<=flags.size())
					flags[flagPosition-1]=value;

			//Continue the while loop
			continue;
		}
			//else the minus comes before the comma
		if (commaPlace>minusPlace)
		{
			//If the minus doesnt come first
			if (minusPlace!=0)
			{
				//Get the first position in the serie
				flagPosition=FUtil::AtoInt(flagString.substr(0,minusPlace));
				//Remove the read position but leave the - sign in the string
				flagString=flagString.substr(minusPlace);
				//Store the new value

				if (flagPosition<=flags.size())
					flags[flagPosition-1]=value;

			}
			else
				//else the minus position comes first
			{
					//Check whether there is a minus before the comma and in that case set commaPlace to minusPlace
				workingString=flagString.substr(1);
				minusPlace=workingString.find('-');
				if (minusPlace!=-1)
				{
					if (minusPlace<commaPlace)
						commaPlace=minusPlace;
				}
					//Remove right part to right of the comma including the comma and store it in workingString
				workingString=flagString.substr(0,commaPlace);
				//Remove the minus sign from workingString and store the value in maxpos
				workingString=workingString.substr(1);
				maxPos=FUtil::AtoInt(workingString);
				//Increase flagPosition
				flagPosition++;
				//Store the new value
				if (flagPosition<=flags.size())
					flags[flagPosition-1]=value;
					//If flagposition is equal to maxpos then remove everything from the string until the next comma´including the next comma
				if (flagPosition==maxPos)
					flagString=flagString.substr(commaPlace+1);
			}
				//Continue the while loop
			continue;
		}
	}
		//Set the flags
 	for (size_t i=0;i<flags.size();i++) {
			if(flags[i]==value&&value>=1) SetStoreFlag(i,flags[i]);
			if(flags[i]==value&&value>=2) SetMultiStoreFlag(i,true);
	}
	//Apply();
	//Delete all dynamic values

	flags.clear();
	return true;	// No option on return implemented when changed to X
}

