#include "./SimB.h"
#include "../Util/FUtil.hpp"
#include "../NewBase/ModelCompNames.hpp"
#include "../NewBase/NewMap.h"
#include "./Sw.h"


SimB::SimB()
{														 
	m_IsNotOriginal=false;
	_enabled=false;
	_DBenabled = true;
	_stype=FUNCTION;
	_DBIntValue = 0;
	m_ReadOnly = false;
	pfMap = nullptr;
	_orgIntValue = 0;
	_DBIntValue = 0;
	_def.data = datatype::SINGLE;
	_def.elem = elements::WATER;
	_def.fproc = fysprocess::STORAGE;
	_def.bproc = bioprocess::NOBPROC;
	_def.ulev = userlevel::NORMAL;
	_ModuleNo = string::npos;

}
SimB::SimB(simtype d_vt, datatype d_v, elements el_v, userlevel user_v)
{
	SimB();
	_enabled=true;
	_stype=d_vt;
	_def.data=d_v;
	_def.elem=el_v;
	_def.fproc=NOFPROC;
	_def.bproc=NOBPROC;
	_def.ulev=user_v;
}
SimB::SimB(simtype d_vt, datatype d_v, elements el_v, fysprocess fproc, bioprocess bproc, userlevel user_v)
{
	SimB();
	_stype=d_vt;
	_def.data=d_v;
	_def.elem=el_v;
	_def.fproc=fproc;
	_def.bproc=bproc;
	_def.ulev=user_v;
}
SimB::SimB(simtype d_vt, datatype d_v, elements el_v)
{
	SimB();
	_stype=d_vt;
	_def.data=d_v;
	_def.elem=el_v;
	_def.fproc=NOFPROC;
	_def.bproc=NOBPROC;
	_def.ulev=NORMAL;
}
SimB::SimB(simtype d_vt, definitions def) 
{
	SimB();
	_stype=d_vt;
	_def=def;
}
SimB::SimB(simtype d_vt) 
{
	SimB();
	_stype=d_vt;
}
SimB::~SimB(void)
{
}

bool SimB::IsTrue(size_t i) const{
	
	auto VariableValue = (conditions_Array[i].variable)->GetIntValue();
	auto ConditionValue = conditions_Array[i].value;
   	switch(conditions_Array[i].op) {
		case SIMB_COMP::EQ	: if(VariableValue == ConditionValue)	return true;	return false;
		case SIMB_COMP::LT	: if(VariableValue  < ConditionValue)	return true;	return false;
		case SIMB_COMP::LE	: if(VariableValue <= ConditionValue)	return true;	return false;
		case SIMB_COMP::NEQ	: if(VariableValue != ConditionValue)	return true;	return false;
		case SIMB_COMP::GE	: if(VariableValue >= ConditionValue)	return true;	return false;
		case SIMB_COMP::GT	: if(VariableValue  > ConditionValue)	return true;	return false;
		//The two lines below has been remarked because these cases should never occur
		//case ON	: if(*VariableValue != 0)		return true;	return false;
		//case OFF: if(*VariableValue == 0)		return true;	return false;
		default	:								return true;
	};
}
void SimB::Notice() {
	_enabled = false;
	for(size_t i=0;i<conditions_Array.size();i++)
		if (!IsTrue(i)) _enabled=true;
}
 void SimB::AddToNoticeList(SimB *point) 
{
	noticelist_Array.push_back(point);		
}


