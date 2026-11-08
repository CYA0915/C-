#define _CRT_SECURE_NO_WARNINGS

#include "Date.h"

#include <ctime>
#include <iomanip>
#include <sstream>
#include <vector>
#include <iostream>

using namespace std;

namespace {

const int DAYS_IN_MONTH[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

bool isLeapYearValue(int year) {
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

int daysInMonthValue(int year, int month) {
    if (month < 1 || month > 12) {
        return 0;
    }
    if (month == 2 && isLeapYearValue(year)) {
        return 29;
    }
    return DAYS_IN_MONTH[month - 1];
}

string trim(const string& text) {
    size_t begin = 0;
    while (begin < text.size() &&
           (text[begin] == ' ' || text[begin] == '\t' || text[begin] == '\r' || text[begin] == '\n')) {
        ++begin;
    }
    size_t end = text.size();
    while (end > begin &&
           (text[end - 1] == ' ' || text[end - 1] == '\t' || text[end - 1] == '\r' || text[end - 1] == '\n')) {
        --end;
    }
    return text.substr(begin, end - begin);
}

bool isDigits(const string& text) {
    if (text.empty()) {
        return false;
    }
    for (size_t i = 0; i < text.size(); ++i) {
        if (text[i] < '0' || text[i] > '9') {
            return false;
        }
    }
    return true;
}

int toInt(const string& text) {
    istringstream iss(text);
    int value = 0;
    iss >> value;
    return value;
}

}

Date::Date() : year_(1970), month_(1), day_(1) {
}

Date::Date(int year, int month, int day) : year_(year), month_(month), day_(day) {
}

int Date::year() const {
    return year_;
}

int Date::month() const {
    return month_;
}

int Date::day() const {
    return day_;
}

bool Date::isValid() const {
    if (year_ < 1 || year_ > 9999) {
        return false;
    }
    if (month_ < 1 || month_ > 12) {
        return false;
    }
    if (day_ < 1 || day_ > daysInMonthValue(year_, month_)) {
        return false;
    }
    return true;
}

bool Date::isLeapYear() const {
    return isLeapYearValue(year_);
}

int Date::daysInMonth() const {
    return daysInMonthValue(year_, month_);
}

string Date::toString() const {
    ostringstream oss;
    oss << year_ << '-'
        << setw(2) << setfill('0') << month_ << '-'
        << setw(2) << setfill('0') << day_;
    return oss.str();
}

bool Date::parse(const string& text, Date& result) {
    string s = trim(text);

    for (size_t i = 0; i < s.size(); ++i) {
        if (s[i] == '/') {
            s[i] = '-';
        }
    }

    vector<string> parts;
    string current;
    for (size_t i = 0; i <= s.size(); ++i) {
        if (i == s.size() || s[i] == '-') {
            parts.push_back(current);
            current.clear();
        } else {
            current += s[i];
        }
    }
    if (parts.size() != 3) {
        return false;
    }
    if (!isDigits(parts[0]) || !isDigits(parts[1]) || !isDigits(parts[2])) {
        return false;
    }

    Date temp(toInt(parts[0]), toInt(parts[1]), toInt(parts[2]));
    if (!temp.isValid()) {
        return false;
    }
    result = temp;
    return true;
}

Date Date::today() {
    time_t now = time(NULL);
    tm* local = localtime(&now);
    if (local == NULL) {
        return Date();
    }
    return Date(local->tm_year + 1900, local->tm_mon + 1, local->tm_mday);
}

bool Date::isBefore(const Date& other) const {
    return toDayNumber() < other.toDayNumber();
}

bool Date::isSame(const Date& other) const {
    return year_ == other.year_ && month_ == other.month_ && day_ == other.day_;
}

int Date::daysBetween(const Date& other) const {
    return static_cast<int>(other.toDayNumber() - toDayNumber());
}

long long Date::toDayNumber() const {
    long long days = 0;

    for (int y = 1; y < year_; ++y) {
        days += isLeapYearValue(y) ? 366 : 365;
    }

    for (int m = 1; m < month_; ++m) {
        days += daysInMonthValue(year_, m);
    }

    days += day_ - 1;
    return days;
}

// ============================================================================
// 文件说明（Date.cpp）
//   Date 类的实现：闰年与每月天数、合法日期检查、"YYYY-MM-DD" 解析/输出、
//   日期比较、相差天数（通过“从公元 1 年起的天数”换算）。
// ============================================================================
