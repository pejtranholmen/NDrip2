#ifndef NO_MS_GUI
#include "../stdafx.h"
#endif
#include "FunctionProperties.h"
#include "../ModelTypes/P.h"
#include "../ModelTypes/Ps.h"
#include "../Util/FUtil.hpp"
//lite mer


FunctionProperties::FunctionProperties(void)
{
#ifndef NO_MS_GUI
	MySingle::instance().SetFunctionPointer(this);
#endif
}


FunctionProperties::~FunctionProperties(void)
{
}

bool FunctionProperties::Init(string filename)
{

	fstream file;
	string str, line;
	size_t pos1, pos2, nrec;
	if(!FUtil::IsFileExisting(filename)) return false;

	file.open(filename,ios::in|ios::binary);
	
	file.seekg(0,ios::end);
	unsigned long long totsize=file.tellg();
	string linebreak="\r\n";
	string delimiters=";";
	string datestring;
	PLOT_PARAM prop;
	PLOT_PARAM_DEP prop_par;
	prop.Num_Of_Curves=1;
	prop.X_Label="";

	file.seekg(0,ios::beg);
	str.clear();
	unsigned long long rrec;
	rrec=100000;
	pos2=0;
	nrec=0;
	size_t count=0;
	bool end=false;
	while(pos2<totsize) {
		pos1=file.tellg();
		if(totsize<pos1+100000)
			rrec=totsize-pos1;
		str.resize(rrec);
		file.read((char*)&str[0],rrec);
		pos2=file.tellg();
		/*size_t nbyte=pos2-pos1;
		for (int i=0;i<nbyte;i++)
			str+=buf[i];*/


		auto ich=str.find(linebreak);
		size_t pos=0;
		line=str.substr(pos,ich-pos);
		while (ich!=string::npos) {
			
			nrec++;
			size_t idel;
			idel=line.find(delimiters);
			string var;
			var=line.substr(0,idel);
			FUtil::trim(var);

			// Var is function Name;

			if(nrec==1) count=0;
			else count++;
			m_FuncIndex.insert(pair<string,size_t>(var, count));
			
			while(idel!=std::string::npos) {
				pos=idel+1;
				idel=line.find(delimiters,pos);
				if(idel!=string::npos&&idel>pos){
					prop.Y_Label=line.substr(pos, idel-pos);
				}
			}
			pos=ich+1;
			ich=str.find(linebreak, pos);
			if(pos>str.size()) {
				idel=string::npos;
				ich=string::npos;
				end=true;
			}
			else {
				line=str.substr(pos, ich-pos);
				idel=line.find(delimiters);
				var=line.substr(0,idel);
			}
			// About parameters linked to the function without Index			
			while(idel!=string::npos&&idel!=line.size()) {
				size_t pos=idel+1;
				idel=line.find(delimiters,pos);
				if(idel==string::npos&&line.size()>pos) 
					idel=line.size();

				if(idel!=string::npos&&idel>pos){
					prop_par.Ps_Names.push_back(line.substr(pos, idel-pos));
				}
			}
		
			if(!end) {
				pos=ich+1;
				ich=str.find(linebreak, pos);
				line=str.substr(pos, ich-pos);
				idel=line.find(delimiters);
				var=line.substr(0,idel);
				// About parameters linked to the function with Index
				while(idel!=std::string::npos&&idel!=line.size()) {
					size_t pos=idel+1;
					idel=line.find(delimiters,pos);
					if(idel==string::npos&&line.size()>pos) 
						idel=line.size();
					if(idel!=string::npos&&idel>pos){
						prop_par.P_Names.push_back(line.substr(pos, idel-pos));
					}
				}

				pos=ich+1;
				ich=str.find(linebreak, pos);
				line=str.substr(pos, ich-pos);
				idel=line.find(delimiters);
				var=line.substr(0,idel);

				pos=ich+1;
				ich=str.find(linebreak, pos);
				line=str.substr(pos, ich-pos);
				idel=line.find(delimiters);
				var=line.substr(0,idel);
				prop.Num_Of_Curves=FUtil::AtoInt(var);
				size_t nvar=0;
				size_t numx;
				while(idel!=std::string::npos) {
					size_t pos=idel+1;
					idel=line.find(delimiters,pos);
					nvar++;
					if(idel!=string::npos&&idel>pos){
						var=line.substr(pos,idel-pos);
						switch (nvar) {
							case 1:	prop.Num_Of_Points=FUtil::AtoInt(var);break;
							case 2:	if(FUtil::AtoInt(var)==0) prop.LogXScale=false; else prop.LogXScale=true;break;
							case 3:	if(FUtil::AtoInt(var)==0) prop.LogYScale=false; else prop.LogYScale=true;break;
							case 4:	numx=FUtil::AtoInt(var);break;
							case 5: prop.Num_Of_Opt=FUtil::AtoInt(var); break;
						}
					}
				}

				pos=ich+1;
				ich=str.find(linebreak, pos);
				line=str.substr(pos, ich-pos);
				idel=line.find(delimiters);
				var=line.substr(0,idel);

				pos=ich+1; // XAxis Line
				ich=str.find(linebreak, pos);
				line=str.substr(pos, ich-pos);
				idel=line.find(delimiters);
				prop.X_Label=line.substr(0,idel);
				prop.X_MaxPar="";
				nvar=0;
				while(idel!=std::string::npos) {
					size_t pos=idel+1;
					idel=line.find(delimiters,pos);
					nvar++;
					if(idel!=string::npos&&idel>pos){
						var=line.substr(pos,idel-pos);
						switch(nvar) {
						case 1: prop.X_Label+="(";prop.X_Label+=var;prop.X_Label+=")";break;
						case 2: prop.X_Min=FUtil::AtoFloat(var); break;
						case 3: prop.X_Max=FUtil::AtoFloat(var); break;
						case 4: prop.X_MaxPar=line.substr(pos, idel-pos);break;
						}

					}
				}

				pos=ich+1;
				ich=str.find(linebreak, pos);
				line=str.substr(pos, ich-pos);
			//	idel=line.find(delimiters);
			//	var=line.substr(0,idel);
				pos=ich+1; // Legends
				ich=str.find(linebreak, pos);
				line=str.substr(pos, ich-pos);
				idel=line.find(delimiters);

				for(size_t i=0; i<prop.Num_Of_Curves&&idel!=string::npos; i++) {

					
						nvar=0;
						while(idel!=std::string::npos) {
							size_t lpos;
							if(nvar>0) 
								lpos=idel+1;
							else 
								lpos=0;
							idel=line.find(delimiters,lpos);
							nvar++;
							if(idel!=string::npos&&idel>lpos){
								var=line.substr(lpos,idel-lpos);
								switch(nvar) {
									case 1: prop.LegendsVec.push_back(var);break;
									case 2: prop.X2_Vec.push_back(FUtil::AtoFloat(var)); break;
									case 3: prop.X3_Vec.push_back(FUtil::AtoFloat(var)); break;
									case 4: prop.X4_Vec.push_back(FUtil::AtoFloat(var)); break;
									case 5: prop.Opt_Vec.push_back(FUtil::AtoInt(var)); break;
								};

							}
						}

						pos=ich+1; // XAxis Line
						ich=str.find(linebreak, pos);
						if(ich==string::npos&&pos<pos2) ich=pos2;
						pos++;
						if(ich>=pos&&ich!=string::npos) {
							line=str.substr(pos, ich-pos);
							idel=line.find(delimiters);
						}
						else {
							line="";
							idel=string::npos;
						}

				}

				m_FuncProp.push_back(prop);
				m_FuncPar.push_back(prop_par);
				prop_par.Ps_Names.clear();
				prop_par.P_Names.clear();
				prop.LegendsVec.clear();prop.X2_Vec.clear(); prop.X3_Vec.clear(); prop.X4_Vec.clear(); prop.Opt_Vec.clear();


			}

		}

	}
	file.close();

	return true;
}

bool FunctionProperties::SetParamDep(vector<Ps*> PsVector, vector<P*> PVector, size_t index)
{
	PLOT_PARAM_DEP p;
	for (size_t i = 0; i < PsVector.size(); i++) {
		p.Ps_Names.push_back(PsVector[i]->GetName());
	}
	for (size_t i = 0; i < PVector.size(); i++) {
		p.P_Names.push_back(PVector[i]->GetName());
	}
	if(m_FuncPar.size()>index)	m_FuncPar[index] = p;
	return true;
}

bool FunctionProperties::SetParamDep(vector<Ps*> PsVector, size_t index)
{
	PLOT_PARAM_DEP p;
	for (size_t i = 0; i < PsVector.size(); i++) {
		p.Ps_Names.push_back(PsVector[i]->GetName());
	}
	if (m_FuncPar.size()>index)	m_FuncPar[index] = p;
	return true;
}
bool FunctionProperties::SetParamDep(vector<P*> PVector, size_t index)
{
	PLOT_PARAM_DEP p;
	for (size_t i = 0; i < PVector.size(); i++) {
		p.P_Names.push_back(PVector[i]->GetName());
	}
	if (m_FuncPar.size()>index)	m_FuncPar[index] = p;
	return true;
}
