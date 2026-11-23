#ifndef BANK_TRANSACTION_H
#define BANK_TRANSACTION_H

#include <string>

#include "../common/Date.h"
#include "../common/Money.h"
#include <iostream>

using namespace std;

enum TransactionType {
    TX_DEPOSIT = 0,
    TX_WITHDRAW,
    TX_TRANSFER_IN,
    TX_TRANSFER_OUT,
    TX_SALARY,
    TX_INTEREST,
    TX_FOOD_PURCHASE   // 食物消费流水类型
};

class Transaction {
public:
    Transaction();
    Transaction(int id, const string& accountNo, TransactionType type,
                Money amount, Money balanceAfter, const Date& date, const string& note);

    int id() const;
    const string& accountNo() const;
    TransactionType type() const;
    Money amount() const;
    Money balanceAfter() const;
    Date date() const;
    const string& note() const;

    string typeName() const;

    static bool typeFromName(const string& name, TransactionType& type);

private:
    int id_;
    string accountNo_;
    TransactionType type_;
    Money amount_;
    Money balanceAfter_;
    Date date_;
    string note_;
};

inline Transaction::Transaction()
    : id_(0), type_(TX_DEPOSIT), amount_(0), balanceAfter_(0) {
}

inline Transaction::Transaction(int id, const string& accountNo, TransactionType type,
                                Money amount, Money balanceAfter, const Date& date, const string& note)
    : id_(id), accountNo_(accountNo), type_(type), amount_(amount),
      balanceAfter_(balanceAfter), date_(date), note_(note) {
}

inline int Transaction::id() const {
    return id_;
}

inline const string& Transaction::accountNo() const {
    return accountNo_;
}

inline TransactionType Transaction::type() const {
    return type_;
}

inline Money Transaction::amount() const {
    return amount_;
}

inline Money Transaction::balanceAfter() const {
    return balanceAfter_;
}

inline Date Transaction::date() const {
    return date_;
}

inline const string& Transaction::note() const {
    return note_;
}

inline string Transaction::typeName() const {
    switch (type_) {
    case TX_DEPOSIT:
        return "存款";
    case TX_WITHDRAW:
        return "取款";
    case TX_TRANSFER_IN:
        return "转入";
    case TX_TRANSFER_OUT:
        return "转出";
    case TX_SALARY:
        return "工资入账";
    case TX_INTEREST:
        return "利息";
    case TX_FOOD_PURCHASE:
        return "食物消费";
    default:
        return "未知";
    }
}

inline bool Transaction::typeFromName(const string& name, TransactionType& type) {
    if (name == "存款") {
        type = TX_DEPOSIT;
        return true;
    }
    if (name == "取款") {
        type = TX_WITHDRAW;
        return true;
    }
    if (name == "转入") {
        type = TX_TRANSFER_IN;
        return true;
    }
    if (name == "转出") {
        type = TX_TRANSFER_OUT;
        return true;
    }
    if (name == "工资入账") {
        type = TX_SALARY;
        return true;
    }
    if (name == "利息") {
        type = TX_INTEREST;
        return true;
    }
    if (name == "食物消费") {
        type = TX_FOOD_PURCHASE;
        return true;
    }
    return false;
}

#endif
