#pragma once
#include "Record.h"
#include "Basic.h"

using namespace std;

class FileOperation {
public:
    bool Save(const Basic& basic, const string& filename);//const只读
    bool Load(Basic& basic, const string& filename);
};