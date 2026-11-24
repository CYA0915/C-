// 原位置：员工考勤系统/attendance/Record.h 中的 Record 和 dayRecord。
// 整合调整：银行管理系统.rar 内的 src/common/Date.h 提供日期类，供银行和考勤共用。
#pragma once
#include <string>
#include "../common/Date.h"

using namespace std;

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
