//   窗口骨架、登录/登出、数据加载保存、通用刷新与工具函数。
//   管理者和员工的标签页分别在 managerpage.cpp / employeepage.cpp 中搭建。
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QAbstractItemView>
#include <QAction>
#include <QApplication>
#include <QDialog>
#include <QMenu>
#include <QCloseEvent>
#include <QComboBox>
#include <QDateEdit>
#include <QDir>
#include <QFormLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QList>
#include <QTableWidgetItem>
#include <QMessageBox>
#include <QPushButton>
#include <QSpinBox>
#include <QStatusBar>
#include <QTabWidget>
#include <QTableWidget>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QWidget>

#include <algorithm>
#include <exception>

const char *MainWindow::kAttendanceFile = "attendance.txt";
const char *MainWindow::kPayrollFile = "payroll.txt";
const char *MainWindow::kFoodFile = "foods.txt";
const char *MainWindow::kPurchaseFile = "purchases.txt";

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setMinimumSize(960, 680);
    resize(1120, 780);
    setStyleSheet(QStringLiteral(
        "QMainWindow,QDialog{background:#f6f8fb;color:#26384d;}"
        "QLabel{color:#26384d;}"
        "QLabel#pageTitle{font-size:20px;font-weight:bold;color:#20334d;}"
        "QLabel#employeeCard{padding:12px;background:#edf4fc;border-radius:5px;color:#294c72;}"
        "QLabel#accountBalance{font-size:20px;font-weight:bold;color:#1761ac;padding:8px 0;}"
        "QTabWidget::pane{border:1px solid #dce3ec;background:white;border-radius:5px;}"
        "QTabBar::tab{padding:9px 18px;background:#edf1f6;color:#42536b;border:0;}"
        "QTabBar::tab:selected{background:white;color:#1761ac;font-weight:bold;}"
        "QPushButton{padding:5px 12px;border:1px solid #d1dbe7;border-radius:5px;background:white;color:#26384d;}"
        "QPushButton:hover{background:#edf4fc;border-color:#82addb;}"
        "QPushButton#primaryButton{background:#2469b2;color:white;border-color:#2469b2;}"
        "QLineEdit,QSpinBox,QDateEdit,QComboBox{padding:5px;border:1px solid #d1dbe7;border-radius:4px;background:white;color:#26384d;}"
        "QTableWidget,QTextEdit{background:white;color:#26384d;alternate-background-color:#f7f9fc;border:1px solid #e0e6ef;gridline-color:#edf1f6;selection-background-color:#dfecfb;selection-color:#173c68;}"
        "QHeaderView::section{padding:7px;background:#f0f4f9;border:0;border-bottom:1px solid #dce3ec;color:#42536b;font-weight:bold;}"
        "QGroupBox{border:1px solid #e0e6ef;border-radius:5px;margin-top:10px;padding-top:10px;background:white;}"
        "QGroupBox::title{subcontrol-origin:margin;left:12px;color:#42536b;}"));

    // 先搭好三个页面，再加载数据，最后停在登录页。
    setupLoginPage();
    setupManagerPage();
    setupEmployeePage();
    loadAllData();
    refreshAll();
    showLoginPage();

    statusBar()->showMessage(QStringLiteral("数据已加载，关闭窗口时自动保存。"));
}

MainWindow::~MainWindow()
{
    delete ui;
}

// 登录 / 登出
void MainWindow::showLoginPage()
{
    currentEmployeeId.clear();
    currentRole.clear();
    if (loginAccountEdit) {
        loginAccountEdit->clear();
    }
    if (loginPasswordEdit) {
        loginPasswordEdit->clear();
    }
    if (loginHintLabel) {
        loginHintLabel->setText(QStringLiteral("管理者：admin / 123456；员工：员工编号 / 123456"));
    }
    ui->pageStack->setCurrentWidget(ui->loginPage);
    if (loginAccountEdit) {
        loginAccountEdit->setFocus();
    }
}

void MainWindow::showManagerPage()
{
    const QDate today = QDate::currentDate();
    if (!initializeAttendance(today.year(), today.month())) {
        return;
    }
    currentEmployeeId.clear();
    currentRole = "管理者";
    refreshManagerUi();
    ui->pageStack->setCurrentWidget(ui->managerPage);
    statusBar()->showMessage(QStringLiteral("已进入管理者页面"));
}

