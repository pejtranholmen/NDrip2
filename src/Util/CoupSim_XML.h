#pragma once
#include "pugixml.hpp"
#include <vector>
#include <string>
#include <fstream>

static vector<string> output_col_names={ "Index", "Initial", "Final", "Min", "Max", "Mean", "Accumulated", "StoreFlag" };
struct Output_InXml { string name{ "" }; int Index{ -1 }; double dvalues[8]{0.,0.,0.,0.,0.,0.,0.,0.}; int flag{ -1 }; };
struct Multi_InXml {
	int numrepeat; string group; string name; int tabindex; int imethod; int imon; double fmin;
	double fmax; double fstart; int nCount; string group_dep; string name_dep; int tabindex_dep; vector<double> tablevalues;
	string db_opt; string db_key;
};
class VALv;
class CHPARv;
class Sw;
class SimB;
class ModelBase;
class CommonModelInfo;
class NE;
class Sim_Xml {
public:
    Sim_Xml() {
        m_IsOpen = true;
        m_xmlfileName = "CoupModel_Sim.xml";
        OpenRegister();
        if (!m_register.is_open()) OpenRegister();
    };

    ~Sim_Xml() { OpenRegister(false); };

    bool OpenRegister(bool read = true) {

        string line_str;
        //if (m_register.is_open()) {
        if (read) {
            if (ReadRegisterFile() == string::npos) {
                return false;
            }
        } else {
            m_register.clear();
            WriteRegisterFile();
            m_register.close();
        }
        return true;
    };

    bool WriteRegisterFile() {

        return false;
    }

    void setColumnAtt(vector<string> att, string Tag = "ColumnAtt") {
        /*	auto child = demoData->getChildByName(Tag);
            if (child == nullptr) {
                child = demoData->createNewChildElement(Tag);
                for(size_t i=0; i<att.size(); i++)
                    child->setAttribute(string(att[i]), int(i+1));

            }*/
    }

    int GetInt(const char *module, const char *type, string name, int default_value) {
/*		auto child3 = getLevel3(module, type, name);
		if (child3 == nullptr) {
			child3 = getLevel2(module, type)->createNewChildElement(name);
			child3->setAttribute("Value", default_value);
			return default_value;
		}
		else
			return child3->getIntAttribute("Value");*/
        return 1;
    };

    void setValue(const char *module, const char *type, const char *name, int newvalue) {
        auto child3 = getLevel3(module, type, name);
        if (child3 == nullptr) {
            //child3 = getLevel2(module, type)->createNewChildElement(name);
            //child3->setAttribute("Value", newvalue);
        }

        //child3->setAttribute("Value" , newvalue);
    };

    pugi::xml_node getLevel3(const char *module, const char *type, const char *name) {
        pugi::xml_node node;

        node = doc.child(module).child(type).child(name);
        return node;
    }

    pugi::xml_node getLevel2(const char *module, const char *type) {
        pugi::xml_node node;
        node = doc.child(module).child(type);
        return node;
    }

    string getstring(const char *name, string default_value) {
        auto child = currentList.child(name);
        /*if (child == nullptr) {
            child = currentList->createNewChildElement(name);
            child->setAttribute("Value", default_value);
            return default_value;
        }
        else
            return child->getstringAttribute("Value").toStdstring();*/
    };

    vector<pair<string, string>> getAllCurrentListstring() {
        vector<pair<string, string>> out;
        /*	pair<string, string> node;
            auto num = currentList->getNumChildElements();
            for (int i = 0; i < num; i++) {
                auto child = currentList->getChildElement(i);
                node.first = child->getTagName().toStdstring();
                node.second = child->getstringAttribute("Value").toStdstring();
                out.push_back(node);
            }*/
        return out;
    }

    void setValue(const char *name, string newvalue) {
        /*	auto child = currentList->getChildByName(name);
            if (child == nullptr) {
                child = currentList->createNewChildElement(name);
                child->setAttribute("Value", newvalue);
            }
            else
                child->setAttribute("Value", newvalue);*/
    };

