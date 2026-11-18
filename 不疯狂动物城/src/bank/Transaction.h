#ifndef BANK_TRANSACTION_H
#define BANK_TRANSACTION_H

#include <string>

#include "../common/Date.h"
#include "../common/Money.h"
#include <iostream>

using namespace std;

enum TransactionType { // [需补全] 增加食物消费类型。
// [补全思路] 在现有枚举末尾追加TX_FOOD_PURCHASE，保留旧枚举值；同步typeName/typeFromName和存储解析。
    TX_DEPOSIT = 0,
    TX_WITHDRAW,
    TX_TRANSFER_IN,
    TX_TRANSFER_OUT,
    TX_SALARY,
    TX_INTEREST,
    TX_FOOD_PURCHASE   // [Qt 前端支持] 食物消费独立成类，便于界面筛选。
};

class Transaction { // [需补全] 关联工资单或食物购买单。
// [补全思路] 增加业务编号，或先约定可解析且唯一的编号字段；金额继续用分，交易日期不等于工资所属月份，工资月份由工资单记录。
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

    string typeName() const; // [需补全] 识别新增消费流水类型。
    // [补全思路] 在实现的switch中增加TX_FOOD_PURCHASE分支，显示食物购买。

    static bool typeFromName(const string& name, TransactionType& type); // [需补全] 读取新增消费流水类型。
    // [补全思路] 增加食物购买名称到枚举的映射，与typeName输出保持一致。

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

// ============================================================================
// 文件说明（Transaction.h）
//   交易流水类 Transaction（头文件内实现）：
//   流水号、账号、交易类型、金额（分）、交易后余额（分）、日期、备注。
//   类型枚举：TX_DEPOSIT / TX_WITHDRAW / TX_TRANSFER_IN / TX_TRANSFER_OUT /
//             TX_SALARY / TX_INTEREST。
//   typeName() / typeFromName() 用于界面显示和 CSV 读写。
// ============================================================================
