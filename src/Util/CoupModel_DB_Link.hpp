#pragma once

#ifdef COUP_POSTGRES

#include <stddef.h>

#include <pqxx/pqxx>
#include "./DB_Doc.hpp"
#include "./FUtil.hpp"



using namespace pqxx;


namespace coup_pg {
    static bool LocalHost{ false };
    static bool MakeMultiRunIfPossible{ false };
    static connection_par local;
    static connection_par remote;
    

    enum class DB_RUN_ACTION { RUN_STORE_ON_CURRENT, RUN_STORE_AS_NEWRECORD };
    static DB_RUN_ACTION db_action=DB_RUN_ACTION::RUN_STORE_ON_CURRENT;

    static bool checkLocalCredentials(string dbname, string user, string password, int port) {
        string init;
        try {
            init = "dbname = " + dbname + " user = " + user + " password = " + password + " port = " + to_string(port);
            connection c(init);
            if (c.is_open()) {
                c.close();
                local.dbname = dbname; local.user = user; local.password = password; local.port = to_string(port);
                return true;
            }
            else {
                c.close();
                return false;
            }
        }
        catch (const std::exception& e) {
            cerr << e.what() << std::endl;
            cerr << init << std::endl;
            return false;
        }
   
    }
    static bool checkRemoteCredentials(string dbname, string user, string password, string host) {
        string init;
        try {
            init = "dbname = " + dbname + " user = " + user + " password = " + password + " host = " + host;
            connection c(init);
            if (c.is_open()) {
                c.close();
                remote.dbname = dbname; remote.user = user; remote.password = password; remote.host = host;
                return true;
            }
            else {
                c.close();
                return false;
            }
        }
        catch (const std::exception& e) {
            cerr << e.what() << std::endl;
            cerr << init << std::endl;
            return false;
        }
    }
    
