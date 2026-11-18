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
