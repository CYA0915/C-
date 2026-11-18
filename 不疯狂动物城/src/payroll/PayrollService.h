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
    vector<PayrollRecord> records; 
public:
    bool addPayroll(const PayrollRecord& record,string& message);
    int findPayrollIndex(const string& employeeId, int year, int month) const;
    const vector<PayrollRecord>& getPayrollRecords() const;
    bool paySalary(BankService& bank, PayrollRecord& record , string& message);
    bool savePayrolls(const string& filename);
    bool loadPayrolls(const string& filename);
};

#endif

// ============================================================================
// 文件说明（PayrollService.h）
//   跨系统工资接口：声明 PayrollService::paySalary()。
//   考勤模块根据出勤情况算出工资后，调用该接口把工资存入员工的银行账户。
// ============================================================================
