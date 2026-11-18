//增删查改
#include "Basic.h"
#include <iostream>

using namespace std;

Basic::Basic() {
    Daycnt = 0;
}

void Basic::addRecord(Date date, string ID,int status) {
    int DayIndex = -1;//索引，判断是否是新日期
    for(int i=0; i<Daycnt; i++) {
        if(day[i].date == date) {
            DayIndex = i;
            break;
        }
    }
    //新日期
    if(DayIndex == -1){
        //边界检查
        if(Daycnt >= MAX_DAYS) {
            cout << "日期已满" << endl;
            return;
        }
        //添加新日期
        day[Daycnt].date = date;
        day[Daycnt].dayrecords[0] = Record(ID, status);//第一条
        day[Daycnt].Recordcnt = 1;
        Daycnt++;
        return;
    }
    //已有日期
    else {
        //记录存在吗
        for(int j=0; j<day[DayIndex].Recordcnt; j++) {
            if(day[DayIndex].dayrecords[j].ID == ID) {
                cout << "该记录已存在" << endl;
                return;
            }
        }
        //添加新记录
            //边界检查
            if(day[DayIndex].Recordcnt >= MAX_RECORDS) {
                cout << "该日期记录已满" << endl;
                return;
            }
            int cnt = day[DayIndex].Recordcnt;
            day[DayIndex].dayrecords[cnt] = Record(ID, status);
            day[DayIndex].Recordcnt++;
            return;
        }
        cout << "错误" << endl;
    return;
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

void Basic::changeRecord(Date date, string ID) { // [需补全] 后续改为接收新状态参数。
// [补全思路] 同步头文件签名并去掉cin，修改前检查工资结算状态；本次保留原实现。
    for(int i=0; i<Daycnt; i++) {
        if(day[i].date == date) {
            for(int j=0; j<day[i].Recordcnt; j++) {
                if(day[i].dayrecords[j].ID == ID) {
                    cout << "记录存在" << endl;
                    cout << "当前状态: " ;
                    if(day[i].dayrecords[j].status == 0) {
                        cout << "缺勤" << endl;
                    }
                    else if(day[i].dayrecords[j].status == 1) {
                        cout << "出勤" << endl;
                    }
                    else if(day[i].dayrecords[j].status == 2) {
                        cout << "请假" << endl;
                    }
                    int newStatus;
                    cout << "请输入新的状态(0:缺勤 1:出勤 2:请假): ";
                    cin >> newStatus;
                    if(newStatus < 0 || newStatus > 2) {
                        cout << "无效状态" << endl;
                        return;
                    }
                    day[i].dayrecords[j].status = newStatus;
                    cout << "修改成功" << endl;
                    return;
                }
            }
        }
    }
    cout << "无记录" << endl;
    return;
}

//search重载部分
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

// [Qt 前端支持] 新的修改接口：状态由参数传入，成功返回 true。
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

// [Qt 前端支持] 返回已使用的日期组数。
int Basic::dayCount() const {
    return Daycnt;
}

// [Qt 前端支持] 越界返回 nullptr，界面必须判空后再读取。
const dayRecord* Basic::dayAt(int index) const {
    if (index < 0 || index >= Daycnt) {
        return nullptr;
    }
    return &day[index];
}
