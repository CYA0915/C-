// 原位置：银行管理系统.rar 内的 src/bank/BankStorage.cpp。
// 交付版调整：接收银行服务并统一加载账户、流水及下一笔流水编号。
#include "BankStorage.h"

#include <algorithm>
#include <fstream>
#include <limits>
#include <memory>
#include <utility>

#include "../bank/BankService.h"
#include <sstream>
#include <iostream>

using namespace std;

BankStorage::BankStorage(std::filesystem::path dataDir) : dataDir_(std::move(dataDir)) {
}

std::filesystem::path BankStorage::accountsFilePath() const {
    return dataDir_ / "accounts.csv";
}

std::filesystem::path BankStorage::transactionsFilePath() const {
    return dataDir_ / "transactions.csv";
}

// 原位置：银行管理系统.rar 内的 src/bank/BankService.cpp 的 loadData 和 saveData。
bool BankStorage::load(BankService& bank) const {
    BankService loaded;
    if (std::filesystem::exists(accountsFilePath()) && loadAccounts(loaded.accounts_) < 0) {
        return false;
    }
    if (std::filesystem::exists(transactionsFilePath()) && loadTransactions(loaded.transactions_) < 0) {
        return false;
    }
    for (const Transaction& transaction : loaded.transactions_) {
        if (transaction.id() == std::numeric_limits<int>::max()) {
            return false;
        }
        loaded.nextTransactionId_ = std::max(loaded.nextTransactionId_, transaction.id() + 1);
    }
    bank.accounts_.swap(loaded.accounts_);
    bank.transactions_.swap(loaded.transactions_);
    bank.nextTransactionId_ = loaded.nextTransactionId_;
    return true;
}

bool BankStorage::save(const BankService& bank) const {
    return saveAccounts(bank.getAccounts()) && saveTransactions(bank.getTransactions());
}

vector<string> BankStorage::splitLine(const string& line) {
    vector<string> parts;
    string current;
    for (size_t i = 0; i <= line.size(); ++i) {
        if (i == line.size() || line[i] == ',') {
            parts.push_back(trim(current));
            current.clear();
        } else {
            current += line[i];
        }
    }
    return parts;
}

string BankStorage::trim(const string& text) {
    size_t begin = 0;
    while (begin < text.size() &&
           (text[begin] == ' ' || text[begin] == '\t' || text[begin] == '\r' || text[begin] == '\n')) {
        ++begin;
    }
    size_t end = text.size();
    while (end > begin &&
           (text[end - 1] == ' ' || text[end - 1] == '\t' || text[end - 1] == '\r' || text[end - 1] == '\n')) {
        --end;
    }
    return text.substr(begin, end - begin);
}

bool BankStorage::parseCents(const string& text, Money& cents) {

    istringstream iss(trim(text));
    long long value = 0;
    if (!(iss >> value)) {
        return false;
    }
    char extra = '\0';
    if (iss >> extra) {
        return false;
    }
    cents = value;
    return true;
}

int BankStorage::loadAccounts(vector<BankAccount*>& accounts) const {
    ifstream fin(accountsFilePath());
    if (!fin) {
        return -1;
    }

    string line;
    getline(fin, line);

    int count = 0;
    while (getline(fin, line)) {
        if (trim(line).empty()) {
            continue;
        }
        vector<string> parts = splitLine(line);
        if (parts.size() != 9) {
            continue;
        }

        const string& accountNo = parts[0];
        const string& ownerId = parts[1];
        const string& ownerName = parts[2];
        const string& typeName = parts[3];

        Money balance = 0;
        Money minBalance = 0;
        Money creditLimit = 0;
        if (!parseCents(parts[4], balance) || !parseCents(parts[5], minBalance) ||
            !parseCents(parts[6], creditLimit)) {
            continue;
        }

        Date openDate;
        Date lastInterestDate;
        if (!Date::parse(parts[7], openDate) || !Date::parse(parts[8], lastInterestDate)) {
            continue;
        }

        std::unique_ptr<BankAccount> account;
        if (typeName == "储蓄账户") {
            if (minBalance < 0 || balance < minBalance) {
                continue;
            }
            account = std::make_unique<SavingsAccount>(accountNo, ownerId, ownerName, openDate,
                                         balance, lastInterestDate, minBalance);
        } else if (typeName == "信用账户") {
            if (creditLimit <= 0) {
                continue;
            }
            account = std::make_unique<CreditAccount>(accountNo, ownerId, ownerName, openDate,
                                        balance, lastInterestDate, creditLimit);
        } else {
            continue;
        }

        bool duplicated = false;
        for (size_t i = 0; i < accounts.size(); ++i) {
            if (accounts[i]->accountNo() == accountNo) {
                duplicated = true;
                break;
            }
        }
        if (duplicated) {
            continue;
        }

        accounts.push_back(account.get());
        account.release();
        ++count;
    }
    return fin.bad() ? -1 : count;
}

bool BankStorage::saveAccounts(const vector<BankAccount*>& accounts) const {
    ofstream fout(accountsFilePath());
    if (!fout) {
        return false;
    }

    fout << "账号,员工编号,姓名,类型,余额(分),最低余额(分),信用额度(分),开户日期,上次结息日期\n";
    for (size_t i = 0; i < accounts.size(); ++i) {
        const BankAccount* account = accounts[i];
        fout << account->accountNo() << ','
             << account->ownerId() << ','
             << account->ownerName() << ','
             << account->typeName() << ','
             << account->balance() << ','
             << account->minBalance() << ','
             << account->creditLimit() << ','
             << account->openDate().toString() << ','
             << account->lastInterestDate().toString() << '\n';
    }
    fout.close();
    return !fout.fail();
}

int BankStorage::loadTransactions(vector<Transaction>& transactions) const {
    ifstream fin(transactionsFilePath());
    if (!fin) {
        return -1;
    }

    string line;
    getline(fin, line);

    int count = 0;
    while (getline(fin, line)) {
        if (trim(line).empty()) {
            continue;
        }
        vector<string> parts = splitLine(line);
        if (parts.size() != 7) {
            continue;
        }

        int id = 0;
        istringstream idStream(parts[0]);
        if (!(idStream >> id)) {
            continue;
        }

        Money amount = 0;
        Money balanceAfter = 0;
        if (!parseCents(parts[3], amount) || !parseCents(parts[4], balanceAfter)) {
            continue;
        }

        TransactionType type;
        if (!Transaction::typeFromName(parts[2], type)) {
            continue;
        }

        Date date;
        if (!Date::parse(parts[5], date)) {
            continue;
        }

        transactions.push_back(Transaction(id, parts[1], type, amount, balanceAfter, date, parts[6]));
        ++count;
    }
    return fin.bad() ? -1 : count;
}

bool BankStorage::saveTransactions(const vector<Transaction>& transactions) const {
    ofstream fout(transactionsFilePath());
    if (!fout) {
        return false;
    }

    fout << "流水号,账号,类型,金额(分),交易后余额(分),日期,备注\n";
    for (size_t i = 0; i < transactions.size(); ++i) {
        const Transaction& t = transactions[i];
        fout << t.id() << ','
             << t.accountNo() << ','
             << t.typeName() << ','
             << t.amount() << ','
             << t.balanceAfter() << ','
             << t.date().toString() << ','
             << t.note() << '\n';
    }
    fout.close();
    return !fout.fail();
}
