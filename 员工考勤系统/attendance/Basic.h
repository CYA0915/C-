#pragma once
//函数声明
#include "Record.h"
using namespace std;

const int MAX_DAYS = 366;
const int MAX_RECORDS = 100;

class Basic{
private:
    dayRecord day[MAX_DAYS];
    int Daycnt;//天数
public:
    Basic();
    void addRecord(Date date, string ID,int status);
    void deleteRecord(Date date, string ID,int status);
    void searchRecord(Date date, string ID);
    void changeRecord(Date date, string ID);
    //search重载部分
    void searchRecord(Date date);
    void searchRecord(string ID);
};