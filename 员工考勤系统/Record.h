#pragma once
//日期和考勤记录结构体声明
#include <string>

using namespace std;

struct Date {
    int year;
    int month;
    int day;
};

struct Record {
    string ID;
    Date date;
    int status;//0:出勤 1:缺勤 2:请假
    //创建数组时无参构造，添加记录使有参构造
    Record() : ID(""), date({0, 0, 0}), status(0) {}
    Record(string ID, Date date, int status) : ID(ID), date(date), status(status) {}
    
};
