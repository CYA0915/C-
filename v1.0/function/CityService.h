// 交付版新增：统一组织排班、开户绑定、算薪、发薪与购买流程。
// 基础接口：员工考勤系统/attendance/Basic.h、动物园管理系统/zoo.h、
// 以及银行管理系统.rar 内的 src/bank/BankService.h。
#pragma once

#include <memory>
#include <string>
#include <vector>

#include "../attendance/Basic.h"
#include "../bank/BankService.h"
#include "../zoo/zoo.h"
#include "FoodPurchaseService.h"
#include "PayrollService.h"

class SalaryPolicy;

class CityService {
public:
    zoo& animals() { return state_->animals; }
    Basic& attendance() { return state_->attendance; }
    BankService& bank() { return state_->bank; }
    const PayrollService& payroll() const { return state_->payroll; }
    const FoodPurchaseService& food() const { return state_->food; }

    static std::vector<Date> scheduledDates(int year, int month);
    bool initializeAttendance(int year, int month, std::string& message);
    bool summarizeAttendance(const std::string& employeeId, int year, int month,
                             AttendanceSummary& summary, std::string& message) const;

    bool updateEmployee(const std::string& employeeId, const std::string& name, Money salary,
                        const std::string& accountNo, std::string& message);
    bool openAccount(const std::string& employeeId, const std::string& accountNo, int type,
                     Money initialAmount, Money accountRule, std::string& message);
    bool closeAccount(const std::string& accountNo, std::string& message);

    bool generatePayroll(const std::string& employeeId, int year, int month,
                         const SalaryPolicy& policy, PayrollRecord& record, std::string& message);
    bool paySalary(const std::string& payrollId, PayrollRecord& record, std::string& message);

    bool addFood(const Food& item, std::string& message);
    bool purchaseFood(const std::string& employeeId, const std::string& foodId, int quantity,
                      int& transactionId, std::string& message);

private:
    friend class CityStorage;

    struct State {
        zoo animals;
        Basic attendance;
        BankService bank;
        PayrollService payroll;
        FoodPurchaseService food;
    };

    // 考勤定长数组放在堆上。整份读取成功后，可以一次替换所有业务状态。
    std::unique_ptr<State> state_ = std::make_unique<State>();
};
