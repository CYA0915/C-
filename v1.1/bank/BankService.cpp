// 原位置：银行管理系统.rar 内的 src/bank/BankService.cpp。
// 交付版调整：集中复用单账户记账，文件读写迁到 file/BankStorage。
#include "BankService.h"

#include <iostream>
#include <sstream>

using namespace std;

namespace {

bool isAllDigits(const string& text) {
    if (text.empty()) {
        return false;
    }
    for (size_t i = 0; i < text.size(); ++i) {
        if (text[i] < '0' || text[i] > '9') {
            return false;
        }
    }
    return true;
}

bool containsComma(const string& text) {
    return text.find(',') != string::npos;
}

string trimText(const string& text) {
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

void printTransactionLine(const Transaction& t) {
    cout << t.id() << '\t'
              << t.accountNo() << '\t'
              << t.typeName() << '\t'
              << moneyToString(t.amount()) << '\t'
              << moneyToString(t.balanceAfter()) << '\t'
              << t.date().toString() << '\t'
              << t.note() << '\n';
}

}

BankService::~BankService() {

    for (size_t i = 0; i < accounts_.size(); ++i) {
        delete accounts_[i];
    }
}

// 银行管理系统.rar 内的 src/bank/BankService.cpp 的 loadData / saveData 已迁到 file/BankStorage.cpp。

bool BankService::openAccount(const string& accountNo, const Employee& owner, int accountType,
                              Money initialAmount, Money extraParam, string& message) {

    const string newAccountNo = trimText(accountNo);
    const string newOwnerId = trimText(owner.id());
    const string newOwnerName = trimText(owner.name());

    if (newAccountNo.size() < 6 || newAccountNo.size() > 19 || !isAllDigits(newAccountNo)) {
        message = "账号必须是 6~19 位数字，例如 6222020000001";
        return false;
    }
    if (findAccount(newAccountNo) != NULL) {
        message = "账号已存在，请换一个账号";
        return false;
    }

    if (newOwnerId.empty() || newOwnerName.empty()) {
        message = "员工编号和姓名不能为空";
        return false;
    }
    if (containsComma(newOwnerId) || containsComma(newOwnerName)) {
        message = "员工编号和姓名中不能包含英文逗号";
        return false;
    }

    if (initialAmount < 0) {
        message = "首次存款金额不能是负数";
        return false;
    }

    Date today = Date::today();
    BankAccount* account = NULL;
    if (accountType == 1) {

        if (extraParam < 0) {
            message = "最低留存余额不能是负数";
            return false;
        }
        if (initialAmount < extraParam) {
            message = "首次存款不能低于最低留存余额";
            return false;
        }
        account = new SavingsAccount(newAccountNo, newOwnerId, newOwnerName,
                                     today, initialAmount, today, extraParam);
    } else if (accountType == 2) {

        if (extraParam <= 0) {
            message = "信用额度必须大于 0";
            return false;
        }
        account = new CreditAccount(newAccountNo, newOwnerId, newOwnerName,
                                    today, initialAmount, today, extraParam);
    } else {
        message = "账户类型只能是 1（储蓄账户）或 2（信用账户）";
        return false;
    }

    accounts_.push_back(account);

    if (initialAmount > 0) {
        addTransaction(newAccountNo, TX_DEPOSIT, initialAmount, account->balance(), "开户存入");
    }

    message = "开户成功，账号：" + newAccountNo;
    return true;
}

bool BankService::closeAccount(const string& accountNo, string& message) {
    BankAccount* account = findAccount(accountNo);
    if (account == NULL) {
        message = "账号不存在";
        return false;
    }

    if (account->balance() < 0) {
        message = "账户还有欠款，不能销户，请先把欠款还清";
        return false;
    }

    Money remaining = account->takeAllBalanceForClose();
    if (remaining > 0) {
        addTransaction(accountNo, TX_WITHDRAW, remaining, 0, "销户取出余额");
    }

    for (size_t i = 0; i < accounts_.size(); ++i) {
        if (accounts_[i] == account) {
            delete account;
            accounts_.erase(accounts_.begin() + i);
            break;
        }
    }

    if (remaining > 0) {
        message = "销户成功，已取出余额 " + moneyToString(remaining) + " 元，账号：" + accountNo;
    } else {
        message = "销户成功，账号：" + accountNo;
    }
    return true;
}

const vector<BankAccount*>& BankService::getAccounts() const {
    return accounts_;
}

const vector<Transaction>& BankService::getTransactions() const {
    return transactions_;
}

BankAccount* BankService::findAccount(const string& accountNo) const {
    const string key = trimText(accountNo);
    for (size_t i = 0; i < accounts_.size(); ++i) {
        if (accounts_[i]->accountNo() == key) {
            return accounts_[i];
        }
    }
    return NULL;
}

BankAccount* BankService::findFirstAccountByOwner(const string& ownerId) const {
    const string key = trimText(ownerId);
    for (size_t i = 0; i < accounts_.size(); ++i) {
        if (accounts_[i]->ownerId() == key) {
            return accounts_[i];
        }
    }
    return NULL;
}

void BankService::printAllAccounts() const {
    if (accounts_.empty()) {
        cout << "当前没有账户。\n";
        return;
    }
    for (size_t i = 0; i < accounts_.size(); ++i) {
        accounts_[i]->printInfo();
        cout << "----------------------------------------\n";
    }
}

void BankService::printAccountsByOwner(const string& ownerId) const {
    const string key = trimText(ownerId);
    bool found = false;
    for (size_t i = 0; i < accounts_.size(); ++i) {
        if (accounts_[i]->ownerId() == key) {
            accounts_[i]->printInfo();
            cout << "----------------------------------------\n";
            found = true;
        }
    }
    if (!found) {
        cout << "没有找到该员工的账户。\n";
    }
}

// 复用原银行的 deposit、withdraw 和 depositSalary，并接入整合时新增的食物扣款。
bool BankService::postTransaction(const string& accountNo, Money amount, TransactionType type,
                                  const string& note, int& transactionId, string& message) {
    transactionId = 0;
    BankAccount* account = findAccount(accountNo);
    if (!account) {
        message = "账号不存在";
        return false;
    }

    const bool debit = type == TX_WITHDRAW || type == TX_FOOD_PURCHASE;
    if (!debit && type != TX_DEPOSIT && type != TX_SALARY) {
        message = "该交易类型不支持单账户记账";
        return false;
    }
    const bool success = debit ? account->withdraw(amount, message) : account->deposit(amount, message);
    if (!success) {
        return false;
    }
    transactionId = addTransaction(account->accountNo(), type, amount, account->balance(), note);
    return true;
}

bool BankService::deposit(const string& accountNo, Money amount, string& message) {
    int transactionId = 0;
    if (!postTransaction(accountNo, amount, TX_DEPOSIT, "柜台存款", transactionId, message)) {
        return false;
    }
    const Transaction& transaction = transactions_.back();
    message = "存款成功，账号：" + transaction.accountNo()
        + "，当前余额：" + moneyToString(transaction.balanceAfter()) + " 元";
    return true;
}

bool BankService::withdraw(const string& accountNo, Money amount, string& message) {
    int transactionId = 0;
    if (!postTransaction(accountNo, amount, TX_WITHDRAW, "柜台取款", transactionId, message)) {
        return false;
    }
    const Transaction& transaction = transactions_.back();
    message = "取款成功，账号：" + transaction.accountNo()
        + "，当前余额：" + moneyToString(transaction.balanceAfter()) + " 元";
    return true;
}

BankAccount* BankService::findAccountForOwner(const string& accountNo, const string& ownerId) const {
    BankAccount* account = findAccount(accountNo);
    return account && account->ownerId() == ownerId ? account : nullptr;
}

bool BankService::transfer(const string& fromAccountNo, const string& toAccountNo,
                           Money amount, string& message) {

    BankAccount* account1 = findAccount(fromAccountNo);
    BankAccount* account2 = findAccount(toAccountNo);
    if (amount <= 0) {
        message = "转账金额必须大于0";
        return false;
    }
    if (account1 == NULL) {
        message = "转出账号不存在";
        return false;
    }
    if (account2 == NULL) {
        message = "转入账号不存在";
        return false;
    }
    if (account1 == account2) {
        message = "转出/转入不能是同一账号";
        return false;
    }
    if (!account1->withdraw(amount, message)) {
        return false;
    }
    account2->deposit(amount, message);
    addTransaction(account1->accountNo(), TX_TRANSFER_OUT, amount, account1->balance(), "转账给" + account2->accountNo());
    addTransaction(account2->accountNo(), TX_TRANSFER_IN, amount, account2->balance(), "来自" + account1->accountNo() + "的转账");
    message = "转账成功，已从 " + account1->accountNo() + " 转出 " + moneyToString(amount) + " 元到 " + account2->accountNo();
    return true;
}

void BankService::printTransactions(const string& accountNo) const {
    cout << "流水号\t账号\t类型\t金额(元)\t余额(元)\t日期\t备注\n";

    bool found = false;
    for (size_t i = 0; i < transactions_.size(); ++i) {
        if (!accountNo.empty() && transactions_[i].accountNo() != accountNo) {
            continue;
        }
        printTransactionLine(transactions_[i]);
        found = true;
    }
    if (!found) {
        cout << "没有找到交易记录。\n";
    }
}

void BankService::printTransactionsByDate(const string& accountNo, const Date& begin,
                                          const Date& end) const {
    cout << "流水号\t账号\t类型\t金额(元)\t余额(元)\t日期\t备注\n";

    bool found = false;
    for (size_t i = 0; i < transactions_.size(); ++i) {
        const Transaction& t = transactions_[i];
        if (!accountNo.empty() && t.accountNo() != accountNo) {
            continue;
        }

        if (t.date().isBefore(begin) || end.isBefore(t.date())) {
            continue;
        }
        printTransactionLine(t);
        found = true;
    }
    if (!found) {
        cout << "没有符合条件的交易记录。\n";
    }
}

bool BankService::settleInterest(string& message) {

    Date today = Date::today();
    Money totalInterest = 0;
    int count = 0;
    for (size_t i = 0; i < accounts_.size(); ++i) {
        BankAccount* account = accounts_[i];
        Money interest = account->calculateInterest(account->lastInterestDate(), today);
        if (interest > 0) {
            string message_;
            account->applyInterest(interest, message_);
            addTransaction(account->accountNo(), TX_INTEREST, interest, account->balance(), "结息");
            totalInterest += interest;
            count++;
        }
        account->setLastInterestDate(today);
    }
    ostringstream oss;
    oss << "结息完成，" << count << " 个账户共产生利息 " << moneyToString(totalInterest) << " 元";
    message = oss.str();
    return true;
}

int BankService::addTransaction(const string& accountNo, TransactionType type, Money amount,
                                Money balanceAfter, const string& note) {
    int id = nextTransactionId_;
    ++nextTransactionId_;

    string safeNote;
    safeNote.reserve(note.size());
    for (size_t i = 0; i < note.size(); ++i) {
        if (note[i] == ',') {
            safeNote += "，";
        } else if (note[i] == '\r' || note[i] == '\n') {
            safeNote += ' ';
        } else {
            safeNote += note[i];
        }
    }

    Date today = Date::today();
    transactions_.push_back(Transaction(id, accountNo, type, amount, balanceAfter, today, safeNote));
    return id;
}