    void setTextValue(const char *name, string newvalue) {
        /*	auto child = currentList->getChildByName(name);
            if (child == nullptr) {
                child = currentList->createNewChildElement("Text");
                child->createTextElement(newvalue);
            }
            else
                child->addTextElement( newvalue);*
        };*/
    }
        double getDouble(const char *name, double default_value) {
            auto child = currentList.child(name);
            /*if (child == nullptr) {
                child = currentList->createNewChildElement(name);
                child->setAttribute("Value", default_value);
                return default_value;
            }
            else
                return child->getDoubleAttribute("Value");*/
            auto attr = child.attribute(name);
            if (attr == nullptr) return default_value;
            attr.as_double(default_value);
        };
        void setValue(const char *name, double newvalue) {
            auto child = currentList.child(name);

        };
        bool getbool(const char *name, bool default_value) {
            auto child = currentList.child(name);

/*            if (child == nullptr) {
                child = currentList->createNewChildElement(name);
                child->setAttribute("Value", default_value);
                return default_value;
            } else
                return child->getDoubleAttribute("Value");*/
            return false;
        };
        void setValue(const char *name, bool newvalue) {
            auto child = currentList.child(name);
       /*     if (child == nullptr) {
                child = currentList->createNewChildElement(name);
                child->setAttribute("Value", newvalue);
            } else
                child->setAttribute("Value", newvalue);*/
        };
        int getInt(const char *name, int default_value) {
            auto child = currentList.child(name);
            return 1;
        };

        void setValue(const char *name, int newvalue) {
            auto child = currentList.child(name);
        };
        void setTable(const char *module, const char *type, const char *name, vector<string> colnames,
                      vector<vector<double>> values) {

           /* auto child3 = getLevel3(module, type, name);
            if (child3 == nullptr) {
                child3 = getLevel2(module, type)->createNewChildElement(name);
            }
            for (size_t i = 0; i < colnames.size(); i++) {
                child3->setAttribute(string(colnames[i]), int(i + 1));
            }
            child3->deleteAllChildElements();
            for (size_t i = 0; i < values.size(); i++) {
                string tag = "Row_" + string(int(i + 1));
                auto child4 = new XmlElement(tag);
                for (size_t j = 0; j < values[i].size(); j++) {
                    child4->setAttribute(string(colnames[j]), values[i][j]);
                }
                child3->addChildElement(child4);
            }*/
        }

        void setOutPutRow(const char *module, const char *type, const char *name, int index_output, int storeflag,
                          vector<double> values) {

          /*  auto child3 = getLevel3(module, type, name);
            if (child3 == nullptr) {
                child3 = getLevel2(module, type)->createNewChildElement(name);
            }

            for (size_t i = 0; i < output_col_names.size(); i++) {
                child3->setAttribute(string(output_col_names[i]), int(i + 1));
            }
            string tag = "Row";
            auto child4 = new XmlElement(tag);
            if (index_output >= 0) child4->setAttribute(string(output_col_names[0]), index_output);
            for (size_t i = 0; i < values.size(); i++) {
                child4->setAttribute(string(output_col_names[i + 1]), values[i]);
            }
            child4->setAttribute(string(output_col_names[7]), storeflag);
            child3->addChildElement(child4);*/
        }
        vector<Output_InXml> getOutputs(const char *type) {
            vector<Output_InXml> out;
/*            Output_InXml node;
            auto child = demoData->getChildByName(type);
            if (child == nullptr) return out;
            auto child2 = child->getFirstChildElement();
            while (child2 != nullptr) {
                auto group = child2->getTagName().toStdstring();
                auto child3 = child2->getFirstChildElement();

                while (child3 != nullptr) {

                    auto child4 = child3->getFirstChildElement();
                    auto rows = child3->getNumChildElements();
                    node.name = child3->getTagName().toStdstring();
                    for (int row = 0; row < rows; row++) {
                        child4 = child3->getChildElement(row);
                        node.Index = child4->getIntAttribute("Index");
                        for (size_t i = 0; i < 6; i++) {
                            node.dvalues[i] = child4->getDoubleAttribute(output_col_names[i + 1]);
                        }
                        node.flag = child4->getIntAttribute("StoreFlag");
                        out.push_back(node);
                    }
                    child3 = child3->getNextElement();
                }
                child2 = child2->getNextElement();
            }*/
            return out;
        };
        void setValidationRow(VALv val);
        std::vector<VALv> getValidationRows();
        void setWithinRunRow(CHPARv ch);
        vector<CHPARv> getWithinRunRows(ModelBase *);
        void setMultiRunRow(int NumberOfRepeatitionsWithinDimension, int nCount, int numpar,
                            string group_psw, const char *name_psw, int TabIndex, int imethod, int imon,
                            vector<double> table, string selection_pcp, string keyselection_pcp,
                            string group_psw_dep, const char *name_psw_dep, int ind_dep,
                            double fmin, double fmax, double fstart, int i1);
        vector<Multi_InXml> getMultiRunRows();


