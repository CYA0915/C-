//   管理者页面：员工、考勤工资、银行、食物四个集成页面。
//   主页面显示工具栏和表格，新增、编辑等表单按需弹出。
//   业务调用集中在按钮槽函数里，刷新集中在 refreshXxx()，便于对照源码阅读。
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "../src/payroll/SalaryPolicy.h"

#include <QComboBox>
#include <QAction>
#include <QDialog>
#include <QMenu>
#include <QSplitter>
#include <QDateEdit>
#include <QDir>
#include <QFormLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QSpinBox>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QWidget>

#include <algorithm>

// 管理者首页包含员工、考勤工资、银行、食物四个标签页。
void MainWindow::setupManagerPage()
{
    QVBoxLayout *root = new QVBoxLayout(ui->managerPage);
    root->setContentsMargins(16, 14, 16, 14);
    root->setSpacing(12);
    QHBoxLayout *top = new QHBoxLayout();
    m.welcomeLabel = new QLabel(QStringLiteral("管理工作台"));
    m.welcomeLabel->setObjectName(QStringLiteral("pageTitle"));
    m.logoutButton = newButton(QStringLiteral("退出登录"));
    top->addWidget(m.welcomeLabel);
    top->addStretch();
    top->addWidget(newDataButton(true));
    top->addWidget(m.logoutButton);
    root->addLayout(top);
    m.tabs = new QTabWidget();
    m.tabs->setObjectName(QStringLiteral("managerTabs"));
    m.tabs->addTab(createZooTab(), QStringLiteral("员工管理"));
    m.tabs->addTab(createAttendanceTab(), QStringLiteral("考勤工资"));
    m.tabs->addTab(createBankTab(), QStringLiteral("银行管理"));
    m.tabs->addTab(createFoodTab(), QStringLiteral("食物管理"));
    root->addWidget(m.tabs, 1);
    connect(m.logoutButton, &QPushButton::clicked, this, &MainWindow::logout);
}


// 动物园管理标签页
QWidget *MainWindow::createZooTab()
{
    QWidget *page = new QWidget();
    QVBoxLayout *root = new QVBoxLayout(page);
    root->setContentsMargins(12, 12, 12, 12);
    root->setSpacing(10);

    QDialog *addDialog = newDialog(QStringLiteral("新增动物员工"));
    QFormLayout *addForm = new QFormLayout(addDialog);
    m.zooTypeCombo = new QComboBox();
    m.zooTypeCombo->addItem(QStringLiteral("老鹰"), QStringLiteral("eagle"));
    m.zooTypeCombo->addItem(QStringLiteral("鹦鹉"), QStringLiteral("parrot"));
    m.zooTypeCombo->addItem(QStringLiteral("狮子"), QStringLiteral("lion"));
    m.zooTypeCombo->addItem(QStringLiteral("大象"), QStringLiteral("elephant"));
    m.zooIdEdit = newLine(QStringLiteral("如 A001"));
    m.zooNameEdit = newLine(QStringLiteral("员工姓名"));
    m.zooWingEdit = newLine(QStringLiteral("翼展 cm，仅鸟类填写"));
    m.zooInitialSalaryEdit = newLine(QStringLiteral("基本工资，元"));
    m.zooInitialSalaryEdit->setText("0");
    addForm->addRow(QStringLiteral("物种"), m.zooTypeCombo);
    addForm->addRow(QStringLiteral("编号"), m.zooIdEdit);
    addForm->addRow(QStringLiteral("姓名"), m.zooNameEdit);
    addForm->addRow(QStringLiteral("翼展"), m.zooWingEdit);
    addForm->addRow(QStringLiteral("基本工资"), m.zooInitialSalaryEdit);
    connect(dialogAction(addDialog, addForm, QStringLiteral("添加员工")), &QPushButton::clicked, this, &MainWindow::addAnimal);
    connect(m.zooTypeCombo, &QComboBox::currentTextChanged, this, [this]() {
        const QString type = m.zooTypeCombo->currentData().toString();
        m.zooWingEdit->setEnabled(type == "eagle" || type == "parrot");
    });

    QDialog *editDialog = newDialog(QStringLiteral("编辑员工资料"));
    QFormLayout *editForm = new QFormLayout(editDialog);
    m.zooSelectedLabel = new QLabel();
    m.zooNewNameEdit = newLine(QStringLiteral("员工姓名"));
    m.zooSalaryEdit = newLine(QStringLiteral("基本工资，元"));
    m.zooAccountEdit = newLine(QStringLiteral("本人名下银行账号，可空"));
    editForm->addRow(QStringLiteral("员工"), m.zooSelectedLabel);
    editForm->addRow(QStringLiteral("姓名"), m.zooNewNameEdit);
    editForm->addRow(QStringLiteral("基本工资"), m.zooSalaryEdit);
    editForm->addRow(QStringLiteral("绑定账号"), m.zooAccountEdit);
    connect(dialogAction(editDialog, editForm, QStringLiteral("保存资料")), &QPushButton::clicked, this, &MainWindow::updateAnimalInfo);

    QDialog *feedDialog = newDialog(QStringLiteral("登记饲养"));
    QFormLayout *feedForm = new QFormLayout(feedDialog);
    m.feedAnimalEdit = newLine();
    m.feedAnimalEdit->setReadOnly(true);
    m.feedDateEdit = new QDateEdit(QDate::currentDate());
    m.feedDateEdit->setCalendarPopup(true);
    m.feedDateEdit->setDisplayFormat("yyyy-MM-dd");
    m.feedAmountEdit = newLine(QStringLiteral("数量 kg"));
    feedForm->addRow(QStringLiteral("员工编号"), m.feedAnimalEdit);
    feedForm->addRow(QStringLiteral("日期"), m.feedDateEdit);
    feedForm->addRow(QStringLiteral("数量 kg"), m.feedAmountEdit);
    connect(dialogAction(feedDialog, feedForm, QStringLiteral("登记饲养")), &QPushButton::clicked, this, &MainWindow::addFeedRecord);

    QDialog *healthDialog = newDialog(QStringLiteral("登记健康"));
    QFormLayout *healthForm = new QFormLayout(healthDialog);
    m.healthAnimalEdit = newLine();
    m.healthAnimalEdit->setReadOnly(true);
    m.healthDateEdit = new QDateEdit(QDate::currentDate());
    m.healthDateEdit->setCalendarPopup(true);
    m.healthDateEdit->setDisplayFormat("yyyy-MM-dd");
    m.healthStatusCombo = new QComboBox();
    m.healthStatusCombo->addItems({QStringLiteral("健康"), QStringLiteral("亚健康"), QStringLiteral("生病")});
    m.healthNoteEdit = newLine(QStringLiteral("备注"));
    healthForm->addRow(QStringLiteral("员工编号"), m.healthAnimalEdit);
    healthForm->addRow(QStringLiteral("日期"), m.healthDateEdit);
    healthForm->addRow(QStringLiteral("健康状态"), m.healthStatusCombo);
    healthForm->addRow(QStringLiteral("备注"), m.healthNoteEdit);
    connect(dialogAction(healthDialog, healthForm, QStringLiteral("登记健康")), &QPushButton::clicked, this, &MainWindow::addHealthRecord);

    QDialog *recordsDialog = newDialog(QStringLiteral("饲养与健康记录"));
    recordsDialog->resize(650, 430);
    QVBoxLayout *recordsLayout = new QVBoxLayout(recordsDialog);
    m.recordView = new QTextEdit();
    m.recordView->setReadOnly(true);
    recordsLayout->addWidget(m.recordView);
    QPushButton *closeRecords = newButton(QStringLiteral("关闭"));
    recordsLayout->addWidget(closeRecords);
    connect(closeRecords, &QPushButton::clicked, recordsDialog, &QDialog::accept);

    QHBoxLayout *bar = new QHBoxLayout();
    QPushButton *add = newButton(QStringLiteral("新增员工"));
    add->setObjectName(QStringLiteral("primaryButton"));
    QPushButton *edit = newButton(QStringLiteral("编辑"));
    QPushButton *openAccount = newButton(QStringLiteral("开户"));
    QPushButton *work = newButton(QStringLiteral("考勤工资"));
    QPushButton *records = newButton(QStringLiteral("动物记录"));
    QPushButton *remove = newButton(QStringLiteral("删除"));
    bar->addWidget(add);
    bar->addWidget(edit);
    bar->addWidget(openAccount);
    bar->addWidget(work);
    bar->addWidget(records);
    bar->addWidget(remove);
    bar->addStretch();
    m.zooFilterCategory = new QComboBox();
    m.zooFilterCategory->addItem(QStringLiteral("全部类别"), QString());
    m.zooFilterCategory->addItem(QStringLiteral("鸟类"), QStringLiteral("鸟类"));
    m.zooFilterCategory->addItem(QStringLiteral("哺乳动物"), QStringLiteral("哺乳动物"));
    m.zooFilterId = newLine(QStringLiteral("编号 / 姓名"));
    m.zooFilterId->setMaximumWidth(180);
    QPushButton *query = newButton(QStringLiteral("查询"));
    bar->addWidget(m.zooFilterCategory);
    bar->addWidget(m.zooFilterId);
    bar->addWidget(query);
    root->addLayout(bar);
    m.zooTable = newTable({QStringLiteral("编号"), QStringLiteral("姓名"), QStringLiteral("类别"),
                           QStringLiteral("物种"), QStringLiteral("翼展/食性"), QStringLiteral("基本工资"), QStringLiteral("银行账号")});
    root->addWidget(m.zooTable, 1);
    root->addWidget(newHint(QStringLiteral("选中员工后可编辑资料、开户或进入考勤工资。")));
    connect(m.zooTable, &QTableWidget::itemSelectionChanged, this, &MainWindow::onZooSelectionChanged);
    connect(add, &QPushButton::clicked, addDialog, &QDialog::open);
    connect(edit, &QPushButton::clicked, this, [this, editDialog]() {
        if (!selectedAnimal()) { showError(QStringLiteral("请先选择一名员工。")); return; }
        onZooSelectionChanged();
        editDialog->open();
    });
    connect(openAccount, &QPushButton::clicked, this, [this]() {
        animal *a = selectedAnimal();
        if (!a) { showError(QStringLiteral("请先选择一名员工。")); return; }
        m.bankOpenOwnerCombo->setCurrentIndex(m.bankOpenOwnerCombo->findData(QString::fromStdString(a->getid())));
        m.bankOpenDialog->open();
    });
    connect(work, &QPushButton::clicked, this, [this]() {
        animal *a = selectedAnimal();
        if (!a) { showError(QStringLiteral("请先选择一名员工。")); return; }
        m.payEmployeeCombo->setCurrentIndex(m.payEmployeeCombo->findData(QString::fromStdString(a->getid())));
        m.tabs->setCurrentIndex(1);
        summarizeAttendance();
    });
    QMenu *recordMenu = new QMenu(records);
    QAction *feedAction = recordMenu->addAction(QStringLiteral("登记饲养"));
    QAction *healthAction = recordMenu->addAction(QStringLiteral("登记健康"));
    QAction *viewAction = recordMenu->addAction(QStringLiteral("查看记录"));
    records->setMenu(recordMenu);
    connect(feedAction, &QAction::triggered, this, [this, feedDialog]() {
        if (!selectedAnimal()) { showError(QStringLiteral("请先选择一名员工。")); return; }
        onZooSelectionChanged(); feedDialog->open();
    });
    connect(healthAction, &QAction::triggered, this, [this, healthDialog]() {
        if (!selectedAnimal()) { showError(QStringLiteral("请先选择一名员工。")); return; }
        onZooSelectionChanged(); healthDialog->open();
    });
    connect(viewAction, &QAction::triggered, this, [this, recordsDialog]() {
        if (!selectedAnimal()) { showError(QStringLiteral("请先选择一名员工。")); return; }
        showAnimalRecords(); recordsDialog->open();
    });
    connect(remove, &QPushButton::clicked, this, &MainWindow::deleteAnimal);
    connect(query, &QPushButton::clicked, this, &MainWindow::refreshZooTable);
    connect(m.zooFilterId, &QLineEdit::returnPressed, this, &MainWindow::refreshZooTable);
    connect(m.zooFilterCategory, &QComboBox::currentTextChanged, this, &MainWindow::refreshZooTable);
    return page;
}

