#include "sqliteexporter.h"

#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QUuid>
#include <QVariant>
#include <QStringList>

#include "../src/zoo/animal.h"
#include "../src/zoo/zoo.h"
#include "../src/attendance/Basic.h"
#include "../src/bank/BankService.h"
#include "../src/payroll/PayrollService.h"
#include "../src/food/FoodPurchaseService.h"

namespace {

// [SQLite 导出] 执行一条建表/索引语句；失败时把 SQL 和错误一起返回。
bool execSql(QSqlQuery &query, const QString &sql, QString &error)
{
    if (!query.exec(sql)) {
        error = QStringLiteral("执行 SQL 失败：") + query.lastError().text() +
                QStringLiteral("\nSQL: ") + sql;
        return false;
    }
    return true;
}

// [SQLite 导出] 预编译并绑定参数后执行；所有表写入都走这里，便于统一报错。
bool insertRow(QSqlQuery &query, const QString &sql, const QVariantList &values, QString &error)
{
    if (!query.prepare(sql)) {
        error = QStringLiteral("准备 SQL 失败：") + query.lastError().text() +
                QStringLiteral("\nSQL: ") + sql;
        return false;
    }
    for (int i = 0; i < values.size(); ++i) {
        query.bindValue(i, values.at(i));
    }
    if (!query.exec()) {
        error = QStringLiteral("写入数据失败：") + query.lastError().text() +
                QStringLiteral("\nSQL: ") + sql;
        return false;
    }
    return true;
}

QString s(const std::string &value)
{
    return QString::fromStdString(value);
}

// [SQLite 导出] 在已打开的数据库连接上建表并写数据。
bool fillDatabase(QSqlDatabase &db,
                  zoo &animalManager,
                  Basic &attendance,
                  BankService &bank,
                  PayrollService &payroll,
                  FoodPurchaseService &foodService,
                  QString &error)
{
    if (!db.transaction()) {
        error = QStringLiteral("开启事务失败：") + db.lastError().text();
        return false;
    }

    QSqlQuery query(db);
    const QStringList schema = {
        QStringLiteral(
            "CREATE TABLE employees ("
            "employee_id TEXT PRIMARY KEY,"
            "name TEXT NOT NULL,"
            "category TEXT,"
            "species TEXT,"
            "extra TEXT,"
            "basic_salary_fen INTEGER NOT NULL DEFAULT 0,"
            "bank_account_no TEXT)"),
        QStringLiteral(
            "CREATE TABLE feed_records ("
            "employee_id TEXT NOT NULL,"
            "date TEXT NOT NULL,"
            "amount_kg REAL NOT NULL)"),
        QStringLiteral(
            "CREATE TABLE health_records ("
            "employee_id TEXT NOT NULL,"
            "date TEXT NOT NULL,"
            "status TEXT,"
            "note TEXT)"),
        QStringLiteral(
            "CREATE TABLE attendance ("
            "date TEXT NOT NULL,"
            "employee_id TEXT NOT NULL,"
            "status INTEGER NOT NULL,"
            "PRIMARY KEY(date, employee_id))"),
        QStringLiteral(
            "CREATE TABLE bank_accounts ("
            "account_no TEXT PRIMARY KEY,"
            "owner_id TEXT NOT NULL,"
            "owner_name TEXT,"
            "account_type TEXT,"
            "balance_fen INTEGER NOT NULL,"
            "min_balance_fen INTEGER NOT NULL,"
            "credit_limit_fen INTEGER NOT NULL,"
            "open_date TEXT,"
            "last_interest_date TEXT)"),
        QStringLiteral(
            "CREATE TABLE transactions ("
            "transaction_id INTEGER PRIMARY KEY,"
            "account_no TEXT NOT NULL,"
            "type TEXT,"
            "amount_fen INTEGER NOT NULL,"
            "balance_after_fen INTEGER NOT NULL,"
            "date TEXT,"
            "note TEXT)"),
        QStringLiteral(
            "CREATE TABLE payrolls ("
            "payroll_id TEXT PRIMARY KEY,"
            "employee_id TEXT NOT NULL,"
            "year INTEGER NOT NULL,"
            "month INTEGER NOT NULL,"
            "account_no TEXT,"
            "basic_salary_fen INTEGER,"
            "absent_deduction_fen INTEGER,"
            "leave_deduction_fen INTEGER,"
            "bonus_fen INTEGER,"
            "final_salary_fen INTEGER,"
            "status TEXT,"
            "bank_transaction_id INTEGER,"
            "scheduled_days INTEGER,"
            "present_days INTEGER,"
            "absent_days INTEGER,"
            "leave_days INTEGER,"
            "missing_days INTEGER)"),
        QStringLiteral(
            "CREATE TABLE foods ("
            "food_id TEXT PRIMARY KEY,"
            "food_name TEXT NOT NULL,"
            "price_per_kg_fen INTEGER NOT NULL)"),
        QStringLiteral(
            "CREATE TABLE food_purchases ("
            "purchase_id TEXT PRIMARY KEY,"
            "employee_id TEXT NOT NULL,"
            "food_id TEXT,"
            "food_name TEXT,"
            "quantity INTEGER,"
            "unit_price_fen INTEGER,"
            "amount_fen INTEGER,"
            "account_no TEXT,"
            "date TEXT,"
            "status TEXT,"
            "bank_transaction_id INTEGER)"),
        QStringLiteral(
            "CREATE TABLE export_info ("
            "key TEXT PRIMARY KEY,"
            "value TEXT)"),
        QStringLiteral("CREATE INDEX idx_attendance_employee ON attendance(employee_id)"),
        QStringLiteral("CREATE INDEX idx_transactions_account ON transactions(account_no)"),
        QStringLiteral("CREATE INDEX idx_payrolls_employee ON payrolls(employee_id, year, month)"),
        QStringLiteral("CREATE INDEX idx_feed_employee ON feed_records(employee_id)"),
        QStringLiteral("CREATE INDEX idx_health_employee ON health_records(employee_id)")
    };
    for (const QString &sql : schema) {
        if (!execSql(query, sql, error)) {
            db.rollback();
            return false;
        }
    }

    // ---- 动物员工 + 饲养/健康记录 ----
    int employeeCount = 0;
    int feedCount = 0;
    int healthCount = 0;
    const std::vector<animal *> &animals = animalManager.getanimals();
    for (size_t i = 0; i < animals.size(); ++i) {
        animal *a = animals[i];
        if (!a) {
            continue;
        }
        QString extra;
        if (bird *b = dynamic_cast<bird *>(a)) {
            extra = QString::number(b->getwinglength()) + QStringLiteral(" cm");
        } else if (mammal *m = dynamic_cast<mammal *>(a)) {
            extra = s(m->getdiettype());
        }
        if (!insertRow(query,
                       QStringLiteral("INSERT INTO employees VALUES (?,?,?,?,?,?,?)"),
                       {s(a->getid()), s(a->getname()), s(a->getcategory()), s(a->getspecies()),
                        extra, QVariant(static_cast<qint64>(a->basicSalary())), s(a->bankAccountNo())},
                       error)) {
            db.rollback();
            return false;
        }
        ++employeeCount;

        const std::vector<feedrecord> &feeds = a->getfeedrecords();
        for (size_t j = 0; j < feeds.size(); ++j) {
            if (!insertRow(query,
                           QStringLiteral("INSERT INTO feed_records VALUES (?,?,?)"),
                           {s(a->getid()), s(feeds[j].date), feeds[j].amount},
                           error)) {
                db.rollback();
                return false;
            }
            ++feedCount;
        }
        const std::vector<healthrecord> &healths = a->gethealthrecords();
        for (size_t j = 0; j < healths.size(); ++j) {
            if (!insertRow(query,
                           QStringLiteral("INSERT INTO health_records VALUES (?,?,?,?)"),
                           {s(a->getid()), s(healths[j].date), s(healths[j].status), s(healths[j].note)},
                           error)) {
                db.rollback();
                return false;
            }
            ++healthCount;
        }
    }

    // ---- 考勤 ----
    int attendanceCount = 0;
    for (int i = 0; i < attendance.dayCount(); ++i) {
        const dayRecord *day = attendance.dayAt(i);
        if (!day) {
            continue;
        }
        for (int j = 0; j < day->Recordcnt; ++j) {
            if (!insertRow(query,
                           QStringLiteral("INSERT INTO attendance VALUES (?,?,?)"),
                           {s(day->date.toString()), s(day->dayrecords[j].ID), day->dayrecords[j].status},
                           error)) {
                db.rollback();
                return false;
            }
            ++attendanceCount;
        }
    }

    // ---- 银行账户与流水 ----
    int accountCount = 0;
    const std::vector<BankAccount *> &accounts = bank.getAccounts();
    for (size_t i = 0; i < accounts.size(); ++i) {
        BankAccount *a = accounts[i];
        if (!a) {
            continue;
        }
        if (!insertRow(query,
                       QStringLiteral("INSERT INTO bank_accounts VALUES (?,?,?,?,?,?,?,?,?)"),
                       {s(a->accountNo()), s(a->ownerId()), s(a->ownerName()), s(a->typeName()),
                        QVariant(static_cast<qint64>(a->balance())),
                        QVariant(static_cast<qint64>(a->minBalance())),
                        QVariant(static_cast<qint64>(a->creditLimit())),
                        s(a->openDate().toString()), s(a->lastInterestDate().toString())},
                       error)) {
            db.rollback();
            return false;
        }
        ++accountCount;
    }

    int transactionCount = 0;
    const std::vector<Transaction> &transactions = bank.getTransactions();
    for (size_t i = 0; i < transactions.size(); ++i) {
        const Transaction &t = transactions[i];
        if (!insertRow(query,
                       QStringLiteral("INSERT INTO transactions VALUES (?,?,?,?,?,?,?)"),
                       {t.id(), s(t.accountNo()), s(t.typeName()),
                        QVariant(static_cast<qint64>(t.amount())),
                        QVariant(static_cast<qint64>(t.balanceAfter())),
                        s(t.date().toString()), s(t.note())},
                       error)) {
            db.rollback();
            return false;
        }
        ++transactionCount;
    }

    // ---- 工资单（含考勤快照） ----
    int payrollCount = 0;
    const std::vector<PayrollRecord> &payrolls = payroll.getPayrollRecords();
    for (size_t i = 0; i < payrolls.size(); ++i) {
        const PayrollRecord &r = payrolls[i];
        if (!insertRow(query,
                       QStringLiteral("INSERT INTO payrolls VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)"),
                       {s(r.payrollId), s(r.employeeId), r.year, r.month, s(r.accountNo),
                        QVariant(static_cast<qint64>(r.basicSalary)),
                        QVariant(static_cast<qint64>(r.absentDeduction)),
                        QVariant(static_cast<qint64>(r.leaveDeduction)),
                        QVariant(static_cast<qint64>(r.bonus)),
                        QVariant(static_cast<qint64>(r.finalsalary)),
                        s(r.status), r.bankTransactionId,
                        r.attendance.scheduledDays, r.attendance.presentDays,
                        r.attendance.absentDays, r.attendance.leaveDays,
                        r.attendance.missingDays},
                       error)) {
            db.rollback();
            return false;
        }
        ++payrollCount;
    }

    // ---- 食物目录与购买记录 ----
    int foodCount = 0;
    const std::vector<Food> &foods = foodService.getFoods();
    for (size_t i = 0; i < foods.size(); ++i) {
        if (!insertRow(query,
                       QStringLiteral("INSERT INTO foods VALUES (?,?,?)"),
                       {s(foods[i].getFoodId()), s(foods[i].getFoodName()),
                        QVariant(static_cast<qint64>(foods[i].getPricePerKg()))},
                       error)) {
            db.rollback();
            return false;
        }
        ++foodCount;
    }

    int purchaseCount = 0;
    const std::vector<FoodPurchaseRecord> &purchases = foodService.getPurchaseRecords();
    for (size_t i = 0; i < purchases.size(); ++i) {
        const FoodPurchaseRecord &p = purchases[i];
        if (!insertRow(query,
                       QStringLiteral("INSERT INTO food_purchases VALUES (?,?,?,?,?,?,?,?,?,?,?)"),
                       {s(p.purchaseId), s(p.employeeId), s(p.foodId), s(p.foodName),
                        p.quantity, QVariant(static_cast<qint64>(p.unitPrice)),
                        QVariant(static_cast<qint64>(p.amount)), s(p.accountNo), s(p.date),
                        s(p.status), p.bankTransactionId},
                       error)) {
            db.rollback();
            return false;
        }
        ++purchaseCount;
    }

    // ---- 导出元信息 ----
    const QList<QPair<QString, QString> > info = {
        {QStringLiteral("exported_at"), QDateTime::currentDateTime().toString(Qt::ISODate)},
        {QStringLiteral("source_data_dir"), QDir::currentPath()},
        {QStringLiteral("format_version"), QStringLiteral("1")},
        {QStringLiteral("employees"), QString::number(employeeCount)},
        {QStringLiteral("feed_records"), QString::number(feedCount)},
        {QStringLiteral("health_records"), QString::number(healthCount)},
        {QStringLiteral("attendance"), QString::number(attendanceCount)},
        {QStringLiteral("bank_accounts"), QString::number(accountCount)},
        {QStringLiteral("transactions"), QString::number(transactionCount)},
        {QStringLiteral("payrolls"), QString::number(payrollCount)},
        {QStringLiteral("foods"), QString::number(foodCount)},
        {QStringLiteral("food_purchases"), QString::number(purchaseCount)}
    };
    for (const QPair<QString, QString> &kv : info) {
        if (!insertRow(query,
                       QStringLiteral("INSERT INTO export_info VALUES (?,?)"),
                       {kv.first, kv.second}, error)) {
            db.rollback();
            return false;
        }
    }

    if (!db.commit()) {
        error = QStringLiteral("提交事务失败：") + db.lastError().text();
        return false;
    }
    return true;
}

} // namespace

