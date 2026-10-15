#include "FileOperation.h"
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>

using namespace std;

bool FileOperation::Save(const Basic& basic, const string& filename) {
    ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    for(int i = 0; i < basic.Daycnt; i++) {
        for(int j = 0; j < basic.day[i].Recordcnt; j++) {
            file << basic.day[i].date.year << " "
                 << basic.day[i].date.month << " "
                 << basic.day[i].date.day << " "
                 << basic.day[i].dayrecords[j].ID << " "
                 << basic.day[i].dayrecords[j].status << endl;
        }
    }
    file.close();
    return !file.fail();
}

bool FileOperation::Load(Basic& basic, const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    string line;
    while (getline(file, line)) {
        if(line.empty()) {
            continue;
        }
        istringstream input(line);
        Date date;
        string ID;
        int status;
        if(!(input >> date.year >> date.month >> date.day >> ID >> status)) {
            return false;
        }
        basic.addRecord(date, ID, status);
    }
    return true;
}