// 动物园表格：按类别和编号筛选；专属列用 dynamic_cast 读取翼展/食性。
void MainWindow::refreshZooTable()
{
    if (!m.zooTable) {
        return;
    }
    const QString category = m.zooFilterCategory ? m.zooFilterCategory->currentData().toString() : QString();
    const QString idFilter = m.zooFilterId ? m.zooFilterId->text().trimmed() : QString();
    const std::vector<animal *> &animals = animalManager.getanimals();

    m.zooTable->setRowCount(0);
    for (size_t i = 0; i < animals.size(); ++i) {
        animal *a = animals[i];
        if (!a) {
            continue;
        }
        const QString categoryText = QString::fromStdString(a->getcategory());
        const QString id = QString::fromStdString(a->getid());
        if (!category.isEmpty() && categoryText != category) {
            continue;
        }
        if (!idFilter.isEmpty() && !id.contains(idFilter, Qt::CaseInsensitive) &&
            !QString::fromStdString(a->getname()).contains(idFilter, Qt::CaseInsensitive)) {
            continue;
        }

        QString extra;
        if (bird *b = dynamic_cast<bird *>(a)) {
            extra = QString::number(b->getwinglength()) + QStringLiteral(" cm");
        } else if (mammal *mm = dynamic_cast<mammal *>(a)) {
            extra = QString::fromStdString(mm->getdiettype());
        }

        const int row = m.zooTable->rowCount();
        m.zooTable->insertRow(row);
        m.zooTable->setItem(row, 0, new QTableWidgetItem(id));
        m.zooTable->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(a->getname())));
        m.zooTable->setItem(row, 2, new QTableWidgetItem(categoryText));
        m.zooTable->setItem(row, 3, new QTableWidgetItem(QString::fromStdString(a->getspecies())));
        m.zooTable->setItem(row, 4, new QTableWidgetItem(extra));
        m.zooTable->setItem(row, 5, new QTableWidgetItem(QString::fromStdString(moneyToString(a->basicSalary()))));
        m.zooTable->setItem(row, 6, new QTableWidgetItem(QString::fromStdString(a->bankAccountNo())));
    }
}

void MainWindow::onZooSelectionChanged()
{
    animal *a = selectedAnimal();
    if (!a) {
        m.zooSelectedLabel->setText(QStringLiteral("未选择"));
        return;
    }
    m.zooSelectedLabel->setText(QString::fromStdString(a->getid() + " | " + a->getname() + " | " + a->getcategory()));
    m.zooNewNameEdit->setText(QString::fromStdString(a->getname()));
    m.zooSalaryEdit->setText(QString::fromStdString(moneyToString(a->basicSalary())));
    m.zooAccountEdit->setText(QString::fromStdString(a->bankAccountNo()));
    m.feedAnimalEdit->setText(QString::fromStdString(a->getid()));
    m.healthAnimalEdit->setText(QString::fromStdString(a->getid()));
}

