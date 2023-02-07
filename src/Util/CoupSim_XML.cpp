

#include "../std.h"
using namespace std;

#include "CoupSim_XML.h"

#include "../NewBase/CH.h"
#include "../PG/PGUtil.h"

#include "../NewBase/CommonModelInfo.h"

const std::vector<const char*> valTableAtt = { "ValFileIndex", "OutType", "Group", "Name","LocalIndex", "n", "R2","A0", "A1", "ME", "RMSE", "MeanSim",
"MeanVal","P_Error","A_Error","LogLi","AccTest","ValFileNumber", "LogTrans","ValidationFileResultIndex","Duration",
"Monitoring", "NSE", "P_NewError", "A_NewError", "BoxCox_Power", "BoxCox_Offset"};

void Sim_Xml::setValidationRow(VALv val)
{
	pugi::xml_node node = doc.child("node");

	node.set_name("Validation");

/*	if (node.getNumAttributes() == 0) {
		for (size_t i = 0; i < valTableAtt.size(); i++) {
			child->setAttribute(valTableAtt[i], int(i + 1));
		}
	}
	auto subchild = pugi::xml_node();
	subchild.->setAttribute(valTableAtt[0], int(val.ValidationFileIndex));
	subchild->setAttribute(valTableAtt[1], int(val.OutputType));
	subchild->setAttribute(valTableAtt[2], String(val.Group));
	subchild->setAttribute(valTableAtt[3], String(val.Name));
	subchild->setAttribute(valTableAtt[4], int(val.LocalIndex));
	subchild->setAttribute(valTableAtt[5], int(val.nNumber));
	subchild->setAttribute(valTableAtt[6], double(val.R2));
	subchild->setAttribute(valTableAtt[7], double(val.A0));
	subchild->setAttribute(valTableAtt[8], double(val.A1));
	subchild->setAttribute(valTableAtt[9], double(val.ME));
	subchild->setAttribute(valTableAtt[10], double(val.RMSE));
	subchild->setAttribute(valTableAtt[11], double(val.MeanSim));
	subchild->setAttribute(valTableAtt[12], double(val.MeanVal));
	subchild->setAttribute(valTableAtt[13], double(val.P_Error));
	subchild->setAttribute(valTableAtt[14], double(val.A_Error));
	subchild->setAttribute(valTableAtt[15], val.LogLi);
	subchild->setAttribute(valTableAtt[16], val.AccTest);
	subchild->setAttribute(valTableAtt[17], int(val.ValidationFileNumber));
	subchild->setAttribute(valTableAtt[18], val.LogTrans);
	subchild->setAttribute(valTableAtt[19], int(val.ValidationFileResultIndex));
	subchild->setAttribute(valTableAtt[20], int(val.Duration));
	subchild->setAttribute(valTableAtt[21], int(val.Monitoring));
	subchild->setAttribute(valTableAtt[22], double(val.NSE));
	subchild->setAttribute(valTableAtt[23], double(val.P_NewError));
	subchild->setAttribute(valTableAtt[24], double(val.A_NewError));
	subchild->setAttribute(valTableAtt[25], double(val.NewBoxCox_Power));
	subchild->setAttribute(valTableAtt[26], double(val.NewBoxCox_Offset));
	child->addChildElement(subchild);*/
}

