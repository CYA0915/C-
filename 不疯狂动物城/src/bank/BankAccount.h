#ifndef BANK_BANKACCOUNT_H
#define BANK_BANKACCOUNT_H

#include <string>

#include "../common/Date.h"
#include "../common/Money.h"
#include <iostream>

using namespace std;

const double SAVINGS_ANNUAL_RATE = 0.015;

const double CREDIT_DAILY_RATE = 0.0005;

class BankAccount { // [需补全] 明确账户所属对象为动物员工。
// [补全思路] ownerId保存Employee编号，账户继续独立于动物继承树；现有储蓄/信用扣款规则可直接复用，无需为每个物种创建账户子类。
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

// ============================================================================
// 文件说明（BankAccount.h）
//   账户继承体系：
//     BankAccount（抽象基类）：账号、户主、余额（分）、开户日期、上次结息日期；
//       纯虚函数 withdraw() / calculateInterest() / typeName()；
//       已实现 deposit()、applyInterest()、printInfo()、minBalance()/creditLimit()。
//     SavingsAccount：最低留存余额；CreditAccount：信用额度，允许透支。
//   利率常量：SAVINGS_ANNUAL_RATE（1.5%）、CREDIT_DAILY_RATE（0.05%）。
//   多态说明：BankService 使用 vector<BankAccount*> 管理账户，通过虚函数调用
//             不同账户的取款和计息规则。
//   完成状态：已完成。
// ============================================================================