// 员工资料及动物历史记录操作。
void MainWindow::addAnimal()
{
    const QString id = m.zooIdEdit->text().trimmed();
    const QString name = m.zooNameEdit->text().trimmed();
    const QString type = m.zooTypeCombo->currentData().toString();

    // 编号唯一、姓名非空；鸟类翼展必须为正数。
    if (id.isEmpty() || name.isEmpty()) {
        showError(QStringLiteral("编号和姓名不能为空。"));
        return;
    }
    if (animalManager.findbyid(id.toStdString()) != nullptr) {
        showError(QStringLiteral("编号已存在，请换一个。"));
        return;
    }

    bool ok = false;
    double wing = 0.0;
    if (type == QStringLiteral("eagle") || type == QStringLiteral("parrot")) {
        wing = m.zooWingEdit->text().trimmed().toDouble(&ok);
        if (!ok || wing <= 0) {
            showError(QStringLiteral("鸟类翼展必须是大于 0 的数字。"));
            return;
        }
    }

    Money salary = 0;
    QString error;
    if (!readMoney(m.zooInitialSalaryEdit, salary, error)) {
        showError(QStringLiteral("基本工资：") + error);
        return;
    }
    animal *a = nullptr;
    if (type == QStringLiteral("eagle")) {
        a = new eagle(id.toStdString(), name.toStdString(), wing);
    } else if (type == QStringLiteral("parrot")) {
        a = new parrot(id.toStdString(), name.toStdString(), wing);
    } else if (type == QStringLiteral("lion")) {
        a = new lion(id.toStdString(), name.toStdString());
    } else if (type == QStringLiteral("elephant")) {
        a = new elephant(id.toStdString(), name.toStdString());
    } else {
        showError(QStringLiteral("未知动物类型。"));
        return;
    }

    a->setBasicSalary(salary);
    animalManager.addanimal(a);   // 所有权移交 zoo
    try {
        animalManager.save();
    } catch (...) {
    }
    const QDate today = QDate::currentDate();
    initializeAttendance(today.year(), today.month());
    refreshAll();
    selectZooRow(id.toStdString());
    m.zooIdEdit->clear();
    m.zooNameEdit->clear();
    m.zooWingEdit->clear();
    showStatus(QStringLiteral("动物添加成功：") + id);
    closeOperationDialog();
}

void MainWindow::updateAnimalInfo()
{
    animal *a = selectedAnimal();
    if (!a) {
        showError(QStringLiteral("请先在表格中选择一只动物。"));
        return;
    }
    const QString name = m.zooNewNameEdit->text().trimmed();
    if (name.isEmpty()) {
        showError(QStringLiteral("请输入员工姓名。"));
        return;
    }
    Money salary = 0;
    QString error;
    if (!readMoney(m.zooSalaryEdit, salary, error)) {
        showError(QStringLiteral("基本工资：") + error);
        return;
    }
    const std::string accountNo = m.zooAccountEdit->text().trimmed().toStdString();
    if (!accountNo.empty()) {
        BankAccount *account = bank.findAccount(accountNo);
        if (!account || account->ownerId() != a->getid()) {
            showError(QStringLiteral("绑定账号必须是该员工名下已开立的银行账户。"));
            return;
        }
    }
    animalManager.updateanimal(a->getid(), name.toStdString());
    a->setBasicSalary(salary);
    a->setBankAccountNo(accountNo);
    try {
        animalManager.save();
    } catch (...) {
    }
    refreshAll();
    showStatus(QStringLiteral("员工资料已保存。"));
    closeOperationDialog();
}

void MainWindow::deleteAnimal()
{
    animal *a = selectedAnimal();
    if (!a) {
        showError(QStringLiteral("请先在表格中选择一只动物。"));
        return;
    }
    const QString id = QString::fromStdString(a->getid());
    if (QMessageBox::question(this, QStringLiteral("确认删除"),
                              QStringLiteral("确定删除动物 ") + id + QStringLiteral(" 吗？\n"
                              "如果已有考勤、账户或工资记录，相关记录不会自动删除。")) != QMessageBox::Yes) {
        return;
    }
    if (!animalManager.removeanimal(id.toStdString())) {
        showError(QStringLiteral("删除失败。"));
        return;
    }
    try {
        animalManager.save();
    } catch (...) {
    }
    refreshAll();
    showStatus(QStringLiteral("已删除：") + id);
}

void MainWindow::addFeedRecord()
{
    const QString id = m.feedAnimalEdit->text().trimmed();
    if (id.isEmpty()) {
        showError(QStringLiteral("请输入动物编号。"));
        return;
    }
    bool ok = false;
    const double amount = m.feedAmountEdit->text().trimmed().toDouble(&ok);
    if (!ok || amount <= 0) {
        showError(QStringLiteral("饲养数量必须是大于 0 的数字（kg）。"));
        return;
    }
    Date date;
    if (!readDate(m.feedDateEdit, date)) {
        showError(QStringLiteral("日期不合法。"));
        return;
    }
    if (!animalManager.feedanimal(id.toStdString(), date.toString(), amount)) {
        showError(QStringLiteral("未找到该动物。"));
        return;
    }
    try {
        animalManager.save();
    } catch (...) {
    }
    showAnimalRecords();
    showStatus(QStringLiteral("饲养记录已添加。"));
    closeOperationDialog();
}

void MainWindow::addHealthRecord()
{
    const QString id = m.healthAnimalEdit->text().trimmed();
    if (id.isEmpty()) {
        showError(QStringLiteral("请输入动物编号。"));
        return;
    }
    Date date;
    if (!readDate(m.healthDateEdit, date)) {
        showError(QStringLiteral("日期不合法。"));
        return;
    }
    const std::string status = m.healthStatusCombo->currentText().toStdString();
    const std::string note = m.healthNoteEdit->text().trimmed().toStdString();
    if (!animalManager.checkhealth(id.toStdString(), date.toString(), status, note)) {
        showError(QStringLiteral("未找到该动物。"));
        return;
    }
    try {
        animalManager.save();
    } catch (...) {
    }
    showAnimalRecords();
    showStatus(QStringLiteral("健康记录已添加。"));
    closeOperationDialog();
}

void MainWindow::showAnimalRecords()
{
    animal *a = selectedAnimal();
    if (!a) {
        const QString id = m.feedAnimalEdit->text().trimmed();
        if (id.isEmpty()) {
            showError(QStringLiteral("请选择动物或输入动物编号。"));
            return;
        }
        a = animalManager.findbyid(id.toStdString());
    }
    if (!a) {
        showError(QStringLiteral("未找到该动物。"));
        return;
    }

    QString text;
    text += QStringLiteral("=== ") + QString::fromStdString(a->getname()) + QStringLiteral(" 饲养记录 ===\n");
    const std::vector<feedrecord> &feeds = a->getfeedrecords();
    if (feeds.empty()) {
        text += QStringLiteral("  （暂无）\n");
    }
    for (size_t i = 0; i < feeds.size(); ++i) {
        text += QStringLiteral("  [") + QString::fromStdString(feeds[i].date) + QStringLiteral("] ")
             + QString::number(feeds[i].amount) + QStringLiteral(" kg\n");
    }
    text += QStringLiteral("=== ") + QString::fromStdString(a->getname()) + QStringLiteral(" 健康记录 ===\n");
    const std::vector<healthrecord> &healths = a->gethealthrecords();
    if (healths.empty()) {
        text += QStringLiteral("  （暂无）\n");
    }
    for (size_t i = 0; i < healths.size(); ++i) {
        text += QStringLiteral("  [") + QString::fromStdString(healths[i].date) + QStringLiteral("] ")
             + QString::fromStdString(healths[i].status) + QStringLiteral("，")
             + QString::fromStdString(healths[i].note) + QStringLiteral("\n");
    }
    m.recordView->setPlainText(text);
}