    static connection initconnection(string work_to_be) {
        string init, ans;
        
        

        if(LocalHost)
            init = "dbname = " + local.dbname + " user = " + local.user + " password = " + local.password + " port = " + local.port;
        else 
            init = "dbname = " + remote.dbname + " user = " + remote.user + " password = " + remote.password + " host = " + remote.host;
//            init = "dbname = teyojvga  user = teyojvga password = lzigAYICibNNBU-Aefp4PszeBl9DkMEs host = "balarama.db.elephantsql.com";

        connection c(init);
        if (c.is_open()) {
            cout << "Open database " << c.dbname() <<" : " << work_to_be << endl;
         //   cin >> ans;
        }
        else {
            cout << "Can't open database" << endl;
        }
        return c;
    };
    
static bool uploadpgFile(std::string filename) {
    CPG pgfile;
    pgfile.Open(filename);
    if (pgfile.IsGood()) {

        string str = pgfile.GetFileName(true);
        auto pos = str.find(".bin");
        if (pos == string::npos) return 1;
        string tablename = str.substr(0, pos);
        size_t numvar = pgfile.GetNumVariables();
        vector<string> pgvar;
        for (size_t i = 0; i < numvar; i++) {
            string name = pgfile.GetVarName(i + 1);
            string id = pgfile.GetVarId(i + 1);
            string newid;
            FUtil::trim(name); FUtil::trim(id);
            newid = name;
            if (id.size() > 0) name = "_" + id;
            if (i > 0) {
                if (pgvar[0] == newid) {
                    newid += "_" + FUtil::STD_ItoAscii(i + 1);
                }
            }
            pgvar.push_back(newid);

        }

        connection c = initconnection("Create timeserie table");

        pqxx::work W{ c };
        try {
            string sql = "CREATE TABLE ";
            sql += "TimeSeries (";
            sql += "Id_TimeSeries SERIAL PRIMARY KEY,";
            sql += "name VARCHAR(128),";
            sql += "NumVar Integer,";
            sql += "NumRecords Integer,";
            sql += "NumRepetions Integer,";
            sql += "StartMin Integer,";
            sql += "EndMin Integer,";
            sql += "Unique_TimeStep Integer);";
            W.exec(sql.c_str());
            W.commit();






            sql = "CREATE TABLE ";
            sql += tablename + "_data (";
            sql += "PGMin INTEGER  PRIMARY KEY,";
            for (size_t i = 0; i < numvar; i++) {
                sql += pgvar[i] + " REAL";
                if (i < numvar - 1) sql += ",";
            }
            sql += ");";
            W.exec(sql.c_str());
            W.commit();
            c = initconnection("insert data");
            pqxx::work W{ c };
            for (size_t i = 0; i < pgfile.GetNumRecords(); i++) {
                sql = "INSERT INTO " + tablename + "_data VALUES ( ";
                sql += FUtil::STD_ItoAscii(pgfile.GetLongTime(i + 1)) + ",";
                for (size_t j = 0; j < numvar; j++) {
                    string strv = FUtil::STD_FtoAscii(pgfile.GetVarValue(j + 1, i + 1));
                    if (strv == "") sql += "NULL";
                    else sql += strv;
                    if (j < numvar - 1) sql += ",";
                }
                sql += ");";
                W.exec(sql.c_str());
            }


            for (size_t i = 0; i < pgfile.GetNumRecords(); i++) {
                sql = "INSERT INTO " + tablename + "_data VALUES ( ";
                sql += FUtil::STD_ItoAscii(pgfile.GetLongTime(i + 1)) + ",";
                for (size_t j = 0; j < numvar; j++) {
                    string strv = FUtil::STD_FtoAscii(pgfile.GetVarValue(j + 1, i + 1));
                    if (strv == "") sql += "NULL";
                    else sql += strv;
                    if (j < numvar - 1) sql += ",";
                }
                sql += ");";
                W.exec(sql.c_str());
            }
            W.commit();

        }
        catch (const std::exception& e) {
            cerr << e.what() << std::endl;
            return 1;
        }

        return 0;
    }
    return 1;
};
static bool load_pg_file(int pkey, map<string, CPG*> links_to_pg_pointers) {
    connection c = initconnection("load data to timeserie stucture");
    pqxx::work txn{ c };

    pqxx::result rr = txn.exec("SELECT id_filename, filename, numvar, numrecords FROM filenamearchive  WHERE id_filename = " + to_string(pkey));

    int numvar = 0, numrec = 0;
    string filename;
    for (auto row_inner : rr) {
        numvar = row_inner["numvar"].as<int>();
        numrec = row_inner["numrecords"].as<int>();
        filename = row_inner["filename"].as<string>();
    }
    auto it=links_to_pg_pointers.find(filename);
    CPG* pPG;
    if (it != links_to_pg_pointers.end())
        pPG = it->second;
    else
        pPG = nullptr;
    if (pPG != nullptr) {
        pPG->SetFileName(filename);
        pPG->SetCompleteRead(true);
        pPG->AdjustSize(numrec, numvar, 1);
    }
    else
        return false;
    rr = txn.exec("SELECT varno, name, i_unit,unit, id, pos, country, station, latitude, longitude, altitude FROM filenamearchive_descriptions  WHERE id_filename = " + to_string(pkey));
    for (auto row_inner : rr) {
        int varno = row_inner["varno"].as<int>();
        string name = row_inner["name"].as<string>();
        int i_unit = row_inner["i_unit"].as<int>();
        string unit = row_inner["unit"].as<string>();
        string id = row_inner["id"].as<string>();
        string pos = row_inner["pos"].as<string>();
        string country = row_inner["country"].as<string>();
        string station = row_inner["station"].as<string>();
        auto latitude = row_inner["latitude"].as<double>();
        auto longitude = row_inner["longitude"].as<double>();
        auto altitude = row_inner["altitude"].as<double>();
        pPG->SetVarName(varno, name);
        pPG->SetVarUnit(varno, unit);
        pPG->SetVarId(varno, id);
        pPG->SetVarPos(varno, pos);
        pPG->SetVarCountry(varno, country);
        pPG->SetVarStation(varno, station);
        pPG->SetVarLat(varno, latitude);
        pPG->SetVarLong(varno, longitude);
        pPG->SetVarAlt(varno, altitude);
   }

    rr = txn.exec("SELECT pgmintime, pgvarvalues FROM filenamearchive_data WHERE id_filename = " + to_string(pkey) + " ORDER BY pgmintime ASC");
    size_t recno = 1;
    for (auto row_inner : rr) {
        size_t min = row_inner[0].as<int>();
        pPG->SetLongTime(recno, min);
        auto pvector = row_inner[1].as_array();
        size_t count = 1;
        auto next = pvector.get_next();
        next = pvector.get_next();
        while (next.second.size() > 0) {

            float value;
            while (next.second.size() > 0) {
                value = stof(string(next.second));
                pPG->SetVarValue(count, recno, value);
                next = pvector.get_next();
                count++;
            }
        }
        recno++;
    }
    if (recno - 1 == numrec) {
        pPG->SetAllValuesAssigned(true);
        if(pPG->CheckStatus())
            cout << "All values assigned to : " << filename << endl;
        else
            cout << "All values are not correctly assigned to : " << filename << endl;
    }
    return true;


}
static bool download_pg_file(int pkey, string localdirectory, bool ExportToCSV, CommonModelInfo* pinfo) {

   
    connection c = initconnection("create pg file");
    pqxx::work txn{ c };
   
    pqxx::result rr = txn.exec("SELECT id_filename, filename, numvar, numrecords FROM filenamearchive  WHERE id_filename = " + to_string(pkey));

    int numvar=0, numrec=0;
    string filename;
    for (auto row_inner : rr) {
        numvar = row_inner["numvar"].as<int>();
        numrec = row_inner["numrecords"].as<int>();
        filename = row_inner["filename"].as<string>();
    }
    if (localdirectory.size() > 0) {
         filename=localdirectory+ FUtil::FileNameOnly(filename);
    }
    else {
        if (!FUtil::IsFileExisting(filename)) {
            auto path = FUtil::GetCurrentPath();
            filename = path + FUtil::FileNameOnly(filename);
        }
    }

    CPG pg(filename);


    pg.SetCompleteRead(true);
    pg.AdjustSize(numrec, numvar,1);


    rr = txn.exec("SELECT varno, name, i_unit,unit, id, pos, country, station, latitude, longitude, altitude FROM filenamearchive_descriptions  WHERE id_filename = " + to_string(pkey));
    for (auto row_inner : rr) {
        int varno = row_inner["varno"].as<int>();
        string name = row_inner["name"].as<string>();
        int i_unit = row_inner["i_unit"].as<int>();
        string unit = row_inner["unit"].as<string>();
        string id = row_inner["id"].as<string>();
        string pos = row_inner["pos"].as<string>();
        string country = row_inner["country"].as<string>();
        string station = row_inner["station"].as<string>();
        auto latitude = row_inner["latitude"].as<double>();
        auto longitude = row_inner["longitude"].as<double>();
        auto altitude = row_inner["altitude"].as<double>();
        pg.SetVarName(varno, name);
        if (i_unit > 0 && i_unit < 100) {
            unit = pinfo->GetUnitString(UNIT_TYPES(i_unit));
        }
        pg.SetVarUnit(varno, unit);
        pg.SetVarId(varno, id);
        pg.SetVarPos(varno, pos);
        pg.SetVarCountry(varno, country);
        pg.SetVarStation(varno, station);
        pg.SetVarLat(varno, latitude);
        pg.SetVarLong(varno, longitude);
        pg.SetVarAlt(varno, altitude);
    }
    rr = txn.exec("SELECT pgmintime, pgvarvalues FROM filenamearchive_data WHERE id_filename = " + to_string(pkey)+" ORDER BY pgmintime ASC");

    auto numrec_koll = size(rr);
    if (numrec_koll > numrec) {
        pg.AdjustSize(numrec_koll, numvar, 1);
    }
    size_t recno = 1;
    for (auto row_inner : rr) {
        size_t min = row_inner[0].as<int>();
        pg.SetLongTime(recno, min);
        auto pvector = row_inner[1].as_array();
        size_t count = 1;
        auto next = pvector.get_next();
        next = pvector.get_next();
       
        while (next.second.size() > 0) {

            float value;
            while (next.second.size() > 0) {
                value = stof(string(next.second));
                pg.SetVarValue(count, recno, value);
                next = pvector.get_next();
                count++;
            }
        }
        recno++;
    }
    if (ExportToCSV) {
        string str = pg.GetFileName();
        auto pos = str.rfind('.');
        string newname;
        if (pos != string::npos)
            newname = str.substr(0, pos + 1) + "csv";
        else
            newname = str + ".csv";
        str=pg.ExportToFile(newname);
        if (str.size() > 0) {
            cout << "time serie file created : " << str << endl;
            return true;
        }
        else
            return false;

    }
    else if (pg.ReCalculateMinMax()) {
        if (filename.find('.') == string::npos) {
            filename += ".BIN";
        }
        pg.SaveAs(filename, true);
        pg.CloseFile();
        cout << "pg file created : " << filename << endl;
        return true;
    }
    else {
        cout << "pg file write error : " << filename << endl;
        return false;
    }
    
}
static pqxx::result query(string tablename)
{
    connection c = initconnection("query table");
    pqxx::work txn{ c };

    pqxx::result r{ txn.exec("SELECT id_group, vectorname, storeflag, conditions[1] FROM " + tablename + " WHERE id_group > 0 AND id_group < 10\
 ORDER BY vectorname ASC") };
    for (auto row : r) {
        std::cout
            // Address column by name.  Use c_str() to get C-style string.
            << row[1].c_str() << row[2].c_str() << endl;




    }

    // Not really needed, since we made no changes, but good habit to be
    // explicit about when the transaction is done.
    txn.commit();

    // Connection object goes out of scope here.  It closes automatically.
    return r;
}
vector<string> create_Init_Tables(CommonModelInfo* pinfo) {
    connection c = initconnection( "Basic Tables");
    pqxx::work W{ c };
    string sql;
    string tablename;
    auto drop = [&](pqxx::work &W, string name) {; string s = "DROP TABLE IF EXISTS " + name + " CASCADE;"; return W.exec(s.c_str()); };
    auto create = [&](string name) { string s = "CREATE TABLE " + name; return s; };
    auto ins_v = [&](pqxx::work& W, string name, vector<string> vstr) { string s; for (int i = 0; i < vstr.size(); i++) { s = "INSERT INTO " + name + " VALUES (" + std::to_string(i) + ",'" + vstr[i] + "');"; W.exec(s); } return name; };
    vector<string> tablenames;
   
    try {
        tablename = "GroupCategories";
        drop(W,tablename);
        sql = create(tablename);
        sql += "( Id_GroupCategory SERIAL PRIMARY KEY, Name VarChar(34) ); ";
        W.exec(sql.c_str());
        auto vv = pinfo->GetGroupCategoryNames();
        auto name=ins_v(W, tablename, vv);
        tablenames.push_back(tablename);

        tablename = "Groups";
        drop(W, tablename);
        sql=create(tablename);
        sql += "(Id_Group SERIAL PRIMARY KEY, Name VarChar(34),Id_GroupCategory Integer REFERENCES GroupCategories(Id_GroupCategory)); ";
        W.exec(sql.c_str());
        auto group = pinfo->GetGroupNames();
        ins_v(W, tablename, group);
        tablenames.push_back(tablename);


        tablename="PhysProcNames";
        drop(W, tablename);
        sql = create(tablename); sql += "(Id_PhysProc SERIAL PRIMARY KEY, Name VarChar(34) ); "; W.exec(sql.c_str());

        vv = pinfo->GetFysProcName();
        ins_v(W, tablename, vv);
        tablenames.push_back(tablename);


        tablename = "BioProcNames";
        drop(W, tablename);
        sql = create(tablename);  sql += "(Id_BioProc SERIAL PRIMARY KEY,Name VarChar(34) ); "; W.exec(sql.c_str());
        vv = pinfo->GetBioProcName();
        ins_v(W, tablename, vv);
        tablenames.push_back(tablename);

        tablename="ElementNames";
        drop(W, tablename);
        sql = create(tablename); sql += "(Id_Element SERIAL PRIMARY KEY,  Name VarChar(34) ); "; W.exec(sql.c_str());
        vv = pinfo->GetElementName();
        ins_v(W, tablename, vv);
        tablenames.push_back(tablename);

        tablename="FileNameArchive";
        drop(W, tablename);
        sql = create(tablename);
        sql += " (Id_FileName SERIAL PRIMARY KEY,";
        sql += "FileName varchar(128) UNIQUE,";
        sql += "ShortName varchar(128),";
        sql += "NumVar Integer,";
        sql += "NumRecords Integer,";
        sql += "NumRepetions Integer,";
        sql += "NormalTimeStep Integer,";
        sql += "Name Varchar(32)[],";
        sql += "I_Units Integer[], ";
        sql += "Id Varchar(24)[], ";
        sql += "Pos Varchar(24)[], ";
        sql += "MinValue REAL[], ";
        sql += "MaxValue REAL[], ";
        sql += "Country Varchar(32)[], ";
        sql += "Station Varchar(32)[], ";
        sql += "Latitude double precision[], ";
        sql += "Longitude double precision[], ";
        sql += "Altitude double precision[]);";
        W.exec(sql.c_str());


        sql = "DROP TABLE IF EXISTS simulations CASCADE;";
        W.exec(sql.c_str());
        tablenames.push_back(tablename);



        tablename="Simulations";
        drop(W, tablename);
        sql = create(tablename);
        sql += "(Id_Simulations SERIAL PRIMARY KEY,";
        sql += "name Varchar(128) UNIQUE,";
        sql += "creator Varchar(128) Default '-',";
        sql += "time TIMESTAMP,";
        sql += "sitename Varchar(32) Default '-',";
        sql += "runno Integer,";
        sql += "comment Varchar(128) Default '-');";
        W.exec(sql.c_str());
        sql = "INSERT INTO Simulations VALUES( ";
        sql += "Default, 'first_test_000001','PEJ',NOW(),Default,1,'When using Default of everything');";
        W.exec(sql.c_str());

        tablenames.push_back(tablename);

        tablename="FileNameArchive_Uses";
        drop(W, tablename);

        sql = create(tablename);
        sql += "(Id_Simulations Integer References Simulations(Id_Simulations),";
        sql += "Id_FileName Integer References FileNameArchive(Id_FileName));";
        W.exec(sql.c_str());
        tablenames.push_back(tablename);


        tablename = "Validation";
        drop(W, tablename);

        sql = create(tablename);
        sql += "(Id_Simulations Integer References Simulations(Id_Simulations),";
        sql += "ValFileIndex Integer ,";
        sql += "output_type Integer ,";
        sql += "groupname varchar(36),";
        sql += "name varchar(32),";
        sql += "LocalIndex Integer,";
        sql += "ValFileNumber Integer ,";
        sql += "ValFile_ResultIndex Integer ,";
        sql += "Monitoring Integer ,";
        sql += "P_Error Real,";
        sql += "A_Error Real,";
        sql += "AccTest bool,";
        sql += "Logtrans bool,";
        sql += "Duration Integer,";
        sql += "n Integer,";
        sql += "R2 Real,";
        sql += "A0 Real,";
        sql += "A1 Real,";
        sql += "ME Real,";
        sql += "RMSE Real,";
        sql += "MEANSIM Real,";
        sql += "MEANVAL Real,";
        sql += "LogLi Real,";
        sql += "NSE Real);";
        W.exec(sql.c_str());
        tablenames.push_back(tablename);

        tablename = "Dynamic_Parameters";
        drop(W, tablename);
        sql = create(tablename);
        sql += "(Id_Simulations Integer References Simulations(Id_Simulations),";
        sql += "ItemType Integer,";
        sql += "Id_Group Integer References Groups(id_group),";
        sql += "name Varchar(32),";
        sql += "vectorindex Integer ,";
        sql += "NumChanges Integer ,";
        sql += "MinDates Integer[],";
        sql += "FloatValues Real[],";
        sql += "StringValue  Varchar[24][]);";
        W.exec(sql.c_str());
        tablenames.push_back(tablename);

        tablename = "FileNameArchive_Descriptions";
        drop(W, tablename);
        sql = create(tablename);
        sql += "(Id_FileName Integer References FileNameArchive(Id_FileName),";
        sql += "VarNo Integer ,";
        sql += "Name Varchar(32),";
        sql += "I_Unit Integer,";
        sql += "Unit Varchar(12),";
        sql += "Id Varchar(24),";
        sql += "Pos Varchar(12),";
        sql += "Country Varchar(12),";
        sql += "Station Varchar(24),";
        sql += "Latitude double precision,";
        sql += "Longitude double precision,";
        sql += "Altitude double precision);";
        W.exec(sql.c_str());
        tablenames.push_back(tablename);


        tablename="FileNameArchive_Data";
        drop(W, tablename);
        sql = create(tablename);
        sql += "(Id_FileName Integer References FileNameArchive(Id_FileName),";
        sql += "PGMinTime Integer,";
        sql += "PGVarValues Real[]);";
        W.exec(sql.c_str());
        tablenames.push_back(tablename);
        W.commit();       
        return tablenames;

    }
    catch (const std::exception& e) {
        cerr << sql.c_str() << std::endl;
        cerr << e.what() << std::endl;
        return tablenames;
    }

};
vector<string> create_Main_Tables(CommonModelInfo* pinfo) {
    connection c = initconnection("output tables");
    string sql;
    vector<string> nametables;
    pqxx::work W{ c };
    try {
        string nametable;
        auto drop = [&](pqxx::work& W, string name) {; string s = "DROP TABLE IF EXISTS " + name + " CASCADE;"; return W.exec(s.c_str()); };
        auto create = [&](string name) { string s = "CREATE TABLE " + name; return s; };


        nametable = "singleoutputs_results";
        drop(W, nametable);

        sql = create(nametable);
        sql += " (";
        sql += "Id_Resultsobtained SERIAL PRIMARY KEY,";
        sql += "Id_Simulations Integer References Simulations(id_Simulations),";
        sql += "Id_singleoutputs integer References singleoutputs(id_singleoutputs),";
        sql += "initial REAL,";
        sql += "final REAL,";
        sql += "min REAL,";
        sql += "max REAL,";
        sql += "mean REAL,";
        sql += "accumulated REAL);";
        W.exec(sql.c_str());
        nametables.push_back(nametable);

        nametable="timeseries_outputs_results";
        drop(W, nametable);
        sql = create(nametable);
        sql += " (Id_Simulations Integer References Simulations(id_Simulations),";
        sql += "PGminTime Integer,";
        sql += "outputs REAL[]);";
        W.exec(sql.c_str());
        nametables.push_back(nametable);

        nametable= "vectoroutputs_results";
        drop(W, nametable);
        sql=create(nametable);
        sql += " (";
        sql += "Id_Resultsobtained SERIAL PRIMARY KEY,";
        sql += "Id_Simulations Integer References Simulations(id_Simulations),";
        sql += "Id_vectoroutputs integer References vectoroutputs(id_vectoroutputs),";
        sql += "NumIndexOut Integer,";
        sql += "IndexOut Integer[],";
        sql += "initial REAL[],";
        sql += "final REAL[],";
        sql += "min REAL[],";
        sql += "max REAL[],";
        sql += "mean REAL[],";
        sql += "accumulated REAL[]);";
        W.exec(sql.c_str());
        nametables.push_back(nametable);


        nametable = "modified_switch_values";
        drop(W, nametable);
        sql = create(nametable);
        sql += " (modifactions SERIAL PRIMARY KEY,";
        sql += "Id_Simulations Integer References Simulations(id_Simulations),";
        sql += "Id_Switch integer References switches(id_switch),";
        sql += "value Integer);";
        W.exec(sql.c_str());
        nametables.push_back(nametable);


        nametable = "modified_singleparameter_values";
        drop(W, nametable);
        
        sql = create(nametable);
        sql += " (modifactions SERIAL PRIMARY KEY,";
        sql += "Id_Simulations Integer References Simulations(id_Simulations),";
        sql += "Id_singlepar integer References singleparameters(id_singlepar),";
        sql += "value real);";
        W.exec(sql.c_str());

        nametables.push_back(nametable);


        nametable = "modified_vectorparameter_values";
        drop(W, nametable);
        sql =create(nametable);
        sql += " (modifactions SERIAL PRIMARY KEY,";
        sql += "Id_Simulations Integer References Simulations(id_Simulations),";
        sql += "Id_vectorpar integer References vectorparameters(id_vectorpar),";
        sql += "ArraySize Integer,";
        sql += "values real[]);";

        W.exec(sql.c_str());
        nametables.push_back(nametable);

        nametable = "modified_singleoutputs_Storevalues";
        drop(W, nametable);
        sql = create(nametable);
        sql += "(results SERIAL PRIMARY KEY,";
        sql += "Id_Simulations Integer References Simulations(id_Simulations),";
        sql += "Id_singleoutputs integer References singleoutputs(id_singleoutputs),";
        sql += "StoreFlag Integer)";
        W.exec(sql.c_str());
        nametables.push_back(nametable);

        nametable = "modified_vectoroutputs_Storevalues";
        drop(W, nametable);
        sql = create(nametable);
        sql += "(results SERIAL PRIMARY KEY,";
        sql += "Id_Simulations Integer References Simulations(id_Simulations),";
        sql += "Id_vectoroutputs integer References vectoroutputs(id_vectoroutputs),";
        sql += "NumberStoreFlag Integer,";
        sql += "StoreFlags Integer[])";
        W.exec(sql.c_str());
        nametables.push_back(nametable);

        nametable = "modified_singleoutputs_resultvalues";
        drop(W,nametable);
        sql = create(nametable);
        sql += " (results SERIAL PRIMARY KEY,";
        sql += "Id_Simulations Integer References Simulations(id_Simulations),";
        sql += "Id_singleoutputs integer References singleoutputs(id_singleoutputs),";
        sql += "initial Real,";
        sql += "final Real,";
        sql += "min Real,";
        sql += "max Real,";
        sql += "mean Real,";
        sql += "accumulated Real,";
        sql += "singlerun_out_index Integer,";
        sql += "multirun_out_index Integer);";
        W.exec(sql.c_str());
        nametables.push_back(nametable);

        nametable = "modified_vectoroutputs_resultvalues";
        drop(W, nametable);
        sql = create(nametable);
        sql += " (modifications SERIAL PRIMARY KEY,";
        sql += "Id_Simulations Integer References Simulations(id_Simulations),";
        sql += "Id_vectoroutputs integer References vectoroutputs(id_vectoroutputs),";
        sql += "NumIndexOut Integer,";
        sql += "IndexOut Integer[],";
        sql += "Initial REAL[],";
        sql += "final REAL[],";
        sql += "min REAL[],";
        sql += "max REAL[],";
        sql += "mean REAL[],";
        sql += "accumulated REAL[],";
        sql += "singlerun_out_index Integer[],";
        sql += "multirun_out_index Integer[]);";
        W.exec(sql.c_str());
        nametables.push_back(nametable);

        nametable = "modified_timeseries_inputs";
        drop(W, nametable);
        sql = create(nametable);
        sql += " (modifications SERIAL PRIMARY KEY,";
        sql += "Id_Simulations Integer References Simulations(id_Simulations),";
        sql += "Id_timeserie integer References timeseriesinputs(id_timeserie),";
        sql += "filename varchar(96));";
        W.exec(sql.c_str());
        nametables.push_back(nametable);

        nametable = "runinfo_categories";
        drop(W, nametable);

        sql = create(nametable);
        sql += "(id_runinfo_cat SERIAL PRIMARY KEY,";
        sql += "name varchar(64));";
        W.exec(sql.c_str());

        sql = "INSERT INTO runinfo_categories VALUES( ";
        sql += "Default, 'General Information');";

        W.exec(sql.c_str());
        sql = "INSERT INTO runinfo_categories VALUES( ";
        sql += "Default, 'Simulation Period');";

        W.exec(sql.c_str());
        sql = "INSERT INTO runinfo_categories VALUES( ";
        sql += "Default, 'Time Resolution');";
        W.exec(sql.c_str());

        sql = "INSERT INTO runinfo_categories VALUES( ";
        sql += "Default, 'Numerical Options');";
        W.exec(sql.c_str());

        sql = "DROP TABLE IF EXISTS runinfo;";
        W.exec(sql.c_str());

        sql = "INSERT INTO runinfo_categories VALUES( ";
        sql += "Default, 'Additional Details');";
        W.exec(sql.c_str());
        nametables.push_back(nametable);

        nametable = "runinfo";
        drop(W, nametable);
        sql = create(nametable);
        sql += " (id_simulations Integer REFERENCES simulations(id_simulations), ";
        sql += "FileVersionNumber Integer ,";
        sql += "ChildDocument boolean,";
        sql += "OriginalFileName varchar(64) Default '-',";
        sql += "ExeFileDate varchar(64) Default '-',";
        sql += "MultiSimulation bool,";
        sql += "Finished bool,";
        sql += "Date_Created Integer Default 0,";
        sql += "Date_Modified Integer Default 0,";
        sql += "SimStartMin Integer,";
        sql += "SimEndMin Integer,";
        sql += "SimPeriodScaling Integer,";
        sql += "PriorSimPeriod Integer,";
        sql += "PostSimPeriod Integer,";
        sql += "TimeResolution Integer,";
        sql += "OutputInterval_minutes Integer,";
        sql += "OutputInterval_days Integer,";
        sql += "NumberofIterations Integer,";
        sql += "LockedMultiStore bool);";
        W.exec(sql.c_str());
        nametables.push_back(nametable);

        nametable= "multirun_methods";
        drop(W, nametable);
        sql = create(nametable);
        sql += " (id_method SERIAL PRIMARY KEY,";
        sql += "name Varchar(24));";
        W.exec(sql.c_str());

        sql = "INSERT INTO multirun_methods VALUES(1,'Linear');"; W.exec(sql.c_str());
        sql = "INSERT INTO multirun_methods VALUES(2,'Log');"; W.exec(sql.c_str());
        sql = "INSERT INTO multirun_methods VALUES(3,'Stocastic Linear');"; W.exec(sql.c_str());
        sql = "INSERT INTO multirun_methods VALUES(4,'Stocastic Log');"; W.exec(sql.c_str());
        sql = "INSERT INTO multirun_methods VALUES(5,'Table');"; W.exec(sql.c_str());
        sql = "INSERT INTO multirun_methods VALUES(6,'DB_Choice');"; W.exec(sql.c_str());
        sql = "INSERT INTO multirun_methods VALUES(7,'Same AS');"; W.exec(sql.c_str());
        sql = "INSERT INTO multirun_methods VALUES(8,'Relative Same As');"; W.exec(sql.c_str());
        sql = "INSERT INTO multirun_methods VALUES(9,'Oposite Relative Same As');"; W.exec(sql.c_str());
        sql = "INSERT INTO multirun_methods VALUES(10,'Bayesian Linear');"; W.exec(sql.c_str());
        sql = "INSERT INTO multirun_methods VALUES(11,'Connected');"; W.exec(sql.c_str());
        sql = "INSERT INTO multirun_methods VALUES(12,'Bayesian Log');"; W.exec(sql.c_str());
        sql = "INSERT INTO multirun_methods VALUES(13,'Nelder Mead');"; W.exec(sql.c_str());
       
        nametables.push_back(nametable);


        nametable = "multirun_settings";
        drop(W, nametable);
        sql = create(nametable);
        sql += " (id_simulations Integer REFERENCES simulations(id_simulations), ";
        sql += "Dimension Integer,";
        sql += "id_Group Integer References Groups(id_group),";
        sql += "name Varchar(36),";
        sql += "vectorindex Integer ,";
        sql += "id_method Integer References multirun_methods(id_method),";
        sql += "monitoring Integer ,";
        sql += "min Real,";
        sql += "Max Real,";
        sql += "Start Real,";
        sql += "Dependence_Name Varchar(34),";
        sql += "Dependence_Index Integer,";
        sql += "FixedValues Real[],";
        sql += "FixedKey Varchar(34)[], ";
        sql += "FixedKeyValue Varchar(34)[],";
        sql += "numberRepetition Integer); ";
        W.exec(sql.c_str());
        nametables.push_back(nametable);

        nametable = "multirun_Results";
        drop(W, nametable);
        sql = create(nametable);
        sql += " (id_simulations Integer REFERENCES simulations(id_simulations), ";
        sql += "RunNo Integer,";
        sql += "Accepted Bool,";
        sql += "Parameters Real[],";
        sql += "R2 Real[],";
        sql += "Intercept Real[],";
        sql += "Slope Real[],";
        sql += "ME Real[],";
        sql += "RMSE Real[],";
        sql += "LOG_Likelihood Real[],";
        sql += "NSE Real[],";
        sql += "MeanPerformance Real[],";
        sql += "OutputSum Real[]);";
        W.exec(sql.c_str());
        nametables.push_back(nametable);

        nametable= "Performance_Indicators";
        drop(W, nametable);
        sql = create(nametable);
        sql += " (id_Perf_Ind SERIAL PRIMARY KEY,";
        sql += "name Varchar(48));";
        W.exec(sql.c_str());
        nametables.push_back(nametable);

        sql = "INSERT INTO Performance_Indicators VALUES(1,'Coefficient of Determination');"; W.exec(sql.c_str());
        sql = "INSERT INTO Performance_Indicators VALUES(2,'Intercept');"; W.exec(sql.c_str());
        sql = "INSERT INTO Performance_Indicators VALUES(3,'Slope');"; W.exec(sql.c_str());
        sql = "INSERT INTO Performance_Indicators VALUES(4,'Mean Error');"; W.exec(sql.c_str());
        sql = "INSERT INTO Performance_Indicators VALUES(5,'RMSE');"; W.exec(sql.c_str());
        sql = "INSERT INTO Performance_Indicators VALUES(6,'LogLikelihood');"; W.exec(sql.c_str());
        sql = "INSERT INTO Performance_Indicators VALUES(7,'Nash-Sutcliffe R2');"; W.exec(sql.c_str());
        nametables.push_back(nametable);

        nametable = "Multirun_Ensemble_Statistics";
        drop(W, nametable);
        sql = create(nametable);
        sql += " (id_simulations Integer REFERENCES simulations(id_simulations), ";
        sql += "RunNo Integer,";
        sql += "EnsembleNo Integer,";
        sql += "Valfilenumber Integer,";
        sql += "Sum bool,";
        sql += "ValFile_ResultIndex Integer,";
        sql += "Id_Perf_Ind Integer References Performance_Indicators(id_Perf_Ind),";
        sql += "n_points Integer,";
        sql += "Prior_mean Real,";
        sql += "Prior_cv Real,";
        sql += "Prior_min Real,";
        sql += "Prior_max Real,";
        sql += "Post_mean Real,";
        sql += "Post_cv Real,";
        sql += "Post_min Real,";
        sql += "Post_max Real,";
        sql += "LowLimits Real,";
        sql += "HighLimits Real);";
        W.exec(sql.c_str());
        nametables.push_back(nametable);
        
        nametable = "multirun_Ensemble_DefinedCriteria";
        drop(W, nametable);
        sql = create(nametable);
        sql += " (id_simulations Integer REFERENCES simulations(id_simulations), ";
        sql += "RunNo Integer,";
        sql += "EnsembleNo Integer,";
        sql += "VariableName varchar(34),";
        sql += "Id_Perf_Ind Integer References Performance_Indicators(id_Perf_Ind),";
        sql += "Min Real,";
        sql += "Max Real);";
        nametables.push_back(nametable);

        nametable = "multirun_Residuals";
        drop(W, nametable);
        sql = create(nametable);
        sql += " (id_simulations Integer REFERENCES simulations(id_simulations), ";
        sql += "RunNo Integer,";
        sql += "Valfilenumber Integer,";
        sql += "ValFile_ResultIndex Integer,";
        sql += "Residuals Real[]);";
        W.exec(sql.c_str());
        nametables.push_back(nametable);

        nametable = "multirun_Ensemble_DefinedCriteria";
        drop(W, nametable);
        sql = create(nametable);
        sql += " (id_simulations Integer REFERENCES simulations(id_simulations), ";
        sql += "RunNo Integer,";
        sql += "Ensemble Integer,";
        sql += "Variable Varchar(64),";
        sql += "Low_limit Real,";
        sql += "High_limit Real);";
        W.exec(sql.c_str());
        nametables.push_back(nametable);
   

        nametable = "multirun_Ensembles";
        drop(W, nametable);
        sql = create(nametable);
        sql += " (id_simulations Integer REFERENCES simulations(id_simulations), ";
        sql += "RunNo Integer,";
        sql += "Valfilenumber Integer,";
        sql += "ValFile_ResultIndex Integer,";
        sql += "Ensemble Integer,";
        sql += "AllCandidates bool,";
        sql += "NumberofCandidates Integer,";
        sql += "PGmin Integer[],";
        sql += "MeanValues Real[],";
        sql += "Min Real[],";
        sql += "Max Real[],";
        sql += "Std Real[],";
        sql += "P10 Real[],";
        sql += "P50 Real[],";
        sql += "P90 Real[]);";
        W.exec(sql.c_str());
        nametables.push_back(nametable);

        nametable = "initial_final_states";
        drop(W, nametable);
        sql = create(nametable);
        sql += " (id_simulations Integer REFERENCES simulations(id_simulations), ";
        sql += "outputname VarChar(34), ";
        sql += "number_of_index Integer,";
        sql += "Initial Real[],";
        sql += "Final Real[])";
        W.exec(sql.c_str());
        nametables.push_back(nametable);



        nametable = "history_of_changes";
        drop(W, nametable);
        sql = create(nametable);
        sql += " (id_simulations Integer REFERENCES simulations(id_simulations), ";
        sql += "runinfotype Integer, ";
        sql += "stringtype VarChar(40), ";
        sql += "groupname VarChar(40), ";
        sql += "itemname VarChar(40), ";
        sql += "stringvalue VarChar(40), ";
        sql += "localindex Integer, ";
        sql += "timestamp Integer, ";
        sql += "username VarChar(40), ";
        sql += "computer VarChar(40)) ";
        W.exec(sql.c_str());
        nametables.push_back(nametable);

       
        nametable = "soil_profiles";
        drop(W, nametable);
        sql = create(nametable);

        sql += " (id_profile SERIAL PRIMARY KEY, ";
        sql += "id_oldkey Integer Unique,";
        
        sql += "Name Varchar(64),";
        sql += "Country Varchar(34),";
        sql += "County Varchar(34),";
        sql += "Longitude float,";
        sql += "Latitude float,";
        sql += "Numlayers Integer,";
        sql += "LowerDepth Integer,";
        sql += "Upper_Clay float,";
        sql += "Upper_Sand float,";
        sql += "Upper_Org float,";
        sql += "Lower_Clay float,";
        sql += "Lower_Sand float,";
        sql += "lower_Org float,";
        sql += "Year Integer,";
        sql += "SoilType Varchar(34))";

        W.exec(sql.c_str());
        nametables.push_back(nametable);


        nametable = "soil_profile_details";
        drop(W, nametable);
        sql = create(nametable);
        sql += " (id_profile Integer REFERENCES soil_profiles(id_profile), ";
        sql += "Descriptions VarChar(4000),";
        sql += "CreatedBy Varchar(34),";
        sql += "DateCreated VarChar(16),";
        sql += "ModifiedBy Varchar(34),";
        sql += "DateModified VarChar(16))";
        W.exec(sql.c_str());
        nametables.push_back(nametable);



        nametable = "soil_layers";
        drop(W, nametable);
        sql = create(nametable);
        sql += " (id_layers SERIAL PRIMARY KEY, ";
        sql += "UpperDepth float, ";
        sql += "LowerDepth float, ";
        sql += "Id_SoilType_Name integer,";
        sql += "DryBulkDensity float,";
        sql += "ParticleDensity float,";
        sql += "OrganicMatter float,";
        sql += "CarbonContent float,";
        sql += "NitrogenContent float,";
        sql += "PhosphorousContent float,";
        sql += "AtterbergFractions float[],";
        sql += "Clay float,";
        sql += "Silt float,";
        sql += "Sand float,";
        sql += "Gravels float,";
        sql += "Number_of_Heads integer,"; 
        sql += "Heads float[],";
        sql += "ThetaMeasured float[],";
        sql += "Saturation float,";
        sql += "WiltingPoint float,";
        sql += "Residual float,";
        sql += "AirEntry float,";
        sql += "Lambda float,";
        sql += "UpperBoundaryHead float,";
        sql += "MacroPore float,";
        sql += "GenAlpha float,";
        sql += "GenM float,";
        sql += "GenN float,";
        sql += "TotConductivity float,";
        sql += "MatricConductivity float,";
        sql += "Mualem_n float,";
        sql += "N_SrCoef float,";
        sql += "N_SECoef float)";
        W.exec(sql.c_str());
        nametables.push_back(nametable);

        nametable = "soil_profile_layer_linking";
        drop(W, nametable);
        sql = create(nametable);
        sql += "(Id_profile Integer References soil_profiles(Id_profile),";
        sql += "Id_layers Integer References soil_layers(Id_Layers),";
        sql += "UpperDepthFromProfile  float Default 0.,";
        sql += "LowerDepthFromProfile  float default 0.)";
        W.exec(sql.c_str());

        nametables.push_back(nametable);
        W.commit();
        return nametables;

    }
    catch (const std::exception& e) {
        cerr << sql.c_str() << std::endl;
        cerr << e.what() << std::endl;
        return nametables;
    }

};
static int query_createtable(enum simtype type, const string str)
{
    string sql;



    connection c = initconnection("Create Tables and insert data");
    pqxx::work W{ c };
    try {

        sql = "DROP TABLE IF EXISTS " + str + " CASCADE;";
        W.exec(sql.c_str());

        sql = "CREATE TABLE ";
        string pkey = "ID_SWITCH SERIAL PRIMARY KEY, NAME VARCHAR(64) UNIQUE,";
        string values = "VALUE         INTEGER ,"\
            "DEFAULTVALUE    INTEGER ,";

        sql += str;
        sql += "( ";
        string firstsection =
            "ID_GROUP     INTEGER REFERENCES Groups(Id_Group) , "\
            "ID_PHYSPROC   INTEGER REFERENCEs PhysProcNames(Id_physproc),"\
            "ID_BIOPROC   INTEGER  REFERENCEs BioProcNames(Id_BioProc)  NOT NULL,"\
            "ID_ELEMENT   INTEGER  REFERENCEs ElementNames(Id_Element) ,";

        if (type == SWITCH) {


            sql += pkey + firstsection + values;


            sql += "ENABLED    BOOLEAN NOT NULL, OPTIONS  varchar(64) ARRAY,";
            sql += "ID_ParentSwitch1 Integer ,";
            sql += "ID_ParentSwitch2 Integer ,";

            sql += "CONDITIONS struc_conditions ARRAY); ";
            W.exec(sql.c_str());
            W.commit();

        }
        switch (type) {


        case PAR_SINGLE:
            sql = "CREATE TABLE " + str + " (";
            sql += "ID_SinglePar SERIAL PRIMARY KEY, NAME VARCHAR(64) UNIQUE NOT NULL, ";
            sql += firstsection;

            sql += "VALUE REAL, "\
                "MIN_ERROR    REAL NOT NULL, "\
                "MAX_ERROR    REAL NOT NULL, "\
                "MIN_WARNING    REAL NOT NULL, "\
                "MAX_WARNING    REAL NOT NULL, "\
                "DEFAULTVALUE    REAL NOT NULL, "\
                "UNIT           INTEGER NOT NULL, ";
            sql += "ID_ParentSwitch1 Integer ,";
            sql += "ID_ParentSwitch2 Integer ,";
            break;
        case PAR_TABLE:
            sql = "CREATE TABLE " + str + " (";
            sql += "ID_VectorPar SERIAL PRIMARY KEY, ID_VectorName VARCHAR(64) UNIQUE NOT NULL, ";
            sql += firstsection;
            sql += "ID_DynamicVector INTEGER REFERENCES NE_Vectors(Id_DynamicVector),"\
                "ID_ARRAYSIZE INTEGER NOT NULL,";

            sql += "VALUE    REAL ARRAY NOT NULL,"\
                "MIN_ERROR    REAL ARRAY NOT NULL,"\
                "MAX_ERROR    REAL ARRAY NOT NULL,"\
                "MIN_WARNING    REAL ARRAY NOT NULL,"\
                "MAX_WARNING    REAL ARRAY NOT NULL,"\
                "DEFAULTVALUE    REAL ARRAY NOT NULL,"\
                "UNIT           INTEGER NOT NULL,";
            sql += "ID_ParentSwitch1 Integer ,";
            sql += "ID_ParentSwitch2 Integer ,";
            break;
        case NUM_ELEMENTS_VECTOR:
            sql = "CREATE TABLE " + str + " (";
            sql += "ID_DynamicVector SERIAL PRIMARY KEY, VectorName VARCHAR(64) UNIQUE NOT NULL, ";

            sql += "VALUE INTEGER NOT NULL,"\
                "DEFAULTVALUE   INTEGER NOT NULL,"\
                "MIN_ERROR    INTEGER NOT NULL,"\
                "MAX_ERROR    INTEGER NOT NULL);";
            break;

        case STATE:
            sql = "DROP TABLE IF EXISTS " + str + " CASCADE; ";
            W.exec(sql.c_str());
            sql = "CREATE TABLE " + str + " (";
            sql += "ID_VectorOutputs SERIAL PRIMARY KEY, Name VARCHAR(64) UNIQUE NOT NULL, ";
            sql += firstsection;
            sql += "Id_SimType INTEGER NOT NULL,";
            sql += "STOREFLAG INTEGER[],"\
                "DEFAULTVALUE DOUBLE PRECISION [] NOT NULL,";
            break;
        case STATE_SINGLE:
            sql = "CREATE TABLE " + str + " (";
            sql += "ID_SingleOutputs SERIAL PRIMARY KEY, Name VARCHAR(64) UNIQUE NOT NULL, ";
            sql += firstsection;
            sql += "Id_SimType INTEGER NOT NULL,";
            sql += "STOREVALUE INTEGER NOT NULL,"\
                "DEFAULTVALUE   DOUBLE PRECISION NOT NULL,";
            break;
        case FUNCTION:
            sql = "CREATE TABLE " + str + " (";
            sql += "ID_ParameterFunctions SERIAL PRIMARY KEY, VectorName VARCHAR(96) UNIQUE NOT NULL, ";
            sql += firstsection;
            sql += "Function_type INTEGER,";
            sql += "Independent_with_index BOOLEAN, ";
            sql += "Single_Parameter_Names VARCHAR(32) ARRAY,";
            sql += "Vector_Parameter_Names VARCHAR(32) ARRAY,";

            break;

        case PGFILE:
            sql = "CREATE TABLE " + str + " (";
            sql += "ID_TimeSerie SERIAL PRIMARY KEY, TimeSerieName VARCHAR(96) UNIQUE NOT NULL, ";
            sql += firstsection;
            sql += "FileName VARCHAR(200) ,";


            break;

        }

        if (type == NUM_ELEMENTS_VECTOR) {
            W.exec(sql.c_str());
            W.commit();
            /*  sql = "CREATE TABLE NE_TO_P";
              sql += "( "\
                  "ID_NE_P SERIAL PRIMARY KEY, NAME VARCHAR(64) REFERENCES all_ne_tables (Name)," \
                  "ID_ARRAYNAME VARCHAR(64) REFERENCES all_tableparameters (ID_ARRAYNAME));";
              W.exec(sql.c_str());*/

        }
        else if (type != SWITCH) {
            sql += "ENABLED    BOOLEAN NOT NULL,"\
                "CONDITIONS  struc_conditions ARRAY);";
            W.exec(sql.c_str());
            W.commit();
        }


        cout << "Records created successfully from " << str << endl;

    }

    catch (const std::exception& e) {
        cerr << e.what() << std::endl;
        return 1;
    }

    return 0;



}
static int query_inserttable(enum simtype type, const string str, vector<SimB*> v_simb, CommonModelInfo* pCommonModel) {
    connection c = initconnection( "insert data ");
    pqxx::work W{ c };

    string sql;
    NE* pNE;
    size_t NumElem;
    vector<SimB*> Switch1, Switch2, Switch3;
    if (type == SWITCH) {

        for (size_t i = 0; i < v_simb.size(); i++) {
            vector<Condition> v = v_simb[i]->GetArrayConditions();
            if (v.size() == 0) {
                Switch1.push_back(v_simb[i]);
            }
        }
        for (size_t i = 0; i < v_simb.size(); i++) {
            vector<Condition> v = v_simb[i]->GetArrayConditions();
            if (v.size() > 0) {
                bool Second = false;
                size_t count = 0;

                for (size_t j = 0; j < v.size(); j++) {
                    for (size_t jj = 0; jj < Switch1.size(); jj++) {
                        if (v[j].variable->GetName() == Switch1[jj]->GetName()) {
                            count++;
                        }
                    }
                }
                if (count == v.size()) Second = true;
                if (Second) Switch2.push_back(v_simb[i]);
            }
        }
        for (size_t i = 0; i < v_simb.size(); i++) {
            vector<Condition> v = v_simb[i]->GetArrayConditions();
            if (v.size() > 0) {
                bool third = false;
                size_t count = 0;

                for (size_t j = 0; j < v.size(); j++) {
                    for (size_t jj = 0; jj < Switch2.size(); jj++) {
                        if (v[j].variable->GetName() == Switch2[jj]->GetName()) {
                            count++;
                        }
                    }
                }
                if (count == v.size()) third = true;
                if (third) Switch3.push_back(v_simb[i]);
            }
        }


        auto insert = [&](vector<SimB*> Switch, string table) {




            for (size_t i = 0; i < Switch.size(); i++) {
                try {
                    Sw* pSimB = static_cast<Sw*>(Switch[i]);
                    sql = "INSERT INTO " + table + " VALUES (";
                    sql += FUtil::STD_ItoAscii(pCommonModel->GetSwitchId(pSimB->GetName())) + ",";
                    sql += "'" + pSimB->GetName() + "', ";
                    if (type != NUM_ELEMENTS_VECTOR) {
                        sql += FUtil::STD_ItoAscii(pSimB->GetGroupNo()) + ',';
                        sql += FUtil::STD_ItoAscii(pSimB->GetFysProcess()) + ',';
                        sql += FUtil::STD_ItoAscii(pSimB->GetBioProcess()) + ',';
                        sql += FUtil::STD_ItoAscii(size_t(pSimB->GetElement())) + ',';
                    }
                    UNIT_TYPES unittype{ UNIT_TYPES::mMASS_UNIT };

                    sql += FUtil::STD_ItoAscii(pSimB->GetIntValue()) + ',';
                    sql += FUtil::STD_ItoAscii(pSimB->GetOriginalIntValue()) + ',';

                    sql += FUtil::bool_toAscii(pSimB->IsEnabled());
                    auto numo = pSimB->GetNumOptions();
                    sql += ",'{""";
                    for (size_t j = 0; j < numo; ++j) {

                        string str = pSimB->GetOption(j);
                        sql += str + """";
                        if (j < numo - 1) sql += ",";
                    };
                    sql += """}',";

                    vector<Condition> v = pSimB->GetArrayConditions();
                    bool jump = false;
                    if (v.size() > 0) {
                        int id_switch = pCommonModel->GetSwitchId(v[0].variable->GetName());
                        sql += FUtil::STD_ItoAscii(id_switch);
                        //if (v[0].variable->GetName() == "Develop") jump = true;
                    }
                    else
                        sql += "NULL";
                    sql += ",";
                    if (v.size() > 1) {
                        int id_switch = pCommonModel->GetSwitchId(v[1].variable->GetName());
                        sql += FUtil::STD_ItoAscii(id_switch);
                    }
                    else
                        sql += "NULL";
                    sql += ",";


                    sql += "Array[";
                    for (size_t j = 0; j < v.size(); ++j) {
                        if (j == 0)
                            sql += "(";
                        else
                            sql += ",(";
                        int id_switch = pCommonModel->GetSwitchId(v[j].variable->GetName());
                        sql += FUtil::STD_ItoAscii(id_switch) + "," + FUtil::STD_ItoAscii(int(v[j].op)) + ", " +
                            FUtil::STD_ItoAscii(v[j].value) + ")";
                    }
                    sql += "]::struc_conditions[]);";
                    if (!jump)   W.exec(sql.c_str());
                }
                catch (const std::exception& e) {
                    cerr << sql.c_str();
                    cerr << e.what() << std::endl;

                }
            }
        };
        insert(v_simb, "Switches");


        W.commit();

        return 0;


    }







    for (size_t i = 0; i < v_simb.size(); i++) {
        try {

            string sqlfk; string argstr;
            if (type != NUM_ELEMENTS_VECTOR) {
                sqlfk = FUtil::STD_ItoAscii(v_simb[i]->GetGroupNo()) + ',';
                sqlfk += FUtil::STD_ItoAscii(v_simb[i]->GetFysProcess()) + ',';
                sqlfk += FUtil::STD_ItoAscii(v_simb[i]->GetBioProcess()) + ',';
                sqlfk += FUtil::STD_ItoAscii(size_t(v_simb[i]->GetElement())) + ',';
            }
            UNIT_TYPES unittype;
            int id_par = 0;
            SimB* pSimB;
            vector<Condition> v;
            vector<string> names;
            bool jump;
            int ityp;
            bool btype;
            switch (type) {
            case SWITCH:
                sql += FUtil::STD_ItoAscii(v_simb[i]->GetIntValue()) + ',';
                sql += FUtil::STD_ItoAscii(v_simb[i]->GetOriginalIntValue()) + ',';
                break;
            case PAR_SINGLE:
                sql = "INSERT INTO " + str + " VALUES (";
                id_par = pCommonModel->GetSingleParId(v_simb[i]->GetName());
                sql += FUtil::STD_ItoAscii(id_par) + ",";
                sql += "'" + v_simb[i]->GetName() + "', ";
                sql += sqlfk;

                sql += FUtil::STD_DtoAscii(static_cast<Ps*>(v_simb[i])->GetValue()) + ',';

                sql += FUtil::STD_FtoAscii(v_simb[i]->GetMinValue()) + ',';
                sql += FUtil::STD_FtoAscii(v_simb[i]->GetMaxValue()) + ',';
                sql += FUtil::STD_FtoAscii(v_simb[i]->GetWarnMinValue()) + ',';
                sql += FUtil::STD_FtoAscii(v_simb[i]->GetWarnMaxValue()) + ',';
                sql += FUtil::STD_FtoAscii(v_simb[i]->GetOriginalValue()) + ',';

                unittype = pCommonModel->GetUnitType(v_simb[i]->GetUnit());

                sql += FUtil::STD_ItoAscii(int(unittype)) + ",";
                pSimB = v_simb[i];
                v = pSimB->GetArrayConditions();
                jump = false;
                if (v.size() > 0) {
                    int id_switch = pCommonModel->GetSwitchId(v[0].variable->GetName());
                    sql += FUtil::STD_ItoAscii(id_switch);
                }
                else
                    sql += "NULL";
                sql += ",";
                if (v.size() > 1) {
                    int id_switch = pCommonModel->GetSwitchId(v[1].variable->GetName());
                    sql += FUtil::STD_ItoAscii(id_switch);
                }
                else
                    sql += "NULL";
                sql += ",";

                break;
            case PAR_TABLE:
                sql = "INSERT INTO " + str + " VALUES (";
                id_par = pCommonModel->GetVectorParId(v_simb[i]->GetName());
                sql += FUtil::STD_ItoAscii(id_par) + ",";
                sql += "'" + v_simb[i]->GetName() + "', ";
                sql += sqlfk;


                pNE = static_cast<P*>(v_simb[i])->GetNEPointer();
                sql += FUtil::STD_ItoAscii(pCommonModel->GetNEVectorId(pNE->GetName())) + ",";
                NumElem = pNE->GetNEValue();
                sql += FUtil::STD_ItoAscii(NumElem) + ",'{";
                for (size_t j = 0; j < NumElem; ++j) {
                    sql += FUtil::STD_DtoAscii(static_cast<P*>(v_simb[i])->GetValue(j));
                    if (j < NumElem - 1) sql += ",";
                }
                sql += "}','{";
                for (size_t j = 0; j < NumElem; ++j) {
                    sql += FUtil::STD_DtoAscii(static_cast<P*>(v_simb[i])->GetMinValue(j));
                    if (j < NumElem - 1) sql += ",";
                }
                sql += "}','{";
                for (size_t j = 0; j < NumElem; ++j) {
                    sql += FUtil::STD_DtoAscii(static_cast<P*>(v_simb[i])->GetMaxValue(j));
                    if (j < NumElem - 1) sql += ",";
                }
                sql += "}','{";
                for (size_t j = 0; j < NumElem; ++j) {
                    sql += FUtil::STD_DtoAscii(static_cast<P*>(v_simb[i])->GetWarnMinValue(j));
                    if (j < NumElem - 1) sql += ",";
                }
                sql += "}','{";
                for (size_t j = 0; j < NumElem; ++j) {
                    sql += FUtil::STD_DtoAscii(static_cast<P*>(v_simb[i])->GetWarnMaxValue(j));
                    if (j < NumElem - 1) sql += ",";
                }
                sql += "}','{";
                for (size_t j = 0; j < NumElem; ++j) {
                    sql += FUtil::STD_DtoAscii(static_cast<P*>(v_simb[i])->GetOriginalValue(j));
                    if (j < NumElem - 1) sql += ",";
                }
                sql += "}',";
                unittype = pCommonModel->GetUnitType(v_simb[i]->GetUnit());
                sql += FUtil::STD_ItoAscii(int(unittype)) + ",";
                pSimB = v_simb[i];
                v = pSimB->GetArrayConditions();
                jump = false;
                if (v.size() > 0) {
                    int id_switch = pCommonModel->GetSwitchId(v[0].variable->GetName());
                    sql += FUtil::STD_ItoAscii(id_switch);
                    if (v[0].variable->GetName() == "Develop") jump = true;
                }
                else
                    sql += "NULL";
                sql += ",";
                if (v.size() > 1) {
                    int id_switch = pCommonModel->GetSwitchId(v[1].variable->GetName());
                    sql += FUtil::STD_ItoAscii(id_switch);
                }
                else
                    sql += "NULL";

                sql += ",";

                break;
            case NUM_ELEMENTS_VECTOR:
                sql = "INSERT INTO " + str + " VALUES (";
                id_par = pCommonModel->GetNEVectorId(v_simb[i]->GetName());
                sql += FUtil::STD_ItoAscii(id_par) + ",";
                sql += "'" + v_simb[i]->GetName() + "', ";


                sql += FUtil::STD_DtoAscii(static_cast<NE*>(v_simb[i])->GetNEValue()) + ',';
                sql += FUtil::STD_FtoAscii(static_cast<NE*>(v_simb[i])->GetNE_MinValue()) + ',';
                sql += FUtil::STD_FtoAscii(static_cast<NE*>(v_simb[i])->GetNE_MaxValue()) + ',';
                sql += FUtil::STD_FtoAscii(static_cast<NE*>(v_simb[i])->GetOriginalNEValue()) + ");";
                break;
            case STATE:
                sql = "INSERT INTO " + str + " VALUES (";
                id_par = pCommonModel->GetVectorOutputId(v_simb[i]->GetName());
                sql += FUtil::STD_ItoAscii(id_par) + ",";
                sql += "'" + v_simb[i]->GetName() + "', ";
                sql += sqlfk;
                sql += FUtil::STD_ItoAscii(v_simb[i]->GetType()) + ",";
                pNE = static_cast<OutVector*>(v_simb[i])->GetNEPointer();

                NumElem = pNE->GetNEValue();
                sql += "'{";
                for (size_t j = 0; j < NumElem; ++j) {

                    sql += FUtil::STD_ItoAscii(static_cast<OutVector*>(v_simb[i])->GetStoreFlag(j));
                    if (j < NumElem - 1) sql += ",";
                }
                sql += "}','{";
                for (size_t j = 0; j < NumElem; ++j) {
                    sql += FUtil::STD_ItoAscii(static_cast<OutVector*>(v_simb[i])->GetValue(j));
                    if (j < NumElem - 1) sql += ",";
                }
                sql += "}',";

                break;
            case STATE_SINGLE:
                sql = "INSERT INTO " + str + " VALUES (";
                id_par = pCommonModel->GetSingleOutputId(v_simb[i]->GetName());
                sql += FUtil::STD_ItoAscii(id_par) + ",";
                sql += "'" + v_simb[i]->GetName() + "', ";
                sql += sqlfk;
                sql += FUtil::STD_ItoAscii(v_simb[i]->GetType()) + ", ";
                sql += FUtil::STD_ItoAscii(static_cast<OutSingle*>(v_simb[i])->GetStoreFlag()) + ",";
                sql += FUtil::STD_DtoAscii(static_cast<OutSingle*>(v_simb[i])->GetValue());
                sql += ",";

                break;
            case FUNCTION:
                sql = "INSERT INTO " + str + " VALUES (";
                argstr= to_string(static_cast<Func*>(v_simb[i])->GetFuncType());
                id_par = pCommonModel->GetParameterFunctionId(v_simb[i]->GetName() + "_" + argstr);
                sql += FUtil::STD_ItoAscii(id_par) + ",";
                sql += "'" + v_simb[i]->GetName() + "_"+argstr+"', "; // name
                sql += sqlfk;
                ityp = static_cast<Func*>(v_simb[i])->GetFuncType();
                sql += to_string(ityp) + ",";
                btype = static_cast<Func*>(v_simb[i])->IsTableType();
                sql += to_string(btype) + ",";
                sql += "'{";
                names = static_cast<Func*>(v_simb[i])->GetParDep_Ps_Names();
                for (size_t i = 0; i < names.size(); ++i) { sql += names[i]; if (i < names.size() - 1) sql += ","; };
                sql += "}','{";
                names = static_cast<Func*>(v_simb[i])->GetParDep_P_Names();
                for (size_t i = 0; i < names.size(); ++i) { sql += names[i]; if (i < names.size() - 1) sql += ","; };
                sql += "}',";
               

                break;
            case PGFILE:
                sql = "INSERT INTO " + str + " VALUES (";
                id_par = pCommonModel->GetTimeSeriesId(v_simb[i]->GetName());
                sql += FUtil::STD_ItoAscii(id_par) + ",";
                sql += "'" + v_simb[i]->GetName() + "', ";
                sql += sqlfk;
                sql += "'" + static_cast<F*>(v_simb[i])->GetStrValue() + "',";
                break;
            }
            if (type != NUM_ELEMENTS_VECTOR) {
                sql += FUtil::bool_toAscii(v_simb[i]->IsEnabled());
                vector<Condition> v = v_simb[i]->GetArrayConditions();
                sql += ",Array[";
                for (size_t j = 0; j < v.size(); ++j) {
                    if (j == 0)
                        sql += "(";
                    else
                        sql += ",(";
                    int id_switch = pCommonModel->GetSwitchId(v[j].variable->GetName());
                    sql += FUtil::STD_ItoAscii(id_switch) + "," + FUtil::STD_ItoAscii(int(v[j].op)) + ", " +
                        FUtil::STD_ItoAscii(v[j].value) + ")";
                }
                sql += "]::struc_conditions[]);";
            }
            if (id_par >= 0)   W.exec(sql.c_str());

        }
        catch (const std::exception& e) {
            cerr << sql << std::endl;
            cerr << e.what() << std::endl;
            return 1;
        }
    }
    W.commit();
    return 0;

};

bool DefineUniqueIdMaps(CommonModelInfo* pinfo, NewMap* pDoc) {
    connection c = initconnection("Define uniqueMapsId");
    string sql;
    pqxx::work W{ c };
    vector<pair<string, int>> v_pair;
    try {
        pqxx::result r{ W.exec("SELECT id_singleoutputs, name FROM singleoutputs") };
        for (auto row : r) {
            v_pair.push_back(pair<string, int>(row[1].c_str(), FUtil::AtoInt(row[0].c_str())));
        }
        pinfo->DefineSingleOutputMap(v_pair); v_pair.clear();
        r= W.exec("SELECT id_vectoroutputs, name FROM vectoroutputs");
        for (auto row : r) {
            v_pair.push_back(pair<string, int>(row[1].c_str(), FUtil::AtoInt(row[0].c_str())));
        }
        pinfo->DefineVectorOutputMap(v_pair);  v_pair.clear();

        r=W.exec("SELECT id_switch, name FROM switches") ;
        for (auto row : r) {
            v_pair.push_back(pair<string, int>(row[1].c_str(), FUtil::AtoInt(row[0].c_str())));
        }
        pinfo->DefineSwitchMap(v_pair); v_pair.clear();

        r= W.exec("SELECT id_singlepar, name FROM singleparameters");
        for (auto row : r) {
            v_pair.push_back(pair<string, int>(row[1].c_str(), FUtil::AtoInt(row[0].c_str())));
        }
        pinfo->DefineSingleParMap(v_pair); v_pair.clear();

        r= W.exec("SELECT id_vectorpar, id_vectorname FROM vectorparameters") ;
        for (auto row : r) {
            v_pair.push_back(pair<string, int>(row[1].c_str(), FUtil::AtoInt(row[0].c_str())));
        }
        pinfo->DefineVectorParMap(v_pair); v_pair.clear();

        r = W.exec("SELECT id_timeserie, timeseriename FROM timeseriesinputs");
        for (auto row : r) {
            v_pair.push_back(pair<string, int>(row[1].c_str(), FUtil::AtoInt(row[0].c_str())));
        }
        pinfo->DefineTimeSeriesMap(v_pair); v_pair.clear();

    }
    catch (const std::exception& e) {
        cerr << sql.c_str() << std::endl;
        cerr << e.what() << std::endl;
        return false;
    }
   
    return true;
}

vector<string> create_Additional_Tables(CommonModelInfo* pinfo, NewMap* pDoc) {
    connection c = initconnection("Additional Tables");
    string sql;
    pqxx::work W{ c };
    vector<string> nametables;
    try {
        string nametable;
        auto drop = [&](pqxx::work& W, string name) {; string s = "DROP TABLE IF EXISTS " + name + " CASCADE;"; return W.exec(s.c_str()); };
        auto create = [&](string name) { string s = "CREATE TABLE " + name; return s; };
     /*   sql = "CREATE TABLE vectorpar_functions";
        sql += "( \
             Id_VectorPar INTEGER REFERENCES vectorparameters(Id_vectorpar),\
            Id_Functions INTEGER REFERENCES parameterfunctions(Id_parameterfunctions)\
           ); ";

        W.exec(sql.c_str());

        sql = "CREATE TABLE SinglePar_Functions";
        sql += "(    \
            Id_SinglePar INTEGER REFERENCES singleparameters(Id_singlepar),\
            Id_Functions INTEGER REFERENCES parameterfunctions(Id_parameterfunctions)\
          ); ";

        W.exec(sql.c_str());*/

        nametable = "ne_vector_outputs";
        drop(W, nametable);

        sql = create(nametable);
        sql += "(Id_DynamicVector INTEGER REFERENCES NE_vectors(Id_dynamicvector),\
            Id_vectoroutputs INTEGER REFERENCES vectoroutputs(Id_vectoroutputs)\
           ); ";

        W.exec(sql.c_str());
        nametables.push_back(nametable);

        nametable = "ne_vector_parameters";
        drop(W, nametable);
        sql = create(nametable);
        sql += " (    \
            Id_DynamicVector INTEGER REFERENCES NE_vectors(Id_dynamicvector),\
            Id_vectorpar INTEGER REFERENCES vectorparameters(Id_vectorpar)\
          ); ";
        W.exec(sql.c_str());

        nametables.push_back(nametable);

       vector<SimB*> v_Allsimb;
        auto sumsimb = [&](simtype t) {
            for (size_t groupno = 0; groupno < 38; groupno++) {
                vector<SimB*> v_simb = pDoc->GetPtrVector(t, groupno, true);
                for (size_t i = 0; i < v_simb.size(); i++)
                    v_Allsimb.push_back(v_simb[i]);

            }
        };

      //  string sql;
        v_Allsimb.clear();
        sumsimb(PAR_SINGLE);
        pinfo->DefineSingleParMap(v_Allsimb);

        v_Allsimb.clear();    sumsimb(PAR_TABLE);
        pinfo->DefineVectorParMap(v_Allsimb);

        v_Allsimb.clear();    sumsimb(FUNCTION);
        pinfo->DefineParameterFunctionMap(v_Allsimb);

        v_Allsimb.clear();
        sumsimb(STATE); sumsimb(FLOW); sumsimb(AUX); sumsimb(DRIVE);
        pinfo->DefineVectorOutputMap(v_Allsimb);
        v_Allsimb = pDoc->GetAll_NEPointers();
        pinfo->DefineNEVectorMap(v_Allsimb);

        size_t countout = 0, countp=0;

         for (size_t i = 0; i < v_Allsimb.size(); ++i) {
              auto ip_NE = pinfo->GetNEVectorId(v_Allsimb[i]->GetName());
                for (size_t j = 0; j < static_cast<NE*>(v_Allsimb[i]) ->GetNumLinks_P(); j++) {
                   P* pP=static_cast<NE*>(v_Allsimb[i])->Get_PLink(j);
                    countp++;
                    sql = "INSERT INTO ne_vector_parameters VALUES";
                    sql += "( ";// +FUtil::STD_ItoAscii(countp) + ",";
                    auto ip_id=pinfo->GetVectorParId(pP->GetName());
                    sql += FUtil::STD_ItoAscii(ip_NE) + "," + FUtil::STD_ItoAscii(ip_id)+");";
                    W.exec(sql.c_str());
                }
                for (size_t j = 0; j < static_cast<NE*>(v_Allsimb[i])->GetNumLinks_X(); j++) {
                    X* pX= static_cast<NE*>(v_Allsimb[i])->Get_XLink(j);
                    countout++;
                    sql = "INSERT INTO ne_vector_outputs VALUES";
                    sql += "( ";// +FUtil::STD_ItoAscii(countps) + ",";
                    auto ip_id = pinfo->GetVectorOutputId(pX->GetName());
                    sql += FUtil::STD_ItoAscii(ip_NE) + "," + FUtil::STD_ItoAscii(ip_id) + ");";
                    W.exec(sql.c_str());
                }
                for (size_t j = 0; j < static_cast<NE*>(v_Allsimb[i])->GetNumLinks_T(); j++) {
                    T* pT = static_cast<NE*>(v_Allsimb[i])->Get_TLink(j);
                    countout++;
                    sql = "INSERT INTO ne_vector_outputs VALUES";
                    sql += "( ";// +FUtil::STD_ItoAscii(countps) + ",";
                    auto ip_id = pinfo->GetVectorOutputId(pT->GetName());
                    sql += FUtil::STD_ItoAscii(ip_NE) + "," + FUtil::STD_ItoAscii(ip_id) + ");";
                    W.exec(sql.c_str());
                }
                for (size_t j = 0; j < static_cast<NE*>(v_Allsimb[i])->GetNumLinks_G(); j++) {
                    G* pG = static_cast<NE*>(v_Allsimb[i])->Get_GLink(j);
                    countout++;
                    sql = "INSERT INTO ne_vector_outputs VALUES";
                    sql += "( ";// +FUtil::STD_ItoAscii(countps) + ",";
                    auto ip_id = pinfo->GetVectorOutputId(pG->GetName());
                    sql += FUtil::STD_ItoAscii(ip_NE) + "," + FUtil::STD_ItoAscii(ip_id) + ");";
                    W.exec(sql.c_str());
                }
                for (size_t j = 0; j < static_cast<NE*>(v_Allsimb[i])->GetNumLinks_D(); j++) {
                    D* pD = static_cast<NE*>(v_Allsimb[i])->Get_DLink(j);
                    countout++;
                    sql = "INSERT INTO ne_vector_outputs VALUES";
                    sql += "( ";// +FUtil::STD_ItoAscii(countps) + ",";
                    auto ip_id = pinfo->GetVectorOutputId(pD->GetName());
                    sql += FUtil::STD_ItoAscii(ip_NE) + "," + FUtil::STD_ItoAscii(ip_id) + ");";
                    W.exec(sql.c_str());
                }
            }

         size_t countps = 0;
         countp = 0;
/*
            for (size_t i = 0; i < v_Allsimb.size(); ++i) {
                vector<string> Pnames= static_cast<Func*>(v_Allsimb[i])->GetParDep_P_Names();
                auto ip_func = pinfo->GetParameterFunctionId(v_Allsimb[i]->GetName());
                for (size_t j = 0; j < Pnames.size(); j++) {
                    countp++;
                    sql = "INSERT INTO vectorpar_functions VALUES";
                    sql += "( ";// +FUtil::STD_ItoAscii(countp) + ",";
                    auto ip_id=pinfo->GetVectorParId(Pnames[j]);
                    sql += FUtil::STD_ItoAscii(ip_id) + "," + FUtil::STD_ItoAscii(ip_func)+");";
                    W.exec(sql.c_str());
                }
                Pnames = static_cast<Func*>(v_Allsimb[i])->GetParDep_Ps_Names();
                ip_func = pinfo->GetParameterFunctionId(v_Allsimb[i]->GetName());
                for (size_t j = 0; j < Pnames.size(); j++) {
                    countp++;
                    sql = "INSERT INTO SinglePar_Functions VALUES";
                    sql += "( ";// +FUtil::STD_ItoAscii(countps) + ",";
                    auto ip_id = pinfo->GetSingleParId(Pnames[j]);
                    sql += FUtil::STD_ItoAscii(ip_id) + "," + FUtil::STD_ItoAscii(ip_func) + ");";
                    W.exec(sql.c_str());
                }
            }
  */
        W.commit();


    }
    catch (const std::exception& e) {
        cerr << sql.c_str() << std::endl;
        cerr << e.what() << std::endl;
    }

    vector<string> newtables;
    auto ikoll = query_createtable(SWITCH, "Switches");
    vector<SimB*> v_Allsimb = pDoc->GetAllPtr(SWITCH);

    pinfo->DefineSwitchMap(v_Allsimb);
    ikoll = query_inserttable(SWITCH, "Switches", v_Allsimb, pDoc->m_pCommonModelInfo);


    ikoll = query_createtable(PAR_SINGLE, "singleparameters");
    v_Allsimb = pDoc->GetAllPtr(PAR_SINGLE);
    pinfo->DefineSingleParMap(v_Allsimb);
    ikoll = query_inserttable(PAR_SINGLE, "singleparameters", v_Allsimb, pDoc->m_pCommonModelInfo);

    ikoll = query_createtable(NUM_ELEMENTS_VECTOR, "NE_Vectors");
    v_Allsimb = pDoc->GetAll_NEPointers();
    pinfo->DefineNEVectorMap(v_Allsimb);

    ikoll = query_inserttable(NUM_ELEMENTS_VECTOR, "NE_Vectors", v_Allsimb, pDoc->m_pCommonModelInfo);

    ikoll = query_createtable(PAR_TABLE, "vectorparameters");
    v_Allsimb = pDoc->GetAllPtr(PAR_TABLE);
    pinfo->DefineVectorParMap(v_Allsimb);
    ikoll = query_inserttable(PAR_TABLE, "vectorparameters", v_Allsimb, pDoc->m_pCommonModelInfo);

    ikoll = query_createtable(FUNCTION    , "functions");
    v_Allsimb = pDoc->GetAllPtr(FUNCTION);
    pinfo->DefineParameterFunctionMap(v_Allsimb);
    ikoll = query_inserttable(FUNCTION, "functions", v_Allsimb, pDoc->m_pCommonModelInfo);



    ikoll = query_createtable(STATE, "VectorOutputs");
    v_Allsimb = pDoc->GetAllPtr(STATE, FLOW, AUX, DRIVE);
    pinfo->DefineVectorOutputMap(v_Allsimb);
    ikoll = query_inserttable(STATE, "VectorOutputs", v_Allsimb, pDoc->m_pCommonModelInfo);


    ikoll = query_createtable(STATE_SINGLE, "SingleOutputs");
    v_Allsimb = pDoc->GetAllPtr(STATE_SINGLE, FLOW_SINGLE, AUX_SINGLE, DRIVE_SINGLE);
    pinfo->DefineSingleOutputMap(v_Allsimb);
    ikoll = query_inserttable(STATE_SINGLE, "SingleOutputs", v_Allsimb, pDoc->m_pCommonModelInfo);

    ikoll = query_createtable(PGFILE, "TimeSeriesInputs");
    v_Allsimb = pDoc->GetAllPtr(PGFILE);
    pinfo->DefineTimeSeriesMap(v_Allsimb);
    ikoll = query_inserttable(PGFILE, "TimeSeriesInputs", v_Allsimb, pDoc->m_pCommonModelInfo);

    return nametables;

};


int transfer_Document(struct sim_doc_simulation str) {
    try {
        connection c = initconnection("Simulations Table");
        pqxx::work W{ c };

        string sql = "INSERT INTO Simulations VALUES ( ";
        sql += "Default,'";
        if (str.sitename.size() > 0 && str.name.find(str.sitename)==string::npos&&str.sitename!="-")
            sql += str.sitename + "_"+str.name+"',";
        else 
            sql += str.name + "',";
        if (str.creator.size() == 0) sql += "Default,";
        else sql += "'" + str.creator + "',NOW(),";
        if (str.sitename.size() == 0) sql += "Default,";
        else sql += "'" + str.sitename + "',";
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
        connection c = initconnection("Run Info");
        pqxx::work W{ c };

        string sql = "INSERT INTO runinfo VALUES ( ";
        sql += to_string(s.key_simulation) + ",";
        sql += to_string(s.fileversionnumber) + ",";
        sql += to_string(s.childdocument) + ",";
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
        connection c = initconnection("Switches");
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
        connection c = initconnection("Single parameters");
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
        connection c = initconnection("vector parameters");
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
        connection c = initconnection("single outputs");
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
        connection c = initconnection("modified vectoroutputs_storevalues");
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
    string sql;
    try {
        connection c = initconnection("modified_timeseries_inputs");
        pqxx::work W{ c };
        {
            sql = "INSERT INTO modified_timeseries_inputs VALUES (default, ";
            sql += to_string(r.key_simulation) + ",";
            sql += to_string(r.id_timeserie) + ",'";
            sql += r.filename;
            sql += "');";
            W.exec(sql.c_str());
        }
        {
            int id_fileName;
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
                cerr << "file: " << r.filename << std::endl;
                W.commit();

                c = initconnection("modified_timeseries_inputs");
                work W{ c };

            }
            if (id_fileName >= 0 && r.NumVar != 0) {

                auto insertvector = [&sql](vector<string> s) {
                    int count = 0;
                    for (string str : s) {
                        count++;
                        if (str.size() > 0&&str.size()<25) sql += '"' + str + '"';
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
                auto insertvector_i = [&sql](vector<int> s) {
                    int count = 0;
                    for (int flt : s) {
                        count++;
                        if (flt >= 0) sql += to_string(flt);
                        else sql += "null";
                        if (count < s.size()) sql += ",";
                    }
                };
                
                string shortname=r.filename;
                auto posend = r.filename.rfind(".");
                auto pos = r.filename.rfind("/");
                if (pos == string::npos)  pos = r.filename.rfind("\\");
                if (posend > pos) {
                    shortname = r.filename.substr(pos + 1, posend - pos - 1);
                }
  
                string testname = shortname;
                // Test of File already exist in archive

                sql = "SELECT Id_filename, filename, shortname FROM FileNameArchive WHERE shortname = '"+shortname+"'";
                result rr = W.exec(sql.c_str());
                bool file_exist = false;
                for (auto row : rr) {
                    id_fileName = row[0].as<int>();
                    file_exist = true;
                    cout << "file exist - a new link will be made to : " << r.filename << endl;
                }

                if (!file_exist) {
                    sql = "INSERT INTO FileNameArchive VALUES (default,'";
                    sql += r.filename + "','";
                    sql += shortname + "',";
                    sql += to_string(r.NumVar) + ",";
                    sql += to_string(r.NumRec) + ",";
                    sql += to_string(r.NumRepetions) + ",";
                    sql += to_string(r.NormalTimeStep) + ",'{";

                    insertvector(r.Name); sql += "}','{"; insertvector_i(r.I_Units); sql += "}','{"; insertvector(r.Id);
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
                        sql += r.Name[i] + "',";
                        sql += to_string(r.I_Units[i]) + ",'";
                        sql += "x ','";
                        sql += r.Id[i] + "','";
                        sql += r.Pos[i] + "','";
                        sql += r.Country[i] + "','";
                        sql += r.Station[i] + "',";
                        sql += to_string(r.Latitude[i]) + ",";
                        sql += to_string(r.Longitude[i]) + ",";
                        sql += to_string(r.Altitude[i]) + ");";
                        result rr = W.exec(sql.c_str());
                    }
                    int count = 0;
                    for (const tuple<int, int, vector<float>>rec : pg) {
                        count++;
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
                        if (count * 100 / r.NumRec % 10 == 0) cout << to_string(int(count * 100 / r.NumRec)) << "% done" << "\r";
                    }

                    cout << "All data from time serie data inserted into table from :" << shortname << endl;
                }



                if (r.NumVar != 0 || file_exist) {
                    sql = "INSERT INTO filenamearchive_uses VALUES (";
                    sql += to_string(r.key_simulation) + ",";
                    sql += to_string(id_fileName) + ");";
                    W.exec(sql.c_str());
                    cout << "New link to : " << shortname << " to " << to_string(id_fileName) << endl;
                }
                W.commit();
            }

        }

      }
    catch (const std::exception& e) {
        cerr << e.what() << std::endl;
        cerr << sql << std::endl;
        char a = 0xb2;
        auto pos1 = sql.find(a);
        if (pos1 != string::npos) {
            cerr << to_string(pos1);
        }
        return -1;
    }
    return 0;
   
};
int transfer_Validation(int simkey, vector< vector<string>>& v) {
    string sql;
    size_t count;
    try {
        connection c = initconnection("validation");
        pqxx::work W{ c };
        for (vector<string> inner : v) {
            sql = "INSERT INTO validation VALUES ( ";
            sql += to_string(simkey) + ",";
            count = 0;
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
        cerr << sql << count << std::endl;
        cerr << "Validation table error";
        return -1;
    }
};
int transfer_dynamic_parameters(int pkey, vector < tuple<int, int, string, int, int, vector<int>, vector<float>, vector<string>>>& t) {
    try {
        connection c = initconnection("dynamic_parameters");
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
    return 0;
};
int transfer_multirun_setting(int pkey, vector <tuple<int, int, string, int, int, int, float, float, float, string, int, vector<float>, vector<string>, vector<string>, int >>& mr) {
    try {
        connection c = initconnection("multirun settings");
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
                sql += "'-',";
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
    return 0;

}
int transfer_MultiStorage(int pkey, NewMap* pDoc) {
    try {
        connection c = initconnection("MultiRun Results");
        pqxx::work W{ c };
        string sql;
        bool accepted = true;
        const int max_indicators{ 7 };
        
        if (!pDoc->MR_Storage_Open()) {
                cout << " MBin file could not be open" << endl;
                return -1;
        }
        
        
        for (size_t RunNo = 0; RunNo < pDoc->m_MStorage.GetNumberOfRuns(); RunNo++) {

            if((RunNo%100)==0)
                cout << "Uploading to data base (" << to_string(RunNo) << ") of " << to_string(pDoc->m_MStorage.GetNumberOfRuns()) << " runs";
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
                for (size_t indicator = 0; indicator < max_indicators; indicator++) {
            
                   if (indicator == max_indicators - 1) {
                       float rmse_obs, rmse_val, nse;
                       size_t ind, ind_rmse;
                        for (size_t i = 0; i < nvalid; ++i) {
                            ind_rmse = 4 * nvalid + i;
                            rmse_obs = pDoc->m_ValidationData.GetValRMSE_O(i) / pDoc->m_ValidationData.GetValn(i);
                            rmse_val = pDoc->m_MStorage.GetValid(RunNo, ind_rmse);
                            nse = 1. - rmse_val * rmse_val / rmse_obs;
                            sql += to_string(nse);
                            count++;
                            if (count < nvalid) sql += ",";
                        }                     
                    }
                    else {
                        for (size_t i = 0; i < nvalid; ++i) {
                            sql += to_string(pDoc->m_MStorage.GetValid(RunNo, i + nvalid * indicator));
                            count++;
                            if (count < nvalid) sql += ",";
                        }
                    }
                    sql += "}','{"; count = 0;

                }

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
        connection c = initconnection("MultiRun Residuals");
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
        connection c = initconnection("Ensemble Statistic");
        pqxx::work W{ c };
        string sql;

        int runno = int(pDoc->m_DocFile.m_SimulationRunNo);
        int ensemble_no = 1;
        size_t nvalid = pDoc->GetNumAll_TSV_ValVariables();

        pDoc->m_MStorage.UpdateAcceptedRuns(true, false);
        pDoc->MR_ReCalculatePostDist(false);

        for (int i = 0; i < pDoc->m_ResidualFileVector.size(); i++) {
            CResiduals* pRes;
            pRes = pDoc->GetValidResidualPointer(i + 1);
            if (pRes != nullptr) {
                pRes->ToBeUpdated();
                pRes->m_MeanDefined = false;
            }
        }
        pDoc->m_MStorage.EvaluateFile(false);


        pDoc->m_MStorage.SetAllToAcceptedRuns();

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
        auto insertvector_i = [&sql](vector<int> s) {
            int count = 0;
            for (int flt : s) {
                count++;
                if (flt >= 0) sql += to_string(flt);
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


        bool AllCandidates{ true };
        for (size_t iall = 0; iall < 2; iall++) {
            if (iall == 1) AllCandidates = false;
            for (int i = 0; i < nvalid; i++) {
            sql = "INSERT INTO multirun_Ensembles VALUES (";
            sql += to_string(pkey) + ",";
            sql += to_string(runno) + ",";
            auto FileNo = pDoc->m_ValidationData.GetValFileNumber(i);
            sql += to_string(FileNo) + ",";
            auto pValFile = pDoc->ValidationFilePointer(FileNo);
            auto ind = pDoc->m_ValidationData.GetValFileIndex(i);
            sql += to_string(ind) + ",";
            sql += to_string(pDoc->m_MStorage.m_ActualEnsemble) + ",";
            auto pRes = (CResiduals*)pDoc->GetResidualPointer(FileNo);
            string filename = pDoc->GetXBinFileName(ind + 1);

                
                CPG* pPG = (CPG*)pValFile->GetPointer();
                if (pPG == nullptr) {
                    pPG = new CPG;
                    bool valid = pPG->ReadContentOfPGFileAndOpenMainPGStreamForReading(pValFile->GetStrValue().c_str());
                    if (valid) pValFile->SetPointer(pPG);
                    pPG->CloseFile();
                }
                auto ntimepoints = pPG->GetNumRecords();

                if (pRes != nullptr && pRes->ReOpenFile(true)) {
                    bool updated = pRes->ToBeUpdated();
                    pRes->SmartUpdateStatistics(true);
                    sql += to_string(AllCandidates)+",";
                    size_t num;
                    if (AllCandidates)
                       num= pDoc->MR_Get_TotalNumberofRuns();
                    else
                        num = pRes->GetNumAccepted();
                    sql += to_string(num) + ",";
                    vector<int> int_vector; int_vector.resize(ntimepoints);
                    vector<float> f_vector; f_vector.resize(ntimepoints);
                    for (size_t rec = 0; rec < ntimepoints; rec++) int_vector[rec] = pPG->GetLongTime(rec+1); sql += "'{"; insertvector_i(int_vector); sql += "}',";
                    if (AllCandidates) {
                        for (size_t rec = 0; rec < ntimepoints; rec++) f_vector[rec] = pRes->GetMeanAllResiduals(ind, rec); sql += "'{"; insertvector_r(f_vector); sql += "}',";
                        for (size_t rec = 0; rec < ntimepoints; rec++) f_vector[rec] = pRes->GetMinAllResiduals(ind, rec); sql += "'{"; insertvector_r(f_vector); sql += "}',";
                        for (size_t rec = 0; rec < ntimepoints; rec++) f_vector[rec] = pRes->GetMaxAllResiduals(ind, rec); sql += "'{"; insertvector_r(f_vector); sql += "}',";
                        for (size_t rec = 0; rec < ntimepoints; rec++) f_vector[rec] = pRes->GetStdAllResiduals(ind, rec); sql += "'{"; insertvector_r(f_vector); sql += "}',";
                        for (size_t rec = 0; rec < ntimepoints; rec++) f_vector[rec] = pRes->GetP10AllResiduals(ind, rec); sql += "'{"; insertvector_r(f_vector); sql += "}',";
                        for (size_t rec = 0; rec < ntimepoints; rec++) f_vector[rec] = pRes->GetP50AllResiduals(ind, rec); sql += "'{"; insertvector_r(f_vector); sql += "}',";
                        for (size_t rec = 0; rec < ntimepoints; rec++) f_vector[rec] = pRes->GetP90AllResiduals(ind, rec); sql += "'{"; insertvector_r(f_vector); sql += "}'";
                    }
                    else {
                        for (size_t rec = 0; rec < ntimepoints; rec++) f_vector[rec] = pRes->GetMeanResiduals(ind, rec); sql += "'{"; insertvector_r(f_vector); sql += "}',";
                        for (size_t rec = 0; rec < ntimepoints; rec++) f_vector[rec] = pRes->GetMinResiduals(ind, rec); sql += "'{"; insertvector_r(f_vector); sql += "}',";
                        for (size_t rec = 0; rec < ntimepoints; rec++) f_vector[rec] = pRes->GetMaxResiduals(ind, rec); sql += "'{"; insertvector_r(f_vector); sql += "}',";
                        for (size_t rec = 0; rec < ntimepoints; rec++) f_vector[rec] = pRes->GetStdResiduals(ind, rec); sql += "'{"; insertvector_r(f_vector); sql += "}',";
                        for (size_t rec = 0; rec < ntimepoints; rec++) f_vector[rec] = pRes->GetP10Residuals(ind, rec); sql += "'{"; insertvector_r(f_vector); sql += "}',";
                        for (size_t rec = 0; rec < ntimepoints; rec++) f_vector[rec] = pRes->GetP50Residuals(ind, rec); sql += "'{"; insertvector_r(f_vector); sql += "}',";
                        for (size_t rec = 0; rec < ntimepoints; rec++) f_vector[rec] = pRes->GetP90Residuals(ind, rec); sql += "'{"; insertvector_r(f_vector); sql += "}'";
                    }
                    sql += ");";

                    W.exec(sql.c_str());
                }
            }
        }


        W.commit();
        return 0;
    }

    catch (const std::exception& e) {
        cerr << e.what() << std::endl;
        return -1;
    }
};
int transfer_initial_final(int pkey, NewMap* pDoc) {
    string sql;
    try {
        connection c = initconnection("Initial_Final_State");
        pqxx::work W{ c };
        vector<SimB*> vpp, vp_final;
        size_t iv = 0;
     


        vpp = pDoc->GetPtrVector("State Variables", "");
        for (size_t i = 0; i < vpp.size(); i++) {      
            auto pPtr = (SimB*)vpp[i];
            vp_final.push_back(pPtr);
        }
        bool IsVector = false;
        for (size_t i = 0; i < vp_final.size(); i++) {
            sql = "INSERT INTO initial_final_states VALUES (";
            sql += to_string(pkey) + ",";          
            sql += "'" + vp_final[i]->GetName() + "',";

            auto pXT = static_cast<X*>(vp_final[i]);
            auto pX = static_cast<Xs*>(vp_final[i]);
            IsVector = vp_final[i]->Is_Vector();


            auto group = pXT->GetGroup();
            auto name = pXT->GetName();
            size_t nn = 0;
            string fi, in;
            fi = in = "'{";
            if (IsVector) {
                nn = pXT->GetNumberOfFlags();
                for (size_t ii = 0; ii < nn; ii++) {
                                
                    fi += to_string(pXT->GetFinal(ii));
                    in +=to_string(pXT->GetInitial(ii));
                    if (ii < nn - 1) {
                        fi += ","; in += ",";
                    }
                }
            }
            else {
                in +=to_string( pX->GetInitial());
                fi +=to_string(pX->GetFinal());
            }
            fi += "}'"; in += "}'";
            sql += to_string(nn) + ",";
            sql += in + "," + fi + ");";
            W.exec(sql.c_str());
        }
        W.commit();

    }
    catch (const std::exception& e) {
        cerr << e.what() << std::endl;
        cerr << sql << endl;
        return -1;
    }
};
int transfer_history(int pkey, NewMap* pDoc) {
    string sql;
    try {
        connection c = initconnection("history");
        pqxx::work W{ c };
        vector<SimB*> vpp, vp_final;
        size_t iv = 0;
        string group, name, str, type;

        size_t startindex;

        if (pDoc->m_ChildDocument) {
            startindex = pDoc->m_History_Array.size() - 1;
            for (size_t i = 0; i < pDoc->m_History_Array.size(); i++) {
                auto h_node = pDoc->m_History_Array[i];
                SimB* pBase;
                pBase = h_node.pBase;
                if (HIST_INFO(h_node.RunInfoType) == HIST_INFO::RUN_INFO) {
                    name = ((CRunInfo*)pBase)->GetName();
                    if (name == "NewRunNo") {
                        str = h_node.strvalue;
                        size_t runno = FUtil::AtoInt(str);
                        if (pDoc->m_DocFile.m_SimulationRunNo - 1 <= runno) startindex = i;
                    }
                }
            }
        }
        else
            startindex = 0;

        for (size_t i = startindex; i < pDoc->m_History_Array.size(); i++) {
            auto h_node = pDoc->m_History_Array[i];
            SimB* pBase;
            pBase = h_node.pBase;
            if (h_node.RunInfoType < 0 || pBase == nullptr) {
                group = "";
                name = "";
                str = "";

            }
            else if (HIST_INFO(h_node.RunInfoType) == HIST_INFO::DATA_BASE_INFO) {
                // DAtaBase
                group = "";
                name = ((CDB*)pBase)->GetName();
                str = pDoc->History_GetString(i);
                type = "DataBase";

            }
            else if (HIST_INFO(h_node.RunInfoType) == HIST_INFO::SWITCH_INFO) {
                group = pBase->GetGroup();
                name = pBase->GetName();
                str = h_node.strvalue;
                type = "Switches";

            }
            else if (HIST_INFO(h_node.RunInfoType) == HIST_INFO::RUN_INFO) {
                group = ((CRunInfo*)pBase)->GetGroup();
                name = ((CRunInfo*)pBase)->GetName();
                str = h_node.strvalue;
                type = "Run Info";

            }
            else if (h_node.RunInfoType == 1) {
                if (pBase->GetName() == "Soil Properties" || pBase->Is_DB()) {
                    group = "";
                    name = ((Sw*)pBase)->GetName();
                }
                else {
                    group = "";
                    name = pBase->GetName();
                    str = FUtil::STD_FtoAscii(pDoc->History_GetFloat(i));
                }
            }
            else if (HIST_INFO(h_node.RunInfoType) == HIST_INFO::PARSINGLE_INFO) {
                group = pBase->GetGroup();
                name = pBase->GetName();
                str = FUtil::STD_FtoAscii(pDoc->History_GetFloat(i));
                type = "Parameters";
            }
            else if (HIST_INFO(h_node.RunInfoType) == HIST_INFO::PARTABLE_INFO) {
                group = pBase->GetGroup();
                name = pBase->GetName();
                str = FUtil::STD_FtoAscii(pDoc->History_GetFloat(i));
                type = "Table Parameters";
            }
            else if (HIST_INFO(h_node.RunInfoType) == HIST_INFO::MODELFILE_INFO) {
                group = pBase->GetGroup();
                name = pBase->GetName();
                type = "ModelFiles";
            }
            else if (HIST_INFO(h_node.RunInfoType) == HIST_INFO::OUTPUT_SINGLE) {
                group = pBase->GetGroup();
                name = pBase->GetName();
                str = h_node.strvalue;
                type = "Single Output";
            }
            else if (HIST_INFO(h_node.RunInfoType) == HIST_INFO::OUTPUT_VECTOR ) {
                group = pBase->GetGroup();
                name = pBase->GetName();
                str = h_node.strvalue;
                type = "Vector Output";
            }

            sql = "INSERT INTO history_of_changes VALUES (";
            sql += to_string(pkey) + ",";
            sql += to_string(h_node.RunInfoType) + ",";
            sql += "'" + to_string(type.c_str()) + "',";
            sql += "'" + to_string(group.c_str()) + "',";
            sql += "'" + to_string(name.c_str()) + "',";
            sql += "'" + to_string(str.c_str()) + "',";
            sql += to_string(h_node.LocalIndex) + ",";
            sql += to_string(h_node.tvalue) + ",";
            sql += "'" + to_string(h_node.User.c_str()) + "',";
            sql += "'" + to_string(h_node.Computer.c_str()) + "')";
            W.exec(sql.c_str());
        }
        W.commit();
    }
    catch (const std::exception& e) {
        cerr << e.what() << std::endl;
        cerr << sql << endl;
        return -1;
    }



}

int CleanFileAchivesFromUnUsedFiles() {
    try {
        connection c = initconnection("Clean FileArchive");
        pqxx::work W{ c };
        vector<int> id_filesExist, id_filesUsed, deleted;
        string sql = "SELECT id_filename FROM FileNameArchive";
        result rr = W.exec(sql.c_str());
        for (auto row : rr) {
            id_filesExist.push_back(row[0].as<int>());
        }
        sql = "SELECT Id_FileName FROM filenamearchive_uses";
        rr = W.exec(sql.c_str());
        for (auto row : rr) {
            id_filesUsed.push_back(row[0].as<int>());
        }
        for (auto pkeyfile : id_filesExist) {
            bool NotUsed = true;
            for (auto pkeyUsed : id_filesUsed) {
                if (pkeyfile == pkeyUsed) NotUsed = false;
            }
            if (NotUsed) {
                auto r = W.exec("DELETE FROM filenamearchive_descriptions WHERE id_filename = " + to_string(pkeyfile));
                r = W.exec("DELETE FROM filenamearchive_data WHERE id_filename = " + to_string(pkeyfile));
                r = W.exec("DELETE FROM filenamearchive WHERE id_filename = " + to_string(pkeyfile));
                deleted.push_back(pkeyfile);
            }
        }
        W.commit();
        cout << to_string(deleted.size()) << " Was deleted from filetables" << endl;
    }

    catch (const std::exception& e) {
        cerr << e.what() << std::endl;
        return -1;
    }

return 0;
}
};
using namespace coup_pg;
#endif