void MainWindow::showEmployeePage(const std::string &employeeId)
{
    const QDate today = QDate::currentDate();
    if (!initializeAttendance(today.year(), today.month())) {
        return;
    }
    currentEmployeeId = employeeId;
    currentRole = "员工";
    refreshEmployeeUi();
    ui->pageStack->setCurrentWidget(ui->employeePage);
    statusBar()->showMessage(QStringLiteral("已进入员工页面：") + QString::fromStdString(employeeId));
}

void MainWindow::handleLogin()
{
    if (!loginRoleCombo || !loginAccountEdit || !loginPasswordEdit) {
        return;
    }
    const QString role = loginRoleCombo->currentData().toString();
    const QString account = loginAccountEdit->text().trimmed();
    const QString password = loginPasswordEdit->text();

    // 管理者使用固定演示账号；员工使用动物园里已存在的员工编号。
    if (role == QStringLiteral("manager")) {
        if (account == QStringLiteral("admin") && password == QStringLiteral("123456")) {
            showManagerPage();
        } else {
            showError(QStringLiteral("管理者账号或密码错误。演示账号：admin / 123456"));
        }
        return;
    }

    if (account.isEmpty()) {
        showError(QStringLiteral("请输入员工编号。"));
        return;
    }
    if (password != QStringLiteral("123456")) {
        showError(QStringLiteral("员工默认密码为 123456。"));
        return;
    }
    animal *a = animalManager.findbyid(account.toStdString());
    if (a == nullptr) {
        showError(QStringLiteral("没有找到该员工，请先在管理者页面的“动物园管理”中添加动物员工。"));
        return;
    }
    showEmployeePage(account.toStdString());
}

void MainWindow::logout()
{
    // 退出登录前保存；保存失败不阻止退出。
    saveAllData(false);
    showLoginPage();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    // 关闭窗口自动保存全部模块。
    saveAllData(false);
    event->accept();
}

// 数据加载 / 保存
bool MainWindow::loadAllData()
{
    bool ok = true;

    // 加载动物和历史记录；捕获文件解析异常。
    try {
        animalManager.load();
    } catch (const std::exception &e) {
        ok = false;
        statusBar()->showMessage(QStringLiteral("动物园数据加载失败：") + QString::fromUtf8(e.what()));
    } catch (...) {
        ok = false;
        statusBar()->showMessage(QStringLiteral("动物园数据加载失败：未知异常"));
    }

    // 考勤文件、银行 CSV、工资单、食物目录、购买记录。
    if (!attendanceFile.Load(attendance, kAttendanceFile)) {
        // 文件不存在是正常的首次运行，不弹窗。
    }
    QDir().mkpath(QStringLiteral("data"));
    bank.loadData();
    payroll.loadPayrolls(kPayrollFile);
    foodService.loadFoods(kFoodFile);
    foodService.loadPurchases(kPurchaseFile);

    const QDate today = QDate::currentDate();
    if (!initializeAttendance(today.year(), today.month())) {
        ok = false;
    }
    return ok;
}

bool MainWindow::initializeAttendance(int year, int month)
{
    const std::vector<Date> dates = buildScheduledDates(year, month);
    const std::vector<animal *> &animals = animalManager.getanimals();
    for (animal *a : animals) {
        attendance.initialize(a->getid(), dates);
        AttendanceSummary summary;
        std::string message;
        if (!attendance.summarize(a->getid(), year, month, dates, summary, message)) {
            showError(QString::fromStdString(message));
            return false;
        }
        if (summary.missingDays > 0) {
            showError(QStringLiteral("考勤记录空间不足：当前最多保存366个日期，每天最多100名员工。"));
            return false;
        }
    }
    if (!attendanceFile.Save(attendance, kAttendanceFile)) {
        showError(QStringLiteral("考勤初始化后保存失败，请检查数据目录。"));
        return false;
    }
    return true;
}

bool MainWindow::saveAllData(bool showResult)
{
    bool ok = true;

    // 依次保存各模块。zoo::save() 没有返回值，只能保证调用完成。
    try {
        animalManager.save();
    } catch (...) {
        ok = false;
    }

    if (!attendanceFile.Save(attendance, kAttendanceFile)) {
        ok = false;
    }
    if (!bank.saveData()) {
        ok = false;
    }
    if (!payroll.savePayrolls(kPayrollFile)) {
        ok = false;
    }
    if (!foodService.saveFoods(kFoodFile)) {
        ok = false;
    }
    if (!foodService.savePurchases(kPurchaseFile)) {
        ok = false;
    }

    if (showResult) {
        if (ok) {
            showInfo(QStringLiteral("全部数据已保存到：\n") + QDir::currentPath());
        } else {
            showError(QStringLiteral("部分数据保存失败，请检查数据目录是否可写：\n") + QDir::currentPath());
        }
    }
    return ok;
}

