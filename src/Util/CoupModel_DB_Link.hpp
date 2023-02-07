#pragma once

#ifdef COUP_POSTGRES
#include <stddef.h>
#include <pqxx/pqxx>

#include "../NewBase/NewMap.h"

//using namespace Aws;
using namespace std;

using namespace pqxx;
/// Query employees from database.  Return result.

namespace coup_pg {
static connection initconnection(string password) {
    password = "pe1950";
    string init = "dbname = postgres user = postgres password = pe1950 port = 5432";
    connection c(init);
    if (c.is_open()) {
        cout << "Opened database successfully: " << c.dbname() << endl;
    }
    else {
        cout << "Can't open database" << endl;
    }
    return c;
}

static connection initconnection() { return initconnection("pe1950win"); };


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

        connection c = initconnection("pe1950win");

        pqxx::work W{ c };
        try {
            string sql = "CREATE TABLE ";
            sql += "TimeSeries (";
            sql += "Id_TimeSeries SERIAL PRIMARY KEY,";
            sql += "name VARCHAR(64),";
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
            c = initconnection("pe1950win");
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
static pqxx::result query(string tablename)
{
    connection c = initconnection("pe1950win");
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
static int create_Init_Tables(CommonModelInfo* pinfo) {
    connection c = initconnection(pinfo->GetPassword());
    string sql;
    pqxx::work W{ c };
    try {
        sql = "DROP TABLE IF EXISTS GroupCategories CASCADE;";
        W.exec(sql.c_str());
        sql = "CREATE TABLE GroupCategories";
        sql += "( "\
            "Id_GroupCategory SERIAL PRIMARY KEY,  \
            Name VarChar(34) ); ";

        W.exec(sql.c_str());

        for (size_t i = 0; i < 9; i++) {
            sql = "INSERT INTO GroupCategories VALUES (";
            sql += FUtil::STD_ItoAscii(i);
            sql += ",'" + pinfo->GetGroupCategoryNames(i) + "');";
            W.exec(sql.c_str());
        }

        sql = "DROP TABLE IF EXISTS Groups CASCADE;";
        W.exec(sql.c_str());

        sql = "CREATE TABLE Groups";
        sql += "( "\
            "Id_Group SERIAL PRIMARY KEY,  \
            Name VarChar(34),\
            Id_GroupCategory Integer REFERENCES GroupCategories(Id_GroupCategory)); ";

        W.exec(sql.c_str());

        auto group = pinfo->GetGroupNames();

        for (size_t i = 0; i < 38; i++) {
            sql = "INSERT INTO Groups VALUES (";
            sql += FUtil::STD_ItoAscii(i);
            sql += ",'" + group[i] + "',";
            int testv = pinfo->GetGroupCategory(group[i]);
            if (testv > 0) {
                sql += FUtil::STD_ItoAscii(testv) + ");";
                W.exec(sql.c_str());
            }
        }

        sql = "DROP TABLE IF EXISTS PhysProcNames CASCADE;";
        W.exec(sql.c_str());

        sql = "CREATE TABLE PhysProcNames";
        sql += "( "\
            "Id_PhysProc SERIAL PRIMARY KEY,  \
            Name VarChar(34) \
            ); ";

        W.exec(sql.c_str());


        for (size_t i = 0; i < 46; i++) {
            sql = "INSERT INTO PhysProcNames VALUES (";
            sql += FUtil::STD_ItoAscii(i);
            sql += ",'" + pinfo->GetFysProcName(i);
            sql += "');";
            W.exec(sql.c_str());
        }

        sql = "DROP TABLE IF EXISTS BioProcNames CASCADE;";
        W.exec(sql.c_str());

        sql = "CREATE TABLE BioProcNames";
        sql += "( "\
            "Id_BioProc SERIAL PRIMARY KEY,  \
            Name VarChar(34) \
            ); ";

        W.exec(sql.c_str());


        for (size_t i = 0; i < 40; i++) {
            sql = "INSERT INTO BioProcNames VALUES (";
            sql += FUtil::STD_ItoAscii(i);
            sql += ",'" + pinfo->GetBioProcName(i);
            sql += "');";
            W.exec(sql.c_str());
        }

        sql = "DROP TABLE IF EXISTS ElementNames CASCADE;";
        W.exec(sql.c_str());

        sql = "CREATE TABLE ElementNames";
        sql += "( "\
            "Id_Element SERIAL PRIMARY KEY,  \
            Name VarChar(34) \
            ); ";

        W.exec(sql.c_str());


        for (size_t i = 0; i < 46; i++) {
            sql = "INSERT INTO ElementNames VALUES (";
            sql += FUtil::STD_ItoAscii(i);
            sql += ",'" + pinfo->ElementName(i);
            sql += "');";
            W.exec(sql.c_str());
        }

        sql = "DROP TABLE IF EXISTS FileNameArchive CASCADE;";
        W.exec(sql.c_str());

        sql = "CREATE TABLE ";
        sql += "FileNameArchive (";
        sql += "Id_FileName SERIAL PRIMARY KEY,";
        sql += "FileName varchar(132) UNIQUE,";
        sql += "ShortName varchar(32),";
        sql += "NumVar Integer,";
        sql += "NumRecords Integer,";
        sql += "NumRepetions Integer,";
        sql += "NormalTimeStep Integer,";
        sql += "Name Varchar(24)[],";
        sql += "Units Varchar(24)[], ";
        sql += "Id Varchar(24)[], ";
        sql += "Pos Varchar(24)[], ";
        sql += "MinValue REAL[], ";
        sql += "MaxValue REAL[], ";
        sql += "Country Varchar(24)[], ";
        sql += "Station Varchar(24)[], ";
        sql += "Latitude double precision[], ";
        sql += "Longitude double precision[], ";
        sql += "Altitude double precision[]);";
        W.exec(sql.c_str());


        sql = "DROP TABLE IF EXISTS simulations CASCADE;";
        W.exec(sql.c_str());



        sql = "CREATE TABLE ";
        sql += "Simulations (";
        sql += "Id_Simulations SERIAL PRIMARY KEY,";
        sql += "name Varchar(36) UNIQUE,";
        sql += "creator Varchar(36) Default '-',";
        sql += "runno Integer,";
        sql += "comment Varchar(96) Default '-');";
        W.exec(sql.c_str());
        sql = "INSERT INTO Simulations VALUES( ";
        sql += "Default, 'first_test','PEJ',1,'When using Default of everything');";

        W.exec(sql.c_str());

        sql = "DROP TABLE IF EXISTS FileNameArchive_Uses CASCADE;";
        W.exec(sql.c_str());

        sql = "CREATE TABLE ";
        sql += "FileNameArchive_Uses (";
        sql += "Id_Simulations Integer References Simulations(Id_Simulations),";
        sql += "Id_FileName Integer References FileNameArchive(Id_FileName));";
        W.exec(sql.c_str());

        sql = "DROP TABLE IF EXISTS Validation CASCADE;";
        W.exec(sql.c_str());

        sql = "CREATE TABLE ";
        sql += "Validation (";
        sql += "Id_Simulations Integer References Simulations(Id_Simulations),";
        sql += "ValFileIndex Integer ,";
        sql += "output_type Integer ,";
        sql += "groupname varchar(36),";
        sql += "name varchar(36),";
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

        sql = "DROP TABLE IF EXISTS Dynamic_Parameters CASCADE;";
        W.exec(sql.c_str());

        sql = "CREATE TABLE ";
        sql += "Dynamic_Parameters (";
        sql += "Id_Simulations Integer References Simulations(Id_Simulations),";
        sql += "ItemType Integer,";
        sql += "Id_Group Integer References Groups(id_group),";
        sql += "name Varchar(36),";
        sql += "vectorindex Integer ,";
        sql += "NumChanges Integer ,";
        sql += "MinDates Integer[],";
        sql += "FloatValues Real[],";
        sql += "StringValue  Varchar[24][]);";
        W.exec(sql.c_str());

        sql = "DROP TABLE IF EXISTS FileNameArchive_Descriptions CASCADE;";
        W.exec(sql.c_str());

        sql = "CREATE TABLE ";
        sql += "FileNameArchive_Descriptions (";
        sql += "Id_FileName Integer References FileNameArchive(Id_FileName),";
        sql += "VarNo Integer ,";
        sql += "Name Varchar(24),";
        sql += "Unit Varchar(12),";
        sql += "Id Varchar(24),";
        sql += "Pos Varchar(12),";
        sql += "Country Varchar(12),";
        sql += "Station Varchar(24),";
        sql += "Latitude double precision,";
        sql += "Longitude double precision,";
        sql += "Altitude double precision);";
        W.exec(sql.c_str());


        sql = "DROP TABLE IF EXISTS FileNameArchive_Data CASCADE;";
        W.exec(sql.c_str());


        sql = "CREATE TABLE ";
        sql += "FileNameArchive_Data (";
        sql += "Id_FileName Integer References FileNameArchive(Id_FileName),";
        sql += "PGMinTime Integer,";
        sql += "PGVarValues Real[]);";
        W.exec(sql.c_str());
        W.commit();




    /*        sql = "DROP TABLE IF EXISTS singleoutputs_results;";
            W.exec(sql.c_str());

            sql = "CREATE TABLE ";
            sql += "singleoutputs_results (";
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


            sql = "DROP TABLE IF EXISTS timeseries_outputs_results;";
            W.exec(sql.c_str());
            sql = "CREATE TABLE ";
            sql += "timeseries_outputs_results (";
            sql += "Id_Simulations Integer References Simulations(id_Simulations),";
            sql += "PGminTime Integer,";
            sql += "outputs REAL[]);";

            W.exec(sql.c_str());


            sql = "DROP TABLE IF EXISTS vectoroutputs_results;";
            W.exec(sql.c_str());

            sql = "CREATE TABLE ";
            sql += "vectoroutputs_results (";
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



            sql = "DROP TABLE IF EXISTS modified_switch_values;";
            W.exec(sql.c_str());

            sql = "CREATE TABLE ";
            sql += "modified_switch_values (";
            sql += "modifactions SERIAL PRIMARY KEY,";
            sql += "Id_Simulations Integer References Simulations(id_Simulations),";
            sql += "Id_Switch integer References switches(id_switch),";
            sql += "value Integer);";

            W.exec(sql.c_str());


            sql = "DROP TABLE IF EXISTS modified_singleparameter_values;";
            W.exec(sql.c_str());

            sql = "CREATE TABLE ";
            sql += "modified_singleparameter_values (";
            sql += "modifactions SERIAL PRIMARY KEY,";
            sql += "Id_Simulations Integer References Simulations(id_Simulations),";
            sql += "Id_singlepar integer References singleparameters(id_singlepar),";
            sql += "value real);";

            W.exec(sql.c_str());
            sql = "DROP TABLE IF EXISTS modified_vectorparameter_values;";
            W.exec(sql.c_str());

            sql = "CREATE TABLE ";
            sql += "modified_vectorparameter_values (";
            sql += "modifactions SERIAL PRIMARY KEY,";
            sql += "Id_Simulations Integer References Simulations(id_Simulations),";
            sql += "Id_vectorpar integer References vectorparameters(id_vectorpar),";
            sql += "ArraySize Integer,";
            sql += "values real[]);";

            W.exec(sql.c_str());



            sql = "DROP TABLE IF EXISTS modified_singleoutputs_Storevalues CASCADE;";
            W.exec(sql.c_str());

            sql = "CREATE TABLE ";
            sql += "modified_singleoutputs_Storevalues (";
            sql += "results SERIAL PRIMARY KEY,";
            sql += "Id_Simulations Integer References Simulations(id_Simulations),";
            sql += "Id_singleoutputs integer References singleoutputs(id_singleoutputs),";
            sql += "StoreFlag Integer)";

            W.exec(sql.c_str());

            sql = "DROP TABLE IF EXISTS modified_vectoroutputs_Storevalues CASCADE;";
            W.exec(sql.c_str());

            sql = "CREATE TABLE ";
            sql += "modified_vectoroutputs_Storevalues (";
            sql += "results SERIAL PRIMARY KEY,";
            sql += "Id_Simulations Integer References Simulations(id_Simulations),";
            sql += "Id_vectoroutputs integer References vectoroutputs(id_vectoroutputs),";
            sql += "NumberStoreFlag Integer,";
            sql += "StoreFlags Integer[])";

            W.exec(sql.c_str());

            sql = "DROP TABLE IF EXISTS modified_singleoutputs_resultvalues CASCADE;";
            W.exec(sql.c_str());

            sql = "CREATE TABLE ";
            sql += "modified_singleoutputs_resultvalues (";
            sql += "results SERIAL PRIMARY KEY,";
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

            sql = "DROP TABLE IF EXISTS modified_vectoroutputs_resultvalues CASCADE;";
            W.exec(sql.c_str());

            sql = "CREATE TABLE ";
            sql += "modified_vectoroutputs_resultvalues (";
            sql += "modifications SERIAL PRIMARY KEY,";
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

            sql = "DROP TABLE IF EXISTS modified_timeseries_inputs CASCADE;";
            W.exec(sql.c_str());

            sql = "CREATE TABLE ";
            sql += "modified_timeseries_inputs (";
            sql += "modifications SERIAL PRIMARY KEY,";
            sql += "Id_Simulations Integer References Simulations(id_Simulations),";
            sql += "Id_timeserie integer References timeseriesinputs(id_timeserie),";
            sql += "filename varchar(96));";

            W.exec(sql.c_str());




            sql = "DROP TABLE IF EXISTS runinfo_categories CASCADE;";
            W.exec(sql.c_str());

            sql = "CREATE TABLE ";
            sql += "runinfo_categories (";
            sql += "id_runinfo_cat SERIAL PRIMARY KEY,";
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

            sql = "DROP TABLE IF EXISTS runinfo CASCADE;";
            W.exec(sql.c_str());

            sql = "CREATE TABLE runinfo (";
            sql += "id_simulations Integer REFERENCES simulations(id_simulations), ";
            sql += "FileVersionNumber Integer ,";
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

            sql = "DROP TABLE IF EXISTS multirun_settings CASCADE;";
            W.exec(sql.c_str());

            sql = "DROP TABLE IF EXISTS multirun_methods CASCADE;";
            W.exec(sql.c_str());

            sql = "CREATE TABLE multirun_methods (";
            sql += "id_method SERIAL PRIMARY KEY,";
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


            sql = "CREATE TABLE multirun_settings (";
            sql += "id_simulations Integer REFERENCES simulations(id_simulations), ";
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

            sql = "DROP TABLE IF EXISTS multirun_Results CASCADE;";
            W.exec(sql.c_str());

            sql = "CREATE TABLE multirun_Results (";
            sql += "id_simulations Integer REFERENCES simulations(id_simulations), ";
            sql += "RunNo Integer,";
            sql += "Accepted Bool,";
            sql += "Parameters Real[],";
            sql += "Performance Real[],";
            sql += "SumPerformance Real[],";
            sql += "OutputSum Real[]);";
            W.exec(sql.c_str());

            sql = "DROP TABLE IF EXISTS Performance_Indicators CASCADE;";
            W.exec(sql.c_str());

            sql = "CREATE TABLE Performance_Indicators (";
            sql += "id_Perf_Ind SERIAL PRIMARY KEY,";
            sql += "name Varchar(48));";
            W.exec(sql.c_str());

            sql = "INSERT INTO Performance_Indicators VALUES(1,'Coefficient of Determination');"; W.exec(sql.c_str());
            sql = "INSERT INTO Performance_Indicators VALUES(2,'Intercept');"; W.exec(sql.c_str());
            sql = "INSERT INTO Performance_Indicators VALUES(3,'Slope');"; W.exec(sql.c_str());
            sql = "INSERT INTO Performance_Indicators VALUES(4,'Mean Error');"; W.exec(sql.c_str());
            sql = "INSERT INTO Performance_Indicators VALUES(5,'RMSE');"; W.exec(sql.c_str());
            sql = "INSERT INTO Performance_Indicators VALUES(6,'LogLikelihood');"; W.exec(sql.c_str());
            sql = "INSERT INTO Performance_Indicators VALUES(7,'Nash-Sutcliffe R2');"; W.exec(sql.c_str());

            sql = "DROP TABLE IF EXISTS Multirun_Ensemble_Statistics CASCADE;";
            W.exec(sql.c_str());
            sql = "CREATE TABLE multirun_Ensemble_Statistics (";
            sql += "id_simulations Integer REFERENCES simulations(id_simulations), ";
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

            sql = "DROP TABLE IF EXISTS multirun_Ensemble_DefinedCriteria CASCADE;";
            W.exec(sql.c_str());
            sql = "CREATE TABLE multirun_Ensemble_DefinedCriteria (";
            sql += "id_simulations Integer REFERENCES simulations(id_simulations), ";
            sql += "RunNo Integer,";
            sql += "EnsembleNo Integer,";
            sql += "VariableName varchar(34),";
            sql += "Id_Perf_Ind Integer References Performance_Indicators(id_Perf_Ind),";
            sql += "Min Real,";
            sql += "Max Real);";



            sql = "DROP TABLE IF EXISTS multirun_Residuals CASCADE;";
            W.exec(sql.c_str());

            sql = "CREATE TABLE multirun_Residuals (";
            sql += "id_simulations Integer REFERENCES simulations(id_simulations), ";
            sql += "RunNo Integer,";
            sql += "Valfilenumber Integer,";
            sql += "ValFile_ResultIndex Integer,";
            sql += "Residuals Real[]);";

            W.exec(sql.c_str());


            sql = "DROP TABLE IF EXISTS multirun_Ensembles CASCADE;";
            W.exec(sql.c_str());

            sql = "CREATE TABLE multirun_Ensembles (";
            sql += "id_simulations Integer REFERENCES simulations(id_simulations), ";
            sql += "RunNo Integer,";
            sql += "Valfilenumber Integer,";
            sql += "ValFile_ResultIndex Integer,";
            sql += "Ensemble Integer,";
            sql += "NumberofCandidates Integer,";
            sql += "SelectionFraction Real,";
            sql += "PGmin Integer[],";
            sql += "MeanValues Real[],";
            sql += "Min Real[],";
            sql += "Max Real[],";
            sql += "Std Real[],";
            sql += "P10 Real[],";
            sql += "P50 Real[],";
            sql += "P90 Real[]);";

            W.exec(sql.c_str());*/
           ;
        return 0;

    }
    catch (const std::exception& e) {
        cerr << sql.c_str() << std::endl;
        cerr << e.what() << std::endl;
        return 1;
    }

};
static int create_Main_Tables(CommonModelInfo* pinfo) {
    connection c = initconnection(pinfo->GetPassword());
    string sql;
    pqxx::work W{ c };
    try {

        sql = "DROP TABLE IF EXISTS singleoutputs_results;";
        W.exec(sql.c_str());

        sql = "CREATE TABLE ";
        sql += "singleoutputs_results (";
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


        sql = "DROP TABLE IF EXISTS timeseries_outputs_results;";
        W.exec(sql.c_str());
        sql = "CREATE TABLE ";
        sql += "timeseries_outputs_results (";
        sql += "Id_Simulations Integer References Simulations(id_Simulations),";
        sql += "PGminTime Integer,";
        sql += "outputs REAL[]);";

        W.exec(sql.c_str());


        sql = "DROP TABLE IF EXISTS vectoroutputs_results;";
        W.exec(sql.c_str());

        sql = "CREATE TABLE ";
        sql += "vectoroutputs_results (";
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



        sql = "DROP TABLE IF EXISTS modified_switch_values;";
        W.exec(sql.c_str());

        sql = "CREATE TABLE ";
        sql += "modified_switch_values (";
        sql += "modifactions SERIAL PRIMARY KEY,";
        sql += "Id_Simulations Integer References Simulations(id_Simulations),";
        sql += "Id_Switch integer References switches(id_switch),";
        sql += "value Integer);";

        W.exec(sql.c_str());


        sql = "DROP TABLE IF EXISTS modified_singleparameter_values;";
        W.exec(sql.c_str());

        sql = "CREATE TABLE ";
        sql += "modified_singleparameter_values (";
        sql += "modifactions SERIAL PRIMARY KEY,";
        sql += "Id_Simulations Integer References Simulations(id_Simulations),";
        sql += "Id_singlepar integer References singleparameters(id_singlepar),";
        sql += "value real);";

        W.exec(sql.c_str());
        sql = "DROP TABLE IF EXISTS modified_vectorparameter_values;";
        W.exec(sql.c_str());

        sql = "CREATE TABLE ";
        sql += "modified_vectorparameter_values (";
        sql += "modifactions SERIAL PRIMARY KEY,";
        sql += "Id_Simulations Integer References Simulations(id_Simulations),";
        sql += "Id_vectorpar integer References vectorparameters(id_vectorpar),";
        sql += "ArraySize Integer,";
        sql += "values real[]);";

        W.exec(sql.c_str());



        sql = "DROP TABLE IF EXISTS modified_singleoutputs_Storevalues CASCADE;";
        W.exec(sql.c_str());

        sql = "CREATE TABLE ";
        sql += "modified_singleoutputs_Storevalues (";
        sql += "results SERIAL PRIMARY KEY,";
        sql += "Id_Simulations Integer References Simulations(id_Simulations),";
        sql += "Id_singleoutputs integer References singleoutputs(id_singleoutputs),";
        sql += "StoreFlag Integer)";

        W.exec(sql.c_str());

        sql = "DROP TABLE IF EXISTS modified_vectoroutputs_Storevalues CASCADE;";
        W.exec(sql.c_str());

        sql = "CREATE TABLE ";
        sql += "modified_vectoroutputs_Storevalues (";
        sql += "results SERIAL PRIMARY KEY,";
        sql += "Id_Simulations Integer References Simulations(id_Simulations),";
        sql += "Id_vectoroutputs integer References vectoroutputs(id_vectoroutputs),";
        sql += "NumberStoreFlag Integer,";
        sql += "StoreFlags Integer[])";

        W.exec(sql.c_str());

        sql = "DROP TABLE IF EXISTS modified_singleoutputs_resultvalues CASCADE;";
        W.exec(sql.c_str());

        sql = "CREATE TABLE ";
        sql += "modified_singleoutputs_resultvalues (";
        sql += "results SERIAL PRIMARY KEY,";
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

        sql = "DROP TABLE IF EXISTS modified_vectoroutputs_resultvalues CASCADE;";
        W.exec(sql.c_str());

        sql = "CREATE TABLE ";
        sql += "modified_vectoroutputs_resultvalues (";
        sql += "modifications SERIAL PRIMARY KEY,";
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

        sql = "DROP TABLE IF EXISTS modified_timeseries_inputs CASCADE;";
        W.exec(sql.c_str());

        sql = "CREATE TABLE ";
        sql += "modified_timeseries_inputs (";
        sql += "modifications SERIAL PRIMARY KEY,";
        sql += "Id_Simulations Integer References Simulations(id_Simulations),";
        sql += "Id_timeserie integer References timeseriesinputs(id_timeserie),";
        sql += "filename varchar(96));";

        W.exec(sql.c_str());




        sql = "DROP TABLE IF EXISTS runinfo_categories CASCADE;";
        W.exec(sql.c_str());

        sql = "CREATE TABLE ";
        sql += "runinfo_categories (";
        sql += "id_runinfo_cat SERIAL PRIMARY KEY,";
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

        sql = "DROP TABLE IF EXISTS runinfo CASCADE;";
        W.exec(sql.c_str());

        sql = "CREATE TABLE runinfo (";
        sql += "id_simulations Integer REFERENCES simulations(id_simulations), ";
        sql += "FileVersionNumber Integer ,";
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

        sql = "DROP TABLE IF EXISTS multirun_settings CASCADE;";
        W.exec(sql.c_str());

        sql = "DROP TABLE IF EXISTS multirun_methods CASCADE;";
        W.exec(sql.c_str());

        sql = "CREATE TABLE multirun_methods (";
        sql += "id_method SERIAL PRIMARY KEY,";
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


        sql = "CREATE TABLE multirun_settings (";
        sql += "id_simulations Integer REFERENCES simulations(id_simulations), ";
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

        sql = "DROP TABLE IF EXISTS multirun_Results CASCADE;";
        W.exec(sql.c_str());

        sql = "CREATE TABLE multirun_Results (";
        sql += "id_simulations Integer REFERENCES simulations(id_simulations), ";
        sql += "RunNo Integer,";
        sql += "Accepted Bool,";
        sql += "Parameters Real[],";
        sql += "Performance Real[],";
        sql += "SumPerformance Real[],";
        sql += "OutputSum Real[]);";
        W.exec(sql.c_str());

        sql = "DROP TABLE IF EXISTS Performance_Indicators CASCADE;";
        W.exec(sql.c_str());

        sql = "CREATE TABLE Performance_Indicators (";
        sql += "id_Perf_Ind SERIAL PRIMARY KEY,";
        sql += "name Varchar(48));";
        W.exec(sql.c_str());

        sql = "INSERT INTO Performance_Indicators VALUES(1,'Coefficient of Determination');"; W.exec(sql.c_str());
        sql = "INSERT INTO Performance_Indicators VALUES(2,'Intercept');"; W.exec(sql.c_str());
        sql = "INSERT INTO Performance_Indicators VALUES(3,'Slope');"; W.exec(sql.c_str());
        sql = "INSERT INTO Performance_Indicators VALUES(4,'Mean Error');"; W.exec(sql.c_str());
        sql = "INSERT INTO Performance_Indicators VALUES(5,'RMSE');"; W.exec(sql.c_str());
        sql = "INSERT INTO Performance_Indicators VALUES(6,'LogLikelihood');"; W.exec(sql.c_str());
        sql = "INSERT INTO Performance_Indicators VALUES(7,'Nash-Sutcliffe R2');"; W.exec(sql.c_str());

        sql = "DROP TABLE IF EXISTS Multirun_Ensemble_Statistics CASCADE;";
        W.exec(sql.c_str());
        sql = "CREATE TABLE multirun_Ensemble_Statistics (";
        sql += "id_simulations Integer REFERENCES simulations(id_simulations), ";
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

        sql = "DROP TABLE IF EXISTS multirun_Ensemble_DefinedCriteria CASCADE;";
        W.exec(sql.c_str());
        sql = "CREATE TABLE multirun_Ensemble_DefinedCriteria (";
        sql += "id_simulations Integer REFERENCES simulations(id_simulations), ";
        sql += "RunNo Integer,";
        sql += "EnsembleNo Integer,";
        sql += "VariableName varchar(34),";
        sql += "Id_Perf_Ind Integer References Performance_Indicators(id_Perf_Ind),";
        sql += "Min Real,";
        sql += "Max Real);";



        sql = "DROP TABLE IF EXISTS multirun_Residuals CASCADE;";
        W.exec(sql.c_str());

        sql = "CREATE TABLE multirun_Residuals (";
        sql += "id_simulations Integer REFERENCES simulations(id_simulations), ";
        sql += "RunNo Integer,";
        sql += "Valfilenumber Integer,";
        sql += "ValFile_ResultIndex Integer,";
        sql += "Residuals Real[]);";

        W.exec(sql.c_str());


        sql = "DROP TABLE IF EXISTS multirun_Ensembles CASCADE;";
        W.exec(sql.c_str());

        sql = "CREATE TABLE multirun_Ensembles (";
        sql += "id_simulations Integer REFERENCES simulations(id_simulations), ";
        sql += "RunNo Integer,";
        sql += "Valfilenumber Integer,";
        sql += "ValFile_ResultIndex Integer,";
        sql += "Ensemble Integer,";
        sql += "NumberofCandidates Integer,";
        sql += "SelectionFraction Real,";
        sql += "PGmin Integer[],";
        sql += "MeanValues Real[],";
        sql += "Min Real[],";
        sql += "Max Real[],";
        sql += "Std Real[],";
        sql += "P10 Real[],";
        sql += "P50 Real[],";
        sql += "P90 Real[]);";

        W.exec(sql.c_str());
        W.commit();
        return 0;

    }
    catch (const std::exception& e) {
        cerr << sql.c_str() << std::endl;
        cerr << e.what() << std::endl;
        return 1;
    }

};
static int create_Additional_Tables(CommonModelInfo* pinfo, unique_ptr<NewMap> pDoc) {
    connection c = initconnection(pinfo->GetPassword());
    string sql;
    pqxx::work W{ c };
    try {
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

        sql = "CREATE TABLE ne_vector_outputs";
        sql += "( \
             Id_DynamicVector INTEGER REFERENCES NE_vectors(Id_dynamicvector),\
            Id_vectoroutputs INTEGER REFERENCES vectoroutputs(Id_vectoroutputs)\
           ); ";

        W.exec(sql.c_str());

        sql = "CREATE TABLE ne_vector_parameters";
        sql += "(    \
            Id_DynamicVector INTEGER REFERENCES NE_vectors(Id_dynamicvector),\
            Id_vectorpar INTEGER REFERENCES vectorparameters(Id_vectorpar)\
          ); ";

        W.exec(sql.c_str());

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
        return 0;

    }
    catch (const std::exception& e) {
        cerr << sql.c_str() << std::endl;
        cerr << e.what() << std::endl;
        return 1;
    }

};
static int query_createtable(enum simtype type, const string str)
{
    string sql;
    connection c = initconnection("pe1950win");
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


        cout << "Records created successfully" << endl;

    }

    catch (const std::exception& e) {
        cerr << e.what() << std::endl;
        return 1;
    }

    return 0;



}
static int query_inserttable(enum simtype type, const string str, vector<SimB*> v_simb, CommonModelInfo* pCommonModel) {
    connection c = initconnection(pCommonModel->GetPassword());
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

            string sqlfk;
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
            bool jump;
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
                id_par = pCommonModel->GetParameterFunctionId(v_simb[i]->GetName());
                sql += FUtil::STD_ItoAscii(id_par) + ",";
                sql += "'" + v_simb[i]->GetName() + "', ";
                sql += sqlfk;

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
            cerr << e.what() << std::endl;
            return 1;
        }
    }
    W.commit();

    return 0;

}
};
using namespace coup_pg;
#endif