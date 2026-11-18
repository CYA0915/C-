#include "PayrollService.h"
#include "PayrollStorage.h"
#include "../bank/BankService.h"
#include <iostream>

using namespace std;

bool PayrollService::paySalary(BankService& bank, PayrollRecord& record , string& message) {
    int index = findPayrollIndex(record.employeeId, record.year, record.month);
    if (index == -1) {
        message = "未找到对应的工资单";
        return false;
    }
    // [Qt 前端支持] 防止重复发薪；金额/账号从已保存的工资单取，不信任界面传入值。
    PayrollRecord& stored = records[index];
    if (stored.status == "已发放") {
        message = "该工资单已发放，不能重复发放";
        return false;
    }
    int transactionId = 0;
    bool success = bank.depositSalary(
        stored.employeeId,
        stored.accountNo,
        stored.finalsalary,
        transactionId,
        message
    );
    if (!success) {
        return false;
    }
    stored.bankTransactionId = transactionId;
    stored.status = "已发放";
    record = stored;
    return true;
}

bool PayrollService::addPayroll(const PayrollRecord& record, string& message) {
    // [Qt 前端支持] 同一员工同一年月只允许一张工资单，避免重复发薪。
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

bool PayrollService::savePayrolls(const string& filename) {
    PayrollStorage storage;
    return storage.Save(records, filename);
}

bool PayrollService::loadPayrolls(const string& filename) {
    PayrollStorage storage;
    return storage.Load(records, filename);
}
// ============================================================================
// 文件说明（PayrollService.cpp）
//   PayrollService::paySalary() 的实现：直接调用 BankService::depositSalary()。
//   工资计算（出勤、迟到扣款等）由考勤模块负责，本文件只处理银行侧入账。
// ============================================================================
