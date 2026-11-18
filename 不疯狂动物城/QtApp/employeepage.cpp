// ============================================================================
// employeepage.cpp
//   员工页面：我的信息、我的考勤、我的账户、我的工资、购买食物。
//   员工页面只读取“当前登录员工”的数据，不能修改动物园和其他员工信息。
// ============================================================================
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QCheckBox>
#include <QComboBox>
#include <QDateEdit>
#include <QFormLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QSpinBox>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QVBoxLayout>
#include <QWidget>

// ============================================================================
// [界面] 员工页面外壳
// ============================================================================
void MainWindow::setupEmployeePage()
{
    QVBoxLayout *root = new QVBoxLayout(ui->employeePage);
    root->setContentsMargins(10, 10, 10, 10);
    root->setSpacing(8);

    QHBoxLayout *top = new QHBoxLayout();
    e.welcomeLabel = new QLabel(QStringLiteral("员工页面"));
    e.welcomeLabel->setStyleSheet(QStringLiteral("font-size:16px;font-weight:bold;color:#1F3864;"));
    e.saveAllButton = newButton(QStringLiteral("保存我的数据"));
    e.logoutButton = newButton(QStringLiteral("退出登录"));
    top->addWidget(e.welcomeLabel);
    top->addStretch();
    top->addWidget(e.saveAllButton);
    top->addWidget(e.logoutButton);
    root->addLayout(top);

    e.tabs = new QTabWidget();
    e.tabs->addTab(createEmployeeInfoTab(), QStringLiteral("我的信息"));
    e.tabs->addTab(createEmployeeAttendanceTab(), QStringLiteral("我的考勤"));
    e.tabs->addTab(createEmployeeAccountTab(), QStringLiteral("我的账户"));
    e.tabs->addTab(createEmployeePayrollTab(), QStringLiteral("我的工资"));
    e.tabs->addTab(createEmployeeFoodTab(), QStringLiteral("购买食物"));
    root->addWidget(e.tabs, 1);

    connect(e.saveAllButton, &QPushButton::clicked, this, [this]() { saveAllData(true); });
    connect(e.logoutButton, &QPushButton::clicked, this, &MainWindow::logout);
}

// ============================================================================
// [界面] 我的信息
// ============================================================================
QWidget *MainWindow::createEmployeeInfoTab()
{
    QWidget *page = new QWidget();
    QVBoxLayout *root = new QVBoxLayout(page);
    root->setContentsMargins(8, 8, 8, 8);

    QGroupBox *group = newGroup(QStringLiteral("员工资料"));
    QVBoxLayout *layout = new QVBoxLayout(group);
    e.infoLabel = new QLabel(QStringLiteral("尚未登录"));
    e.infoLabel->setWordWrap(true);
    e.infoLabel->setTextFormat(Qt::RichText);
    e.infoLabel->setStyleSheet(QStringLiteral("font-size:11pt;line-height:1.6;"));
    QPushButton *refreshButton = newButton(QStringLiteral("刷新"));
    layout->addWidget(e.infoLabel);
    layout->addWidget(refreshButton);
    layout->addStretch();
    connect(refreshButton, &QPushButton::clicked, this, &MainWindow::refreshEmployeeInfo);

    root->addWidget(group);
    return page;
}

void MainWindow::refreshEmployeeInfo()
{
    if (!e.infoLabel) {
        return;
    }
    animal *a = animalManager.findbyid(currentEmployeeId);
    if (!a) {
        e.infoLabel->setText(QStringLiteral("未找到当前员工。"));
        return;
    }
    QString account = QString::fromStdString(a->bankAccountNo());
    if (account.isEmpty()) {
        account = QStringLiteral("（未绑定）");
    }
    e.infoLabel->setText(
        QStringLiteral("<b>员工编号：</b>%1<br>"
                       "<b>姓名：</b>%2<br>"
                       "<b>类别：</b>%3<br>"
                       "<b>物种：</b>%4<br>"
                       "<b>基本工资：</b>%5 元<br>"
                       "<b>工资/消费账号：</b>%6<br>"
                       "<b>当前角色：</b>员工")
            .arg(QString::fromStdString(a->getid()))
            .arg(QString::fromStdString(a->getname()))
            .arg(QString::fromStdString(a->getcategory()))
            .arg(QString::fromStdString(a->getspecies()))
            .arg(QString::fromStdString(moneyToString(a->basicSalary())))
            .arg(account));
}

