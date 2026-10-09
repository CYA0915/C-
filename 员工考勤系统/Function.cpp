#include "Function.h"
#include <iostream>

using namespace std;

Function::Function() {
    cnt = 0;
}

void Function::addRecord(string ID, Date date, int status) {
    //合法判断
    if(cnt >= MAX_RECORDS) {
        cout << "真的装不下了,请删除一些记录后重试" << endl;
        return;
    }
    //实现
    records[cnt] = Record(ID, date, status);//这么写需要补struct的构造函数
    cnt++;
}

void Function::searchRecord(string ID) {
    if(cnt == 0) {
        cout << "这里什么也没有" << endl;
        return;
    }
    for(int i=0; i<cnt; i++) {
        if(records[i].ID == ID) {
            cout << "ID: " << records[i].ID << ", Date: " 
            << records[i].date.year << "-" << records[i].date.month 
            << "-" << records[i].date.day << ", Status: " << records[i].status << endl;
            return;
        }
    }
    cout << "无记录" << endl;
}

void Function::showRecord(string ID) {
    //很乱,要改
    if(cnt == 0) {
        cout << "这里什么也没有" << endl;
        return;
    }
    for(int i=0; i<cnt; i++) {
        if(records[i].ID == ID) {
            cout << "ID: " << records[i].ID << ", Date: " 
            << records[i].date.year << "-" << records[i].date.month 
            << "-" << records[i].date.day << ", Status: " << records[i].status << endl;
        }
    }
}

void Function::deleteRecord(string ID, Date date, int status) {
    for(int i=0; i<cnt; i++) {
        //数组实现，是否该换链表
        if(records[i].ID == ID && records[i].date.year == date.year && records[i].date.month == date.month && records[i].date.day == date.day && records[i].status == status) {
            for(int j=i; j<cnt-1; j++) {
                records[j] = records[j+1];
            }
            cnt--;
            return;
        }
    }
    cout << "无记录" << endl;
}