// 考勤管理标签页
QWidget *MainWindow::createAttendanceTab()
{
    QWidget *page = new QWidget();
    QVBoxLayout *root = new QVBoxLayout(page);
    root->setContentsMargins(12, 12, 12, 12);
    root->setSpacing(10);
    QHBoxLayout *bar = new QHBoxLayout();
    m.payEmployeeCombo = new QComboBox();
    m.payEmployeeCombo->setMinimumWidth(170);
    m.payYearSpin = new QSpinBox();
    m.payYearSpin->setRange(2000, 2100);
    m.payYearSpin->setValue(QDate::currentDate().year());
    m.payMonthSpin = new QSpinBox();
    m.payMonthSpin->setRange(1, 12);
    m.payMonthSpin->setValue(QDate::currentDate().month());
    QPushButton *query = newButton(QStringLiteral("查询考勤"));
    QPushButton *generate = newButton(QStringLiteral("生成工资单"));
    generate->setObjectName(QStringLiteral("primaryButton"));
    bar->addWidget(m.payEmployeeCombo, 1);
    bar->addWidget(m.payYearSpin);
    bar->addWidget(new QLabel(QStringLiteral("年")));
    bar->addWidget(m.payMonthSpin);
    bar->addWidget(new QLabel(QStringLiteral("月")));
    bar->addWidget(query);
    bar->addWidget(generate);
    root->addLayout(bar);
    m.paySummaryLabel = newHint(QStringLiteral("选择员工和月份即可查询考勤；工资按固定缺勤、请假扣款计算。"));
    root->addWidget(m.paySummaryLabel);

    QDialog *changeDialog = newDialog(QStringLiteral("设置考勤记录"));
    QFormLayout *changeForm = new QFormLayout(changeDialog);
    m.attEmployeeCombo = new QComboBox();
    m.attDateEdit = new QDateEdit(QDate::currentDate());
    m.attDateEdit->setCalendarPopup(true);
    m.attDateEdit->setDateRange(QDate(2000, 1, 1), QDate(2100, 12, 31));
    m.attDateEdit->setDisplayFormat("yyyy-MM-dd");
    m.attNewStatusCombo = new QComboBox();
    m.attNewStatusCombo->addItem(QStringLiteral("缺勤"), 0);
    m.attNewStatusCombo->addItem(QStringLiteral("出勤"), 1);
    m.attNewStatusCombo->addItem(QStringLiteral("请假"), 2);
    changeForm->addRow(QStringLiteral("员工"), m.attEmployeeCombo);
    changeForm->addRow(QStringLiteral("日期"), m.attDateEdit);
    changeForm->addRow(QStringLiteral("状态"), m.attNewStatusCombo);
    connect(dialogAction(changeDialog, changeForm, QStringLiteral("保存考勤")), &QPushButton::clicked, this, &MainWindow::changeAttendance);

    QDialog *salaryDialog = newDialog(QStringLiteral("计算月度工资"));
    QFormLayout *salaryForm = new QFormLayout(salaryDialog);
    m.payAbsentEdit = newLine(QStringLiteral("每缺勤一次扣款，元"));
    m.payLeaveEdit = newLine(QStringLiteral("每请假一次扣款，元"));
    m.payBonusEdit = newLine(QStringLiteral("本月奖金，元"));
    m.payAbsentEdit->setText("0");
    m.payLeaveEdit->setText("0");
    m.payBonusEdit->setText("0");
    salaryForm->addRow(QStringLiteral("缺勤日扣款"), m.payAbsentEdit);
    salaryForm->addRow(QStringLiteral("请假日扣款"), m.payLeaveEdit);
    salaryForm->addRow(QStringLiteral("奖金"), m.payBonusEdit);
    salaryForm->addRow(newHint(QStringLiteral("使用主页面选定的员工和月份，只能结算已结束月份。")));
    connect(dialogAction(salaryDialog, salaryForm, QStringLiteral("计算并生成工资单")), &QPushButton::clicked, this, &MainWindow::generatePayroll);

    QSplitter *splitter = new QSplitter(Qt::Vertical);
    splitter->setChildrenCollapsible(false);
    QWidget *attendancePage = new QWidget();
    QVBoxLayout *attendanceLayout = new QVBoxLayout(attendancePage);
    attendanceLayout->setContentsMargins(0, 0, 0, 0);
    QHBoxLayout *attendanceBar = new QHBoxLayout();
    attendanceBar->addWidget(new QLabel(QStringLiteral("考勤明细")));
    attendanceBar->addStretch();
    QPushButton *change = newButton(QStringLiteral("设置考勤"));
    QPushButton *remove = newButton(QStringLiteral("删除记录"));
    attendanceBar->addWidget(change);
    attendanceBar->addWidget(remove);
    attendanceLayout->addLayout(attendanceBar);
    m.attTable = newTable({QStringLiteral("日期"), QStringLiteral("员工编号"), QStringLiteral("状态")});
    attendanceLayout->addWidget(m.attTable);
    splitter->addWidget(attendancePage);
    QWidget *payPage = new QWidget();
    QVBoxLayout *payLayout = new QVBoxLayout(payPage);
    payLayout->setContentsMargins(0, 0, 0, 0);
    QHBoxLayout *payBar = new QHBoxLayout();
    payBar->addWidget(new QLabel(QStringLiteral("工资单")));
    payBar->addStretch();
    QPushButton *pay = newButton(QStringLiteral("发放选中工资单"));
    payBar->addWidget(pay);
    payLayout->addLayout(payBar);
    m.payTable = newTable({QStringLiteral("工资单号"), QStringLiteral("员工"), QStringLiteral("年月"),
                          QStringLiteral("基本工资"), QStringLiteral("缺勤扣"), QStringLiteral("请假扣"),
                          QStringLiteral("奖金"), QStringLiteral("实发"), QStringLiteral("状态"), QStringLiteral("银行流水")});
    payLayout->addWidget(m.payTable);
    splitter->addWidget(payPage);
    splitter->setStretchFactor(0, 3);
    splitter->setStretchFactor(1, 2);
    root->addWidget(splitter, 1);
    root->addWidget(newHint(QStringLiteral("统一周一至周五排班；签到和请假由员工操作，未来日期的缺勤只是初始值。")));
    connect(query, &QPushButton::clicked, this, &MainWindow::summarizeAttendance);
    connect(generate, &QPushButton::clicked, this, [this, salaryDialog]() {
        if (m.payEmployeeCombo->currentData().toString().isEmpty()) {
            showError(QStringLiteral("请先选择一名员工。")); return;
        }
        salaryDialog->open();
    });
    connect(change, &QPushButton::clicked, this, [this, changeDialog]() {
        if (m.attEmployeeCombo->count() == 0) {
            showError(QStringLiteral("请先添加一名员工。")); return;
        }
        const int row = m.attTable->currentRow();
        QString id = m.payEmployeeCombo->currentData().toString();
        const std::vector<Date> dates = buildScheduledDates(m.payYearSpin->value(), m.payMonthSpin->value());
        if (!dates.empty()) {
            m.attDateEdit->setDate(QDate(dates[0].year(), dates[0].month(), dates[0].day()));
        }
        m.attNewStatusCombo->setCurrentIndex(0);
        if (row >= 0) {
            id = m.attTable->item(row, 1)->text();
            m.attDateEdit->setDate(QDate::fromString(m.attTable->item(row, 0)->text(), "yyyy-MM-dd"));
            const QString status = m.attTable->item(row, 2)->text();
            m.attNewStatusCombo->setCurrentIndex(m.attNewStatusCombo->findText(status));
        }
        const int employeeIndex = m.attEmployeeCombo->findData(id);
        if (employeeIndex >= 0) { m.attEmployeeCombo->setCurrentIndex(employeeIndex); }
        changeDialog->open();
    });
    connect(remove, &QPushButton::clicked, this, &MainWindow::deleteAttendance);
    connect(pay, &QPushButton::clicked, this, &MainWindow::paySelectedPayroll);
    connect(m.payEmployeeCombo, &QComboBox::currentTextChanged, this, &MainWindow::refreshWorkTables);
    connect(m.payYearSpin, &QSpinBox::textChanged, this, &MainWindow::refreshWorkTables);
    connect(m.payMonthSpin, &QSpinBox::textChanged, this, &MainWindow::refreshWorkTables);
    return page;
}

void MainWindow::refreshAttendanceTable()
{
    if (!m.attTable) {
        return;
    }
    const std::string selectedId = m.payEmployeeCombo->currentData().toString().toStdString();
    const int year = m.payYearSpin->value();
    const int month = m.payMonthSpin->value();
    m.attTable->setRowCount(0);
    for (int i = 0; i < attendance.dayCount(); ++i) {
        const dayRecord *d = attendance.dayAt(i);
        if (!d || d->date.year() != year || d->date.month() != month) {
            continue;
        }
        for (int j = 0; j < d->Recordcnt; ++j) {
            if (!selectedId.empty() && d->dayrecords[j].ID != selectedId) {
                continue;
            }
            const int row = m.attTable->rowCount();
            m.attTable->insertRow(row);
            m.attTable->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(d->date.toString())));
            m.attTable->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(d->dayrecords[j].ID)));
            m.attTable->setItem(row, 2, new QTableWidgetItem(statusText(d->dayrecords[j].status)));
        }
    }
}

