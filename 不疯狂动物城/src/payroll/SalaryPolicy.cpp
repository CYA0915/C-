#include "SalaryPolicy.h"

SalaryPolicy::SalaryPolicy(
    Money absentDeductionPerDay,
    Money leaveDeductionPerDay,
    Money bonus
) : absentDeductionPerDay(absentDeductionPerDay),
    leaveDeductionPerDay(leaveDeductionPerDay),
    bonus(bonus) {}

bool SalaryPolicy::calculate(
    const Employee& employee,
    const AttendanceSummary& summary,
    PayrollRecord& result,
    string& message
) const {
    if (summary.employeeID != employee.id() || employee.id().empty()) {
        message = "员工编号不匹配或为空";
        return false;
    }

    long long totalDays = summary.presentDays;
    totalDays += summary.absentDays;
    totalDays += summary.leaveDays;
    totalDays += summary.missingDays;

    if (totalDays != summary.scheduledDays) {
        message = "考勤统计不一致";
        return false;
    }
    if (summary.missingDays > 0) {
        message = "考勤存在未录入天数";
        return false;
    }

    Money salary = employee.basicSalary();
    Money absentDeduction = summary.absentDays * absentDeductionPerDay;
    Money leaveDeduction = summary.leaveDays * leaveDeductionPerDay;

    PayrollRecord record;
    record.employeeId = employee.id();
    record.year = summary.year;
    record.month = summary.month;
    record.payrollId = employee.id() + "-" + to_string(summary.year) + "-" + to_string(summary.month);
    record.accountNo = employee.bankAccountNo();

    record.attendance = summary;
    record.basicSalary = salary;
    record.absentDeduction = absentDeduction;
    record.leaveDeduction = leaveDeduction;
    record.bonus = bonus;
    record.finalsalary = salary - absentDeduction - leaveDeduction + bonus;
    record.status = "未发放";
    record.bankTransactionId = 0; // 尚未发放，没有银行流水

    result = record;
    message = "工资计算成功";
    return true;
}
