#pragma once
struct sim_doc_simulation {
    string name;
    string creator;
    int simno;
    string comment;
};
struct run_info_document {
    int key_simulation;
    int fileversionnumber;
    string originalfilename;
    string exefiledata;
    bool multisimulation;
    bool finished;
    time_t date_created;
    time_t date_modified;
    int simstartmin;
    int simendmin;
    int simperiodscaling;
    int priorsimperiod;
    int postsimperiod;
    int timeresolution;
    int output_interval_minutes;
    int output_interval_days;
    int numberofiterations;
    bool lockedmultistore;
};
struct switch_set {
    int key_simulation;
    int id_switch;
    int value;
};
struct singlepar_set {
    int key_simulation;
    int id_singlepar;
    float value;
};
struct vectorpar_set {
    int key_simulation;
    int id_vectorpar;
    vector<float> values;
};
struct singleoutput_set {
    int key_simulation;
    int id_singleoutput;
    int storeflagvalue;
    float initial, final, min, max, mean, cumulated;
    int singlerun_out_index, multirun_out_index;
};
struct vectoroutput_set {
    int key_simulation;
    int id_vectoroutput;
    int numberstoreflags;
    vector<int> storeflags;
    vector<int> selectedindex;
    vector<float> initials, finals, mins, maxs, means, cumulateds;
    vector<int> singlerun_out_index, multirun_out_index;
};
struct timeserie_set {
    int key_simulation;
    int id_timeserie;
    string filename;
    int NumVar;
    int NumRec;
    int NumRepetions;
    int NormalTimeStep;
    vector<string> Name;
    vector<string> Units;
    vector<string> Id;
    vector<string> Pos;
    vector<float> Min;
    vector<float> Max;
    vector<string> Country;
    vector<string> Station;
    vector<double> Latitude;
    vector<double> Longitude;
    vector<double> Altitude;
};
struct pgfile_set {
    int key_file;
    int MinTime;
    vector<float> var;
};