void MainWindow::refreshAll()
{
    if (m.tabs) {
        refreshManagerUi();
    }
    if (!currentEmployeeId.empty()) {
        refreshEmployeeUi();
    }
}

void MainWindow::refreshManagerUi()
{
    if (!m.tabs) {
        return;
    }
    fillEmployeeCombos();
    fillFoodCombos();
    refreshZooTable();
    refreshAttendanceTable();
    refreshBankAccounts();
    refreshBankTransactions();
    refreshPayrollTable();
    refreshFoodTable();
    refreshPurchaseTable();
    if (m.welcomeLabel) {
        m.welcomeLabel->setText(QStringLiteral("管理工作台"));
    }
}

void MainWindow::refreshEmployeeUi()
{
    if (currentEmployeeId.empty()) {
        return;
    }
    fillFoodCombos();
    refreshEmployeeInfo();
    refreshEmployeeAttendance();
    refreshEmployeeAccounts();
    refreshEmployeeTransactions();
    refreshEmployeePayroll();
    refreshEmployeePurchases();
    if (e.welcomeLabel) {
        e.welcomeLabel->setText(QStringLiteral("员工工作台"));
    }
}

void MainWindow::refreshWorkTables()
{
    refreshAttendanceTable();
    refreshPayrollTable();
    m.paySummaryLabel->setText(QStringLiteral("考勤与工资单使用同一员工、同一月份；点击“查询考勤”查看统计。"));
}

// 下拉框数据
void MainWindow::fillEmployeeCombos()
{
    // 所有“员工”相关下拉框共用同一份 zoo 数据；显示“编号 | 姓名”，数据存编号。
    QStringList ids;
    QStringList labels;
    const std::vector<animal *> &animals = animalManager.getanimals();
    for (size_t i = 0; i < animals.size(); ++i) {
        if (animals[i] == nullptr) {
            continue;
        }
        const std::string id = animals[i]->getid();
        ids << QString::fromStdString(id);
        labels << QString::fromStdString(id + " | " + animals[i]->getname());
    }

    QList<QComboBox *> combos = {
        m.bankOpenOwnerCombo, m.attEmployeeCombo, m.payEmployeeCombo
    };
    for (QComboBox *combo : combos) {
        if (!combo) {
            continue;
        }
        const QString old = combo->currentData().toString();
        combo->clear();
        if (combo == m.payEmployeeCombo) {
            combo->addItem(QStringLiteral("全部员工"), QString());
        }
        for (int i = 0; i < ids.size(); ++i) {
            combo->addItem(labels[i], ids[i]);
        }
        int index = combo->findData(old);
        if (index >= 0) {
            combo->setCurrentIndex(index);
        }
    }
}

void MainWindow::fillFoodCombos()
{
    if (!e.foodCombo) {
        return;
    }
    // 只有员工端提供购买入口，更新员工的食物下拉框。
    const std::vector<Food> &foods = foodService.getFoods();
    const QString old = e.foodCombo->currentData().toString();
    e.foodCombo->clear();
    for (const Food &food : foods) {
        const QString label = QString::fromStdString(food.getFoodId() + " | " +
                                                     food.getFoodName() + " | " +
                                                     moneyToString(food.getPricePerKg()) + " 元/kg");
        e.foodCombo->addItem(label, QString::fromStdString(food.getFoodId()));
    }
    const int index = e.foodCombo->findData(old);
    if (index >= 0) {
        e.foodCombo->setCurrentIndex(index);
    }
}

// 通用工具
QTableWidget *MainWindow::newTable(const QStringList &headers)
{
    // 统一表格风格：整行选择、只读、交替行色、最后一列拉伸。
    QTableWidget *table = new QTableWidget();
    table->setColumnCount(headers.size());
    table->setHorizontalHeaderLabels(headers);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setSelectionMode(QAbstractItemView::SingleSelection);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setAlternatingRowColors(true);
    table->verticalHeader()->setVisible(false);
    table->horizontalHeader()->setStretchLastSection(true);
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    table->setMinimumHeight(140);
    table->verticalHeader()->setDefaultSectionSize(32);
    return table;
}

QPushButton *MainWindow::newButton(const QString &text)
{
    QPushButton *button = new QPushButton(text);
    button->setMinimumHeight(32);
    button->setAutoDefault(false);
    button->setCursor(Qt::PointingHandCursor);
    return button;
}

