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

// ============================================================================
// 文件说明（Employee.cpp）
//   Employee 的构造、员工编号/姓名读写以及 printInfo() 的实现。
// ============================================================================
