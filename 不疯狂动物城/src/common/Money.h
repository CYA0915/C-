#ifndef COMMON_MONEY_H
#define COMMON_MONEY_H

#include <string>
#include <iostream>

using namespace std;

typedef long long Money;

string moneyToString(Money amount);

bool parseMoney(const string& text, Money& cents);

#endif

// ============================================================================
// 文件说明（Money.h）
//   金额工具：typedef long long Money，单位是“分”（100 表示 1.00 元）。
//   提供 moneyToString()（分 -> “元”字符串）与 parseMoney()（用户输入 -> 分）。
//   全项目金额计算都使用整数分，避免 double 精度误差。
// ============================================================================