void MainWindow::changeAttendance()
{
    if (m.attEmployeeCombo->count() == 0) {
        showError(QStringLiteral("请先添加一名员工。"));
        return;
    }
    Date date;
    if (!readDate(m.attDateEdit, date)) {
        showError(QStringLiteral("请选择有效日期。"));
        return;
    }
    const std::string id = m.attEmployeeCombo->currentData().toString().toStdString();
    const int status = m.attNewStatusCombo->currentData().toInt();
    if (!attendance.changeRecord(date, id, status)) {
        attendance.addRecord(date, id, status);
    }
    Record records[MAX_RECORDS];
    const int count = attendance.getRecord(date, records);
    bool saved = false;
    for (int i = 0; i < count; ++i) {
        if (records[i].ID == id && records[i].status == status) {
            saved = true;
            break;
        }
    }
    if (!saved) {
        showError(QStringLiteral("考勤记录空间不足，无法补录。"));
        return;
    }
    if (!attendanceFile.Save(attendance, kAttendanceFile)) {
        showError(QStringLiteral("考勤已设置，但文件保存失败。"));
        return;
    }
    m.payEmployeeCombo->setCurrentIndex(m.payEmployeeCombo->findData(QString::fromStdString(id)));
    m.payYearSpin->setValue(date.year());
    m.payMonthSpin->setValue(date.month());
    refreshWorkTables();
    showStatus(QStringLiteral("考勤记录已保存。"));
    closeOperationDialog();
}

void MainWindow::deleteAttendance()
{
    if (!m.attTable || m.attTable->currentRow() < 0) {
        showError(QStringLiteral("请先在表格中选择一条考勤记录。"));
        return;
    }
    const int row = m.attTable->currentRow();
    Date date;
    if (!Date::parse(m.attTable->item(row, 0)->text().toStdString(), date)) {
        showError(QStringLiteral("日期解析失败。"));
        return;
    }
    const std::string id = m.attTable->item(row, 1)->text().toStdString();
    const QString statusTextValue = m.attTable->item(row, 2)->text();
    int status = 1;
    if (statusTextValue == QStringLiteral("缺勤")) {
        status = 0;
    } else if (statusTextValue == QStringLiteral("请假")) {
        status = 2;
    }
    if (QMessageBox::question(this, QStringLiteral("确认删除"),
                              QStringLiteral("确定删除该考勤记录吗？")) != QMessageBox::Yes) {
        return;
    }
    attendance.deleteRecord(date, id, status);
    if (!attendanceFile.Save(attendance, kAttendanceFile)) {
        showError(QStringLiteral("考勤已删除，但保存失败。"));
    }
    refreshAttendanceTable();
    showStatus(QStringLiteral("考勤记录已删除。"));
}

void MainWindow::summarizeAttendance()
{
    if (!m.payEmployeeCombo || m.payEmployeeCombo->count() == 0) {
        showError(QStringLiteral("还没有员工。"));
        return;
    }
    const std::string id = m.payEmployeeCombo->currentData().toString().toStdString();
    const int year = m.payYearSpin->value();
    const int month = m.payMonthSpin->value();
    if (!initializeAttendance(year, month)) {
        return;
    }
    const std::vector<Date> dates = buildScheduledDates(year, month);
    refreshAttendanceTable();
    refreshPayrollTable();
    if (id.empty()) {
        m.paySummaryLabel->setText(QStringLiteral("已显示全体员工本月考勤和工资单；选择具体员工可查看统计或生成工资单。"));
        return;
    }

    AttendanceSummary summary;
    std::string message;
    if (!attendance.summarize(id, year, month, dates, summary, message)) {
        m.paySummaryLabel->setText(QStringLiteral("统计失败：") + QString::fromStdString(message));
        return;
    }
    m.paySummaryLabel->setText(
        QStringLiteral("应出勤 %1 天；出勤 %2；缺勤 %3；请假 %4；未录入 %5")
            .arg(summary.scheduledDays).arg(summary.presentDays)
            .arg(summary.absentDays).arg(summary.leaveDays).arg(summary.missingDays));
}

// 银行管理标签页
QWidget *MainWindow::createBankTab()
{
    QWidget *page = new QWidget();
    QVBoxLayout *root = new QVBoxLayout(page);
    root->setContentsMargins(12, 12, 12, 12);
    root->setSpacing(10);
    m.bankOpenDialog = newDialog(QStringLiteral("开立银行账户"));
    QFormLayout *openForm = new QFormLayout(m.bankOpenDialog);
    m.bankOpenNoEdit = newLine(QStringLiteral("6~19 位数字"));
    m.bankOpenOwnerCombo = new QComboBox();
    m.bankOpenTypeCombo = new QComboBox();
    m.bankOpenTypeCombo->addItem(QStringLiteral("储蓄账户"), 1);
    m.bankOpenTypeCombo->addItem(QStringLiteral("信用账户"), 2);
    m.bankOpenInitialEdit = newLine(QStringLiteral("首次存款，元"));
    m.bankOpenExtraEdit = newLine(QStringLiteral("最低留存余额，元"));
    m.bankOpenInitialEdit->setText("0");
    m.bankOpenExtraEdit->setText("0");
    openForm->addRow(QStringLiteral("员工"), m.bankOpenOwnerCombo);
    openForm->addRow(QStringLiteral("账号"), m.bankOpenNoEdit);
    openForm->addRow(QStringLiteral("账户类型"), m.bankOpenTypeCombo);
    openForm->addRow(QStringLiteral("首次存款"), m.bankOpenInitialEdit);
    openForm->addRow(QStringLiteral("最低余额 / 信用额度"), m.bankOpenExtraEdit);
    connect(m.bankOpenTypeCombo, &QComboBox::currentTextChanged, this, [this]() {
        m.bankOpenExtraEdit->setPlaceholderText(m.bankOpenTypeCombo->currentData().toInt() == 2
                                              ? QStringLiteral("信用额度，元") : QStringLiteral("最低留存余额，元"));
    });
    connect(dialogAction(m.bankOpenDialog, openForm, QStringLiteral("确认开户")), &QPushButton::clicked, this, &MainWindow::openBankAccount);

    QDialog *moneyDialog = newDialog(QStringLiteral("账户资金操作"));
    QVBoxLayout *moneyLayout = new QVBoxLayout(moneyDialog);
    QTabWidget *operations = new QTabWidget();
    QWidget *depositPage = new QWidget();
    QFormLayout *depositForm = new QFormLayout(depositPage);
    m.bankDepositAccountEdit = newLine(QStringLiteral("账号"));
    m.bankDepositAmountEdit = newLine(QStringLiteral("金额，元"));
    depositForm->addRow(QStringLiteral("账号"), m.bankDepositAccountEdit);
    depositForm->addRow(QStringLiteral("存款金额"), m.bankDepositAmountEdit);
    connect(dialogAction(moneyDialog, depositForm, QStringLiteral("确认存款")), &QPushButton::clicked, this, &MainWindow::depositMoney);
    operations->addTab(depositPage, QStringLiteral("存款"));
    QWidget *withdrawPage = new QWidget();
    QFormLayout *withdrawForm = new QFormLayout(withdrawPage);
    m.bankWithdrawAccountEdit = newLine(QStringLiteral("账号"));
    m.bankWithdrawAmountEdit = newLine(QStringLiteral("金额，元"));
    withdrawForm->addRow(QStringLiteral("账号"), m.bankWithdrawAccountEdit);
    withdrawForm->addRow(QStringLiteral("取款金额"), m.bankWithdrawAmountEdit);
    connect(dialogAction(moneyDialog, withdrawForm, QStringLiteral("确认取款")), &QPushButton::clicked, this, &MainWindow::withdrawMoney);
    operations->addTab(withdrawPage, QStringLiteral("取款"));
    QWidget *transferPage = new QWidget();
    QFormLayout *transferForm = new QFormLayout(transferPage);
    m.bankFromEdit = newLine(QStringLiteral("转出账号"));
    m.bankToEdit = newLine(QStringLiteral("转入账号"));
    m.bankTransferAmountEdit = newLine(QStringLiteral("金额，元"));
    transferForm->addRow(QStringLiteral("转出账号"), m.bankFromEdit);
    transferForm->addRow(QStringLiteral("转入账号"), m.bankToEdit);
    transferForm->addRow(QStringLiteral("转账金额"), m.bankTransferAmountEdit);
    connect(dialogAction(moneyDialog, transferForm, QStringLiteral("确认转账")), &QPushButton::clicked, this, &MainWindow::transferMoney);
    operations->addTab(transferPage, QStringLiteral("转账"));
    moneyLayout->addWidget(operations);

    QHBoxLayout *bar = new QHBoxLayout();
    QPushButton *open = newButton(QStringLiteral("开户"));
    open->setObjectName(QStringLiteral("primaryButton"));
    QPushButton *money = newButton(QStringLiteral("资金操作"));
    QPushButton *close = newButton(QStringLiteral("销户"));
    QPushButton *interest = newButton(QStringLiteral("结息"));
    m.bankFilterAccountEdit = newLine(QStringLiteral("账号 / 员工编号 / 姓名"));
    QPushButton *query = newButton(QStringLiteral("查询"));
    bar->addWidget(open);
    bar->addWidget(money);
    bar->addWidget(close);
    bar->addWidget(interest);
    bar->addStretch();
    bar->addWidget(m.bankFilterAccountEdit, 1);
    bar->addWidget(query);
    root->addLayout(bar);
    QTabWidget *views = new QTabWidget();
    m.accountTable = newTable({QStringLiteral("账号"), QStringLiteral("员工编号"), QStringLiteral("姓名"),
                              QStringLiteral("类型"), QStringLiteral("余额"), QStringLiteral("最低余额/额度"),
                              QStringLiteral("开户日期"), QStringLiteral("上次结息")});
    views->addTab(m.accountTable, QStringLiteral("账户列表"));
    QWidget *transactions = new QWidget();
    QVBoxLayout *txLayout = new QVBoxLayout(transactions);
    QHBoxLayout *dates = new QHBoxLayout();
    m.txBeginDateEdit = new QDateEdit(QDate::currentDate().addMonths(-1));
    m.txEndDateEdit = new QDateEdit(QDate::currentDate());
    for (QDateEdit *edit : {m.txBeginDateEdit, m.txEndDateEdit}) {
        edit->setCalendarPopup(true); edit->setDisplayFormat("yyyy-MM-dd");
    }
    dates->addWidget(new QLabel(QStringLiteral("日期范围")));
    dates->addWidget(m.txBeginDateEdit);
    dates->addWidget(new QLabel(QStringLiteral("至")));
    dates->addWidget(m.txEndDateEdit);
    QPushButton *txQuery = newButton(QStringLiteral("查询流水"));
    dates->addWidget(txQuery);
    dates->addStretch();
    txLayout->addLayout(dates);
    m.txTable = newTable({QStringLiteral("流水号"), QStringLiteral("账号"), QStringLiteral("类型"),
                         QStringLiteral("金额"), QStringLiteral("交易后余额"), QStringLiteral("日期"), QStringLiteral("备注")});
    txLayout->addWidget(m.txTable);
    views->addTab(transactions, QStringLiteral("交易流水"));
    root->addWidget(views, 1);
    connect(open, &QPushButton::clicked, m.bankOpenDialog, &QDialog::open);
    connect(money, &QPushButton::clicked, this, [this, moneyDialog]() {
        BankAccount *account = selectedAccount();
        if (account) {
            const QString no = QString::fromStdString(account->accountNo());
            m.bankDepositAccountEdit->setText(no);
            m.bankWithdrawAccountEdit->setText(no);
            m.bankFromEdit->setText(no);
        }
        moneyDialog->open();
    });
    connect(close, &QPushButton::clicked, this, &MainWindow::closeBankAccount);
    connect(interest, &QPushButton::clicked, this, &MainWindow::settleInterest);
    connect(query, &QPushButton::clicked, this, &MainWindow::filterBankTransactions);
    connect(txQuery, &QPushButton::clicked, this, &MainWindow::filterBankTransactions);
    connect(m.bankFilterAccountEdit, &QLineEdit::returnPressed, this, &MainWindow::filterBankTransactions);
    return page;
}

