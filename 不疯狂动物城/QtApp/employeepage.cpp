//   员工页面：工作台（资料/考勤）、钱包（账户/流水/工资）、食物购买。
//   员工页面只读取“当前登录员工”的数据，不能修改动物园和其他员工信息。
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QComboBox>
#include <QDateEdit>
#include <QDialog>
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

// 员工页面外壳
void MainWindow::setupEmployeePage()
{
    QVBoxLayout *root = new QVBoxLayout(ui->employeePage);
    root->setContentsMargins(16, 14, 16, 14);
    root->setSpacing(12);
    QHBoxLayout *top = new QHBoxLayout();
    e.welcomeLabel = new QLabel(QStringLiteral("员工工作台"));
    e.welcomeLabel->setObjectName(QStringLiteral("pageTitle"));
    e.logoutButton = newButton(QStringLiteral("退出登录"));
    top->addWidget(e.welcomeLabel);
    top->addStretch();
    top->addWidget(newDataButton(false));
    top->addWidget(e.logoutButton);
    root->addLayout(top);
    e.tabs = new QTabWidget();
    e.tabs->setObjectName(QStringLiteral("employeeTabs"));
    e.tabs->addTab(createEmployeeAttendanceTab(), QStringLiteral("工作台"));
    e.tabs->addTab(createEmployeeAccountTab(), QStringLiteral("我的钱包"));
    e.tabs->addTab(createEmployeeFoodTab(), QStringLiteral("购买食物"));
    root->addWidget(e.tabs, 1);
    connect(e.logoutButton, &QPushButton::clicked, this, &MainWindow::logout);
}

// 我的信息

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
        account = QStringLiteral("未绑定" );
    }
    e.infoLabel->setText(QStringLiteral("%1  ·  %2  ·  %3 / %4\n基本工资 %5 元    工资 / 消费账户 %6")
        .arg(QString::fromStdString(a->getname()), QString::fromStdString(a->getid()),
             QString::fromStdString(a->getcategory()), QString::fromStdString(a->getspecies()),
             QString::fromStdString(moneyToString(a->basicSalary())), account));
}

// 我的考勤
QWidget *MainWindow::createEmployeeAttendanceTab()
{
    QWidget *page = new QWidget();
    QVBoxLayout *root = new QVBoxLayout(page);
    root->setContentsMargins(12, 12, 12, 12);
    root->setSpacing(10);
    e.infoLabel = new QLabel();
    e.infoLabel->setObjectName(QStringLiteral("employeeCard"));
    e.infoLabel->setWordWrap(true);
    root->addWidget(e.infoLabel);
    QDialog *leaveDialog = newDialog(QStringLiteral("申请请假"));
    QFormLayout *leaveForm = new QFormLayout(leaveDialog);
    e.leaveDateEdit = new QDateEdit(QDate::currentDate());
    e.leaveDateEdit->setCalendarPopup(true);
    e.leaveDateEdit->setDisplayFormat("yyyy-MM-dd");
    e.leaveDateEdit->setMinimumDate(QDate::currentDate());
    leaveForm->addRow(QStringLiteral("请假日期"), e.leaveDateEdit);
    leaveForm->addRow(newHint(QStringLiteral("仅登记今天或以后的应出勤日，已签到或请假的日期不能重复提交。")));
    connect(dialogAction(leaveDialog, leaveForm, QStringLiteral("提交请假")), &QPushButton::clicked, this, &MainWindow::employeeRequestLeave);
    QHBoxLayout *bar = new QHBoxLayout();
    QPushButton *signIn = newButton(QStringLiteral("今日签到"));
    signIn->setObjectName(QStringLiteral("primaryButton"));
    QPushButton *leave = newButton(QStringLiteral("申请请假"));
    bar->addWidget(signIn);
    bar->addWidget(leave);
    bar->addStretch();
    e.attYearSpin = new QSpinBox();
    e.attYearSpin->setRange(2000, 2100);
    e.attYearSpin->setValue(QDate::currentDate().year());
    e.attMonthSpin = new QSpinBox();
    e.attMonthSpin->setRange(1, 12);
    e.attMonthSpin->setValue(QDate::currentDate().month());
    QPushButton *query = newButton(QStringLiteral("查询考勤"));
    bar->addWidget(e.attYearSpin);
    bar->addWidget(new QLabel(QStringLiteral("年")));
    bar->addWidget(e.attMonthSpin);
    bar->addWidget(new QLabel(QStringLiteral("月")));
    bar->addWidget(query);
    root->addLayout(bar);
    e.attSummaryLabel = newHint(QStringLiteral("选择月份查看本人考勤。"));
    root->addWidget(e.attSummaryLabel);
    e.attTable = newTable({QStringLiteral("日期"), QStringLiteral("状态")});
    root->addWidget(e.attTable, 1);
    root->addWidget(newHint(QStringLiteral("周一至周五出勤；未来日期的缺勤是初始状态，月底后才结算工资。")));
    connect(signIn, &QPushButton::clicked, this, &MainWindow::employeeSignIn);
    connect(leave, &QPushButton::clicked, this, [this, leaveDialog]() {
        e.leaveDateEdit->setMinimumDate(QDate::currentDate());
        leaveDialog->open();
    });
    connect(query, &QPushButton::clicked, this, &MainWindow::employeeSummarizeAttendance);
    connect(e.attYearSpin, &QSpinBox::textChanged, this, &MainWindow::refreshEmployeeAttendance);
    connect(e.attMonthSpin, &QSpinBox::textChanged, this, &MainWindow::refreshEmployeeAttendance);
    return page;
}