// ============================================================================
// [界面] 我的考勤
// ============================================================================
QWidget *MainWindow::createEmployeeAttendanceTab()
{
    QWidget *page = new QWidget();
    QVBoxLayout *root = new QVBoxLayout(page);
    root->setContentsMargins(8, 8, 8, 8);

    // [功能] 员工签到/请假：签到固定今天；请假可选今天或以后。
    QGroupBox *actionGroup = newGroup(QStringLiteral("签到 / 请假"));
    QGridLayout *actionLayout = new QGridLayout(actionGroup);
    QPushButton *signInButton = newButton(QStringLiteral("今日签到"));
    e.leaveDateEdit = new QDateEdit(QDate::currentDate());
    e.leaveDateEdit->setCalendarPopup(true);
    e.leaveDateEdit->setDisplayFormat(QStringLiteral("yyyy-MM-dd"));
    e.leaveDateEdit->setMinimumDate(QDate::currentDate());
    QPushButton *leaveButton = newButton(QStringLiteral("申请请假"));
    actionLayout->addWidget(new QLabel(QStringLiteral("签到日期：今天")), 0, 0);
    actionLayout->addWidget(signInButton, 0, 1);
    actionLayout->addWidget(new QLabel(QStringLiteral("请假日期")), 0, 2);
    actionLayout->addWidget(e.leaveDateEdit, 0, 3);
    actionLayout->addWidget(leaveButton, 0, 4);
    actionLayout->addWidget(newHint(QStringLiteral("签到只记录今天；请假可选今天或以后。同一天已有记录时不能重复操作；"
                                                   "如需修改已提交的考勤，请联系管理者。")), 1, 0, 1, 5);
    connect(signInButton, &QPushButton::clicked, this, &MainWindow::employeeSignIn);
    connect(leaveButton, &QPushButton::clicked, this, &MainWindow::employeeRequestLeave);
    root->addWidget(actionGroup);

    QGroupBox *sumGroup = newGroup(QStringLiteral("我的月度考勤"));
    QHBoxLayout *sumLayout = new QHBoxLayout(sumGroup);
    e.attYearSpin = new QSpinBox();
    e.attYearSpin->setRange(2000, 2100);
    e.attYearSpin->setValue(QDate::currentDate().year());
    e.attMonthSpin = new QSpinBox();
    e.attMonthSpin->setRange(1, 12);
    e.attMonthSpin->setValue(QDate::currentDate().month());
    e.attWorkdayCheck = new QCheckBox(QStringLiteral("只算周一至周五"));
    e.attWorkdayCheck->setChecked(true);
    QPushButton *sumButton = newButton(QStringLiteral("统计"));
    e.attSummaryLabel = new QLabel(QStringLiteral("尚未统计"));
    e.attSummaryLabel->setStyleSheet(QStringLiteral("color:#2E5C8A;"));
    sumLayout->addWidget(e.attYearSpin);
    sumLayout->addWidget(new QLabel(QStringLiteral("年")));
    sumLayout->addWidget(e.attMonthSpin);
    sumLayout->addWidget(new QLabel(QStringLiteral("月")));
    sumLayout->addWidget(e.attWorkdayCheck);
    sumLayout->addWidget(sumButton);
    sumLayout->addWidget(e.attSummaryLabel, 1);
    connect(sumButton, &QPushButton::clicked, this, &MainWindow::employeeSummarizeAttendance);

    root->addWidget(sumGroup);
    e.attTable = newTable(QStringList() << QStringLiteral("日期") << QStringLiteral("状态"));
    root->addWidget(e.attTable, 1);
    QPushButton *refreshButton = newButton(QStringLiteral("刷新我的考勤"));
    root->addWidget(refreshButton);
    connect(refreshButton, &QPushButton::clicked, this, &MainWindow::refreshEmployeeAttendance);
    return page;
}

void MainWindow::refreshEmployeeAttendance()
{
    if (!e.attTable) {
        return;
    }
    e.attTable->setRowCount(0);
    if (currentEmployeeId.empty()) {
        return;
    }
    for (int i = 0; i < attendance.dayCount(); ++i) {
        const dayRecord *d = attendance.dayAt(i);
        if (!d) {
            continue;
        }
        for (int j = 0; j < d->Recordcnt; ++j) {
            if (d->dayrecords[j].ID != currentEmployeeId) {
                continue;
            }
            const int row = e.attTable->rowCount();
            e.attTable->insertRow(row);
            e.attTable->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(d->date.toString())));
            e.attTable->setItem(row, 1, new QTableWidgetItem(statusText(d->dayrecords[j].status)));
        }
    }
}