//This function calls Notice for each of the objects
//in the noticelist array
void SimB::SendNotice() const{	  
	for (size_t i=0;i<noticelist_Array.size();i++)
	{
		//ASSERT(noticelist_Array[i]);
		//VERIFY(noticelist_Array[i]);
		noticelist_Array[i]->Notice();
	}
}
void SimB::Decode(string ConditionString) {
	bool		is_value;
	int			i = 0, lenght;
	//conditions.clear();
	//If he string is empty there are no conditions to be parsed
	if (ConditionString.size() == 0) return;
	//lenght=length of condition string
	lenght = ConditionString.size();
	//Step through the whole condition string
	while (i < lenght) {
		Condition	store;
		//temporary strings
		std::string		var, oper, val;
		// Erase unwanted spaces
		while (i < lenght && ConditionString[i] == ' ') i++;
		//Jump over the sign |
		if (i < lenght && ConditionString[i] == '|') { i++; continue; }
		//While ConditionString[i] is a character or a space
		//and while the whole string isnt processed
		//add ConditionString[i] to the temporary string var
		while (i < lenght && (ConditionString[i] == ' ' ||
			(ConditionString[i] >= 'A' && ConditionString[i] <= 'Z') ||
			(ConditionString[i] >= 'a' && ConditionString[i] <= 'z')))
			var += ConditionString[i++];
		//end of while loop

		//Here the parameter name is read into var
		//Set k to the number of chars in var minus one
		//to be used as an index variable for var
		int k = var.size() - 1;
		//Cut off trailing spaces
		//Strange statement though
		while (var.size() > 0 && var[k--] == ' ')
			var = var.substr(0, var.size() - 1);

		//Jump over spaces
		while (i < lenght && ConditionString[i] == ' ') i++;
		//Jump over |
		if (i < lenght && ConditionString[i] == '|') { i++; continue; }

		//Here the operator is read into oper
		//while ConditionString[i] is either =,<,> or /
		//and while the whole string hasnt been processed
		while (i < lenght &&
			(ConditionString[i] == '=' || ConditionString[i] == '<'
				|| ConditionString[i] == '>' || ConditionString[i] == '/'))
			oper += ConditionString[i++];


		//The statement belov might be erroneous because 
		//is_value is first used below this statement and is_value
		//vill be undefined the first time the while loop loops
		//However is_value should always be true so I remark these lines
		// If there is a value:
		// then erase unwanted spaces and read the value into val
		//else set val to "0"

		//if (is_value) {									
		while (i < lenght && ConditionString[i] == ' ') i++;
		while (i < lenght && ConditionString[i] >= '0' && ConditionString[i] <= '9')
			val += ConditionString[i++];
		//}
		//else 
		//	val="0";

		//Jump over trailing spaces
		while (i < lenght && ConditionString[i] != '|') i++;
		//Initialize the values of the store variable of class condition
		//This is done by decoding var, oper and val
#ifndef PGONLY
		store.variable = dynamic_cast<Sw*>(pfMap->GetPtr(SWITCH, var));
#else
		store.variable = nullptr;
#endif
		store.op = OpDecode(oper, &is_value);
		store.value = ValDecode(val);

		// Adding this condition to list
		//If store contains a Sw* and the operand isnt 0
		//then add this condition to the conditions array
		if (store.variable != nullptr && store.op != SIMB_COMP::NOVALUE)
			conditions_Array.push_back(store);
		//If theres left of the string and ConditionString[i]==| then
		//Why is there a continue here. It shouldnt be needed
		if (i < lenght && ConditionString[i] == '|')
		{
			i++;
			continue;
		}
	}
}
int SimB::ValDecode(string ValueString) const{   
	return FUtil::AtoInt(ValueString);
}

//Function for decoding an operand stored in a string
//if the operands corresponds to values then
//true is returned, else false is returned
SIMB_COMP SimB::OpDecode(string OperandString,bool *BoolValue) const{    
	if (OperandString == "=") {	*BoolValue = true;  return SIMB_COMP::EQ; };
	if (OperandString == "=="){	*BoolValue = true;  return SIMB_COMP::EQ; };
	if (OperandString == "<="){	*BoolValue = true;  return SIMB_COMP::LE; };
	if (OperandString == "<") {	*BoolValue = true;  return SIMB_COMP::LT; };
	if (OperandString == "!="){	*BoolValue = true;  return SIMB_COMP::NEQ; };
	if (OperandString == ">") {	*BoolValue = true;  return SIMB_COMP::GT; };
	if (OperandString == ">="){	*BoolValue = true;  return SIMB_COMP::GE; };
	//These have been remarked because OperandString can never 
	//attain these values
//	if (OperandString == "ON"){	*BoolValue = false; return ON; };
//	if (OperandString == "OFF"){*BoolValue = false; return OFF;};
return SIMB_COMP::NOVALUE;
}
void SimB::BuildDependencies(NewMap* fmap) {	 
	//The line below is for not having to pass mappointer as a parameter
	//That is not fine but fully functional. This means that
	//worldmap points to the wmap structure in CFortranmap
	pfMap = fmap;
	// Decodes conditions
	Decode(cond);				 								
	// Updates disabled counter for checking whether
	//this object should be disabled
	Notice();					 	
	//Add this object to the noticelist of every object
	//in this variables conditionsarray
	for (size_t i=0;i<conditions_Array.size();i++) {		  			// From all condition variables
		(conditions_Array[i].variable)->AddToNoticeList(this);}			// Get change notices
}

