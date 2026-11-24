// 原位置：银行管理系统.rar 内的 src/common/Employee.cpp，原有编号、姓名和访问接口。
// 整合调整：统一动物园管理系统/animal.h 原有的动物编号、姓名，供考勤按员工编号查找。
// 基本工资和绑定账号是整合时新增的字段，供算薪、发薪和购食使用。
#include "Employee.h"

#include <iostream>

using namespace std;

Employee::Employee() {
}

Employee::Employee(const string& id, const string& name) : id_(id), name_(name) {
}

Employee::~Employee() {
}

const string& Employee::id() const {
    return id_;
}

const string& Employee::name() const {
    return name_;
}

void Employee::setId(const string& id) {
    id_ = id;
}

void Employee::setName(const string& name) {
    name_ = name;
}

void Employee::printInfo() const {
    cout << "员工编号：" << id_ << "，姓名：" << name_ << "\n";
}

// 整合新增：基本工资和绑定账号的访问接口。
Money Employee::basicSalary() const {
    return basicSalary_;
}

void Employee::setBasicSalary(Money salary) {
    basicSalary_ = salary;
}

void Employee::setBankAccountNo(const string& accountNo) {
    bankAccountNo_ = accountNo;
}

const string& Employee::bankAccountNo() const {
    return bankAccountNo_;
}