// ============================================================================
// [功能] 员工签到：只写今天，状态=出勤(1)
// ============================================================================
void MainWindow::employeeSignIn()
{
    if (currentEmployeeId.empty()) {
        showError(QStringLiteral("当前没有登录员工。"));
        return;
    }
    const Date today = Date::today();
    Record records[MAX_RECORDS];
    const int count = attendance.getRecord(today, records);
    for (int i = 0; i < count; ++i) {
        if (records[i].ID != currentEmployeeId) {
            continue;
        }
        if (records[i].status == 1) {
            showInfo(QStringLiteral("今天已经签到过了。"));
        } else if (records[i].status == 2) {
            showInfo(QStringLiteral("今天已经请过假，不能重复签到。"));
        } else {
            showError(QStringLiteral("今天已被标记为缺勤，请联系管理者修改。"));
        }
        return;
    }

    attendance.addRecord(today, currentEmployeeId, 1);
    if (!attendanceFile.Save(attendance, kAttendanceFile)) {
        showError(QStringLiteral("签到已记录，但保存 attendance.txt 失败，请检查数据目录。"));
    } else {
        showInfo(QStringLiteral("签到成功：") + QString::fromStdString(today.toString()));
    }
    refreshEmployeeAttendance();
}

// ============================================================================
// [功能] 员工请假：写所选日期，状态=请假(2)
// ============================================================================
void MainWindow::employeeRequestLeave()
{
    if (currentEmployeeId.empty()) {
        showError(QStringLiteral("当前没有登录员工。"));
        return;
    }
    Date date;
    if (!readDate(e.leaveDateEdit, date)) {
        showError(QStringLiteral("请假日期不合法。"));
        return;
    }
    if (date.isBefore(Date::today())) {
        showError(QStringLiteral("请假日期不能早于今天；历史考勤请联系管理者修改。"));
        return;
    }
    Record records[MAX_RECORDS];
    const int count = attendance.getRecord(date, records);
    for (int i = 0; i < count; ++i) {
        if (records[i].ID == currentEmployeeId) {
            showError(QStringLiteral("该日期已有考勤记录，不能重复请假；如需修改请联系管理者。"));
            return;
        }
    }

    attendance.addRecord(date, currentEmployeeId, 2);
    if (!attendanceFile.Save(attendance, kAttendanceFile)) {
        showError(QStringLiteral("请假已记录，但保存 attendance.txt 失败，请检查数据目录。"));
    } else {
        showInfo(QStringLiteral("请假申请已记录：") + QString::fromStdString(date.toString()));
    }
    refreshEmployeeAttendance();
}

void MainWindow::employeeSummarizeAttendance()
{
    if (currentEmployeeId.empty()) {
        return;
    }
    const int year = e.attYearSpin->value();
    const int month = e.attMonthSpin->value();
    const std::vector<Date> dates = buildScheduledDates(year, month, e.attWorkdayCheck->isChecked());
    AttendanceSummary summary;
    std::string message;
    if (!attendance.summarize(currentEmployeeId, year, month, dates, summary, message)) {
        e.attSummaryLabel->setText(QStringLiteral("统计失败：") + QString::fromStdString(message));
        return;
    }
    e.attSummaryLabel->setText(
        QStringLiteral("应出勤 %1；出勤 %2；缺勤 %3；请假 %4；未录入 %5")
            .arg(summary.scheduledDays).arg(summary.presentDays)
            .arg(summary.absentDays).arg(summary.leaveDays).arg(summary.missingDays));
}