vector<VALv> Sim_Xml::getValidationRows()
{
	vector<VALv> out;
	VALv node;

	auto child = doc.child("Validation");

	/*int rows = child->getNumChildElements();
	
		for (int row=0; row < rows; row++) {
			auto subchild = child->getChildElement(row);
			node.ValidationFileIndex	= subchild->getIntAttribute(valTableAtt[0]);
			node.OutputType				= subchild->getIntAttribute(valTableAtt[1]);
			node.Group = subchild->getStringAttribute(valTableAtt[2]).toStdString();
			node.Name =subchild->getStringAttribute(valTableAtt[3]).toStdString();
			node.LocalIndex = subchild->getIntAttribute(valTableAtt[4]);
			node.nNumber = subchild->getIntAttribute(valTableAtt[5]);
			node.R2 = float(subchild->getDoubleAttribute(valTableAtt[6]));
			node.A0 = float(subchild->getDoubleAttribute(valTableAtt[7]));
			node.A1 = float(subchild->getDoubleAttribute(valTableAtt[8]));
			node.ME = float(subchild->getDoubleAttribute(valTableAtt[9]));
			node.RMSE = float(subchild->getDoubleAttribute(valTableAtt[10]));
			node.MeanSim = float(subchild->getDoubleAttribute(valTableAtt[11]));
			node.MeanVal = float(subchild->getDoubleAttribute(valTableAtt[12]));
			node.P_Error = float(subchild->getDoubleAttribute(valTableAtt[13]));
			node.A_Error = float(subchild->getDoubleAttribute(valTableAtt[14]));
			node.LogLi = float(subchild->getDoubleAttribute(valTableAtt[15]));
			node.AccTest = subchild->getIntAttribute(valTableAtt[16]);
			node.ValidationFileNumber = subchild->getIntAttribute(valTableAtt[17]);
			node.LogTrans = subchild->getIntAttribute(valTableAtt[18]);
			node.ValidationFileResultIndex = subchild->getIntAttribute(valTableAtt[19]);
			node.Duration = subchild->getIntAttribute(valTableAtt[20]);
			node.Monitoring = subchild->getIntAttribute(valTableAtt[21]);
			node.NSE = float(subchild->getDoubleAttribute(valTableAtt[22]));
			node.P_NewError = float(subchild->getDoubleAttribute(valTableAtt[23]));
			node.A_NewError = float(subchild->getDoubleAttribute(valTableAtt[24]));
			node.NewBoxCox_Power = float(subchild->getDoubleAttribute(valTableAtt[25]));
			node.NewBoxCox_Offset = float(subchild->getDoubleAttribute(valTableAtt[26]));
			out.push_back(node);
		}*/

	return out;
}
static std::vector<string> withinRunTableAtt = {  "Module", "Name", "TabIndex", "NumChangeDate", "DateForChange", "Value", "Id", "Key"};
void Sim_Xml::setWithinRunRow(CHPARv ch)
{	//withinRun Not complete !!!
	auto child = doc.child("WithinRunOptions");

/*	if (child->getNumAttributes() == 0) {
		for (size_t i = 0; i < withinRunTableAtt.size(); i++) {
			child->setAttribute(withinRunTableAtt[i], int(i + 1));
		}
	}
	
	Ps* pPar = dynamic_cast<Ps*>(ch.pBase);
	P* pP = dynamic_cast<P*>(ch.pBase);
	CDB* pDB = dynamic_cast<CDB*>(ch.pBase);

	int TabIndex = int(ch.TabIndex);
	
		pPar = (Ps*)ch.pBase;
		CommonModelInfo* pCommonModelInfo = pPar->getCommonModelPointer();
		string group_psw = pPar->GetGroup();
		string name_psw = pPar->GetName();
		group_psw = pCommonModelInfo->GetGroupIdNames(group_psw);
		pCommonModelInfo->trim_xml(name_psw);
		string date;  double fvalue;
		auto subchild = new XmlElement("Row");
		subchild->setAttribute(withinRunTableAtt[0], group_psw);
		subchild->setAttribute(withinRunTableAtt[1], name_psw);
		subchild->setAttribute(withinRunTableAtt[2], TabIndex);
		subchild->setAttribute(withinRunTableAtt[3], int(ch.NumDates));
		for (size_t ii = 0; ii < ch.NumDates; ii++) {
			unsigned int min;
			auto subchild2 = new XmlElement("SubRow");
			if (ch.Type == 0) {
				min = pPar->GetChaDate(ii);
				date = PGUtil::StringDatum(min);
				fvalue = pPar->GetChaParValue(ii);			
				subchild2->setAttribute(withinRunTableAtt[4], date);
				subchild2->setAttribute(withinRunTableAtt[5], fvalue);
			}
			if (ch.Type == 1) {
				min = pP->GetChaDate(TabIndex,ii);
				date = PGUtil::StringDatum(min);
				fvalue = pP->GetChaParValue(TabIndex,ii);
				subchild2->setAttribute(withinRunTableAtt[4], date);
				subchild2->setAttribute(withinRunTableAtt[5], fvalue);
			}	
			else if (ch.Type == 2) {
				min = pDB->GetChaDate( ii);
				date = PGUtil::StringDatum(min);
				subchild2->setAttribute(withinRunTableAtt[4], date);
				string chaparvalue = pDB->GetChaParValue(ii);
				string chaparkey = pDB->GetChaParKey(ii); 
				subchild2->setAttribute(withinRunTableAtt[6], chaparvalue);
				subchild2->setAttribute(withinRunTableAtt[7], chaparkey);
			}
			subchild->addChildElement(subchild2);		
		}
		child->addChildElement(subchild);*/
}
vector<CHPARv> Sim_Xml::getWithinRunRows(ModelBase* pModelBase)
{
    vector<CHPARv> out;
/*	CHPARv node;
	CommonModelInfo* pCommonModelInfo = pModelBase->getCommonModelInfoPointer();
	auto child = demoData->getChildByName("WithinRunOptions");
	if (child == nullptr) return out;
	auto numpar = child->getNumChildElements();
	for (int i = 0; i < numpar; i++) {
		auto subchild = child->getChildElement(i);
		auto numdates = subchild->getNumChildElements();
		node.NumDates = numdates;
		node.mindates.resize(numdates);
		node.fvalues.resize(numdates);
		string group = subchild->getStringAttribute(withinRunTableAtt[0]).toStdString();
		group = pCommonModelInfo->getOrigName(group);
		string name = subchild->getStringAttribute(withinRunTableAtt[1]).toStdString();
		name = pCommonModelInfo->getOrigName(name);
		string tabindex = subchild->getStringAttribute(withinRunTableAtt[2]).toStdString();
		
		string numdatesstr = subchild->getStringAttribute(withinRunTableAtt[3]).toStdString();

		node.pBase = pModelBase->GetPtr(simtype::PAR_SINGLE, name);
		node.Type = 0;
		node.TabIndex = -1;
		if (node.pBase == nullptr) {
			node.pBase = pModelBase->GetPtr(simtype::PAR_TABLE, name);
			if (node.pBase != nullptr) {
				node.Type = 1;
				node.TabIndex = FUtil::AtoInt(tabindex);
			}
		}
		if (node.pBase == nullptr) {
			node.pBase = pModelBase->GetPtr(simtype::DB, name);
			if (node.pBase != nullptr) {
				node.Type = 2;
			}
		}
		node.NumDates = numdates;
		node.mindates.resize(numdates);
		for (int ii = 0; ii < numdates; ii++) {
			auto subchild2 = subchild->getChildElement(ii);
			string datestring = subchild2->getStringAttribute(withinRunTableAtt[4]).toStdString();
			node.mindates[ii] = PGUtil::MinutConv(PGUtil::Ato_IPGTID(datestring));
			if (node.Type == 0) {
				 node.fvalues[ii] = subchild2->getDoubleAttribute(withinRunTableAtt[5]);
			}
			else if (node.Type == 1) {
				if (node.Type != 2) node.fvalues[ii] = subchild2->getDoubleAttribute(withinRunTableAtt[5]);
			}
			else if (node.Type == 2) {
				node.Id = subchild2->getStringAttribute(withinRunTableAtt[6]).toStdString();
				node.Key =subchild2->getStringAttribute(withinRunTableAtt[7]).toStdString();
			}	
		}
		out.push_back(node);
	}*/
	return out;
}
static std::vector<string> MultiTableAtt = { "Dim", "Module", "Name", "TabIndex", "Method", "Monitoring",
"Min", "Max", "Start", "Dependence", "n", "Db_type", "Db_key", "Group_Dep", "Name_Dep", "TabIndex_Dep" };
void Sim_Xml::setMultiRunRow(int NumberOfRepeatitionsWithinDimension, int nCount, int numpar,
	string group_psw, const char* name_psw, int TabIndex, int imethod, int imon, vector<double> table, string selection_pcp, string keyselection_pcp,
	string group_psw_dep, const char* name_psw_dep, int ind_dep,
	double fmin, double fmax, double fstart, int i1) {

	pugi::xml_node child = doc.child("MultiRunOptions");
	/*if (child == nullptr) child = demoData->createNewChildElement("MultiRunOptions");
	if (child->getNumAttributes() == 0) {
		for (size_t i = 0; i < MultiTableAtt.size(); i++) {
			child->setAttribute(MultiTableAtt[i], int(i + 1));
		}
	}
	auto subchild = new XmlElement("Row");
	subchild->setAttribute(MultiTableAtt[0], NumberOfRepeatitionsWithinDimension);
	subchild->setAttribute(MultiTableAtt[1], String(group_psw));
	subchild->setAttribute(MultiTableAtt[2], String(name_psw));
	subchild->setAttribute(MultiTableAtt[3], TabIndex);
	subchild->setAttribute(MultiTableAtt[4], imethod);
	subchild->setAttribute(MultiTableAtt[5], imon);
	subchild->setAttribute(MultiTableAtt[6], fmin);
	subchild->setAttribute(MultiTableAtt[7], fmax);
	subchild->setAttribute(MultiTableAtt[8], fstart);
	//subchild->setAttribute(MultiTableAtt[9], );
	subchild->setAttribute(MultiTableAtt[10], nCount);
	if (imethod == 4) {	//Table
		for (size_t i = 0; i < table.size(); i++) {
			auto childt = new XmlElement("Table");
			childt->setAttribute("Value", table[i]);
			subchild->addChildElement(childt);
		}
	}
	else if (imethod == 5) { //DataBase
		subchild->setAttribute(MultiTableAtt[11], selection_pcp);
		subchild->setAttribute(MultiTableAtt[12], keyselection_pcp);
	}
	else if (imethod > 5) {
		subchild->setAttribute(MultiTableAtt[13], group_psw_dep);
		subchild->setAttribute(MultiTableAtt[14], name_psw_dep);
		subchild->setAttribute(MultiTableAtt[15], ind_dep);
	}
	
	child->addChildElement(subchild);*/
}
vector<Multi_InXml> Sim_Xml::getMultiRunRows()
{
	vector<Multi_InXml> out;
	Multi_InXml node;

	auto child = doc.child("MultiRunOptions");
	/*if (child == nullptr) return out;
	int rows = child->getNumChildElements();
	for (int row = 0; row < rows; row++) {
		auto subchild = child->getChildElement(row);

		node.numrepeat = subchild->getIntAttribute(MultiTableAtt[0]);
		node.group = subchild->getStringAttribute(MultiTableAtt[1]).toStdString();
		node.name = subchild->getStringAttribute(MultiTableAtt[2]).toStdString();
		node.tabindex = subchild->getIntAttribute(MultiTableAtt[3]);
		node.imethod = subchild->getIntAttribute(MultiTableAtt[4]);
		node.imon = subchild->getIntAttribute(MultiTableAtt[5]);
		node.fmin = subchild->getDoubleAttribute(MultiTableAtt[6]);
		node.fmax = subchild->getDoubleAttribute(MultiTableAtt[7]);
		node.fstart = subchild->getDoubleAttribute(MultiTableAtt[8]);

		node.nCount = subchild->getIntAttribute(MultiTableAtt[10]);

		if (node.imethod == 4) {
			auto tablerows = subchild->getNumChildElements();
			node.tablevalues.resize(tablerows);
			for (size_t i = 0; i < tablerows; i++) {
				auto childt = subchild->getChildElement(i);
				node.tablevalues[i] = childt->getDoubleAttribute("Value");
			}
		}
		else if (node.imethod == 5) {
			node.db_opt = subchild->getStringAttribute(MultiTableAtt[11]).toStdString();
			node.db_key = subchild->getStringAttribute(MultiTableAtt[12]).toStdString();
		}
		else if (node.imethod > 5) {
			node.group_dep = subchild->getStringAttribute(MultiTableAtt[13]).toStdString();
			node.name_dep = subchild->getStringAttribute(MultiTableAtt[14]).toStdString();
			node.tabindex_dep = subchild->getIntAttribute(MultiTableAtt[15]);
		}
		out.push_back(node);
	}*/
	return out;
}



