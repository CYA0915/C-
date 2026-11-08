#ifndef APP_PAYROLLSERVICE_H
#define APP_PAYROLLSERVICE_H

#include <string>

#include "../common/Money.h"
#include <iostream>

using namespace std;

class BankService;

class PayrollService { // [需补全] 补充从考勤到银行的完整工资流程。
// [补全思路] 建议增加calculate和按工资单发放接口；依次查询zoo员工、汇总Basic、检查考勤完整、用SalaryPolicy计算并生成PayrollRecord。
// [补全思路] 发放前检查员工月份唯一性与账户绑定；银行入账和工资状态保存共同确认，重复调用返回已有结果，不再加钱。
public:

    bool paySalary(BankService& bank, const string& employeeId, Money amount, string& message); // [需补全] 将手工金额入账扩展为按月工资单发放。
    // [补全思路] 通过工资单取得金额与账号并带业务编号调用银行；保存失败时标记待恢复，不能按现有bool结果直接标为已发放。
    // [补全思路] 实发为0时记录已结算且不生成零元存款；负工资不入账，需检查工资规则。
};

#endif

// ============================================================================
// 文件说明（PayrollService.h）
//   跨系统工资接口：声明 PayrollService::paySalary()。
//   考勤模块根据出勤情况算出工资后，调用该接口把工资存入员工的银行账户。
// ============================================================================
