#ifndef BANK_BANKSTORAGE_H
#define BANK_BANKSTORAGE_H

#include <string>
#include <vector>

#include "BankAccount.h"
#include "Transaction.h"
#include <iostream>

using namespace std;

class BankStorage {
public:
    BankStorage(const string& dataDir = "data");

    void setDataDir(const string& dataDir);
    string dataDir() const;

    int loadAccounts(vector<BankAccount*>& accounts) const;
    bool saveAccounts(const vector<BankAccount*>& accounts) const;

    int loadTransactions(vector<Transaction>& transactions) const;
    bool saveTransactions(const vector<Transaction>& transactions) const;

    string accountsFilePath() const;
    string transactionsFilePath() const;

private:
    string dataDir_;

    static vector<string> splitLine(const string& line);
    static string trim(const string& text);
    static bool parseCents(const string& text, Money& cents);
};

#endif
