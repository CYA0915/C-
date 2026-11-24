// 原位置：银行管理系统.rar 内的 src/common/Money.h，保留原整数分金额类型和转换函数。
// 整合复用：银行交易、薪资和食物金额统一使用该类型。
#ifndef COMMON_MONEY_H
#define COMMON_MONEY_H

#include <string>
#include <iostream>

using namespace std;

typedef long long Money;

string moneyToString(Money amount);

bool parseMoney(const string& text, Money& cents);

#endif
