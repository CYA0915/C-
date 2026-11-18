#pragma once
#include <string>
#include "../common/Money.h"

using namespace std;

// [Qt 前端支持] 食物购买单快照：保存购买时的食物名称、单价和数量，
// 后续改价不会改变历史记录；bankTransactionId 用于和银行流水对账。
struct FoodPurchaseRecord {
    string purchaseId;          // 购买单编号，如 FP1
    string employeeId;          // 购买员工编号
    string foodId;              // 食物编号
    string foodName;            // 购买时的食物名称快照
    int quantity = 0;           // 数量，单位 kg
    Money unitPrice = 0;        // 购买时单价，分/kg
    Money amount = 0;           // 总金额，分
    string accountNo;           // 扣款账号
    string date;                // 购买日期 YYYY-MM-DD
    string status = "已支付";   // 已支付 / 支付失败
    int bankTransactionId = 0;  // 对应银行流水号
};
