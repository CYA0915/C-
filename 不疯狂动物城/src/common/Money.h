#ifndef COMMON_MONEY_H
#define COMMON_MONEY_H

#include <string>
#include <iostream>

using namespace std;

typedef long long Money;

string moneyToString(Money amount);

bool parseMoney(const string& text, Money& cents);

#endif
