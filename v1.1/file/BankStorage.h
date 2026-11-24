// 原位置：银行管理系统.rar 内的 src/bank/BankStorage.h。
// 交付版调整：接收银行服务并统一加载账户、流水及下一笔流水编号。
#ifndef BANK_BANKSTORAGE_H
#define BANK_BANKSTORAGE_H

#include <filesystem>
#include <string>
#include <vector>

#include "../bank/BankAccount.h"
#include "../bank/Transaction.h"
#include <iostream>

using namespace std;

class BankService;

class BankStorage {
public:
    explicit BankStorage(std::filesystem::path dataDir);

    bool load(BankService& bank) const;
    bool save(const BankService& bank) const;

private:
    std::filesystem::path dataDir_;
    std::filesystem::path accountsFilePath() const;
    std::filesystem::path transactionsFilePath() const;
    int loadAccounts(vector<BankAccount*>& accounts) const;
    bool saveAccounts(const vector<BankAccount*>& accounts) const;
    int loadTransactions(vector<Transaction>& transactions) const;
    bool saveTransactions(const vector<Transaction>& transactions) const;

    static vector<string> splitLine(const string& line);
    static string trim(const string& text);
    static bool parseCents(const string& text, Money& cents);
};

#endif
