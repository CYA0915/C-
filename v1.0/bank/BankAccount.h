// 原位置：银行管理系统.rar 内的 src/bank/BankAccount.h。
#ifndef BANK_BANKACCOUNT_H
#define BANK_BANKACCOUNT_H

#include <string>

#include "../common/Date.h"
#include "../common/Money.h"
#include <iostream>

using namespace std;

const double SAVINGS_ANNUAL_RATE = 0.015;

const double CREDIT_DAILY_RATE = 0.0005;

class BankAccount {
protected:
    string accountNo_;
    string ownerId_;
    string ownerName_;
    Money balance_;
    Date openDate_;
    Date lastInterestDate_;
public:
    BankAccount();
    BankAccount(const string& accountNo, const string& ownerId, const string& ownerName,
                const Date& openDate, Money balance, const Date& lastInterestDate);
    virtual ~BankAccount();

    const string& accountNo() const;
    const string& ownerId() const;
    const string& ownerName() const;
    Money balance() const;
    Date openDate() const;
    Date lastInterestDate() const;

    void setLastInterestDate(const Date& date);

    Money takeAllBalanceForClose();

    bool deposit(Money amount, string& message);

    virtual bool withdraw(Money amount, string& message) = 0;

    virtual bool applyInterest(Money interest, string& message);

    virtual string typeName() const = 0;

    virtual Money calculateInterest(const Date& from, const Date& to) const = 0;

    virtual void printInfo() const;

    virtual Money minBalance() const;
    virtual Money creditLimit() const;
};

class SavingsAccount : public BankAccount {
public:
    SavingsAccount();
    SavingsAccount(const string& accountNo, const string& ownerId, const string& ownerName,
                   const Date& openDate, Money balance, const Date& lastInterestDate, Money minBalance);

    string typeName() const;
    bool withdraw(Money amount, string& message);
    Money calculateInterest(const Date& from, const Date& to) const;
    void printInfo() const;
    Money minBalance() const;

private:
    Money minBalance_;
};

class CreditAccount : public BankAccount {
public:
    CreditAccount();
    CreditAccount(const string& accountNo, const string& ownerId, const string& ownerName,
                  const Date& openDate, Money balance, const Date& lastInterestDate, Money creditLimit);

    string typeName() const;
    bool withdraw(Money amount, string& message);
    bool applyInterest(Money interest, string& message);
    Money calculateInterest(const Date& from, const Date& to) const;
    void printInfo() const;
    Money creditLimit() const;

private:
    Money creditLimit_;
};

#endif
