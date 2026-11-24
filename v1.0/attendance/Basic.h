// 原位置：员工考勤系统/attendance/Basic.h。
// 整合扩展：在原有按日期和员工编号管理考勤的基础上，补充月度汇总和缺勤初始化。
#pragma once
#include "Record.h"
#include <vector>
#include "AttendanceSummary.h"

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
    bool changeRecord(Date date, const string& ID, int newStatus);
    void searchRecord(Date date);
    void searchRecord(string ID);
    bool summarize(
        const string& employeeID,
        int year,
        int month,
        const vector<Date>& scheduledDates,
        AttendanceSummary& result,
        string& message
    )const;

    int getRecord(Date date, Record result[]) const;
    // 只读遍历接口：日期数量与第 index 个 dayRecord，供表格展示。
    int dayCount() const;
    const dayRecord* dayAt(int index) const;
    // 只补充缺少的应出勤记录，默认缺勤，保留已有出勤和请假。
    void initialize(const string& ID,const vector<Date>& scheduledDates);
};
