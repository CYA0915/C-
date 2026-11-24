// 原位置：银行管理系统.rar 内的 src/common/Employee.h，原有编号、姓名和访问接口。
// 整合调整：统一动物园管理系统/animal.h 原有的动物编号、姓名，供考勤按员工编号查找。
// 基本工资和绑定账号是整合时新增的字段，供算薪、发薪和购食使用。
#ifndef COMMON_EMPLOYEE_H
#define COMMON_EMPLOYEE_H

#include <string>
#include <iostream>
#include "Money.h"

using namespace std;

class Employee {
// 整合约定：动物园管理系统/animal.h 的动物编号同时作为考勤员工编号。
protected:
    string id_;
    string name_;
    // 整合新增：基本工资以分计，绑定账号用来发薪和购买食物。
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
