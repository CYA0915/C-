#ifndef COMMON_EMPLOYEE_H
#define COMMON_EMPLOYEE_H

#include <string>
#include <iostream>
#include "Money.h"

using namespace std;

class Employee {
//动物编号就是员工编号，避免另建一套员工对象。
protected:
    string id_;
    string name_;
    Money basicSalary_ = 0; //基本工资初始化为0，单位分
    string bankAccountNo_;
public:
    Employee();
    Employee(const string& id, const string& name);
    virtual ~Employee();

    const string& id() const;
    const string& name() const;

    void setId(const string& id);
    void setName(const string& name);
    Money basicSalary() const;
    void setBasicSalary(Money salary);
    const string& bankAccountNo() const;
    void setBankAccountNo(const string& accountNo);
    virtual void printInfo() const;
};

#endif
