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

class BankService { // [需补全] 新增食物消费与指定业务账户接口。
// [补全思路] 建议payForFood(employeeId,accountNo,amount,purchaseId,transactionId,message)；校验账户归属及有效性后复用account->withdraw，记消费流水。
// [补全思路] 工资和消费使用员工明确绑定的账户；返回持久化状态和流水标识，不能只返回内存操作是否成功。
public:
    BankService();
    BankService(const string& dataDir);
    ~BankService();

    bool loadData();
    bool saveData(); // [需补全] 明确跨文件保存失败的结果。
    // [补全思路] 当前分别写账户与流水，可能只成功一个；后续采用临时文件和可恢复提交记录，或者统一业务快照方案；未完成持久化不能向工资/购买层报告最终成功。

    bool openAccount(const string& accountNo, const Employee& owner, int accountType,
                     Money initialAmount, Money extraParam, string& message); // [需补全] 开户绑定已登记的动物员工。
                     // [补全思路] 调用方从zoo取得animal对象作为Employee引用传入；成功且保存完成后绑定工资/消费账户，失败不保留虚假绑定。
    bool closeAccount(const string& accountNo, string& message); // [需补全] 处理工资及消费账户绑定。
    // [补全思路] 销户前检查绑定、余额、欠款和待处理工资；绑定账户需先更换；保留历史账号与流水关联。

    BankAccount* findAccount(const string& accountNo) const;

    BankAccount* findFirstAccountByOwner(const string& ownerId) const; // [需补全] 不再用第一个账户自动决定工资/消费账户。
    // [补全思路] 保留本函数用于一般查询；发薪和购买改为读取明确的账户绑定，并再次校验ownerId。

    void printAllAccounts() const;
    void printAccountsByOwner(const string& ownerId) const;

    bool deposit(const string& accountNo, Money amount, string& message);
    bool withdraw(const string& accountNo, Money amount, string& message);
    bool transfer(const string& fromAccountNo, const string& toAccountNo,
                  Money amount, string& message);

    bool depositSalary(const string& employeeId, Money amount, string& message); // [需补全] 关联工资单并区分入账与保存结果。
    // [补全思路] 补充指定账号和工资单编号，校验重复入账；返回流水编号；现有函数保存失败仍返回true，扩展时必须修正此语义。

    void printTransactions(const string& accountNo) const;

    void printTransactionsByDate(const string& accountNo, const Date& begin, const Date& end) const;

    bool settleInterest(string& message);

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
