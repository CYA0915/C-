// 原位置：银行管理系统.rar 内的 src/common/Date.h，沿用日期解析、校验和天数运算。
// 整合复用：员工考勤系统/attendance/Record.h 原有独立日期结构，整合版考勤共用银行的 Date。
#ifndef COMMON_DATE_H
#define COMMON_DATE_H

#include <string>
#include <iostream>

using namespace std;

class Date {
public:
    Date();
    Date(int year, int month, int day);

    int year() const;
    int month() const;
    int day() const;

    bool isValid() const;
    bool isLeapYear() const;
    int daysInMonth() const;

    string toString() const;

    static bool parse(const string& text, Date& result);

    static Date today();

    bool isBefore(const Date& other) const;
    bool isSame(const Date& other) const;

    int daysBetween(const Date& other) const;

    // 保留员工考勤系统/attendance/Record.h 原先使用的日期相等判断接口。
    bool operator==(const Date& other) const {
        return isSame(other);
    }

private:
    int year_;
    int month_;
    int day_;

    long long toDayNumber() const;
};

#endif
