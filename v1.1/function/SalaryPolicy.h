// 整合新增：首见于不疯狂动物城/src/payroll/SalaryPolicy.h。
// 业务依据：员工考勤系统/attendance/Basic.h 的考勤记录经月度汇总后，与基本工资一起用于算薪。
#pragma once
#include <string>
#include "../common/Employee.h"
#include "../attendance/AttendanceSummary.h"
#include "PayrollRecord.h"

using namespace std;

class SalaryPolicy {
private:
    Money absentDeductionPerDay = 0;
    Money leaveDeductionPerDay = 0;
    Money bonus; // 奖金

public:
    SalaryPolicy(
        Money absentDeductionPerDay,
        Money leaveDeductionPerDay,
        Money bonus = 0
    );

    bool calculate(
        const Employee& employee,
        const AttendanceSummary& summary,
        PayrollRecord& result,
        string& message
    ) const;
};
