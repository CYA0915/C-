// 原位置：银行管理系统.rar 内的 src/bank/BankService.h。
// 交付版调整：集中复用单账户记账，文件读写迁到 file/BankStorage。
#ifndef BANK_BANKSERVICE_H
#define BANK_BANKSERVICE_H

#include <string>
#include <vector>

#include "BankAccount.h"
#include "Transaction.h"
#include "../common/Date.h"
#include "../common/Employee.h"
#include "../common/Money.h"
#include <iostream>

using namespace std;

class BankService {
public:
    BankService() = default;
    ~BankService();

    bool openAccount(const string& accountNo, const Employee& owner, int accountType,
                     Money initialAmount, Money extraParam, string& message);
    bool closeAccount(const string& accountNo, string& message);
    BankAccount* findAccount(const string& accountNo) const;
    // 只读访问账户/流水集合，供界面表格刷新。
    const vector<BankAccount*>& getAccounts() const;
    const vector<Transaction>& getTransactions() const;

    BankAccount* findFirstAccountByOwner(const string& ownerId) const;
    void printAllAccounts() const;
    void printAccountsByOwner(const string& ownerId) const;

    bool deposit(const string& accountNo, Money amount, string& message);
    bool withdraw(const string& accountNo, Money amount, string& message);
    bool transfer(const string& fromAccountNo, const string& toAccountNo,
                  Money amount, string& message);

    // 原银行存取款和工资入账共用此接口，整合时新增的食物扣款也调用它。
    bool postTransaction(const string& accountNo, Money amount, TransactionType type,
                         const string& note, int& transactionId, string& message);
    BankAccount* findAccountForOwner(const string& accountNo, const string& ownerId) const;

    void printTransactions(const string& accountNo) const;

    void printTransactionsByDate(const string& accountNo, const Date& begin, const Date& end) const;

    bool settleInterest(string& message);

private:
    friend class BankStorage;

    int addTransaction(const string& accountNo, TransactionType type, Money amount,
                       Money balanceAfter, const string& note);

    BankService(const BankService&);
    BankService& operator=(const BankService&);

    vector<BankAccount*> accounts_;
    vector<Transaction> transactions_;
    int nextTransactionId_ = 1;
};

#endif