string SimB::GetCondition(size_t index) const{
	string VariableName;
	if (index<0 || index>=conditions_Array.size()) return VariableName;
	//VariableName is set to be the correct name
	VariableName=(conditions_Array[index].variable)->_name;
	//add a space character to the name
	VariableName += ' ';
	//Depending on the value of the condition add an operand and a value to the VariableName
   	switch(conditions_Array[index].op) {
	case SIMB_COMP::LE	: VariableName += "<= ";
	VariableName += FUtil::STD_ItoAscii(conditions_Array[index].value); break;
	case SIMB_COMP::LT	: VariableName += "< ";
	VariableName += FUtil::STD_ItoAscii(conditions_Array[index].value); break;
	case SIMB_COMP::GE	: VariableName += ">= ";
	VariableName += FUtil::STD_ItoAscii(conditions_Array[index].value); break;
	case SIMB_COMP::GT	: VariableName += "> ";
	VariableName += FUtil::STD_ItoAscii(conditions_Array[index].value); break;
	case SIMB_COMP::EQ	: VariableName += "= ";
	VariableName += FUtil::STD_ItoAscii(conditions_Array[index].value); break;
	case SIMB_COMP::NEQ	: VariableName += "!= ";
	VariableName += FUtil::STD_ItoAscii(conditions_Array[index].value); break;
	case SIMB_COMP::ON	: VariableName += "ON"; break;
	case SIMB_COMP::OFF: VariableName += "OFF"; break;
	default	: VariableName += "??"; break;
	};
	//check if this is true, if it is then add 'is true' to VariableName
	if (IsTrue(index)) VariableName += " is true";
	//othrwise add 'is false'
	else	VariableName += " is false";
	return VariableName;
}
string SimB::GetDepName(size_t i) const{ 
	string VariableName;
	//Return the name of the variable that is dependant of this variable.
	//If there is no such dependence then return the variablename
	if (i<noticelist_Array.size()) 
		return noticelist_Array[i]->GetName(); 
	return VariableName;}
string SimB::GetDepUnit(size_t i) const{ 
	string VariableUnit=" ";
	//Return the name of the variable that is dependant of this variable.
	//If there is no such dependence then return the variablename
	if (i<noticelist_Array.size()) 
		return noticelist_Array[i]->GetUnit(); 
	return VariableUnit;}

SimB* SimB::GetDepPtr(size_t i) const{
	//Return a pointer to the variable that is dependent of this variable
	//If there is no variable in the noticelist that is dependent then return nullptr
	if (i<noticelist_Array.size()) 
		return noticelist_Array[i]; 
	return nullptr;}


string SimB::GetProcessName() const {
	if(_def.bproc==11&&_def.fproc==17)  return GetElementName();
	else if(_def.fproc==17) return ModelCompNames::BioProcessNames[_def.bproc];
	else if(_def.bproc!=11) return ModelCompNames::BioProcessNames[_def.bproc];
	else return GetFysProcessName();
}
string SimB::GetElementName() const {
	return ModelCompNames::ElementNames[_def.elem];
}
string SimB::GetGroup()  {
	return ModelCompNames::GroupNames[_ModuleNo];
}
string SimB::GetFysProcessName() const {
	return ModelCompNames::FysProcessNames[_def.fproc];
}
string SimB::GetBioProcessName() const {
	return ModelCompNames::BioProcessNames[_def.bproc];
}
string SimB::GetTypeName() const 
 {	switch(_stype) { 
	case STATE: case STATE_SINGLE:
		return "State Variables";
	case FLOW:	case FLOW_SINGLE:
		return "Flow Variables";	
	case AUX: case AUX_SINGLE: return "Auxiliary Variables";
	case DRIVE: case DRIVE_SINGLE: return "Driving Variables";}
	return "";
}
