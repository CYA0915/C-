#ifndef COMMON_EMPLOYEE_H
#define COMMON_EMPLOYEE_H

#include <string>
#include <iostream>
#include "Money.h"

using namespace std;

class Employee { // [需补全] 补充动物员工公共资料。
// [补全思路] 增加基本工资Money（单位分）、工资及消费账户号、在职状态；
//动物编号就是员工编号，避免另建一套员工对象。
protected:
    string id_;
    string name_;
    Money basicSalary_;
    string bankAccountNo_;
public:
    Employee();
    Employee(const string& id, const string& name); // [需补全] 扩展员工构造参数。
    // [补全思路] 初始化基本工资和状态；与animal及各子类约定同一构造参数传递顺序；开户和文件读写放在服务中，不放进构造函数。
    virtual ~Employee();

    const string& id() const;
    const string& name() const;

    void setId(const string& id); // [需补全] 限制已关联业务的身份编号修改。
    // [补全思路] 动物建立考勤、账户或工资记录后不允许直接改编号；优先创建后固定编号，防止历史记录断开。
    void setName(const string& name); // [需补全] 保持动物姓名与账户显示一致。
    // [补全思路] 动物改名调用此接口；账户ownerId保持不变，ownerName作为开户时快照，当前姓名从动物员工资料查询。
    Money basicSalary() const;
    void setBasicSalary(Money salary); //设置已关联业务的基本工资修改。
    const string& bankAccountNo() const;
    void setBankAccountNo(const string& accountNo); //设置限制已关联业务的账户号修改。
    virtual void printInfo() const;
};

#endif

// ============================================================================
// 文件说明（Employee.h）
//   员工基类：保存员工编号和姓名，提供 id()/name()/printInfo()，析构函数为虚函数。
//   组内共用：银行账户通过“员工编号”关联员工；考勤/动物园模块可在此基础上派生。
// ============================================================================
