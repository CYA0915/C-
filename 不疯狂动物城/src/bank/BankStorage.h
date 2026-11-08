#ifndef BANK_BANKSTORAGE_H
#define BANK_BANKSTORAGE_H

#include <string>
#include <vector>

#include "BankAccount.h"
#include "Transaction.h"
#include <iostream>

using namespace std;

class BankStorage { // [需补全] 读写新增流水类型和业务关联信息。
// [补全思路] 扩展交易CSV字段，统一业务编号格式并约定旧行默认值；保留金额单位为分。
public:
    BankStorage(const string& dataDir = "data");

    void setDataDir(const string& dataDir);
    string dataDir() const;

    int loadAccounts(vector<BankAccount*>& accounts) const;
    bool saveAccounts(const vector<BankAccount*>& accounts) const;

    int loadTransactions(vector<Transaction>& transactions) const; // [需补全] 恢复消费/工资关联并检查重复。
    // [补全思路] 解析新增类型及业务编号；发现未知类型或坏行时明确报告，不静默当作其他交易。
    bool saveTransactions(const vector<Transaction>& transactions) const; // [需补全] 配合消费/工资新增字段和整体提交。
    // [补全思路] 输出业务编号等新增字段，检查写入失败；与账户和上层业务记录的保存由统一提交方案协调。

    string accountsFilePath() const;
    string transactionsFilePath() const;

private:
    string dataDir_;

    static vector<string> splitLine(const string& line);
    static string trim(const string& text);
    static bool parseCents(const string& text, Money& cents);
};

#endif

// ============================================================================
// 文件说明（BankStorage.h）
//   CSV 文件存储类：负责 accounts.csv 和 transactions.csv 的读写。
//   accounts.csv 列：账号,员工编号,姓名,类型,余额(分),最低余额(分),
//                    信用额度(分),开户日期,上次结息日期
//   transactions.csv 列：流水号,账号,类型,金额(分),交易后余额(分),日期,备注
// ============================================================================
