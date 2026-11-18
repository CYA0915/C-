#ifndef BANK_BANKSERVICE_H
#define BANK_BANKSERVICE_H

#include <string>
#include <vector>

#include "BankAccount.h"
#include "BankStorage.h"
#include "Transaction.h"
#include "../common/Date.h"
#include "../common/Employee.h"
#include "../common/Money.h"
#include <iostream>

using namespace std;

class BankService {
public:
    BankService();
    BankService(const string& dataDir);
    ~BankService();

    bool loadData();
    bool saveData(); 

    bool openAccount(const string& accountNo, const Employee& owner, int accountType,
                     Money initialAmount, Money extraParam, string& message);
    bool closeAccount(const string& accountNo, string& message); 
    BankAccount* findAccount(const string& accountNo) const;
    // [Qt 前端支持] 只读访问账户/流水集合，供界面表格刷新。
    const vector<BankAccount*>& getAccounts() const;
    const vector<Transaction>& getTransactions() const;

    BankAccount* findFirstAccountByOwner(const string& ownerId) const; 
    void printAllAccounts() const;
    void printAccountsByOwner(const string& ownerId) const;

    bool deposit(const string& accountNo, Money amount, string& message);
    bool withdraw(const string& accountNo, Money amount, string& message);
    bool transfer(const string& fromAccountNo, const string& toAccountNo,
                  Money amount, string& message);

    bool depositSalary(
        const string& employeeId,
        const string& accountNo,
        Money amount,
        int& transactionId,
        string& message
    );

    void printTransactions(const string& accountNo) const;

    void printTransactionsByDate(const string& accountNo, const Date& begin, const Date& end) const;

    bool settleInterest(string& message);

    bool payForFood(const string& accountNo, Money amount, const string& object,int& transactionId, string& message);

private:

    int addTransaction(const string& accountNo, TransactionType type, Money amount,
                       Money balanceAfter, const string& note);

    BankService(const BankService&);
    BankService& operator=(const BankService&);

    vector<BankAccount*> accounts_;
    vector<Transaction> transactions_;
    BankStorage storage_;
    int nextTransactionId_;
};

#endif

// ============================================================================
// 文件说明（BankService.h）
//   银行核心业务类：管理账户（vector<BankAccount*>）和流水（vector<Transaction>）。
//   对外接口：loadData/saveData、openAccount/closeAccount、deposit/withdraw/transfer、
//             depositSalary、printAllAccounts/printAccountsByOwner、
//             printTransactions/printTransactionsByDate、settleInterest。
//   数据持久化委托 BankStorage 完成；界面层 MainMenu 只调用本类。
//   完成状态：已完成。
// ============================================================================
