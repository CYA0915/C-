#include "PayrollService.h"

#include "../bank/BankService.h"
#include <iostream>

using namespace std;

bool PayrollService::paySalary(BankService& bank, const string& employeeId,
                               Money amount, string& message) { // [需补全] 当前仅转发入账，尚无考勤工资计算。
                               // [补全思路] 按头文件方案先生成并核验工资单，再处理入账及发放状态；本次保留函数体。

    return bank.depositSalary(employeeId, amount, message);
}

// ============================================================================
// 文件说明（PayrollService.cpp）
//   PayrollService::paySalary() 的实现：直接调用 BankService::depositSalary()。
//   工资计算（出勤、迟到扣款等）由考勤模块负责，本文件只处理银行侧入账。
// ============================================================================
