#pragma once
//函数声明
#include "Record.h"
using namespace std;

const int MAX_RECORDS = 100;

class Function {
private:
    Record records[MAX_RECORDS];
    int cnt;//记录数
public:
    Function();
    void addRecord(string ID,Date date,int status);
    void showRecord(string ID);
    void deleteRecord(string ID,Date date,int status);
    void searchRecord(string ID);
};