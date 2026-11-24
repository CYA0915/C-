// 原位置：员工考勤系统/attendance/Basic.cpp。
// 整合扩展：在原有按日期和员工编号管理考勤的基础上，补充月度汇总和缺勤初始化。
#include "Basic.h"
#include <iostream>

using namespace std;

Basic::Basic() {
    Daycnt = 0;
}

bool Basic::addRecord(Date date, string ID, int status) {
    if (!date.isValid() || ID.empty() || status < 0 || status > 2) {
        return false;
    }
    int index = 0;
    for (; index < Daycnt; ++index) {
        if (day[index].date == date) {
            break;
        }
    }
    if (index == Daycnt) {
        if (Daycnt == MAX_DAYS) {
            return false;
        }
        day[index].date = date;
        ++Daycnt;
    }
    dayRecord& records = day[index];
    for (int i = 0; i < records.Recordcnt; ++i) {
        if (records.dayrecords[i].ID == ID) {
            return false;
        }
    }
    if (records.Recordcnt == MAX_RECORDS) {
        return false;
    }
    records.dayrecords[records.Recordcnt++] = Record(ID, status);
    return true;
}

void Basic::searchRecord(Date date, string ID) {
    if(Daycnt == 0) {
        cout << "无日期" << endl;
        return;
    }
    for(int i=0; i<Daycnt; i++) {
        if(day[i].date == date) {
            for(int j=0; j<day[i].Recordcnt; j++) {
                if(day[i].dayrecords[j].ID == ID) {
                    cout << "ID: " << day[i].dayrecords[j].ID << ", Date: "
                         << day[i].date.year() << "-" << day[i].date.month()
                         << "-" << day[i].date.day() << ", Status: " << day[i].dayrecords[j].status << endl;
                    return;
                }
            }
        }
    }
    cout << "无记录" << endl;
    return;
}

void Basic::deleteRecord(Date date,string ID, int status) {
    for(int i=0; i<Daycnt; i++) {
        if(day[i].date == date) {
            for(int j=0; j<day[i].Recordcnt; j++) {
                if(day[i].dayrecords[j].ID == ID) {
                    if(day[i].dayrecords[j].status == status) {
                        for(int k=j; k<day[i].Recordcnt-1; k++) {
                            day[i].dayrecords[k] = day[i].dayrecords[k+1];
                        }
                        day[i].Recordcnt--;
                        cout << "删除成功" << endl;
                        return;
                    }
                    else {
                        cout << "记录不匹配，删除失败" << endl;
                        return;
                    }
                }
        }
    }
}
    cout << "无记录" << endl;
    return;
}

void Basic::searchRecord(Date date) {
    if(Daycnt == 0) {
        cout << "无日期" << endl;
        return;
    }
    for(int i=0; i<Daycnt; i++) {
        if(day[i].date == date) {
            cout << "Date: " << day[i].date.year() << "-" << day[i].date.month() << "-" << day[i].date.day() << endl;
            for(int j=0; j<day[i].Recordcnt; j++) {
                cout << "    "<<"ID: " << day[i].dayrecords[j].ID << ", Status: " << day[i].dayrecords[j].status << endl;
            }
            return;
        }
    }
    cout << "无日期记录" << endl;
    return;
}

void Basic::searchRecord(string ID) {
    if(Daycnt == 0) {
        cout << "无日期" << endl;
        return;
    }
    bool found = false;
    for(int i=0; i<Daycnt; i++) {
        for(int j=0; j<day[i].Recordcnt; j++) {
            if(day[i].dayrecords[j].ID == ID) {
                found = true;
                cout << "ID: " << day[i].dayrecords[j].ID << ", Date: "
                     << day[i].date.year() << "-" << day[i].date.month()
                     << "-" << day[i].date.day() << ", Status: " << day[i].dayrecords[j].status << endl;
            }
        }
    }
    if(!found) {
        cout << "无ID记录" << endl;
    }
    return;
}

int Basic::getRecord(Date date, Record result[]) const {
    for(int i=0; i<Daycnt; i++) {
        if(day[i].date == date) {
            for(int j=0; j<day[i].Recordcnt; j++) {
                result[j] = day[i].dayrecords[j];
            }
            return day[i].Recordcnt;
        }
    }
    return 0;
}

bool Basic::summarize(
    const string& employeeID,
    int year,
    int month,
    const vector<Date>& scheduledDates,
    AttendanceSummary& result,
    string& message
) const {
    if(employeeID.empty() || !Date(year,month,1).isValid()) {
        message = "员工编号为空或年月不合法";
        return false;
    }

    AttendanceSummary summary;
    summary.employeeID = employeeID;
    summary.year = year;
    summary.month = month;
    bool attended[32] = {};

    for(const Date& date : scheduledDates){
        //合法判断
        if (!date.isValid() ||
        date.year() != year ||
        date.month() != month) {
        message = "应出勤日期不合法或不属于统计月份";
        return false;
    }
        //出勤日期重复判断
        if(attended[date.day()] == 1){
            message = "存在重复应出勤日期";
            return false;
        }
        //应出勤日期统计
        attended[date.day()] = true;
        summary.scheduledDays++;

        //员工处理
        Record records[MAX_RECORDS];
        int count = getRecord(date,records);
        bool found = 0;
        for(int i = 0;i<count;i++){
            if(records[i].ID != employeeID){
                continue;
            }
            //处理出勤状态
            found = 1;
            switch(records[i].status) {
                case 0:
                    summary.absentDays++;
                    break;
                case 1:
                    summary.presentDays++;
                    break;
                case 2:
                    summary.leaveDays++;
                    break;
                default:
                    message = "考勤记录中存在未知状态,请检查";
                    return false;
            }
            break;
        }
        if(!found) {
            summary.missingDays++;
        }
    }
    result = summary;
    message = "统计成功";
    return true;
}

// 按日期和员工编号修改状态，成功返回 true。
bool Basic::changeRecord(Date date, const string& ID, int newStatus) {
    if (newStatus < 0 || newStatus > 2) {
        return false;
    }
    for (int i = 0; i < Daycnt; ++i) {
        if (day[i].date == date) {
            for (int j = 0; j < day[i].Recordcnt; ++j) {
                if (day[i].dayrecords[j].ID == ID) {
                    day[i].dayrecords[j].status = newStatus;
                    return true;
                }
            }
        }
    }
    return false;
}

// 返回已使用的日期组数。
int Basic::dayCount() const {
    return Daycnt;
}

// 越界返回 nullptr，界面必须判空后再读取。
const dayRecord* Basic::dayAt(int index) const {
    if (index < 0 || index >= Daycnt) {
        return nullptr;
    }
    return &day[index];
}

void Basic::initialize(const string& ID, const vector<Date>& scheduledDates) {
    for (const Date& date : scheduledDates) {
        Record records[MAX_RECORDS];
        int count = getRecord(date, records);
        bool found = false;
        for (int i = 0; i < count; i++){
            if (records[i].ID == ID) {
                found = true;
                break;
            }
        }
        if (!found) {
            addRecord(date, ID, 0); // 无记录日期默认状态为缺勤
        }
    }
}