// ============================================================================
// [界面] 我的账户
// ============================================================================
QWidget *MainWindow::createEmployeeAccountTab()
{
    QWidget *page = new QWidget();
    QVBoxLayout *root = new QVBoxLayout(page);
    root->setContentsMargins(8, 8, 8, 8);

    QGroupBox *opGroup = newGroup(QStringLiteral("存取款"));
    QHBoxLayout *opLayout = new QHBoxLayout(opGroup);
    e.accountCombo = new QComboBox();
    e.depositAmountEdit = newLine(QStringLiteral("存款金额，元"));
    QPushButton *depositButton = newButton(QStringLiteral("存款"));
    e.withdrawAmountEdit = newLine(QStringLiteral("取款金额，元"));
    QPushButton *withdrawButton = newButton(QStringLiteral("取款"));
    opLayout->addWidget(new QLabel(QStringLiteral("我的账户")));
    opLayout->addWidget(e.accountCombo, 1);
    opLayout->addWidget(e.depositAmountEdit);
    opLayout->addWidget(depositButton);
    opLayout->addWidget(e.withdrawAmountEdit);
    opLayout->addWidget(withdrawButton);
    connect(depositButton, &QPushButton::clicked, this, &MainWindow::employeeDeposit);
    connect(withdrawButton, &QPushButton::clicked, this, &MainWindow::employeeWithdraw);

    e.accountTable = newTable(QStringList() << QStringLiteral("账号") << QStringLiteral("类型")
                                            << QStringLiteral("余额") << QStringLiteral("最低余额/额度")
                                            << QStringLiteral("开户日期") << QStringLiteral("上次结息"));
    e.txTable = newTable(QStringList() << QStringLiteral("流水号") << QStringLiteral("账号")
                                       << QStringLiteral("类型") << QStringLiteral("金额")
                                       << QStringLiteral("余额") << QStringLiteral("日期")
                                       << QStringLiteral("备注"));
    QPushButton *refreshButton = newButton(QStringLiteral("刷新账户和流水"));
    connect(refreshButton, &QPushButton::clicked, this, [this]() {
        refreshEmployeeAccounts();
        refreshEmployeeTransactions();
    });

    root->addWidget(opGroup);
    root->addWidget(new QLabel(QStringLiteral("我的账户")));
    root->addWidget(e.accountTable, 1);
    root->addWidget(new QLabel(QStringLiteral("我的交易流水")));
    root->addWidget(e.txTable, 1);
    root->addWidget(refreshButton);
    e.accountStatusLabel = newHint(QStringLiteral("提示：员工只能操作自己名下的账户。"));
    root->addWidget(e.accountStatusLabel);
    return page;
}

void MainWindow::refreshEmployeeAccounts()
{
    if (!e.accountTable) {
        return;
    }
    e.accountTable->setRowCount(0);
    if (e.accountCombo) {
        e.accountCombo->clear();
    }
    const std::vector<BankAccount *> &accounts = bank.getAccounts();
    for (size_t i = 0; i < accounts.size(); ++i) {
        BankAccount *a = accounts[i];
        if (!a || a->ownerId() != currentEmployeeId) {
            continue;
        }
        const int row = e.accountTable->rowCount();
        e.accountTable->insertRow(row);
        e.accountTable->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(a->accountNo())));
        e.accountTable->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(a->typeName())));
        e.accountTable->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(moneyToString(a->balance()))));
        const QString extra = a->typeName() == "储蓄账户"
                ? QStringLiteral("最低 ") + QString::fromStdString(moneyToString(a->minBalance()))
                : QStringLiteral("额度 ") + QString::fromStdString(moneyToString(a->creditLimit()));
        e.accountTable->setItem(row, 3, new QTableWidgetItem(extra));
        e.accountTable->setItem(row, 4, new QTableWidgetItem(QString::fromStdString(a->openDate().toString())));
        e.accountTable->setItem(row, 5, new QTableWidgetItem(QString::fromStdString(a->lastInterestDate().toString())));
        if (e.accountCombo) {
            e.accountCombo->addItem(QString::fromStdString(a->accountNo()), QString::fromStdString(a->accountNo()));
        }
    }
}

void MainWindow::refreshEmployeeTransactions()
{
    if (!e.txTable) {
        return;
    }
    e.txTable->setRowCount(0);
    const std::vector<Transaction> &txs = bank.getTransactions();
    for (size_t i = 0; i < txs.size(); ++i) {
        const Transaction &t = txs[i];
        BankAccount *a = bank.findAccount(t.accountNo());
        if (!a || a->ownerId() != currentEmployeeId) {
            continue;
        }
        const int row = e.txTable->rowCount();
        e.txTable->insertRow(row);
        e.txTable->setItem(row, 0, new QTableWidgetItem(QString::number(t.id())));
        e.txTable->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(t.accountNo())));
        e.txTable->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(t.typeName())));
        e.txTable->setItem(row, 3, new QTableWidgetItem(QString::fromStdString(moneyToString(t.amount()))));
        e.txTable->setItem(row, 4, new QTableWidgetItem(QString::fromStdString(moneyToString(t.balanceAfter()))));
        e.txTable->setItem(row, 5, new QTableWidgetItem(QString::fromStdString(t.date().toString())));
        e.txTable->setItem(row, 6, new QTableWidgetItem(QString::fromStdString(t.note())));
    }
}