void Sim_Xml::setModelGroup(const vector<SimB*> v_simb, CommonModelInfo* pCommonModelInfo)
{
	if (v_simb.size() == 0) return;

	
	auto ppoint = v_simb[0]->getCommonModelPointer();
	if (ppoint == nullptr) {
		for (size_t i = 0; i < v_simb.size(); i++) {
			v_simb[i]->setCommonModelPointer(pCommonModelInfo);
		}
	}
	auto groupname = pCommonModelInfo->GetGroupNames(v_simb[0]->GetGroupNo());

/*    pCommonModelInfo->trim_xml(groupname);
	auto child = demoData->getChildByName(groupname);
	if (child == nullptr) 	child = demoData->createNewChildElement(groupname);
	 simtype etype = v_simb[0]->GetType();
	 string nametype= simType_StringId[static_cast<size_t>(etype)];
	 pCommonModelInfo->trim_xml(nametype);
	 auto child2 = child->getChildByName(nametype);
	 if(child2 ==nullptr)  child2 = child->createNewChildElement(nametype);
	  child2->deleteAllChildElements();
	  Sw* pSw;	OutSingle* pX; OutVector* pV; size_t icount; int n;	Ps* pPs; P* pP;	Func* pFunc; String conditions;	String conditionsbase{ "Condition_" };
	  for (size_t i = 0; i < v_simb.size(); i++) {
		  auto org_name = v_simb[i]->GetName();
		  auto name = org_name;
			  pCommonModelInfo->trim_xml(name);
			  auto child3 = new XmlElement(String(name));
			  child3->setAttribute("Name", String(org_name));
			  switch (etype) {
			  case simtype::SWITCH:
				  pSw = static_cast<Sw*>(v_simb[i]);
				  n=pSw->GetNumOptions();
				  for (size_t j = 0; j < n; j++) {
					  auto txt = pSw->GetOption(j);
					  child3->setAttribute(String(Sw_StringOptions[j]), String(txt));
				  }
				  child3->setAttribute("Default", String(pSw->GetOption(pSw->GetOriginalIntValue())));
				  icount = 0;
				  while (pSw->GetCondition(icount) != "") {
					  conditions = conditionsbase + FUtil::STD_ItoAscii(icount + 1);
					  child3->setAttribute(conditions, String(pSw->GetCondition(icount)));
					  icount++;
				  }
				  break;
			  case simtype::STATE_SINGLE:
			  case simtype::FLOW_SINGLE:
			  case simtype::AUX_SINGLE:
			  case simtype::DRIVE_SINGLE:
				  pX = static_cast<OutSingle*>(v_simb[i]);
				  child3->setAttribute("Value", pX->GetValue());
				  child3->setAttribute("Unit", pX->GetUnit());
				  icount = 0;
				  while (pX->GetCondition(icount) != "") {
					  conditions = conditionsbase + FUtil::STD_ItoAscii(icount + 1);
					  child3->setAttribute(conditions, String(pX->GetCondition(icount)));
					  icount++;
				  }
				  break;
			  case simtype::STATE:
			  case simtype::FLOW:
			  case simtype::AUX:
			  case simtype::DRIVE:
				  pV = static_cast<OutVector*>(v_simb[i]);
				  child3->setAttribute("Value", pV->GetValue(size_t(0)));
				  child3->setAttribute("Unit", pV->GetUnit());
				  icount = 0;
				  while (pV->GetCondition(icount) != "") {
					  conditions = conditionsbase + FUtil::STD_ItoAscii(icount + 1);
					  child3->setAttribute(conditions, String(pV->GetCondition(icount)));
					  icount++;
				  }
				  break;
			  case simtype::PAR_SINGLE:
				  pPs = static_cast<Ps*>(v_simb[i]);
				  child3->setAttribute("Value", pPs->GetValue());
				  child3->setAttribute("Unit", pPs->GetUnit());
				  child3->setAttribute("Min", pPs->GetMinValue());
				  child3->setAttribute("Max", pPs->GetMaxValue());
				  //if(pPs->GetMinValue()< pPs->GetWarnMinValue()) child3->setAttribute("WarnMin", pPs->GetWarnMinValue());
				  //if (pPs->GetMaxValue() > pPs->GetWarnMaxValue()) child3->setAttribute("WarnMax", pPs->GetWarnMaxValue());
				  icount = 0;
				  while (pPs->GetCondition(icount) != "") {
					  conditions = conditionsbase + FUtil::STD_ItoAscii(icount + 1);
					  child3->setAttribute(conditions, String(pPs->GetCondition(icount)));
					  icount++;
				  }
				  break;
			  case simtype::PAR_TABLE:
				  pP = static_cast<P*>(v_simb[i]);
				  child3->setAttribute("Value", pP->GetValue(size_t(0)));
				  child3->setAttribute("Unit", pP->GetUnit());
				  child3->setAttribute("Min", pP->GetMinValue(0));
				  child3->setAttribute("Max", pP->GetMaxValue(0));
				  //if (pP->GetMinValue(0) < pP->GetWarnMinValue(0)) child3->setAttribute("WarnMin", pP->GetWarnMinValue(0));
				  //if (pP->GetMaxValue(0) > pP->GetWarnMaxValue(0)) child3->setAttribute("WarnMax", pP->GetWarnMaxValue(0));
				  icount = 0;
				  while (pP->GetCondition(icount) != "") {
					  conditions = conditionsbase + FUtil::STD_ItoAscii(icount + 1);
					  child3->setAttribute(conditions, String(pP->GetCondition(icount)));
					  icount++;
				  }
				  break;
			  case simtype::FUNCTION:
				  pFunc = static_cast<Func*>(v_simb[i]);
				  vector<string> pnames=pFunc->GetParDep_P_Names();
				  icount = 0;
				  for (auto it = pnames.begin(); it != pnames.end(); ++it) {
					  string str = (*it);
					  conditions = "P_dependentPar" + FUtil::STD_ItoAscii(icount + 1);
					  child3->setAttribute(conditions, str);
					  icount++;
				  }
				  pnames = pFunc->GetParDep_Ps_Names();
				  icount = 0;
				  for (auto it = pnames.begin(); it != pnames.end(); ++it) {
					  string str = (*it);
					  conditions = "Ps_dependentPar" + FUtil::STD_ItoAscii(icount + 1);
					  child3->setAttribute(conditions, str);
					  icount++;
				  }

				  //if (pP->GetMinValue(0) < pP->GetWarnMinValue(0)) child3->setAttribute("WarnMin", pP->GetWarnMinValue(0));
				  //if (pP->GetMaxValue(0) > pP->GetWarnMaxValue(0)) child3->setAttribute("WarnMax", pP->GetWarnMaxValue(0));
				  icount = 0;
				  while (pFunc->GetCondition(icount) != "") {
					  conditions = conditionsbase + FUtil::STD_ItoAscii(icount + 1);
					  child3->setAttribute(conditions, String(pP->GetCondition(icount)));
					  icount++;
				  }
				  break;

			  }
			  child3->setAttribute("DataType", v_simb[i]->GetDataType());
			  child3->setAttribute("Element", String(v_simb[i]->GetElementName()));
			  child3->setAttribute("FysProcess", String(v_simb[i]->GetFysProcessName()));
			  child3->setAttribute("BioProcess", String(v_simb[i]->GetBioProcessName()));
			  child2->addChildElement(child3);
	  }*/
}
void Sim_Xml::setModelNE_All(const vector<NE*> v_ne)
{
	const char* nametype = "NE";
	auto child = doc.child(nametype);
	/*if (child == nullptr) 	child = demoData->createNewChildElement(nametype);
	child->deleteAllChildElements();
	NE* pNE;
	for (size_t i = 0; i < v_ne.size(); i++) {
		pNE = v_ne[i];
		auto org_name = v_ne[i]->GetName();
		auto child2 = new XmlElement(String(nametype) + String("_") + String(FUtil::ItoNumAscii(i + 1)));
		child2->setAttribute("Name", String(org_name));
		child2->setAttribute("Value", int(pNE->GetNEValue()));
		child2->setAttribute("MinValue", int(pNE->GetNE_MinValue()));
		child2->setAttribute("MaxValue", int(pNE->GetNE_MaxValue()));
		child->addChildElement(child2);
	}*/

}
void Sim_Xml::setModelItems(const vector<SimB*> v_simb, CommonModelInfo* pCommonModelInfo)
{
	if (v_simb.size() == 0) return;


	auto ppoint = v_simb[0]->getCommonModelPointer();
/*	if (ppoint == nullptr) {
		for (size_t i = 0; i < v_simb.size(); i++) {
			v_simb[i]->setCommonModelPointer(pCommonModelInfo);

		}
	}
	simtype etype = v_simb[0]->GetType();
	string nametype = simType_StringId[size_t(etype)];
	pCommonModelInfo->trim_xml(nametype);
	auto child = demoData->getChildByName(nametype);
	if (child == nullptr) 	child = demoData->createNewChildElement(nametype);

	child->deleteAllChildElements();

	NE* pNE;
	Sw* pSw;	OutSingle* pX; OutVector* pV; size_t icount; int n;	Ps* pPs; P* pP;	Func* pFunc; String conditions;	String conditionsbase{ "Condition_" };
	for (size_t i = 0; i < v_simb.size(); i++) {
		auto org_name = v_simb[i]->GetName();
		auto name = org_name;
		pCommonModelInfo->trim_xml(name);
		auto child2 = new XmlElement(String(nametype)+String("_")+String(FUtil::ItoNumAscii(i+1)));
		child2->setAttribute("Name", String(org_name));
		switch (etype) {
		case simtype::SWITCH:
			pSw = static_cast<Sw*>(v_simb[i]);
			n = pSw->GetNumOptions();
			for (size_t j = 0; j < n; j++) {
				auto txt = pSw->GetOption(j);
				child2->setAttribute(String(Sw_StringOptions[j]), String(txt));
			}
			child2->setAttribute("Default", String(pSw->GetOption(pSw->GetOriginalIntValue())));
			icount = 0;
			while (pSw->GetCondition(icount) != "") {
				conditions = conditionsbase + FUtil::STD_ItoAscii(icount + 1);
				child2->setAttribute(conditions, String(pSw->GetCondition(icount)));
				icount++;
			}
			break;
		case simtype::STATE_SINGLE:
		case simtype::FLOW_SINGLE:
		case simtype::AUX_SINGLE:
		case simtype::DRIVE_SINGLE:
			pX = static_cast<OutSingle*>(v_simb[i]);
			child2->setAttribute("Value", pX->GetValue());
			child2->setAttribute("Unit", pX->GetUnit());
			icount = 0;
			while (pX->GetCondition(icount) != "") {
				conditions = conditionsbase + FUtil::STD_ItoAscii(icount + 1);
				child2->setAttribute(conditions, String(pX->GetCondition(icount)));
				icount++;
			}
			break;
		case simtype::STATE:
		case simtype::FLOW:
		case simtype::AUX:
		case simtype::DRIVE:
			pV = static_cast<OutVector*>(v_simb[i]);
			child2->setAttribute("Value", pV->GetValue(size_t(0)));
			child2->setAttribute("Unit", pV->GetUnit());
			pNE = pV->GetNEPointer();
			child2->setAttribute("IndexName", pNE->GetName());
			child2->setAttribute("IndexValue", int(pNE->GetNEValue()));
			icount = 0;
			while (pV->GetCondition(icount) != "") {
				conditions = conditionsbase + FUtil::STD_ItoAscii(icount + 1);
				child2->setAttribute(conditions, String(pV->GetCondition(icount)));
				icount++;
			}
			break;
		case simtype::PAR_SINGLE:
			pPs = static_cast<Ps*>(v_simb[i]);
			child2->setAttribute("Value", pPs->GetValue());
			child2->setAttribute("Unit", pPs->GetUnit());
			child2->setAttribute("Min", pPs->GetMinValue());
			child2->setAttribute("Max", pPs->GetMaxValue());
			//if(pPs->GetMinValue()< pPs->GetWarnMinValue()) child3->setAttribute("WarnMin", pPs->GetWarnMinValue());
			//if (pPs->GetMaxValue() > pPs->GetWarnMaxValue()) child3->setAttribute("WarnMax", pPs->GetWarnMaxValue());
			icount = 0;
			while (pPs->GetCondition(icount) != "") {
				conditions = conditionsbase + FUtil::STD_ItoAscii(icount + 1);
				child2->setAttribute(conditions, String(pPs->GetCondition(icount)));
				icount++;
			}
			break;
		case simtype::PAR_TABLE:
			pP = static_cast<P*>(v_simb[i]);
			child2->setAttribute("Value", pP->GetValue(size_t(0)));
			child2->setAttribute("Unit", pP->GetUnit());
			child2->setAttribute("Min", pP->GetMinValue(0));
			child2->setAttribute("Max", pP->GetMaxValue(0));
			pNE = pP->GetNEPointer();
			child2->setAttribute("IndexName", pNE->GetName());
			child2->setAttribute("IndexValue", int(pNE->GetNEValue()));
			//if (pP->GetMinValue(0) < pP->GetWarnMinValue(0)) child3->setAttribute("WarnMin", pP->GetWarnMinValue(0));
			//if (pP->GetMaxValue(0) > pP->GetWarnMaxValue(0)) child3->setAttribute("WarnMax", pP->GetWarnMaxValue(0));
			icount = 0;
			while (pP->GetCondition(icount) != "") {
				conditions = conditionsbase + FUtil::STD_ItoAscii(icount + 1);
				child2->setAttribute(conditions, String(pP->GetCondition(icount)));
				icount++;
			}
			break;
		case simtype::FUNCTION:
			pFunc = static_cast<Func*>(v_simb[i]);
			vector<string> pnames = pFunc->GetParDep_P_Names();
			icount = 0;
			for (auto it = pnames.begin(); it != pnames.end(); ++it) {
				string str = (*it);
				conditions = "P_dependentPar" + FUtil::STD_ItoAscii(icount + 1);
				child2->setAttribute(conditions, str);
				icount++;
			}
			pnames = pFunc->GetParDep_Ps_Names();
			icount = 0;
			for (auto it = pnames.begin(); it != pnames.end(); ++it) {
				string str = (*it);
				conditions = "Ps_dependentPar" + FUtil::STD_ItoAscii(icount + 1);
				child2->setAttribute(conditions, str);
				icount++;
			}

			//if (pP->GetMinValue(0) < pP->GetWarnMinValue(0)) child3->setAttribute("WarnMin", pP->GetWarnMinValue(0));
			//if (pP->GetMaxValue(0) > pP->GetWarnMaxValue(0)) child3->setAttribute("WarnMax", pP->GetWarnMaxValue(0));
			icount = 0;
			while (pFunc->GetCondition(icount) != "") {
				conditions = conditionsbase + FUtil::STD_ItoAscii(icount + 1);
				child2->setAttribute(conditions, String(pP->GetCondition(icount)));
				icount++;
			}
			break;

		}

		auto groupname = pCommonModelInfo->GetGroupNames(v_simb[i]->GetGroupNo());
		child2->setAttribute("Group", groupname);
		child2->setAttribute("DataType", v_simb[i]->GetDataType());
		child2->setAttribute("Element", String(v_simb[i]->GetElementName()));
		child2->setAttribute("FysProcess", String(v_simb[i]->GetFysProcessName()));
		child2->setAttribute("BioProcess", String(v_simb[i]->GetBioProcessName()));
		child->addChildElement(child2);
	}*/
}
vector<pair<string, int>> Sim_Xml::getAllInt(const char* type)
{
	vector<pair<string, int>> out;
	pair<string, int> p;
	auto child = doc.child(type);
	/*if (child != nullptr) {
		auto child2 = child->getFirstChildElement();
		while (child2 != nullptr) {
			string group = child2->getTagName().toStdString();
			auto child3 = child2->getFirstChildElement();
			while (child3 != nullptr) {
				p.first = child3->getTagName().toStdString();
				p.second = child3->getIntAttribute("Value", 0);
				out.push_back(p);
				child3=child3->getNextElement();
			}
			child2 = child2->getNextElement();
		}
	}*/
	return out;
}
vector<pair<string, double>> Sim_Xml::getAllDouble(const char* type)
{
	vector<pair<string, double>> out;
	pair<string, double> p;
	/*auto child = doc.getNodeByName(type);
	if (child != nullptr) {
		auto child2 = child->getFirstChildElement();
		while (child2 != nullptr) {
			string group = child2->getTagName().toStdString();
			auto child3 = child2->getFirstChildElement();
			while (child3 != nullptr) {
				p.first = child3->getTagName().toStdString();
				p.second = child3->getDoubleAttribute("Value", 0);
				out.push_back(p);
				child3 = child3->getNextElement();
			}
			child2 = child2->getNextElement();
		}
	}*/
	return out;
}
vector<pair<string, string>> Sim_Xml::getAllstring(const char* type)
{
	vector<pair<string, string>> out;
	pair<string, string> p;
	auto child = doc.child(type);
	/*if (child != nullptr) {
		auto child2 = child->getFirstChildElement();
		while (child2 != nullptr) {
			string group = child2->getTagName().toStdString();
			auto child3 = child2->getFirstChildElement();
			while (child3 != nullptr) {
				p.first = child3->getTagName().toStdString();
				p.second = child3->getStringAttribute("Value", "").toStdString();
				out.push_back(p);
				child3 = child3->getNextElement();
			}
			child2 = child2->getNextElement();
		}
	}*/
	return out;
}
vector<pair<string, string>> Sim_Xml::getAllChangedItems()
{
	vector<pair<string, string>> out;
	pair<string, string> p;
	const char* name = "MapWithChangedNames";
	auto child = doc.child(name);
	/*if (child != nullptr) {
		auto child2 = child->getFirstChildElement();
		while (child2 != nullptr) {
			p.first = child2->getTagName().toStdString();
			p.second = child2->getStringAttribute("Value").toStdString();
			out.push_back(p);
			child2 = child2->getNextElement();
		}
	}*/
	return out;
}
vector<string> Sim_Xml::getAllNames(const char* type) {
	vector<string> out;
	auto child = doc.child(type);
	/*if (child != nullptr) {
		auto child2 = child->getFirstChildElement();
		while (child2 != nullptr) {
			string tag = child2->getTagName().toStdString();
			string name = child2->getStringAttribute("Name","").toStdString();
			out.push_back(name);
			child2 = child2->getNextElement();
		}
	}*/

	return out;
}
vector<pair<string, pair<vector<string>, vector<vector<double>>>>> Sim_Xml::getAllDoubleTables(const char* type)
{
	vector<pair<string,pair<vector<string>, vector<vector<double>>>>> out;
	pair<string, pair<vector<string>, vector<vector<double>>>> t;
	pair < vector<string>, vector<vector<double>>> c;
	
	auto child = doc.child(type);
	/*if (child != nullptr) {
		auto child2 = child->getFirstChildElement();
		while (child2 != nullptr) {
			string group = child2->getTagName().toStdString();
			auto child3 = child2->getFirstChildElement();
			while (child3 != nullptr) {
				c.second.clear();
				t.first = child3->getTagName().toStdString();
				auto num = child3->getNumAttributes();
				c.first.clear();
				for (int i = 0; i < num; i++) {
					c.first.push_back(child3->getAttributeName(i).toStdString());
				}
				auto rows = child3->getNumChildElements();
				auto child4 = child3->getFirstChildElement();
				
				vector<double> rowvalue; rowvalue.resize(num);
				

				for (int j = 0; j<rows; j++) {
					c.second.push_back(rowvalue);
					for (size_t i = 0; i < num; i++) {
						c.second[j][i] = child4->getDoubleAttribute(String(c.first[i]));
					}					
					child4=child3->getChildElement(j);
				}
				t.second = c;
				out.push_back(t);
				child3 = child3->getNextElement();

			}

			child2 = child2->getNextElement();
		}
	}*/
	return out;
}
