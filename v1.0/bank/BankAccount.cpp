// 原位置：银行管理系统.rar 内的 src/bank/BankAccount.cpp。
#include "BankAccount.h"

#include <iostream>

using namespace std;

BankAccount::BankAccount()
    : balance_(0) {
}

BankAccount::BankAccount(const string& accountNo, const string& ownerId, const string& ownerName,
                         const Date& openDate, Money balance, const Date& lastInterestDate)
    : accountNo_(accountNo), ownerId_(ownerId), ownerName_(ownerName),
      balance_(balance), openDate_(openDate), lastInterestDate_(lastInterestDate) {
}

BankAccount::~BankAccount() {

}

const string& BankAccount::accountNo() const {
    return accountNo_;
}

const string& BankAccount::ownerId() const {
    return ownerId_;
}

const string& BankAccount::ownerName() const {
    return ownerName_;
}

Money BankAccount::balance() const {
    return balance_;
}

Date BankAccount::openDate() const {
    return openDate_;
}

Date BankAccount::lastInterestDate() const {
    return lastInterestDate_;
}

void BankAccount::setLastInterestDate(const Date& date) {
    lastInterestDate_ = date;
}

Money BankAccount::takeAllBalanceForClose() {
    Money amount = balance_;
    balance_ = 0;
    return amount;
}

bool BankAccount::deposit(Money amount, string& message) {

    if (amount <= 0) {
        message = "存款金额必须大于 0";
        return false;
    }
    balance_ += amount;
    message = "存款成功";
    return true;
}

bool BankAccount::applyInterest(Money interest, string& message) {

    return deposit(interest, message);
}

void BankAccount::printInfo() const {

    cout << "账户类型：" << typeName() << "\n";
    cout << "账号：" << accountNo_ << "\n";
    cout << "户主：" << ownerName_ << "（员工编号：" << ownerId_ << "）\n";
    cout << "余额：" << moneyToString(balance_) << " 元\n";
    cout << "开户日期：" << openDate_.toString() << "\n";
    cout << "上次结息日期：" << lastInterestDate_.toString() << "\n";
}

Money BankAccount::minBalance() const {

    return 0;
}

Money BankAccount::creditLimit() const {

    return 0;
}

SavingsAccount::SavingsAccount()
    : minBalance_(0) {
}

SavingsAccount::SavingsAccount(const string& accountNo, const string& ownerId,
                               const string& ownerName, const Date& openDate, Money balance,
                               const Date& lastInterestDate, Money minBalance)
    : BankAccount(accountNo, ownerId, ownerName, openDate, balance, lastInterestDate),
      minBalance_(minBalance) {
}

string SavingsAccount::typeName() const {
    return "储蓄账户";
}

bool SavingsAccount::withdraw(Money amount, string& message) {

    if (amount <= 0) {
        message = "取款金额必须大于 0";
        return false;
    }
    if (balance_ - amount < minBalance_) {
        message = "余额不足，取款后不能低于最低留存余额";
        return false;
    }
    balance_ -= amount;
    message = "取款成功";
    return true;
}

Money SavingsAccount::calculateInterest(const Date& from, const Date& to) const {

    int days = from.daysBetween(to);
    if (days <= 0) {
        return 0;
    }
    double interest_ = balance_ * SAVINGS_ANNUAL_RATE * days / 365;
    return (Money)(interest_ + 0.5);
}

void SavingsAccount::printInfo() const {
    BankAccount::printInfo();
    cout << "最低留存余额：" << moneyToString(minBalance_) << " 元\n";
}

Money SavingsAccount::minBalance() const {
    return minBalance_;
}

CreditAccount::CreditAccount()
    : creditLimit_(0) {
}

CreditAccount::CreditAccount(const string& accountNo, const string& ownerId,
                             const string& ownerName, const Date& openDate, Money balance,
                             const Date& lastInterestDate, Money creditLimit)
    : BankAccount(accountNo, ownerId, ownerName, openDate, balance, lastInterestDate),
      creditLimit_(creditLimit) {
}

string CreditAccount::typeName() const {
    return "信用账户";
}

bool CreditAccount::withdraw(Money amount, string& message) {

    if (amount <= 0) {
        message = "取款金额必须大于 0";
        return false;
    }
    if (balance_ - amount < -creditLimit_) {
        message = "取款超额,取款后欠款不能高于最高信用额度";
        return false;
    }
    balance_ -= amount;
    message = "取款成功";
    return true;
}

Money CreditAccount::calculateInterest(const Date& from, const Date& to) const {

    int days = from.daysBetween(to);
    if (days <= 0) {
        return 0;
    }
    if (balance_ >= 0) {
        return 0;
    }
    double debt = (double)(-balance_);
    double interest_ = debt * CREDIT_DAILY_RATE * days;
    return (Money)(interest_ + 0.5);
}

bool CreditAccount::applyInterest(Money interest, string& message) {

    if (interest <= 0) {
        message = "利息必须大于 0";
        return false;
    }

    balance_ -= interest;
    message = "利息已计入欠款";
    return true;
}

void CreditAccount::printInfo() const {
    BankAccount::printInfo();
    cout << "信用额度：" << moneyToString(creditLimit_) << " 元\n";
    if (balance_ < 0) {
        cout << "当前欠款：" << moneyToString(-balance_)
                  << " 元，可用额度：" << moneyToString(creditLimit_ + balance_) << " 元\n";
    }
}

Money CreditAccount::creditLimit() const {
    return creditLimit_;
}