void MainWindow::employeeDeposit()
{
    if (!e.accountCombo || e.accountCombo->count() == 0) {
        showError(QStringLiteral("你还没有银行账户，请联系管理者开户。"));
        return;
    }
    Money amount = 0;
    QString error;
    if (!readMoney(e.depositAmountEdit, amount, error)) {
        showError(QStringLiteral("存款金额：") + error);
        return;
    }
    std::string message;
    if (!bank.deposit(e.accountCombo->currentData().toString().toStdString(), amount, message)) {
        showError(QString::fromStdString(message));
        return;
    }
    refreshEmployeeAccounts();
    refreshEmployeeTransactions();
    showInfo(QString::fromStdString(message));
}

void MainWindow::employeeWithdraw()
{
    if (!e.accountCombo || e.accountCombo->count() == 0) {
        showError(QStringLiteral("你还没有银行账户，请联系管理者开户。"));
        return;
    }
    Money amount = 0;
    QString error;
    if (!readMoney(e.withdrawAmountEdit, amount, error)) {
        showError(QStringLiteral("取款金额：") + error);
        return;
    }
    std::string message;
    if (!bank.withdraw(e.accountCombo->currentData().toString().toStdString(), amount, message)) {
        showError(QString::fromStdString(message));
        return;
    }
    refreshEmployeeAccounts();
    refreshEmployeeTransactions();
    showInfo(QString::fromStdString(message));
}

// ============================================================================
// [界面] 我的工资
// ============================================================================
QWidget *MainWindow::createEmployeePayrollTab()
{
    QWidget *page = new QWidget();
    QVBoxLayout *root = new QVBoxLayout(page);
    root->setContentsMargins(8, 8, 8, 8);
    e.payTable = newTable(QStringList() << QStringLiteral("工资单号") << QStringLiteral("年月")
                                        << QStringLiteral("基本工资") << QStringLiteral("缺勤扣")
                                        << QStringLiteral("请假扣") << QStringLiteral("奖金")
                                        << QStringLiteral("实发") << QStringLiteral("状态")
                                        << QStringLiteral("银行流水号"));
    QPushButton *refreshButton = newButton(QStringLiteral("刷新我的工资单"));
    e.payStatusLabel = newHint(QStringLiteral("工资由管理者生成和发放；员工只能查看。"));
    root->addWidget(e.payTable, 1);
    root->addWidget(refreshButton);
    root->addWidget(e.payStatusLabel);
    connect(refreshButton, &QPushButton::clicked, this, &MainWindow::refreshEmployeePayroll);
    return page;
}

void MainWindow::refreshEmployeePayroll()
{
    if (!e.payTable) {
        return;
    }
    e.payTable->setRowCount(0);
    const std::vector<PayrollRecord> &records = payroll.getPayrollRecords();
    for (size_t i = 0; i < records.size(); ++i) {
        const PayrollRecord &r = records[i];
        if (r.employeeId != currentEmployeeId) {
            continue;
        }
        const int row = e.payTable->rowCount();
        e.payTable->insertRow(row);
        e.payTable->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(r.payrollId)));
        e.payTable->setItem(row, 1, new QTableWidgetItem(QStringLiteral("%1-%2").arg(r.year).arg(r.month, 2, 10, QLatin1Char('0'))));
        e.payTable->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(moneyToString(r.basicSalary))));
        e.payTable->setItem(row, 3, new QTableWidgetItem(QString::fromStdString(moneyToString(r.absentDeduction))));
        e.payTable->setItem(row, 4, new QTableWidgetItem(QString::fromStdString(moneyToString(r.leaveDeduction))));
        e.payTable->setItem(row, 5, new QTableWidgetItem(QString::fromStdString(moneyToString(r.bonus))));
        e.payTable->setItem(row, 6, new QTableWidgetItem(QString::fromStdString(moneyToString(r.finalsalary))));
        e.payTable->setItem(row, 7, new QTableWidgetItem(QString::fromStdString(r.status)));
        e.payTable->setItem(row, 8, new QTableWidgetItem(QString::number(r.bankTransactionId)));
    }
}

