// 整合新增：从考勤记录归纳月度数据，首见于不疯狂动物城/src/attendance/AttendanceSummary.h。
// 数据基础：员工考勤系统/attendance/Record.h 的员工编号和考勤状态。
#pragma once
#include <string>
using namespace std;
//某ID月考勤总体统计
struct AttendanceSummary {
    string employeeID;
    int year = 0;
    int month = 0;
    int scheduledDays = 0; //应出勤
    int presentDays = 0;//出勤
    int absentDays = 0;//缺勤
    int leaveDays = 0;//请假
    int missingDays = 0;//未录入
};
