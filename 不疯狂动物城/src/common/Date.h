#ifndef COMMON_DATE_H
#define COMMON_DATE_H

#include <string>
#include <iostream>

using namespace std;

class Date { // [需补全] 统一考勤与银行的日期类型。
// [补全思路] 保留本类；后续移除attendance/Record.h中同名Date，并调整考勤访问方式；比较可用isSame，或为本类增加operator==。
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

// ============================================================================
// 文件说明（Date.h）
//   日期类 Date：保存年/月/日，提供合法性判断、today()、字符串解析与格式化、
//   日期比较、相差天数计算。交易日期、开户日期、结息日期均使用该类。
//   依赖：仅标准库；被 bank/、app/ 模块使用。
// ============================================================================
