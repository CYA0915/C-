#pragma once
#include <string>
#include <vector>
#include "PayrollRecord.h"

using namespace std;
class PayrollStorage { 
public:
    bool Save(const vector<PayrollRecord>& records, const string& filename);
    bool Load(vector<PayrollRecord>& records, const string& filename);
};