        bool IsRegisterOpen() { return m_IsOpen; };
        void setModelGroup(vector<SimB *>, CommonModelInfo *);
        void setModelItems(vector<SimB *>, CommonModelInfo *);
        void setModelNE_All(vector<NE *>);

        void setRegister(const string tag, const int value) {
            /*auto child = dataList.node(tag);
            if (child != nullptr)
                child->setAttribute("Value", value);*/
        };
        void setRegister(const string tag, const string value) {
            //dataList->getChildByName(tag)->setAttribute("Value", value);
        };
        void setRegister(const string tag, const double value) {
            //	dataList->getChildByName(tag)->setAttribute("Value", value);
        };
        int getIntRegister(const string tag) {
            //	return dataList->getChildByName(tag)->getIntAttribute("Value");
        };
        string getstringRegister(const string tag) {
            //	return dataList->getChildByName(tag)->getstringAttribute("Value");
        };
        double getDoubleRegister(const string tag) {
            //	return dataList->getChildByName(tag)->getDoubleAttribute("Value");
        };
        void setHiddenData(const string tag, const int value) {
            /*	auto child = dataStore->getChildByName(tag);
                if (child != nullptr)
                    child->setAttribute("Value", value);
                else {
                    child = dataStore->createNewChildElement(tag);
                    child->setAttribute("Value", value);
                }*/
        };
        void setHiddenData(const string tag, const string value) {
            /*auto child = dataStore->getChildByName(tag);
            if (child != nullptr)
                 child->setAttribute("Value", value);
            else {
              child = dataStore->createNewChildElement(tag);
              child->setAttribute("Value", value);
            }*/
        };
        void setHiddenData(const string tag, const double value) {
            /*auto child = dataStore->getChildByName(tag);
            if (child != nullptr)
                child->setAttribute("Value", value);
            else {
                child = dataStore->createNewChildElement(tag);
                child->setAttribute("Value", value);
            }*/
        };
        void setHiddenChildData(const string tag, const double value) {
            /*auto child = dataStore->getChildByName(tag);
            if (child != nullptr)
                child->setAttribute("Value", value);
            else {
                child = dataStore->createNewChildElement(tag);
                child->setAttribute("Value", value);
                auto child2= child->createNewChildElement("Options");
                        child2->setAttribute("Value", value);
            }*/
        };
        int getHiddenIntData(const string tag) {
            /*auto child = dataStore->getChildByName(tag);
            if (child != nullptr)
                return child->getIntAttribute("Value");
            else
                return 0;*/
        };

        string getHiddenstringData(const string tag) {
            /*auto child = dataStore->getChildByName(tag);
            if (child != nullptr)
                return child->getstringAttribute("Value");
            else
                return "";*/
        };
        double getHiddenDoubleData(const string tag) {
            /*auto child = dataStore->getChildByName(tag);
            if (child != nullptr)
                return child->getDoubleAttribute("Value");
            else
                return double(MISSING);*/
        };

        bool setFileName(const char *name) {
//            if (name.find(":") == string::npos)
  //              return false;
          
            doc.load_file(name);
            m_xmlfileName = name;

            return true;

        };


        void saveFile() {
            WriteRegisterFile();
        };
        size_t ReadRegisterFile() {

            return 0;
        }
        vector<pair<string, int>> getAllInt(const char *type);
        vector<pair<string, double>> getAllDouble(const char *type);
        vector<pair<string, string>> getAllstring(const char *type);
        vector<string> getAllNames(const char *type);
        vector<pair<string, string>> getAllChangedItems();
        void insert_ChangedNames(vector<pair<string, string>>);
        vector<pair<string, pair<vector<string>, vector<vector<double>>>>> getAllDoubleTables(const char *type);
        void setSimFileName(string file) { m_xmlSimfileName = file; };
        void setModelFileName(string file) { m_xmlModelfileName = file; };
        string getSimFileName() { return m_xmlSimfileName; };
        string getModelFileName() { return m_xmlModelfileName; };



        private:
        pugi::xml_document doc;
        // A pointer to the sub-node of demoData that contains the list of columns
        pugi::xml_node dataList;
        pugi::xml_node currentList;

        pugi::xml_node dataStore;
        pugi::xml_node comboBoxItems;


        bool m_IsOpen;
        bool m_typeAsHighestLevel{true};
        fstream m_register;
        string m_xmlfileName;
        string m_xmlSimfileName;
        string m_xmlModelfileName;
};