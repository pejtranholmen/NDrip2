#pragma once
#include <cstdlib>
#include <iostream>
#ifdef MS_CODE
#include <direct.h>
#else
#include <unistd.h>

#endif
#include <filesystem>
#include <sys/stat.h>


using namespace std;

namespace FileSystemUtil {


    string GetCurrentPath() {
        string path;
#ifdef MS_CODE
        char* buff;
        buff = _getcwd(nullptr, 0);
        path = buff; path += "\\";
#else
        path = get_current_dir_name();
        path += "/";
#endif
        return path;
    };


    vector<string> GetFileList(string type) {
        vector<string> out;
        struct stat sb;
        namespace fs = std::filesystem;
        string path = GetCurrentPath();
        // Looping until all the items of the directory are
        // exhausted
        if (type.size() == 0) type = ".";
        
        for (const auto& entry : fs::directory_iterator(path)) {

            // Converting the path to const char * in the
            // subsequent lines
            std::filesystem::path outfilename = entry.path();
            std::string outfilename_str = outfilename.string();
            const char* path = outfilename_str.c_str();

            // Testing whether the path points to a
            // non-directory or not If it does, displays path
            if (stat(path, &sb) == 0 && !(sb.st_mode & S_IFDIR)&& outfilename_str.find(type)!=string::npos)
                out.push_back(outfilename_str);
         }
         return out;
    }

};