// [SQLite 导出] 外层负责连接生命周期：打开 → 建表/写数据 → 提交 → 关闭 → 移除连接。
bool SqliteExporter::exportAll(const QString &filePath,
                               zoo &animalManager,
                               Basic &attendance,
                               BankService &bank,
                               PayrollService &payroll,
                               FoodPurchaseService &foodService,
                               QString &error)
{
    error.clear();
    if (filePath.trimmed().isEmpty()) {
        error = QStringLiteral("导出路径为空");
        return false;
    }
    if (!QSqlDatabase::isDriverAvailable(QStringLiteral("QSQLITE"))) {
        error = QStringLiteral("当前 Qt 没有 QSQLITE 驱动，请安装 Qt SQL 的 SQLite 插件");
        return false;
    }

    // 覆盖导出：先删除旧文件；如果文件被占用，会给出明确错误。
    if (QFile::exists(filePath) && !QFile::remove(filePath)) {
        error = QStringLiteral("无法覆盖旧文件，可能被占用：") + filePath;
        return false;
    }

    const QString connectionName = QStringLiteral("sqlite_export_") +
            QUuid::createUuid().toString(QUuid::WithoutBraces);
    bool ok = false;
    {
        QSqlDatabase db = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), connectionName);
        db.setDatabaseName(filePath);
        if (!db.open()) {
            error = QStringLiteral("打开 SQLite 失败：") + db.lastError().text();
        } else {
            ok = fillDatabase(db, animalManager, attendance, bank, payroll, foodService, error);
            db.close();
        }
    }
    QSqlDatabase::removeDatabase(connectionName);
    return ok;
}