void MainWindow::refreshEmployeeAttendance()
{
    if (!e.attTable) {
        return;
    }
    e.attSummaryLabel->setText(QStringLiteral("点击“查询考勤”查看所选月份的出勤统计。"));
    e.attTable->setRowCount(0);
    if (currentEmployeeId.empty()) {
        return;
    }
    for (int i = 0; i < attendance.dayCount(); ++i) {
        const dayRecord *d = attendance.dayAt(i);
        if (!d) {
            continue;
        }
        if (d->date.year() != e.attYearSpin->value() ||
            d->date.month() != e.attMonthSpin->value()) {
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

// 员工签到：将今天的默认缺勤改为出勤。
void MainWindow::employeeSignIn()
{
    if (currentEmployeeId.empty()) {
        showError(QStringLiteral("当前没有登录员工。"));
        return;
    }
    const Date today = Date::today();
    if (!initializeAttendance(today.year(), today.month())) {
        return;
    }
    Record records[MAX_RECORDS];
    const int count = attendance.getRecord(today, records);
    for (int i = 0; i < count; ++i) {
        if (records[i].ID != currentEmployeeId) {
            continue;
        }
        if (records[i].status == 1) {
            showInfo(QStringLiteral("今天已经签到过了。"));
            return;
        } else if (records[i].status == 2) {
            showInfo(QStringLiteral("今天已经请过假，不能重复签到。"));
            return;
        }
        if (!attendance.changeRecord(today, currentEmployeeId, 1)) {
            showError(QStringLiteral("签到失败：未找到对应的考勤记录。"));
            return;
        }
        if (!attendanceFile.Save(attendance, kAttendanceFile)) {
            showError(QStringLiteral("签到已记录，但保存 attendance.txt 失败，请检查数据目录。"));
        } else {
            showInfo(QStringLiteral("签到成功：") + QString::fromStdString(today.toString()));
        }
        refreshEmployeeAttendance();
        return;
    }
    showInfo(QStringLiteral("今天是休息日，无需签到。"));
}

// 员工请假：将所选应出勤日期改为请假。
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
    if (!initializeAttendance(date.year(), date.month())) {
        return;
    }
    Record records[MAX_RECORDS];
    const int count = attendance.getRecord(date, records);
    bool found = false;
    for (int i = 0; i < count; ++i) {
        if (records[i].ID == currentEmployeeId) {
            if (records[i].status != 0) {
                showInfo(QStringLiteral("该日期已经出勤或请假，不能重复请假。"));
                return;
            }
            found = true;
            break;
        }
    }
    if (!found) {
        showInfo(QStringLiteral("该日期是休息日，无需请假。"));
        return;
    }
    if (!attendance.changeRecord(date, currentEmployeeId, 2)) {
        showError(QStringLiteral("请假失败：未找到对应的考勤记录。"));
        return;
    }
    if (!attendanceFile.Save(attendance, kAttendanceFile)) {
        showError(QStringLiteral("请假已记录，但保存 attendance.txt 失败，请检查数据目录。"));
    } else {
        showInfo(QStringLiteral("请假申请已记录：") + QString::fromStdString(date.toString()));
    }
    refreshEmployeeAttendance();
    closeOperationDialog();
}

void MainWindow::employeeSummarizeAttendance()
{
    if (currentEmployeeId.empty()) {
        return;
    }
    const int year = e.attYearSpin->value();
    const int month = e.attMonthSpin->value();
    if (!initializeAttendance(year, month)) {
        return;
    }
    const std::vector<Date> dates = buildScheduledDates(year, month);
    refreshEmployeeAttendance();
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

// 我的账户
QWidget *MainWindow::createEmployeeAccountTab()
{
    QWidget *page = new QWidget();
    QVBoxLayout *root = new QVBoxLayout(page);
    root->setContentsMargins(12, 12, 12, 12);
    root->setSpacing(10);
    QDialog *moneyDialog = newDialog(QStringLiteral("我的账户操作"));
    QFormLayout *form = new QFormLayout(moneyDialog);
    e.accountCombo = new QComboBox();
    QComboBox *operation = new QComboBox();
    operation->addItem(QStringLiteral("存款"), 1);
    operation->addItem(QStringLiteral("取款"), 2);
    e.accountAmountEdit = newLine(QStringLiteral("金额，元"));
    form->addRow(QStringLiteral("本人账户"), e.accountCombo);
    form->addRow(QStringLiteral("业务类型"), operation);
    form->addRow(QStringLiteral("金额"), e.accountAmountEdit);
    connect(dialogAction(moneyDialog, form, QStringLiteral("确认办理")), &QPushButton::clicked, this, [this, operation]() {
        if (operation->currentData().toInt() == 1) { employeeDeposit(); }
        else { employeeWithdraw(); }
    });
    QHBoxLayout *bar = new QHBoxLayout();
    e.balanceLabel = new QLabel(QStringLiteral("尚未绑定账户"));
    e.balanceLabel->setObjectName(QStringLiteral("accountBalance"));
    QPushButton *money = newButton(QStringLiteral("存取款"));
    QPushButton *refresh = newButton(QStringLiteral("刷新"));
    bar->addWidget(e.balanceLabel, 1);
    bar->addWidget(money);
    bar->addWidget(refresh);
    root->addLayout(bar);
    QTabWidget *views = new QTabWidget();
    e.txTable = newTable({QStringLiteral("流水号"), QStringLiteral("账号"), QStringLiteral("类型"),
                         QStringLiteral("金额"), QStringLiteral("余额"), QStringLiteral("日期"), QStringLiteral("备注")});
    views->addTab(e.txTable, QStringLiteral("交易流水"));
    views->addTab(createEmployeePayrollTab(), QStringLiteral("工资记录"));
    e.accountTable = newTable({QStringLiteral("账号"), QStringLiteral("类型"), QStringLiteral("余额"),
                              QStringLiteral("最低余额/额度"), QStringLiteral("开户日期"), QStringLiteral("上次结息")});
    views->addTab(e.accountTable, QStringLiteral("我的账户"));
    root->addWidget(views, 1);
    root->addWidget(newHint(QStringLiteral("工资和食物付款均可在银行流水中查询，只显示本人账户。")));
    connect(money, &QPushButton::clicked, moneyDialog, &QDialog::open);
    connect(refresh, &QPushButton::clicked, this, &MainWindow::refreshEmployeeUi);
    return page;
}

void MainWindow::refreshEmployeeAccounts()
{
    if (!e.accountTable) {
        return;
    }
    const QString oldAccount = e.accountCombo ? e.accountCombo->currentData().toString() : QString();
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
    animal *employee = animalManager.findbyid(currentEmployeeId);
    BankAccount *bound = employee ? bank.findAccount(employee->bankAccountNo()) : nullptr;
    if (e.balanceLabel) {
        e.balanceLabel->setText(bound && bound->ownerId() == currentEmployeeId
            ? QStringLiteral("绑定账户余额  %1 元").arg(QString::fromStdString(moneyToString(bound->balance())))
            : QStringLiteral("尚未绑定银行账户"));
    }
    if (e.accountCombo) {
        int index = e.accountCombo->findData(oldAccount);
        if (index < 0 && employee) {
            index = e.accountCombo->findData(QString::fromStdString(employee->bankAccountNo()));
        }
        if (index >= 0) { e.accountCombo->setCurrentIndex(index); }
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
    if (!readMoney(e.accountAmountEdit, amount, error)) {
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
    closeOperationDialog();
}

void MainWindow::employeeWithdraw()
{
    if (!e.accountCombo || e.accountCombo->count() == 0) {
        showError(QStringLiteral("你还没有银行账户，请联系管理者开户。"));
        return;
    }
    Money amount = 0;
    QString error;
    if (!readMoney(e.accountAmountEdit, amount, error)) {
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
    closeOperationDialog();
}

// 我的工资
QWidget *MainWindow::createEmployeePayrollTab()
{
    QWidget *page = new QWidget();
    QVBoxLayout *root = new QVBoxLayout(page);
    root->setContentsMargins(0, 0, 0, 0);
    e.payTable = newTable({QStringLiteral("工资单号"), QStringLiteral("年月"), QStringLiteral("基本工资"),
                          QStringLiteral("缺勤扣"), QStringLiteral("请假扣"), QStringLiteral("奖金"),
                          QStringLiteral("实发"), QStringLiteral("状态"), QStringLiteral("银行流水")});
    root->addWidget(e.payTable, 1);
    root->addWidget(newHint(QStringLiteral("管理者按月计算和发放工资，员工在此查看。")));
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

// 购买食物
QWidget *MainWindow::createEmployeeFoodTab()
{
    QWidget *page = new QWidget();
    QVBoxLayout *root = new QVBoxLayout(page);
    root->setContentsMargins(12, 12, 12, 12);
    root->setSpacing(12);
    QHBoxLayout *bar = new QHBoxLayout();
    e.foodCombo = new QComboBox();
    e.foodQtySpin = new QSpinBox();
    e.foodQtySpin->setRange(1, 10000);
    e.foodQtySpin->setValue(1);
    QPushButton *buy = newButton(QStringLiteral("购买"));
    buy->setObjectName(QStringLiteral("primaryButton"));
    bar->addWidget(new QLabel(QStringLiteral("食物")));
    bar->addWidget(e.foodCombo, 1);
    bar->addWidget(new QLabel(QStringLiteral("数量 kg")));
    bar->addWidget(e.foodQtySpin);
    bar->addWidget(buy);
    root->addLayout(bar);
    root->addWidget(newHint(QStringLiteral("使用本人绑定账户付款，单价显示在食物名称后；付款记录同步进入银行流水。")));
    root->addWidget(new QLabel(QStringLiteral("我的购买记录")));
    e.purchaseTable = newTable({QStringLiteral("购买单"), QStringLiteral("食物"), QStringLiteral("数量 kg"),
                               QStringLiteral("金额"), QStringLiteral("账号"), QStringLiteral("日期"),
                               QStringLiteral("状态"), QStringLiteral("银行流水")});
    root->addWidget(e.purchaseTable, 1);
    connect(buy, &QPushButton::clicked, this, &MainWindow::employeePurchaseFood);
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
    foodService.savePurchases(kPurchaseFile);
    refreshEmployeeAccounts();
    refreshEmployeeTransactions();
    refreshEmployeePurchases();
    showInfo(QString::fromStdString(message) + QStringLiteral("\n银行流水号：") + QString::number(transactionId));
}