// ============================================================================
// [界面] 购买食物
// ============================================================================
QWidget *MainWindow::createEmployeeFoodTab()
{
    QWidget *page = new QWidget();
    QVBoxLayout *root = new QVBoxLayout(page);
    root->setContentsMargins(8, 8, 8, 8);

    QGroupBox *buyGroup = newGroup(QStringLiteral("用我的账户购买食物"));
    QHBoxLayout *buyLayout = new QHBoxLayout(buyGroup);
    e.foodCombo = new QComboBox();
    e.foodQtySpin = new QSpinBox();
    e.foodQtySpin->setRange(1, 10000);
    e.foodQtySpin->setValue(1);
    QPushButton *buyButton = newButton(QStringLiteral("购买"));
    buyLayout->addWidget(new QLabel(QStringLiteral("食物")));
    buyLayout->addWidget(e.foodCombo, 1);
    buyLayout->addWidget(new QLabel(QStringLiteral("数量 kg")));
    buyLayout->addWidget(e.foodQtySpin);
    buyLayout->addWidget(buyButton);
    connect(buyButton, &QPushButton::clicked, this, &MainWindow::employeePurchaseFood);

    e.purchaseTable = newTable(QStringList() << QStringLiteral("购买单") << QStringLiteral("食物")
                                              << QStringLiteral("数量 kg") << QStringLiteral("金额")
                                              << QStringLiteral("账号") << QStringLiteral("日期")
                                              << QStringLiteral("状态") << QStringLiteral("银行流水"));
    root->addWidget(buyGroup);
    root->addWidget(new QLabel(QStringLiteral("我的购买记录")));
    root->addWidget(e.purchaseTable, 1);
    e.foodStatusLabel = newHint(QStringLiteral("提示：购买成功后会从账户扣款，并写一条饲养记录。"));
    root->addWidget(e.foodStatusLabel);
    return page;
}

void MainWindow::refreshEmployeePurchases()
{
    if (!e.purchaseTable) {
        return;
    }
    e.purchaseTable->setRowCount(0);
    const std::vector<FoodPurchaseRecord> &records = foodService.getPurchaseRecords();
    for (size_t i = 0; i < records.size(); ++i) {
        const FoodPurchaseRecord &r = records[i];
        if (r.employeeId != currentEmployeeId) {
            continue;
        }
        const int row = e.purchaseTable->rowCount();
        e.purchaseTable->insertRow(row);
        e.purchaseTable->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(r.purchaseId)));
        e.purchaseTable->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(r.foodName)));
        e.purchaseTable->setItem(row, 2, new QTableWidgetItem(QString::number(r.quantity)));
        e.purchaseTable->setItem(row, 3, new QTableWidgetItem(QString::fromStdString(moneyToString(r.amount))));
        e.purchaseTable->setItem(row, 4, new QTableWidgetItem(QString::fromStdString(r.accountNo)));
        e.purchaseTable->setItem(row, 5, new QTableWidgetItem(QString::fromStdString(r.date)));
        e.purchaseTable->setItem(row, 6, new QTableWidgetItem(QString::fromStdString(r.status)));
        e.purchaseTable->setItem(row, 7, new QTableWidgetItem(QString::number(r.bankTransactionId)));
    }
}

void MainWindow::employeePurchaseFood()
{
    if (currentEmployeeId.empty()) {
        return;
    }
    animal *a = animalManager.findbyid(currentEmployeeId);
    if (!a) {
        showError(QStringLiteral("员工不存在。"));
        return;
    }
    if (a->bankAccountNo().empty()) {
        showError(QStringLiteral("你还没有绑定银行账号，请联系管理者开户。"));
        return;
    }
    if (!e.foodCombo || e.foodCombo->count() == 0) {
        showError(QStringLiteral("还没有可购买的食物。"));
        return;
    }
    const int quantity = e.foodQtySpin->value();
    int transactionId = 0;
    std::string message;
    if (!foodService.purchaseFood(bank, *a, e.foodCombo->currentData().toString().toStdString(),
                                  quantity, transactionId, message)) {
        showError(QString::fromStdString(message));
        return;
    }
    animalManager.feedanimal(currentEmployeeId, Date::today().toString(), quantity);
    try {
        animalManager.save();
    } catch (...) {
    }
    foodService.savePurchases(kPurchaseFile);
    refreshEmployeeAccounts();
    refreshEmployeeTransactions();
    refreshEmployeePurchases();
    showInfo(QString::fromStdString(message) + QStringLiteral("\n银行流水号：") + QString::number(transactionId));
}
