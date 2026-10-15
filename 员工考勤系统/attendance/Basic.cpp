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
                         << day[i].date.year << "-" << day[i].date.month 
                         << "-" << day[i].date.day << ", Status: " << day[i].dayrecords[j].status << endl;
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

void Basic::changeRecord(Date date, string ID) {
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
            cout << "Date: " << day[i].date.year << "-" << day[i].date.month << "-" << day[i].date.day << endl;
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
                     << day[i].date.year << "-" << day[i].date.month 
                     << "-" << day[i].date.day << ", Status: " << day[i].dayrecords[j].status << endl;
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