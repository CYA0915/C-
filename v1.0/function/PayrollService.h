// 原位置：银行管理系统.rar 内的 src/app/PayrollService.h，原实现负责转发工资入账。
// 整合扩展：接收基于考勤计算的工资单，按员工和年月查重，记录发放状态及流水号。
#ifndef APP_PAYROLLSERVICE_H
#define APP_PAYROLLSERVICE_H

#include <string>
#include "PayrollRecord.h"
#include <vector>
#include "../common/Money.h"
#include <iostream>

using namespace std;
class BankService;

class PayrollService {
private:
    friend class CityStorage;
    vector<PayrollRecord> records;
public:
    bool addPayroll(const PayrollRecord& record,string& message);
    int findPayrollIndex(const string& employeeId, int year, int month) const;
    const vector<PayrollRecord>& getPayrollRecords() const;
    bool paySalary(BankService& bank, PayrollRecord& record , string& message);
    // 原位置：savePayrolls / loadPayrolls 已合入 file/CityStorage。
};

#endif
