#pragma once
#include <string>
#include <vector>

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