int transfer_Document(struct sim_doc_simulation str) {
    try {
        connection c = initconnection("pe1950win");
        pqxx::work W{ c };

        string sql = "INSERT INTO Simulations VALUES ( ";
        sql += "Default,'";
        sql += str.name + "',";
        if (str.creator.size() == 0) sql += "Default,";
        else sql += "'" + str.creator + "',";
        sql += to_string(str.simno) + ",";
        if (str.comment.size() == 0) sql += "Default";
        else sql += "'" + str.comment + "'";
        sql += ") returning id_simulations; ";

        result r = W.exec(sql.c_str());
        W.commit();
        return  r.begin().at(0).as<int>();
    }
    catch (const std::exception& e) {
        cerr << e.what() << std::endl;
        return -1;
    }
    return 0;
};
int transfer_RunInfo_Document(struct run_info_document s) {
    try {
        connection c = initconnection();
        pqxx::work W{ c };

        string sql = "INSERT INTO runinfo VALUES ( ";
        sql += to_string(s.key_simulation) + ",";
        sql += to_string(s.fileversionnumber) + ",";
        if (s.originalfilename.size() == 0) sql += "null,";
        else sql += "'" + s.originalfilename + "',";
        if (s.exefiledata.size() == 0) sql += "null,";
        else sql += "'" + s.exefiledata + "',";
        sql += to_string(s.multisimulation) + ",";
        sql += to_string(s.finished) + ",";
        sql += to_string(int(s.date_created)) + ",";
        sql += to_string(int(s.date_modified)) + ",";
        sql += to_string(s.simstartmin) + ",";
        sql += to_string(s.simendmin) + ",";
        sql += to_string(s.simperiodscaling) + ",";
        sql += to_string(s.priorsimperiod) + ",";
        sql += to_string(s.postsimperiod) + ",";
        sql += to_string(s.timeresolution) + ",";
        sql += to_string(s.output_interval_minutes) + ",";
        sql += to_string(s.output_interval_days) + ",";
        sql += to_string(s.numberofiterations) + ",";
        sql += to_string(s.lockedmultistore);
        sql += ");";

        result r = W.exec(sql.c_str());
        W.commit();
    }
    catch (const std::exception& e) {
        cerr << e.what() << std::endl;
        return -1;
    }
    return 0;
};
int transfer_Modified_Switches(vector<switch_set> s) {
    try {
        connection c = initconnection();
        pqxx::work W{ c };

        for (switch_set r : s) {
            string sql = "INSERT INTO modified_switch_values VALUES (default, ";
            sql += to_string(r.key_simulation) + ",";
            sql += to_string(r.id_switch) + ",";
            sql += to_string(r.value);
            sql += ");";
            result r = W.exec(sql.c_str());
        }
        W.commit();
    }
    catch (const std::exception& e) {
        cerr << e.what() << std::endl;
        return -1;
    }

    return 0;

}
int transfer_Modified_SinglePar(vector<singlepar_set> s) {
    try {
        connection c = initconnection();
        pqxx::work W{ c };
        for (singlepar_set r : s) {
            string sql = "INSERT INTO modified_singleparameter_values VALUES (default, ";
            sql += to_string(r.key_simulation) + ",";
            sql += to_string(r.id_singlepar) + ",";
            sql += to_string(r.value);
            sql += ");";
            result r = W.exec(sql.c_str());
        }
        W.commit();
    }
    catch (const std::exception& e) {
        cerr << e.what() << std::endl;
        return -1;
    }

    return 0;

}
int transfer_Modified_VectorPar(vector<vectorpar_set> s) {
    try {
        connection c = initconnection();
        pqxx::work W{ c };
        for (vectorpar_set r : s) {
            string sql = "INSERT INTO modified_vectorparameter_values VALUES (default, ";
            sql += to_string(r.key_simulation) + ",";
            sql += to_string(r.id_vectorpar) + ",";
            sql += to_string(r.values.size()) + ",'{";
            int ncount{ 0 };
            for (float v : r.values) {
                sql += to_string(v);
                if (ncount < r.values.size() - 1) sql += ",";
                ncount++;
            }
            sql += "}');";
            result r = W.exec(sql.c_str());
        }
        W.commit();
    }
    catch (const std::exception& e) {
        cerr << e.what() << std::endl;
        return -1;
    }

    return 0;

}
int transfer_Modified_SingleOutput(vector<singleoutput_set> s) {
    try {
        connection c = initconnection();
        pqxx::work W{ c };
        for (singleoutput_set r : s) {
            string sql = "INSERT INTO modified_singleoutputs_storevalues VALUES (default, ";
            sql += to_string(r.key_simulation) + ",";
            sql += to_string(r.id_singleoutput) + ",";
            sql += to_string(r.storeflagvalue);
            sql += ");";
            W.exec(sql.c_str());
            sql = "INSERT INTO modified_singleoutputs_resultvalues VALUES (default, ";
            sql += to_string(r.key_simulation) + ",";
            sql += to_string(r.id_singleoutput) + ",";
            sql += to_string(r.initial) + ",";
            sql += to_string(r.final) + ",";
            sql += to_string(r.min) + ",";
            sql += to_string(r.max) + ",";
            sql += to_string(r.mean) + ",";
            sql += to_string(r.cumulated) + ",";
            sql += to_string(r.singlerun_out_index) + ",";
            sql += to_string(r.multirun_out_index) + ");";
            result r = W.exec(sql.c_str());
        }
        W.commit();
    }
    catch (const std::exception& e) {
        cerr << e.what() << std::endl;
        return -1;
    }

    return 0;

}
int transfer_Modified_VectorOutput(vector<vectoroutput_set> s) {
    try {
        connection c = initconnection();
        pqxx::work W{ c };
        for (vectoroutput_set r : s) {
            string sql = "INSERT INTO modified_vectoroutputs_storevalues VALUES (default, ";
            sql += to_string(r.key_simulation) + ",";
            sql += to_string(r.id_vectoroutput) + ",";

            sql += to_string(r.storeflags.size()) + ",'{";
            int ncount{ 0 };
            for (int v : r.storeflags) {
                sql += to_string(v);
                if (ncount < r.storeflags.size() - 1) sql += ",";
                ncount++;
            }
            sql += "}');";
            W.exec(sql.c_str());
            sql = "INSERT INTO modified_vectoroutputs_resultvalues VALUES (default, ";
            sql += to_string(r.key_simulation) + ",";
            sql += to_string(r.id_vectoroutput) + ",";
            sql += to_string(r.selectedindex.size()) + ",'{";
            ncount = 0;
            for (int si : r.selectedindex) {
                sql += to_string(si);
                if (ncount < r.selectedindex.size() - 1) sql += ",";
                ncount++;
            }
            sql += "}','{";
            auto insertvector = [&sql](vector<float> v) {

                int ncount = 0;
                for (float vv : v) {
                    sql += to_string(vv);
                    if (ncount < v.size() - 1) sql += ",";
                    ncount++;
                }
                sql += "}'";
            };
            auto insertvector_i = [&sql](vector<int> v) {

                int ncount = 0;
                for (int vv : v) {
                    sql += to_string(vv);
                    if (ncount < v.size() - 1) sql += ",";
                    ncount++;
                }
                sql += "}'";
            };

            insertvector(r.initials);
            sql += ",'{";
            insertvector(r.finals);
            sql += ",'{";
            insertvector(r.mins);
            sql += ",'{";
            insertvector(r.maxs);
            sql += ",'{";
            insertvector(r.means);
            sql += ",'{";
            insertvector(r.cumulateds);
            sql += ",'{";
            insertvector_i(r.singlerun_out_index);
            sql += ",'{";
            insertvector_i(r.multirun_out_index);
            sql += ");";
            W.exec(sql.c_str());

        }
        W.commit();
    }
    catch (const std::exception& e) {
        cerr << e.what() << std::endl;
        return -1;
    }

    return 0;

}
int transfer_Modified_TimeSeries(timeserie_set& r, vector<tuple<int, int, vector<float>>>& pg) {
    try {
        connection c = initconnection();
        pqxx::work W{ c };
        {
            string sql = "INSERT INTO modified_timeseries_inputs VALUES (default, ";
            sql += to_string(r.key_simulation) + ",";
            sql += to_string(r.id_timeserie) + ",'";
            sql += r.filename;
            sql += "');";
            W.exec(sql.c_str());
        }

        {
            int id_fileName;
            string sql;
            try {
                sql = "SELECT Id_FileName FROM FileNameArchive WHERE FileName=";
                sql += "'" + r.filename + "'";
                result rr = W.exec(sql.c_str());
                id_fileName = 0;
                for (auto row : rr) {
                    id_fileName = row[0].as<int>();
                }
            }
            catch (const std::exception& e) {
                id_fileName = 0;
                cerr << e.what() << std::endl;
                W.commit();
                c = initconnection();
                work W{ c };

            }
            if (id_fileName == 0 && r.NumVar != 0) {

                auto insertvector = [&sql](vector<string> s) {
                    int count = 0;
                    for (string str : s) {
                        count++;
                        if (str.size() > 0) sql += '"' + str + '"';
                        else sql += "null";
                        if (count < s.size()) sql += ",";
                    }
                };
                auto insertvector_r = [&sql](vector<float> s) {
                    int count = 0;
                    for (float flt : s) {
                        count++;
                        if (flt > -1.E37) sql += to_string(flt);
                        else sql += "null";
                        if (count < s.size()) sql += ",";
                    }
                };
                auto insertvector_d = [&sql](vector<double> s) {
                    int count = 0;
                    for (double flt : s) {
                        count++;
                        if (flt > -1.E37) sql += to_string(flt);
                        else sql += "null";
                        if (count < s.size()) sql += ",";
                    }
                };
                string shortname;
                auto posend = r.filename.rfind(".");
                auto pos = r.filename.rfind("/");
                if (pos == string::npos)  pos = r.filename.rfind("\\");
                if (posend > pos) {
                    shortname = r.filename.substr(pos + 1, posend - pos - 1);
                }

                sql = "INSERT INTO FileNameArchive VALUES (default,'";
                sql += r.filename + "','";
                sql += shortname + "',";
                sql += to_string(r.NumVar) + ",";
                sql += to_string(r.NumRec) + ",";
                sql += to_string(r.NumRepetions) + ",";
                sql += to_string(r.NormalTimeStep) + ",'{";

                insertvector(r.Name); sql += "}','{"; insertvector(r.Units); sql += "}','{"; insertvector(r.Id);
                sql += "}','{"; insertvector(r.Pos); sql += "}','{";
                insertvector_r(r.Min); sql += "}','{"; insertvector_r(r.Max); sql += "}','{";
                insertvector(r.Country); sql += "}','{"; insertvector(r.Station); sql += "}','{";
                insertvector_d(r.Latitude); sql += "}','{"; insertvector_d(r.Longitude); sql += "}','{"; insertvector_d(r.Altitude);
                sql += "}'";
                sql += ") returning Id_FileName; ";
                result rr = W.exec(sql.c_str());
                for (auto row : rr)  id_fileName = row[0].as<int>();


                for (size_t i = 0; i < r.NumVar; i++) {
                    sql = "INSERT INTO FileNameArchive_Descriptions VALUES (";
                    sql += to_string(id_fileName) + ",";
                    sql += to_string(i + 1) + ",'";
                    sql += r.Name[i] + "','";
                    sql += r.Units[i] + "','";
                    sql += r.Id[i] + "','";
                    sql += r.Pos[i] + "','";
                    sql += r.Country[i] + "','";
                    sql += r.Station[i] + "',";
                    sql += to_string(r.Latitude[i]) + ",";
                    sql += to_string(r.Longitude[i]) + ",";
                    sql += to_string(r.Altitude[i]) + ");";
                    result rr = W.exec(sql.c_str());
                }

                for (const tuple<int, int, vector<float>>rec : pg) {
                    sql = "INSERT INTO FileNameArchive_Data VALUES (";
                    sql += to_string(id_fileName) + ",";
                    sql += to_string(get<1>(rec)) + ",'{";
                    vector<float> var = get<2>(rec);
                    for (int i = 0; i < r.NumVar; i++) {
                        sql += to_string(get<2>(rec)[i]);
                        if (i < r.NumVar - 1) sql += ",";
                    }
                    sql += "}');";
                    W.exec(sql.c_str());
                }

            }

            if (r.NumVar != 0) {
                sql = "INSERT INTO filenamearchive_uses VALUES (";
                sql += to_string(r.key_simulation) + ",";
                sql += to_string(id_fileName) + ");";
                W.exec(sql.c_str());
            }
        }



        W.commit();
    }
    catch (const std::exception& e) {
        cerr << e.what() << std::endl;
        return -1;
    }

};
int transfer_Validation(int simkey, vector< vector<string>>& v) {

    try {
        connection c = initconnection();
        pqxx::work W{ c };
        for (vector<string> inner : v) {
            string sql = "INSERT INTO validation VALUES ( ";
            sql += to_string(simkey) + ",";
            size_t count = 0;
            for (string str : inner) {
                sql += str;
                count++;
                if (count < inner.size()) sql += ",";
            }
            sql += ");";
            W.exec(sql.c_str());
        }
        W.commit();
        return 0;
    }



    catch (const std::exception& e) {
        cerr << e.what() << std::endl;
        return -1;
    }
};
int transfer_dynamic_parameters(int pkey, vector < tuple<int, int, string, int, int, vector<int>, vector<float>, vector<string>>>& t) {
    try {
        connection c = initconnection();
        pqxx::work W{ c };
        for (const tuple<int, int, string, int, int, vector<int>, vector<float>, vector<string>> first : t) {
            string sql = "INSERT INTO dynamic_parameters VALUES (";
            sql += to_string(pkey) + ",";
            sql += to_string(get<0>(first)) + ",";
            sql += to_string(get<1>(first)) + ",'";
            sql += to_string(get<2>(first)) + "',";
            sql += to_string(get<3>(first)) + ",";
            sql += to_string(get<4>(first)) + ",'{";

            vector<int> min; min = get<5>(first);
            size_t count = 0;
            for (const int value : min) {
                sql += to_string(value);
                count++; if (count < min.size()) sql += ",";
            }
            sql += "}','{"; count = 0;

            vector<float> values; values = get<6>(first);
            for (const float value : get<6>(first)) {
                sql += to_string(value);
                count++; if (count < values.size()) sql += ",";
            }
            sql += "}','{"; count = 0;
            vector<string> strvalues; strvalues = get<7>(first);
            for (const string value : get<7>(first)) {
                sql += "'" + value + "'";
                count++; if (count < strvalues.size()) sql += ",";
            }
            sql += "}');";
            W.exec(sql.c_str());

        }
        W.commit();
    }

    catch (const std::exception& e) {
        cerr << e.what() << std::endl;
        return -1;
    }
};
int transfer_multirun_setting(int pkey, vector <tuple<int, int, string, int, int, int, float, float, float, string, int, vector<float>, vector<string>, vector<string>, int >>& mr) {
    try {
        connection c = initconnection();
        pqxx::work W{ c };
        for (const tuple<int, int, string, int, int, int, float, float, float, string, int, vector<float>, vector<string>, vector<string>, int> first : mr) {
            string sql = "INSERT INTO multirun_settings VALUES (";
            sql += to_string(pkey) + ",";
            sql += to_string(get<0>(first)) + ",";
            sql += to_string(get<1>(first)) + ",'";
            sql += get<2>(first) + "',";
            sql += to_string(get<3>(first)) + ",";
            sql += to_string(get<4>(first)) + ",";
            sql += to_string(get<5>(first)) + ",";
            sql += to_string(get<6>(first)) + ",";
            sql += to_string(get<7>(first)) + ",";
            sql += to_string(get<8>(first)) + ",";
            if (get<9>(first).size() > 0)
                sql += "'" + get<9>(first) + "',";
            else
                sql += "Null,";
            sql += to_string(get<10>(first)) + ",'{";

            vector<float> values; values = get<11>(first);
            size_t count = 0;
            for (const float value : values) {
                sql += to_string(value);
                count++; if (count < values.size()) sql += ",";
            }
            sql += "}','{"; count = 0;

            vector<string> strvalues; strvalues = get<12>(first);
            for (const string value : get<12>(first)) {
                sql += "'" + to_string(value) + "'";
                count++; if (count < values.size()) sql += ",";
            }
            sql += "}','{"; count = 0;
            vector<string> strvalues2; strvalues2 = get<13>(first);
            for (const string value : get<13>(first)) {
                sql += "'" + value + "'";
                count++; if (count < strvalues.size()) sql += ",";
            }
            sql += "}',";
            sql += to_string(get<14>(first)) + ");";
            W.exec(sql.c_str());
        }
        W.commit();
    }
    catch (const std::exception& e) {
        cerr << e.what() << std::endl;
        return -1;
    }

}
int transfer_MultiStorage(int pkey, NewMap* pDoc) {
    try {
        connection c = initconnection();
        pqxx::work W{ c };
        string sql;
        bool accepted = true;
        for (size_t RunNo = 0; RunNo < pDoc->m_MStorage.GetNumberOfRuns(); RunNo++) {
            sql = "INSERT INTO multirun_Results VALUES (";
            sql += to_string(pkey) + ",";
            sql += to_string(RunNo + 1) + ",";
            sql += to_string(accepted) + ",'{";
            size_t count = 0;
            size_t npar = pDoc->MR_Get_NumTotalPar();
            for (size_t i = 0; i < npar; ++i) {
                sql += to_string(pDoc->m_MStorage.GetMultiP(RunNo, i));
                count++;
                if (count < npar) sql += ",";
            }
            sql += "}','{"; count = 0;
            size_t nvalid = pDoc->GetNumAll_TSV_ValVariables();
            for (size_t i = 0; i < nvalid; ++i) {
                sql += to_string(pDoc->m_MStorage.GetValid(RunNo, i));
                count++;
                if (count < nvalid) sql += ",";
            }
            sql += "}','{"; count = 0;
            size_t nsumvalid = pDoc->m_ValidationData.GetNumSumVarVariables() * 7;
            for (size_t i = 0; i < nsumvalid; ++i) {
                sql += to_string(pDoc->m_MStorage.GetValidSum(RunNo, i));
                count++;
                if (count < nsumvalid) sql += ",";
            }
            sql += "}','{"; count = 0;


            for (size_t i = 0; i < pDoc->m_MStorage.GetsizeOfSumVar(); ++i) {
                sql += to_string(pDoc->m_MStorage.GetSumV(RunNo, i));
                count++;
                if (count < pDoc->m_MStorage.GetsizeOfSumVar()) sql += ",";
            }
            sql += "}');";
            W.exec(sql.c_str());
        }
        W.commit();
        return 0;
    }

    catch (const std::exception& e) {
        cerr << e.what() << std::endl;
        return -1;
    }
}
int transfer_Residuals(int pkey, NewMap* pDoc) {
    try {
        connection c = initconnection();
        pqxx::work W{ c };
        string sql;
        bool accepted = true;


        size_t nvalid = pDoc->GetNumAll_TSV_ValVariables();
        for (int i = 0; i < nvalid; i++) {
            auto FileNo = pDoc->m_ValidationData.GetValFileNumber(i);
            auto pValFile = pDoc->ValidationFilePointer(FileNo);
            auto ind = pDoc->m_ValidationData.GetValFileIndex(i);
            string filename = pDoc->GetXBinFileName(ind + 1);
            auto pRes = (CResiduals*)pDoc->GetResidualPointer(FileNo);
            if (pRes->OpenFile(filename, true)) {

                for (size_t RunNo = 0; RunNo < pDoc->m_MStorage.GetNumberOfRuns(); RunNo++) {


                    vector<float> v_res = pRes->GetResidualVector(RunNo, i + 1);

                    sql = "INSERT INTO multirun_Residuals VALUES (";
                    size_t count = 0;
                    sql += to_string(pkey) + ",";
                    sql += to_string(RunNo + 1) + ",";
                    sql += to_string(FileNo + 1) + ",";
                    sql += to_string(ind + 1) + ",'{";
                    for (float value : v_res) {
                        count++;
                        sql += to_string(value);
                        if (count < v_res.size()) sql += ",";
                    }
                    sql += "}');";
                    W.exec(sql.c_str());

                }
                pRes->CloseResidualFile(true);
            }
        }
        W.commit();
        return 0;
    }

    catch (const std::exception& e) {
        cerr << e.what() << std::endl;
        return -1;
    }
}
int transfer_ensemble_statistics(int pkey, NewMap* pDoc) {
    try {
        connection c = initconnection();
        pqxx::work W{ c };
        string sql;

        int runno = int(pDoc->m_DocFile.m_SimulationRunNo);
        int ensemble_no = 1;
        size_t nvalid = pDoc->GetNumAll_TSV_ValVariables();

        for (int perf_ind = 1; perf_ind < 8; perf_ind++) {
            int index_start = (perf_ind - 1) * nvalid;
            int split_ind = 0;

            for (int i = 0; i < nvalid; i++) {

                int criteria = pDoc->m_MStorage.GetCriteria(i + index_start, split_ind);
                float allmax = pDoc->m_MStorage.GetMaxValidAll(i + index_start, split_ind);
                float allmin = pDoc->m_MStorage.GetMinValidAll(i + index_start, split_ind);
                float allmean = pDoc->m_MStorage.GetMeanValidAll(i + index_start, split_ind);

                float allcv = pDoc->m_MStorage.GetCV_ValidAll(i + index_start, split_ind);
                float selmax = pDoc->m_MStorage.GetMaxValid(i + index_start, split_ind);
                float selmin = pDoc->m_MStorage.GetMinValid(i + index_start, split_ind);
                float selcv = pDoc->m_MStorage.GetCV_Valid(i + index_start, split_ind);
                float selmean = pDoc->m_MStorage.GetMeanValid(i + index_start, split_ind);

                float lowvalue = pDoc->m_MStorage.GetValidLow(i + index_start, split_ind);
                float highvalue = pDoc->m_MStorage.GetValidHigh(i + index_start, split_ind);

                auto FileNo = pDoc->m_ValidationData.GetValFileNumber(i);
                auto pValFile = pDoc->ValidationFilePointer(FileNo);
                auto ind = pDoc->m_ValidationData.GetValFileIndex(i);
                string filename = pDoc->GetXBinFileName(ind + 1);
                auto pRes = (CResiduals*)pDoc->GetValidResidualPointer(FileNo);

                sql = "INSERT INTO multirun_Ensemble_Statistics VALUES (";
                size_t count = 0;
                sql += to_string(pkey) + ",";
                sql += to_string(runno) + ",";
                sql += to_string(ensemble_no) + ",";
                sql += to_string(FileNo + 1) + ",";
                sql += to_string(false) + ",";
                sql += to_string(ind + 1) + ",";
                sql += to_string(perf_ind) + ",";
                sql += to_string(pDoc->m_ValidationData.GetValn(i)) + ",";
                sql += to_string(allmean) + ",";
                sql += to_string(allcv) + ",";
                sql += to_string(allmin) + ",";
                sql += to_string(allmax) + ",";
                sql += to_string(selmean) + ",";
                sql += to_string(selcv) + ",";
                sql += to_string(selmin) + ",";
                sql += to_string(selmax) + ",";
                sql += to_string(lowvalue) + ",";
                sql += to_string(highvalue) + ");";
                W.exec(sql.c_str());
            }
        }

        auto save_ens = pDoc->m_MStorage.m_ActualEnsemble;

        // defined critera
        for (int var = 0; var < 7; var++) {
            auto  MeanCriteria = pDoc->m_MStorage.m_Valid_SV_Mean[pDoc->m_MStorage.m_ActualEnsemble].criteria[var];
            for (int split_ind = 0; split_ind <= pDoc->m_MStorage.GetNumSplitPeriods(); split_ind++) {
                for (size_t iens = 0; iens < pDoc->m_MStorage.m_AcceptedRuns.size(); iens++) {
                    pDoc->m_MStorage.m_ActualEnsemble = iens;
                    for (int i = 0; i < nvalid; i++) {
                        auto index = i + var * nvalid;
                        auto criteria = pDoc->m_MStorage.GetCriteria(index, split_ind);
                        if (criteria > 0) {
                            sql = "INSERT INTO multirun_Ensemble_DefinedCriteria VALUES (";
                            sql += to_string(pkey) + ",";
                            sql += to_string(runno) + ",";
                            sql += to_string(iens + 1) + ",";

                            auto FileNo = pDoc->m_ValidationData.GetValFileNumber(i);
                            auto pValFile = pDoc->ValidationFilePointer(FileNo);
                            auto ind = pDoc->m_ValidationData.GetValFileIndex(i);
                            CPG* pPG = (CPG*)pValFile->GetPointer();
                            if (pPG == nullptr) {
                                pPG = new CPG;
                                bool valid = pPG->ReadContentOfPGFileAndOpenMainPGStreamForReading(pValFile->GetStrValue().c_str());
                                if (valid) pValFile->SetPointer(pPG);
                                pPG->CloseFile();
                            }
                            auto test = pValFile->GetDataDescription(ind);

                            if (pDoc->m_MStorage.m_Valid_TSV[iens].VarTranformed != 0) {
                                test += " - ";
                                test += pDoc->m_MStorage.GetTransformationIndicator(iens);
                            }
                            sql += test + ",";
                            if (criteria == 1)
                                sql += to_string(pDoc->m_MStorage.GetValidLow(index, split_ind)) + ",null)";
                            else if (criteria == 2)
                                sql += "Null," + to_string(pDoc->m_MStorage.GetValidHigh(index, split_ind));
                            else if (criteria == 3) {
                                sql += to_string(pDoc->m_MStorage.GetValidLow(index, split_ind)) + ",";
                                sql += to_string(pDoc->m_MStorage.GetValidHigh(index, split_ind));
                            }
                            W.exec(sql.c_str());
                        }
                    }
                }
            }
        }

        // defined ensembles
        for (int i = 0; i < nvalid; i++) {

            auto FileNo = pDoc->m_ValidationData.GetValFileNumber(i);
            auto pValFile = pDoc->ValidationFilePointer(FileNo);
            auto ind = pDoc->m_ValidationData.GetValFileIndex(i);
            auto pRes = (CResiduals*)pDoc->GetResidualPointer(FileNo);


            CPG* pPG = (CPG*)pValFile->GetPointer();
            if (pPG == nullptr) {
                pPG = new CPG;
                bool valid = pPG->ReadContentOfPGFileAndOpenMainPGStreamForReading(pValFile->GetStrValue().c_str());
                if (valid) pValFile->SetPointer(pPG);
                pPG->CloseFile();
            }
            auto ntimepoints = pPG->GetNumRecords();

            if (pRes != nullptr) {
                vector<unsigned int> pgminvector;
                for (size_t rec = 0; rec < ntimepoints; rec++) {
                    sql += to_string(pPG->GetLongTime(i));
                    if (rec < ntimepoints - 1) sql += ",";
                }
                //for (size_t rec = 0; rec < ntimepoints; rec++) {
                //    sql += to_string(pRes->GetMeanAllResiduals(ind,rec));
                //    if (rec < ntimepoints - 1) sql += ",";
                //}



            }
        }


        W.commit();
        return 0;
    }

    catch (const std::exception& e) {
        cerr << e.what() << std::endl;
        return -1;
    }
}