QLineEdit *MainWindow::newLine(const QString &placeholder)
{
    QLineEdit *edit = new QLineEdit();
    edit->setPlaceholderText(placeholder);
    edit->setClearButtonEnabled(true);
    return edit;
}

QLabel *MainWindow::newHint(const QString &text)
{
    QLabel *label = new QLabel(text);
    label->setWordWrap(true);
    label->setStyleSheet(QStringLiteral("color:#555;"));
    return label;
}

QGroupBox *MainWindow::newGroup(const QString &title)
{
    return new QGroupBox(title);
}

QDialog *MainWindow::newDialog(const QString &title)
{
    QDialog *dialog = new QDialog(this);
    dialog->setWindowTitle(title);
    dialog->setMinimumWidth(440);
    dialog->setModal(true);
    return dialog;
}

QPushButton *MainWindow::dialogAction(QDialog *dialog, QFormLayout *form, const QString &text)
{
    QHBoxLayout *buttons = new QHBoxLayout();
    QPushButton *action = newButton(text);
    action->setObjectName(QStringLiteral("primaryButton"));
    action->setDefault(true);
    QPushButton *cancel = newButton(QStringLiteral("取消"));
    buttons->addStretch();
    buttons->addWidget(cancel);
    buttons->addWidget(action);
    form->addRow(buttons);
    connect(cancel, &QPushButton::clicked, dialog, &QDialog::reject);
    return action;
}

QPushButton *MainWindow::newDataButton(bool manager)
{
    QPushButton *button = newButton(QStringLiteral("数据"));
    QMenu *menu = new QMenu(button);
    QAction *save = menu->addAction(QStringLiteral("保存全部数据"));
    connect(save, &QAction::triggered, this, [this]() { saveAllData(true); });
    if (manager) {
        QAction *load = menu->addAction(QStringLiteral("重新加载数据"));
        connect(load, &QAction::triggered, this, [this]() {
            if (loadAllData()) {
                refreshAll();
                showStatus(QStringLiteral("数据已重新加载。"));
            }
        });
    }
    QAction *location = menu->addAction(QStringLiteral("查看数据位置"));
    connect(location, &QAction::triggered, this, [this]() {
        showInfo(QStringLiteral("数据保存在：\n") + QDir::currentPath());
    });
    button->setMenu(menu);
    return button;
}

void MainWindow::closeOperationDialog()
{
    QDialog *dialog = qobject_cast<QDialog *>(QApplication::activeModalWidget());
    if (dialog) {
        dialog->accept();
    }
}

QString MainWindow::statusText(int status) const
{
    switch (status) {
    case 0: return QStringLiteral("缺勤");
    case 1: return QStringLiteral("出勤");
    case 2: return QStringLiteral("请假");
    default: return QStringLiteral("未知");
    }
}

bool MainWindow::readMoney(QLineEdit *edit, Money &amount, QString &error) const
{
    // 金额输入统一走 parseMoney，界面只负责把错误翻译成中文。
    if (!edit) {
        error = QStringLiteral("金额输入框不存在");
        return false;
    }
    const QString text = edit->text().trimmed();
    if (text.isEmpty()) {
        error = QStringLiteral("请输入金额");
        return false;
    }
    if (!parseMoney(text.toStdString(), amount)) {
        error = QStringLiteral("金额格式不正确，请如 100 或 100.50");
        return false;
    }
    return true;
}

bool MainWindow::readDate(QDateEdit *edit, Date &date) const
{
    if (!edit) {
        return false;
    }
    date = dateFromQDate(edit->date());
    return date.isValid();
}

Date MainWindow::dateFromQDate(const QDate &date) const
{
    return Date(date.year(), date.month(), date.day());
}

std::vector<Date> MainWindow::buildScheduledDates(int year, int month) const
{
    // 所有页面统一按周一至周五排班。
    std::vector<Date> dates;
    QDate first(year, month, 1);
    if (!first.isValid()) {
        return dates;
    }
    const int days = first.daysInMonth();
    for (int day = 1; day <= days; ++day) {
        QDate d(year, month, day);
        if (d.dayOfWeek() == 6 || d.dayOfWeek() == 7) {
            continue;
        }
        dates.push_back(Date(year, month, day));
    }
    return dates;
}

animal *MainWindow::selectedAnimal() const
{
    // 动物园表格第 0 列是编号。
    if (!m.zooTable || m.zooTable->currentRow() < 0) {
        return nullptr;
    }
    QTableWidgetItem *item = m.zooTable->item(m.zooTable->currentRow(), 0);
    if (!item) {
        return nullptr;
    }
    return animalManager.findbyid(item->text().toStdString());
}