void MainWindow::refreshBankAccounts()
{
    if (!m.accountTable) {
        return;
    }
    const QString filter = m.bankFilterAccountEdit ? m.bankFilterAccountEdit->text().trimmed() : QString();
    m.accountTable->setRowCount(0);
    const std::vector<BankAccount *> &accounts = bank.getAccounts();
    for (size_t i = 0; i < accounts.size(); ++i) {
        BankAccount *a = accounts[i];
        if (!a) {
            continue;
        }
        const QString no = QString::fromStdString(a->accountNo());
        // 账号、员工编号、姓名任意一个包含筛选词都显示，支持按员工查询。
        if (!filter.isEmpty() &&
            !no.contains(filter, Qt::CaseInsensitive) &&
            !QString::fromStdString(a->ownerId()).contains(filter, Qt::CaseInsensitive) &&
            !QString::fromStdString(a->ownerName()).contains(filter, Qt::CaseInsensitive)) {
            continue;
        }
        const int row = m.accountTable->rowCount();
        m.accountTable->insertRow(row);
        m.accountTable->setItem(row, 0, new QTableWidgetItem(no));
        m.accountTable->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(a->ownerId())));
        m.accountTable->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(a->ownerName())));
        m.accountTable->setItem(row, 3, new QTableWidgetItem(QString::fromStdString(a->typeName())));
        m.accountTable->setItem(row, 4, new QTableWidgetItem(QString::fromStdString(moneyToString(a->balance()))));
        const QString extra = a->typeName() == "储蓄账户"
                ? QStringLiteral("最低 ") + QString::fromStdString(moneyToString(a->minBalance()))
                : QStringLiteral("额度 ") + QString::fromStdString(moneyToString(a->creditLimit()));
        m.accountTable->setItem(row, 5, new QTableWidgetItem(extra));
        m.accountTable->setItem(row, 6, new QTableWidgetItem(QString::fromStdString(a->openDate().toString())));
        m.accountTable->setItem(row, 7, new QTableWidgetItem(QString::fromStdString(a->lastInterestDate().toString())));
    }
}

void MainWindow::refreshBankTransactions()
{
    if (!m.txTable) {
        return;
    }
    const QString filter = m.bankFilterAccountEdit ? m.bankFilterAccountEdit->text().trimmed() : QString();
    Date begin;
    Date end;
    const bool hasBegin = readDate(m.txBeginDateEdit, begin);
    const bool hasEnd = readDate(m.txEndDateEdit, end);

    m.txTable->setRowCount(0);
    const std::vector<Transaction> &txs = bank.getTransactions();
    for (size_t i = 0; i < txs.size(); ++i) {
        const Transaction &t = txs[i];
        BankAccount *account = bank.findAccount(t.accountNo());
        QString accountText = QString::fromStdString(t.accountNo());
        if (account) {
            accountText += " " + QString::fromStdString(account->ownerId() + " " + account->ownerName());
        }
        if (!filter.isEmpty() && !accountText.contains(filter, Qt::CaseInsensitive)) {
            continue;
        }
        if (hasBegin && t.date().isBefore(begin)) {
            continue;
        }
        if (hasEnd && end.isBefore(t.date())) {
            continue;
        }
        const int row = m.txTable->rowCount();
        m.txTable->insertRow(row);
        m.txTable->setItem(row, 0, new QTableWidgetItem(QString::number(t.id())));
        m.txTable->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(t.accountNo())));
        m.txTable->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(t.typeName())));
        m.txTable->setItem(row, 3, new QTableWidgetItem(QString::fromStdString(moneyToString(t.amount()))));
        m.txTable->setItem(row, 4, new QTableWidgetItem(QString::fromStdString(moneyToString(t.balanceAfter()))));
        m.txTable->setItem(row, 5, new QTableWidgetItem(QString::fromStdString(t.date().toString())));
        m.txTable->setItem(row, 6, new QTableWidgetItem(QString::fromStdString(t.note())));
    }
}

