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