BankAccount *MainWindow::selectedAccount() const
{
    // 账户表格第 0 列是账号。
    if (!m.accountTable || m.accountTable->currentRow() < 0) {
        return nullptr;
    }
    QTableWidgetItem *item = m.accountTable->item(m.accountTable->currentRow(), 0);
    if (!item) {
        return nullptr;
    }
    return bank.findAccount(item->text().toStdString());
}

void MainWindow::selectZooRow(const std::string &id)
{
    if (!m.zooTable) {
        return;
    }
    const QString key = QString::fromStdString(id);
    for (int row = 0; row < m.zooTable->rowCount(); ++row) {
        if (m.zooTable->item(row, 0) && m.zooTable->item(row, 0)->text() == key) {
            m.zooTable->selectRow(row);
            return;
        }
    }
}

void MainWindow::showInfo(const QString &text)
{
    QMessageBox::information(this, QStringLiteral("提示"), text);
}

void MainWindow::showError(const QString &text)
{
    QMessageBox::warning(this, QStringLiteral("提示"), text);
}

void MainWindow::showStatus(const QString &text)
{
    statusBar()->showMessage(text, 5000);
}

// 登录页：角色 + 账号 + 密码 + 登录按钮
void MainWindow::setupLoginPage()
{
    QVBoxLayout *root = new QVBoxLayout(ui->loginPage);
    root->setContentsMargins(80, 70, 80, 70);
    root->setSpacing(12);

    QLabel *title = new QLabel(QStringLiteral("不疯狂动物城"));
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet(QStringLiteral("font-size:30px;font-weight:bold;color:#1F3864;"));
    QLabel *subtitle = new QLabel(QStringLiteral("动物园 · 考勤 · 银行 · 工资 · 食物 一体化管理"));
    subtitle->setAlignment(Qt::AlignCenter);
    subtitle->setStyleSheet(QStringLiteral("font-size:12px;color:#666;"));

    QGroupBox *box = newGroup(QStringLiteral("登录"));
    box->setMaximumWidth(520);
    QFormLayout *form = new QFormLayout(box);
    form->setSpacing(10);

    loginRoleCombo = new QComboBox();
    loginRoleCombo->addItem(QStringLiteral("管理者"), QStringLiteral("manager"));
    loginRoleCombo->addItem(QStringLiteral("员工"), QStringLiteral("employee"));
    loginAccountEdit = newLine(QStringLiteral("管理者：admin；员工：员工编号"));
    loginPasswordEdit = newLine(QStringLiteral("密码"));
    loginPasswordEdit->setEchoMode(QLineEdit::Password);
    QPushButton *loginButton = newButton(QStringLiteral("登录"));
    loginButton->setMinimumHeight(36);
    loginHintLabel = newHint(QStringLiteral("管理者：admin / 123456；员工：员工编号 / 123456"));
    QLabel *note = newHint(QStringLiteral("首次使用：先用管理者登录，在“动物园管理”添加动物员工，"
                                          "再在“银行管理”开户；员工才能登录和购买食物。"));

    form->addRow(QStringLiteral("角色"), loginRoleCombo);
    form->addRow(QStringLiteral("账号"), loginAccountEdit);
    form->addRow(QStringLiteral("密码"), loginPasswordEdit);
    form->addRow(loginButton);
    form->addRow(loginHintLabel);
    form->addRow(note);

    // 角色切换时更新账号/密码提示。
    connect(loginRoleCombo, &QComboBox::currentTextChanged, this, [this](const QString &) {
        if (loginRoleCombo->currentData().toString() == QStringLiteral("manager")) {
            loginAccountEdit->setPlaceholderText(QStringLiteral("管理者账号 admin"));
            loginHintLabel->setText(QStringLiteral("管理者演示账号：admin / 123456"));
        } else {
            loginAccountEdit->setPlaceholderText(QStringLiteral("员工编号，如 A001"));
            loginHintLabel->setText(QStringLiteral("员工默认密码：123456"));
        }
    });
    connect(loginButton, &QPushButton::clicked, this, &MainWindow::handleLogin);
    connect(loginPasswordEdit, &QLineEdit::returnPressed, this, &MainWindow::handleLogin);
    connect(loginAccountEdit, &QLineEdit::returnPressed, this, &MainWindow::handleLogin);

    root->addWidget(title);
    root->addWidget(subtitle);
    root->addSpacing(10);
    QHBoxLayout *center = new QHBoxLayout();
    center->addStretch();
    center->addWidget(box);
    center->addStretch();
    root->addLayout(center);
    root->addStretch();
}