void MainWindow::openBankAccount()
{
    const QString no = m.bankOpenNoEdit->text().trimmed();
    if (no.isEmpty()) {
        showError(QStringLiteral("请输入账号。"));
        return;
    }
    if (m.bankOpenOwnerCombo->count() == 0) {
        showError(QStringLiteral("还没有员工，请先在动物园管理中添加动物。"));
        return;
    }
    const std::string ownerId = m.bankOpenOwnerCombo->currentData().toString().toStdString();
    animal *ownerAnimal = animalManager.findbyid(ownerId);
    if (!ownerAnimal) {
        showError(QStringLiteral("员工不存在。"));
        return;
    }
    Money initial = 0;
    Money extra = 0;
    QString error;
    if (!readMoney(m.bankOpenInitialEdit, initial, error)) {
        showError(QStringLiteral("首次存款：") + error);
        return;
    }
    if (!readMoney(m.bankOpenExtraEdit, extra, error)) {
        showError(QStringLiteral("额外参数：") + error);
        return;
    }
    const int type = m.bankOpenTypeCombo->currentData().toInt();
    std::string message;
    if (!bank.openAccount(no.toStdString(), *ownerAnimal, type, initial, extra, message)) {
        showError(QString::fromStdString(message));
        return;
    }

    // 开户成功后把账号绑定到员工，工资/食物购买才能找到账户。
    ownerAnimal->setBankAccountNo(no.toStdString());
    try {
        animalManager.save();
    } catch (...) {
    }
    refreshAll();
    showInfo(QString::fromStdString(message));
    closeOperationDialog();
}

void MainWindow::closeBankAccount()
{
    BankAccount *a = selectedAccount();
    if (!a) {
        showError(QStringLiteral("请先在账户表中选择一个账户。"));
        return;
    }
    const QString no = QString::fromStdString(a->accountNo());
    const std::string ownerId = a->ownerId();
    if (QMessageBox::question(this, QStringLiteral("确认销户"),
                              QStringLiteral("确定注销账户 ") + no + QStringLiteral(" 吗？")) != QMessageBox::Yes) {
        return;
    }
    std::string message;
    if (!bank.closeAccount(no.toStdString(), message)) {
        showError(QString::fromStdString(message));
        return;
    }
    // 解绑员工账号，防止工资/购买继续指向已销户账户。
    if (animal *owner = animalManager.findbyid(ownerId)) {
        if (owner->bankAccountNo() == no.toStdString()) {
            owner->setBankAccountNo("");
            try {
                animalManager.save();
            } catch (...) {
            }
        }
    }
    refreshAll();
    showInfo(QString::fromStdString(message));
}

void MainWindow::depositMoney()
{
    Money amount = 0;
    QString error;
    if (!readMoney(m.bankDepositAmountEdit, amount, error)) {
        showError(QStringLiteral("存款金额：") + error);
        return;
    }
    std::string message;
    if (!bank.deposit(m.bankDepositAccountEdit->text().trimmed().toStdString(), amount, message)) {
        showError(QString::fromStdString(message));
        return;
    }
    refreshAll();
    showInfo(QString::fromStdString(message));
    closeOperationDialog();
}

void MainWindow::withdrawMoney()
{
    Money amount = 0;
    QString error;
    if (!readMoney(m.bankWithdrawAmountEdit, amount, error)) {
        showError(QStringLiteral("取款金额：") + error);
        return;
    }
    std::string message;
    if (!bank.withdraw(m.bankWithdrawAccountEdit->text().trimmed().toStdString(), amount, message)) {
        showError(QString::fromStdString(message));
        return;
    }
    refreshAll();
    showInfo(QString::fromStdString(message));
    closeOperationDialog();
}

void MainWindow::transferMoney()
{
    Money amount = 0;
    QString error;
    if (!readMoney(m.bankTransferAmountEdit, amount, error)) {
        showError(QStringLiteral("转账金额：") + error);
        return;
    }
    std::string message;
    if (!bank.transfer(m.bankFromEdit->text().trimmed().toStdString(),
                       m.bankToEdit->text().trimmed().toStdString(), amount, message)) {
        showError(QString::fromStdString(message));
        return;
    }
    refreshAll();
    showInfo(QString::fromStdString(message));
    closeOperationDialog();
}

void MainWindow::settleInterest()
{
    std::string message;
    if (!bank.settleInterest(message)) {
        showError(QString::fromStdString(message));
        return;
    }
    refreshAll();
    showInfo(QString::fromStdString(message));
}

void MainWindow::filterBankTransactions()
{
    refreshBankAccounts();
    refreshBankTransactions();
}

// 考勤工资页面中的工资单操作。

void MainWindow::refreshPayrollTable()
{
    if (!m.payTable) {
        return;
    }
    m.payTable->setRowCount(0);
    const std::vector<PayrollRecord> &records = payroll.getPayrollRecords();
    for (size_t i = 0; i < records.size(); ++i) {
        const PayrollRecord &r = records[i];
        const std::string id = m.payEmployeeCombo->currentData().toString().toStdString();
        if (r.year != m.payYearSpin->value() || r.month != m.payMonthSpin->value() ||
            (!id.empty() && r.employeeId != id)) {
            continue;
        }

        const int row = m.payTable->rowCount();
        m.payTable->insertRow(row);
        m.payTable->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(r.payrollId)));
        m.payTable->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(r.employeeId)));
        m.payTable->setItem(row, 2, new QTableWidgetItem(QStringLiteral("%1-%2").arg(r.year).arg(r.month, 2, 10, QLatin1Char('0'))));
        m.payTable->setItem(row, 3, new QTableWidgetItem(QString::fromStdString(moneyToString(r.basicSalary))));
        m.payTable->setItem(row, 4, new QTableWidgetItem(QString::fromStdString(moneyToString(r.absentDeduction))));
        m.payTable->setItem(row, 5, new QTableWidgetItem(QString::fromStdString(moneyToString(r.leaveDeduction))));
        m.payTable->setItem(row, 6, new QTableWidgetItem(QString::fromStdString(moneyToString(r.bonus))));
        m.payTable->setItem(row, 7, new QTableWidgetItem(QString::fromStdString(moneyToString(r.finalsalary))));
        m.payTable->setItem(row, 8, new QTableWidgetItem(QString::fromStdString(r.status)));
        m.payTable->setItem(row, 9, new QTableWidgetItem(QString::number(r.bankTransactionId)));
    }
}

void MainWindow::generatePayroll()
{
    if (!m.payEmployeeCombo || m.payEmployeeCombo->count() == 0) {
        showError(QStringLiteral("还没有员工。"));
        return;
    }
    const std::string id = m.payEmployeeCombo->currentData().toString().toStdString();
    if (id.empty()) {
        showError(QStringLiteral("请先选择一名员工。"));
        return;
    }
    animal *a = animalManager.findbyid(id);
    if (!a) {
        showError(QStringLiteral("员工不存在。"));
        return;
    }
    Money absent = 0;
    Money leave = 0;
    Money bonus = 0;
    QString error;
    if (!readMoney(m.payAbsentEdit, absent, error) ||
        !readMoney(m.payLeaveEdit, leave, error) ||
        !readMoney(m.payBonusEdit, bonus, error)) {
        showError(QStringLiteral("扣款/奖金：") + error);
        return;
    }
    const int year = m.payYearSpin->value();
    const int month = m.payMonthSpin->value();
    if (QDate::currentDate() < QDate(year, month, 1).addMonths(1)) {
        showError(QStringLiteral("该月份尚未结束，请在月份结束后结算工资。"));
        return;
    }
    if (!initializeAttendance(year, month)) {
        return;
    }
    const std::vector<Date> dates = buildScheduledDates(year, month);
    refreshAttendanceTable();

    AttendanceSummary summary;
    std::string message;
    if (!attendance.summarize(id, year, month, dates, summary, message)) {
        m.paySummaryLabel->setText(QStringLiteral("考勤汇总失败：") + QString::fromStdString(message));
        return;
    }

    SalaryPolicy policy(absent, leave, bonus);
    PayrollRecord record;
    if (!policy.calculate(*a, summary, record, message)) {
        m.paySummaryLabel->setText(QStringLiteral("工资计算失败：") + QString::fromStdString(message));
        return;
    }
    if (!payroll.addPayroll(record, message)) {
        m.paySummaryLabel->setText(QString::fromStdString(message));
        return;
    }
    payroll.savePayrolls(kPayrollFile);
    refreshPayrollTable();
    m.paySummaryLabel->setText(
        QStringLiteral("已生成：%1，实发 %2 元。考勤：应出勤 %3，出勤 %4，缺勤 %5，请假 %6，未录入 %7")
            .arg(QString::fromStdString(record.payrollId))
            .arg(QString::fromStdString(moneyToString(record.finalsalary)))
            .arg(summary.scheduledDays).arg(summary.presentDays)
            .arg(summary.absentDays).arg(summary.leaveDays).arg(summary.missingDays));
    closeOperationDialog();
}

