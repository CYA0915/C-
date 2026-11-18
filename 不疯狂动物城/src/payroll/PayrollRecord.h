#pragma once
#include <string>
#include "../common/Money.h"
#include "../attendance/AttendanceSummary.h"

using namespace std;

class PayrollRecord { 
public:
    string payrollId; // 工资单编号
    string employeeId; // 员工编号

    int year = 0;
    int month = 0;

    string accountNo; // 收款账号

    AttendanceSummary attendance;//计算工资时所用的考勤统计

    Money basicSalary = 0; // 基本工资
    Money absentDeduction = 0; //缺勤扣款
    Money leaveDeduction = 0; //请假扣款
    Money bonus = 0; // 奖金
    Money finalsalary = 0; // 最后实发工资

    string status = "未发放"; //未发放、已发放
    int bankTransactionId = 0; //银行流水号
    // 待实现：工资单、所属月份、发放状态；待定义字段。 // [需补全] 定义月工资单和发放状态。
// [补全思路] 建议字段：payrollId、employeeId、year、month、accountNo、基本工资、考勤快照、扣款、奖金、实发、状态、银行流水号。
// [补全思路] 员工编号+年月唯一；状态至少区分未发放、处理中、已发放；保存后可恢复，已发放工资单不随后来考勤或基本工资变化而改写。
};
