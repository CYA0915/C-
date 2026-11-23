#include "BankStorage.h"

#include <fstream>
#include <sstream>
#include <iostream>

using namespace std;

BankStorage::BankStorage(const string& dataDir) : dataDir_(dataDir) {
}

void BankStorage::setDataDir(const string& dataDir) {
    dataDir_ = dataDir;
}

string BankStorage::dataDir() const {
    return dataDir_;
}

string BankStorage::accountsFilePath() const {
    return dataDir_ + "/accounts.csv";
}

string BankStorage::transactionsFilePath() const {
    return dataDir_ + "/transactions.csv";
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
    ifstream fin(accountsFilePath().c_str());
    if (!fin) {
        return 0;
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

        BankAccount* account = NULL;
        if (typeName == "储蓄账户") {
            if (minBalance < 0 || balance < minBalance) {
                continue;
            }
            account = new SavingsAccount(accountNo, ownerId, ownerName, openDate,
                                         balance, lastInterestDate, minBalance);
        } else if (typeName == "信用账户") {
            if (creditLimit <= 0) {
                continue;
            }
            account = new CreditAccount(accountNo, ownerId, ownerName, openDate,
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
            delete account;
            continue;
        }

        accounts.push_back(account);
        ++count;
    }
    return count;
}

bool BankStorage::saveAccounts(const vector<BankAccount*>& accounts) const {
    ofstream fout(accountsFilePath().c_str());
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
    fout.flush();
    return fout.good();
}

int BankStorage::loadTransactions(vector<Transaction>& transactions) const {
    ifstream fin(transactionsFilePath().c_str());
    if (!fin) {
        return 0;
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
    return count;
}

bool BankStorage::saveTransactions(const vector<Transaction>& transactions) const {
    ofstream fout(transactionsFilePath().c_str());
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
    fout.flush();
    return fout.good();
}
