// 交付版新增：统一组织排班、开户绑定、算薪、发薪与购买流程。
// 基础接口：员工考勤系统/attendance/Basic.h、动物园管理系统/zoo.h、
// 以及银行管理系统.rar 内的 src/bank/BankService.h。
#include "CityService.h"

#include <algorithm>

#include "SalaryPolicy.h"

namespace {

bool isClosedMonth(int year, int month) {
    const Date firstDay(year, month, 1);
    return firstDay.isValid() && Date(year, month, firstDay.daysInMonth()).isBefore(Date::today());
}

}

// 整合新增：为员工考勤系统/attendance/Basic.h 的按日考勤提供周一至周五排班。
std::vector<Date> CityService::scheduledDates(int year, int month) {
    const Date firstDay(year, month, 1);
    std::vector<Date> dates;
    if (!firstDay.isValid()) {
        return dates;
    }

    const Date monday(1970, 1, 5);
    int weekday = (monday.daysBetween(firstDay) % 7 + 7) % 7;
    for (int day = 1; day <= firstDay.daysInMonth(); ++day) {
        if (weekday < 5) {
            dates.emplace_back(year, month, day);
        }
        weekday = (weekday + 1) % 7;
    }
    return dates;
}

// 整合新增：遍历动物园管理系统/zoo.h 的动物集合，用考勤服务补齐员工缺勤记录。
bool CityService::initializeAttendance(int year, int month, std::string& message) {
    const auto dates = scheduledDates(year, month);
    if (dates.empty()) {
        message = "考勤年月不合法";
        return false;
    }
    for (const animal* employee : state_->animals.getanimals()) {
        const std::string& id = employee->id();
        state_->attendance.initialize(id, dates);
        AttendanceSummary summary;
        if (!state_->attendance.summarize(id, year, month, dates, summary, message)) {
            return false;
        }
        if (summary.missingDays > 0) {
            message = "考勤记录空间不足：当前最多保存366个日期，每天最多100名员工";
            return false;
        }
    }
    message = "考勤初始化完成";
    return true;
}

// 整合扩展：在员工考勤系统/attendance/Basic.h 的考勤记录基础上按员工和年月汇总。
bool CityService::summarizeAttendance(const std::string& employeeId, int year, int month,
                                     AttendanceSummary& summary, std::string& message) const {
    if (!state_->animals.findbyid(employeeId)) {
        message = "员工不存在";
        return false;
    }
    return state_->attendance.summarize(employeeId, year, month, scheduledDates(year, month),
                                        summary, message);
}

// 整合扩展：动物园管理系统/zoo.cpp 的 updateanimal 原先修改动物姓名，现结合工资和账号。
bool CityService::updateEmployee(const std::string& employeeId, const std::string& name, Money salary,
                                 const std::string& accountNo, std::string& message) {
    animal* employee = state_->animals.findbyid(employeeId);
    if (!employee || name.empty() || salary < 0) {
        message = "请检查员工、姓名和基本工资";
        return false;
    }
    const BankAccount* account = nullptr;
    if (!accountNo.empty()) {
        account = state_->bank.findAccountForOwner(accountNo, employeeId);
        if (!account) {
            message = "绑定账号必须是该员工名下已开立的银行账户";
            return false;
        }
    }
    employee->setName(name);
    employee->setBasicSalary(salary);
    employee->setBankAccountNo(account ? account->accountNo() : "");
    message = "员工资料已更新";
    return true;
}

// 原位置：银行管理系统.rar 内的 src/bank/BankService.cpp 的 openAccount 和 closeAccount。
// 整合扩展：开户时绑定动物员工，销户时解除当前绑定。
bool CityService::openAccount(const std::string& employeeId, const std::string& accountNo, int type,
                              Money initialAmount, Money accountRule, std::string& message) {
    animal* employee = state_->animals.findbyid(employeeId);
    if (!employee) {
        message = "员工不存在";
        return false;
    }
    if (!state_->bank.openAccount(accountNo, *employee, type, initialAmount, accountRule, message)) {
        return false;
    }
    employee->setBankAccountNo(state_->bank.findAccount(accountNo)->accountNo());
    return true;
}

bool CityService::closeAccount(const std::string& accountNo, std::string& message) {
    const BankAccount* account = state_->bank.findAccount(accountNo);
    if (!account) {
        message = "账号不存在";
        return false;
    }
    const std::string canonicalNo = account->accountNo();
    animal* employee = state_->animals.findbyid(account->ownerId());
    // 在销户前用银行查找规则比较账号，兼容旧资料的首尾空格。
    const bool unbind = employee && state_->bank.findAccount(employee->bankAccountNo()) == account;
    if (!state_->bank.closeAccount(canonicalNo, message)) {
        return false;
    }
    if (unbind) {
        employee->setBankAccountNo("");
    }
    return true;
}

// 整合流程：在员工考勤系统/attendance/Basic.h 的考勤基础上算薪，
// 发放时复用银行管理系统.rar 内的 src/app/PayrollService.cpp 的银行入账流程。
bool CityService::generatePayroll(const std::string& employeeId, int year, int month,
                                  const SalaryPolicy& policy, PayrollRecord& record, std::string& message) {
    const animal* employee = state_->animals.findbyid(employeeId);
    if (!employee) {
        message = "员工不存在";
        return false;
    }
    if (!isClosedMonth(year, month)) {
        message = "请选择已经结束的有效月份结算工资";
        return false;
    }
    if (state_->payroll.findPayrollIndex(employeeId, year, month) != -1) {
        message = "该员工本月工资单已存在";
        return false;
    }
    if (!initializeAttendance(year, month, message)) {
        return false;
    }
    AttendanceSummary summary;
    return summarizeAttendance(employeeId, year, month, summary, message)
        && policy.calculate(*employee, summary, record, message)
        && state_->payroll.addPayroll(record, message);
}

bool CityService::paySalary(const std::string& payrollId, PayrollRecord& record, std::string& message) {
    const auto& records = state_->payroll.getPayrollRecords();
    const auto match = std::find_if(records.begin(), records.end(), [&](const PayrollRecord& candidate) {
        return candidate.payrollId == payrollId;
    });
    if (match == records.end()) {
        message = "未找到对应的工资单";
        return false;
    }
    if (!isClosedMonth(match->year, match->month)) {
        message = "该月份尚未结束，不能发放整月工资";
        return false;
    }
    record = *match;
    return state_->payroll.paySalary(state_->bank, record, message);
}

// 整合新增：将食物目录录入交给 FoodPurchaseService。
bool CityService::addFood(const Food& item, std::string& message) {
    return state_->food.addFood(item, message);
}

// 整合新增：从动物园管理系统/zoo.h 的动物集合查找员工，再使用绑定的银行账号购买。
bool CityService::purchaseFood(const std::string& employeeId, const std::string& foodId,
                               int quantity, int& transactionId, std::string& message) {
    transactionId = 0;
    const animal* employee = state_->animals.findbyid(employeeId);
    if (!employee) {
        message = "员工不存在";
        return false;
    }
    return state_->food.purchaseFood(state_->bank, *employee, foodId, quantity, transactionId, message);
}
