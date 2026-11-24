// 原位置：银行管理系统.rar 内的 src/app/PayrollService.cpp，原实现负责转发工资入账。
// 整合扩展：接收基于考勤计算的工资单，按员工和年月查重，记录发放状态及流水号。
#include "PayrollService.h"
#include "../bank/BankService.h"
#include <iostream>

using namespace std;

bool PayrollService::paySalary(BankService& bank, PayrollRecord& record , string& message) {
    int index = findPayrollIndex(record.employeeId, record.year, record.month);
    if (index == -1) {
        message = "未找到对应的工资单";
        return false;
    }
    // 防止重复发薪；金额/账号从已保存的工资单取，不信任界面传入值。
    PayrollRecord& stored = records[index];
    if (stored.status == "已发放") {
        message = "该工资单已发放，不能重复发放";
        return false;
    }
    // 原位置：银行管理系统.rar 内的 src/bank/BankService.cpp 的 depositSalary。
    BankAccount* account = bank.findAccount(stored.accountNo);
    if (!account) {
        message = "该员工还没有银行账户，无法发工资";
        return false;
    }
    if (account->ownerId() != stored.employeeId) {
        message = "该账号不属于当前员工，不能发工资";
        return false;
    }

    int transactionId = 0;
    if (!bank.postTransaction(account->accountNo(), stored.finalsalary, TX_SALARY,
                              "工资入账", transactionId, message)) {
        return false;
    }
    stored.bankTransactionId = transactionId;
    stored.status = "已发放";
    record = stored;
    message = "工资入账成功，账号：" + account->accountNo()
        + "，当前余额：" + moneyToString(account->balance()) + " 元";
    return true;
}

bool PayrollService::addPayroll(const PayrollRecord& record, string& message) {
    // 同一员工同一年月只允许一张工资单，避免重复发薪。
    if (findPayrollIndex(record.employeeId, record.year, record.month) != -1) {
        message = "该员工本月工资单已存在";
        return false;
    }
    records.push_back(record);
    message = "工资单添加成功";
    return true;
}

int PayrollService::findPayrollIndex(const string& employeeId, int year, int month) const {
    int n = records.size();
    for (int i = 0; i < n; ++i) {
        if(records[i].employeeId == employeeId && records[i].year == year && records[i].month == month) {
            return i;
        }
    }
    return -1; //没找到
}

const vector<PayrollRecord>& PayrollService::getPayrollRecords() const {
    return records;
}
