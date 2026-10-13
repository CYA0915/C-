#pragma once
//日期和考勤记录结构体声明
#include <string>

using namespace std;

struct Date {
    int year;
    int month;
    int day;
    bool operator==(const Date& other) const {
        return (year == other.year && month == other.month && day == other.day);
    }
};

struct Record {
    string ID;
    int status;//0:缺勤 1:出勤 2:请假
    //创建数组时无参构造，添加记录使有参构造
    Record() : ID(""),  status(0) {}
    Record(string ID, int status) : ID(ID), status(status) {}
};

struct dayRecord {
    Date date;
    Record dayrecords[100];//一天最多一百条
    int Recordcnt;
    dayRecord() : Recordcnt(0) {}

};