void MainWindow::paySelectedPayroll()
{
    if (!m.payTable || m.payTable->currentRow() < 0) {
        showError(QStringLiteral("请先选择一张工资单。"));
        return;
    }
    const int row = m.payTable->currentRow();
    const std::string payrollId = m.payTable->item(row, 0)->text().toStdString();
    const std::vector<PayrollRecord> &records = payroll.getPayrollRecords();
    int index = -1;
    for (int i = 0; i < (int)records.size(); ++i) {
        if (records[i].payrollId == payrollId) {
            index = i;
            break;
        }
    }
    if (index == -1) {
        showError(QStringLiteral("未找到对应的工资单。"));
        return;
    }
    PayrollRecord record = records[index];
    if (QDate::currentDate() < QDate(record.year, record.month, 1).addMonths(1)) {
        showError(QStringLiteral("该月份尚未结束，不能发放整月工资。"));
        return;
    }
    if (record.status == "已发放") {
        showError(QStringLiteral("该工资单已发放。"));
        return;
    }
    std::string message;
    if (!payroll.paySalary(bank, record, message)) {
        showError(QString::fromStdString(message));
        return;
    }
    payroll.savePayrolls(kPayrollFile);
    refreshAll();
    showInfo(QString::fromStdString(message));
}

// 食物管理标签页
QWidget *MainWindow::createFoodTab()
{
    QWidget *page = new QWidget();
    QVBoxLayout *root = new QVBoxLayout(page);
    root->setContentsMargins(12, 12, 12, 12);
    root->setSpacing(10);
    QDialog *addDialog = newDialog(QStringLiteral("添加食物"));
    QFormLayout *form = new QFormLayout(addDialog);
    m.foodIdEdit = newLine(QStringLiteral("如 F001"));
    m.foodNameEdit = newLine(QStringLiteral("食物名称"));
    m.foodPriceEdit = newLine(QStringLiteral("单价，元/kg"));
    form->addRow(QStringLiteral("编号"), m.foodIdEdit);
    form->addRow(QStringLiteral("名称"), m.foodNameEdit);
    form->addRow(QStringLiteral("单价"), m.foodPriceEdit);
    connect(dialogAction(addDialog, form, QStringLiteral("添加食物")), &QPushButton::clicked, this, &MainWindow::addFood);
    QHBoxLayout *bar = new QHBoxLayout();
    QPushButton *add = newButton(QStringLiteral("添加食物"));
    add->setObjectName(QStringLiteral("primaryButton"));
    QPushButton *refresh = newButton(QStringLiteral("刷新"));
    bar->addWidget(add);
    bar->addWidget(refresh);
    bar->addStretch();
    bar->addWidget(newHint(QStringLiteral("员工自行购买，管理者维护目录和查询记录。")));
    root->addLayout(bar);
    QTabWidget *views = new QTabWidget();
    m.foodTable = newTable({QStringLiteral("编号"), QStringLiteral("名称"), QStringLiteral("单价 元/kg")});
    views->addTab(m.foodTable, QStringLiteral("食物目录"));
    QWidget *history = new QWidget();
    QVBoxLayout *historyLayout = new QVBoxLayout(history);
    QHBoxLayout *queryBar = new QHBoxLayout();
    m.purchaseFilterEdit = newLine(QStringLiteral("员工编号 / 食物 / 日期 / 银行流水号，可空"));
    QPushButton *query = newButton(QStringLiteral("查询购买记录"));
    queryBar->addWidget(m.purchaseFilterEdit, 1);
    queryBar->addWidget(query);
    historyLayout->addLayout(queryBar);
    m.purchaseTable = newTable({QStringLiteral("购买单"), QStringLiteral("员工"), QStringLiteral("食物"),
                               QStringLiteral("数量 kg"), QStringLiteral("金额"), QStringLiteral("账号"),
                               QStringLiteral("日期"), QStringLiteral("状态"), QStringLiteral("银行流水")});
    historyLayout->addWidget(m.purchaseTable);
    views->addTab(history, QStringLiteral("购买记录"));
    root->addWidget(views, 1);
    connect(add, &QPushButton::clicked, addDialog, &QDialog::open);
    connect(query, &QPushButton::clicked, this, &MainWindow::refreshPurchaseTable);
    connect(m.purchaseFilterEdit, &QLineEdit::returnPressed, this, &MainWindow::refreshPurchaseTable);
    connect(refresh, &QPushButton::clicked, this, [this]() { refreshFoodTable(); refreshPurchaseTable(); });
    return page;
}

void MainWindow::refreshFoodTable()
{
    if (!m.foodTable) {
        return;
    }
    m.foodTable->setRowCount(0);
    const std::vector<Food> &foods = foodService.getFoods();
    for (size_t i = 0; i < foods.size(); ++i) {
        const int row = m.foodTable->rowCount();
        m.foodTable->insertRow(row);
        m.foodTable->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(foods[i].getFoodId())));
        m.foodTable->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(foods[i].getFoodName())));
        m.foodTable->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(moneyToString(foods[i].getPricePerKg()))));
    }
}

void MainWindow::refreshPurchaseTable()
{
    if (!m.purchaseTable) {
        return;
    }
    const QString filter = m.purchaseFilterEdit ? m.purchaseFilterEdit->text().trimmed() : QString();
    m.purchaseTable->setRowCount(0);
    const std::vector<FoodPurchaseRecord> &records = foodService.getPurchaseRecords();
    for (size_t i = 0; i < records.size(); ++i) {
        const FoodPurchaseRecord &r = records[i];
        const QString text = QString::fromStdString(r.purchaseId + " " + r.employeeId + " " +
                                                   r.foodId + " " + r.foodName + " " +
                                                   r.accountNo + " " + r.date) +
                             " " + QString::number(r.bankTransactionId);
        if (!filter.isEmpty() && !text.contains(filter, Qt::CaseInsensitive)) {
            continue;
        }
        const int row = m.purchaseTable->rowCount();
        m.purchaseTable->insertRow(row);
        m.purchaseTable->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(r.purchaseId)));
        m.purchaseTable->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(r.employeeId)));
        m.purchaseTable->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(r.foodName)));
        m.purchaseTable->setItem(row, 3, new QTableWidgetItem(QString::number(r.quantity)));
        m.purchaseTable->setItem(row, 4, new QTableWidgetItem(QString::fromStdString(moneyToString(r.amount))));
        m.purchaseTable->setItem(row, 5, new QTableWidgetItem(QString::fromStdString(r.accountNo)));
        m.purchaseTable->setItem(row, 6, new QTableWidgetItem(QString::fromStdString(r.date)));
        m.purchaseTable->setItem(row, 7, new QTableWidgetItem(QString::fromStdString(r.status)));
        m.purchaseTable->setItem(row, 8, new QTableWidgetItem(QString::number(r.bankTransactionId)));
    }
}

void MainWindow::addFood()
{
    const QString id = m.foodIdEdit->text().trimmed();
    const QString name = m.foodNameEdit->text().trimmed();
    if (id.isEmpty() || name.isEmpty()) {
        showError(QStringLiteral("食物编号和名称不能为空。"));
        return;
    }
    Money price = 0;
    QString error;
    if (!readMoney(m.foodPriceEdit, price, error) || price <= 0) {
        showError(QStringLiteral("单价必须是大于 0 的金额。"));
        return;
    }
    std::string message;
    if (!foodService.addFood(Food(id.toStdString(), name.toStdString(), price), message)) {
        showError(QString::fromStdString(message));
        return;
    }
    foodService.saveFoods(kFoodFile);
    fillFoodCombos();
    refreshFoodTable();
    showStatus(QStringLiteral("食物添加成功。"));
    closeOperationDialog();
}
