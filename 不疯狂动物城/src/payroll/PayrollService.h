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
