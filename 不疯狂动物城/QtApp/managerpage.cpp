// ============================================================================
// managerpage.cpp
//   管理者页面：动物园、考勤、银行、工资、食物五个标签页。
//   每个标签页都由“输入区 + 操作按钮 + 表格/文本区”组成，只使用基本控件。
//   业务调用集中在按钮槽函数里，刷新集中在 refreshXxx()，便于对照源码阅读。
// ============================================================================
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "sqliteexporter.h"
#include "../src/payroll/SalaryPolicy.h"

#include <QCheckBox>
#include <QComboBox>
#include <QDateEdit>
#include <QDir>
#include <QFileDialog>
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

// ============================================================================
// [界面] 管理者页面外壳：欢迎行 + 保存/退出 + 五个标签页
// ============================================================================
void MainWindow::setupManagerPage()
{
    QVBoxLayout *root = new QVBoxLayout(ui->managerPage);
    root->setContentsMargins(10, 10, 10, 10);
    root->setSpacing(8);

    QHBoxLayout *top = new QHBoxLayout();
    m.welcomeLabel = new QLabel(QStringLiteral("管理者页面"));
    m.welcomeLabel->setStyleSheet(QStringLiteral("font-size:16px;font-weight:bold;color:#1F3864;"));
    QPushButton *exportButton = newButton(QStringLiteral("导出 SQLite"));
    m.saveAllButton = newButton(QStringLiteral("保存全部数据"));
    m.logoutButton = newButton(QStringLiteral("退出登录"));
    top->addWidget(m.welcomeLabel);
    top->addStretch();
    top->addWidget(exportButton);
    top->addWidget(m.saveAllButton);
    top->addWidget(m.logoutButton);
    root->addLayout(top);

    m.tabs = new QTabWidget();
    m.tabs->addTab(createZooTab(), QStringLiteral("动物园管理"));
    m.tabs->addTab(createAttendanceTab(), QStringLiteral("考勤管理"));
    m.tabs->addTab(createBankTab(), QStringLiteral("银行管理"));
    m.tabs->addTab(createPayrollTab(), QStringLiteral("工资管理"));
    m.tabs->addTab(createFoodTab(), QStringLiteral("食物管理"));
    root->addWidget(m.tabs, 1);

    // [功能] 顶部按钮只做两件事：整体保存、退出登录。
    connect(exportButton, &QPushButton::clicked, this, &MainWindow::exportSqlite);
    connect(m.saveAllButton, &QPushButton::clicked, this, [this]() { saveAllData(true); });
    connect(m.logoutButton, &QPushButton::clicked, this, &MainWindow::logout);
}

// ============================================================================
// [功能] SQLite 数据导出
//   把所有模块的当前内存数据写成一个 .db 文件；不修改内存数据。
//   表结构见 sqliteexporter.cpp：employees/attendance/bank_accounts/...
// ============================================================================
void MainWindow::exportSqlite()
{
    const QString defaultPath = QDir::currentPath() + QStringLiteral("/不疯狂动物城导出.db");
    const QString filePath = QFileDialog::getSaveFileName(
        this, QStringLiteral("导出 SQLite 数据库"), defaultPath,
        QStringLiteral("SQLite 数据库 (*.db *.sqlite);;所有文件 (*)"));
    if (filePath.trimmed().isEmpty()) {
        return;
    }

    QString error;
    if (!SqliteExporter::exportAll(filePath, animalManager, attendance, bank, payroll, foodService, error)) {
        showError(QStringLiteral("SQLite 导出失败：\n") + error);
        return;
    }
    showInfo(QStringLiteral("SQLite 导出成功：\n") + filePath +
             QStringLiteral("\n\n已导出表：employees、feed_records、health_records、attendance、"
                            "bank_accounts、transactions、payrolls、foods、food_purchases、export_info。"));
    showStatus(QStringLiteral("SQLite 已导出：") + filePath);
}

// ============================================================================
// [界面] 动物园管理标签页
// ============================================================================
QWidget *MainWindow::createZooTab()
{
    QWidget *page = new QWidget();
    QVBoxLayout *root = new QVBoxLayout(page);
    root->setContentsMargins(8, 8, 8, 8);
    root->setSpacing(6);

    QHBoxLayout *top = new QHBoxLayout();

    // ---- 添加动物 ----
    QGroupBox *addGroup = newGroup(QStringLiteral("添加动物"));
    QFormLayout *addForm = new QFormLayout(addGroup);
    m.zooTypeCombo = new QComboBox();
    m.zooTypeCombo->addItem(QStringLiteral("老鹰"), QStringLiteral("eagle"));
    m.zooTypeCombo->addItem(QStringLiteral("鹦鹉"), QStringLiteral("parrot"));
    m.zooTypeCombo->addItem(QStringLiteral("狮子"), QStringLiteral("lion"));
    m.zooTypeCombo->addItem(QStringLiteral("大象"), QStringLiteral("elephant"));
    m.zooIdEdit = newLine(QStringLiteral("如 A001"));
    m.zooNameEdit = newLine(QStringLiteral("动物姓名"));
    m.zooWingEdit = newLine(QStringLiteral("翼展 cm，仅鸟类"));
    QPushButton *addAnimalButton = newButton(QStringLiteral("添加动物"));
    addForm->addRow(QStringLiteral("类型"), m.zooTypeCombo);
    addForm->addRow(QStringLiteral("编号"), m.zooIdEdit);
    addForm->addRow(QStringLiteral("姓名"), m.zooNameEdit);
    addForm->addRow(QStringLiteral("翼展"), m.zooWingEdit);
    addForm->addRow(addAnimalButton);
    connect(addAnimalButton, &QPushButton::clicked, this, &MainWindow::addAnimal);

    // 鸟类才需要翼展；狮子/大象固定食性，不需要额外输入。
    connect(m.zooTypeCombo, &QComboBox::currentTextChanged, this, [this](const QString &) {
        const QString type = m.zooTypeCombo->currentData().toString();
        m.zooWingEdit->setEnabled(type == QStringLiteral("eagle") || type == QStringLiteral("parrot"));
        if (!m.zooWingEdit->isEnabled()) {
            m.zooWingEdit->clear();
        }
    });

    // ---- 选中动物的员工资料 ----
    QGroupBox *infoGroup = newGroup(QStringLiteral("选中动物资料"));
    QFormLayout *infoForm = new QFormLayout(infoGroup);
    m.zooSelectedLabel = new QLabel(QStringLiteral("未选择"));
    m.zooSelectedLabel->setStyleSheet(QStringLiteral("font-weight:bold;color:#2E5C8A;"));
    m.zooSalaryEdit = newLine(QStringLiteral("基本工资，单位元"));
    m.zooAccountEdit = newLine(QStringLiteral("工资/消费账号"));
    m.zooNewNameEdit = newLine(QStringLiteral("新姓名"));
    QHBoxLayout *infoButtons = new QHBoxLayout();
    QPushButton *saveInfoButton = newButton(QStringLiteral("保存工资/账号"));
    QPushButton *renameButton = newButton(QStringLiteral("改名"));
    QPushButton *deleteButton = newButton(QStringLiteral("删除动物"));
    infoButtons->addWidget(saveInfoButton);
    infoButtons->addWidget(renameButton);
    infoButtons->addWidget(deleteButton);
    infoForm->addRow(QStringLiteral("当前"), m.zooSelectedLabel);
    infoForm->addRow(QStringLiteral("基本工资"), m.zooSalaryEdit);
    infoForm->addRow(QStringLiteral("银行账号"), m.zooAccountEdit);
    infoForm->addRow(QStringLiteral("新姓名"), m.zooNewNameEdit);
    infoForm->addRow(infoButtons);
    connect(saveInfoButton, &QPushButton::clicked, this, &MainWindow::updateAnimalInfo);
    connect(renameButton, &QPushButton::clicked, this, &MainWindow::renameAnimal);
    connect(deleteButton, &QPushButton::clicked, this, &MainWindow::deleteAnimal);

    // ---- 查询 / 保存 ----
    QGroupBox *queryGroup = newGroup(QStringLiteral("查询 / 保存"));
    QFormLayout *queryForm = new QFormLayout(queryGroup);
    m.zooFilterCategory = new QComboBox();
    m.zooFilterCategory->addItem(QStringLiteral("全部类别"), QString());
    m.zooFilterCategory->addItem(QStringLiteral("鸟类"), QStringLiteral("鸟类"));
    m.zooFilterCategory->addItem(QStringLiteral("哺乳动物"), QStringLiteral("哺乳动物"));
    m.zooFilterId = newLine(QStringLiteral("按编号筛选，可空"));
    QHBoxLayout *queryButtons = new QHBoxLayout();
    QPushButton *refreshButton = newButton(QStringLiteral("刷新"));
    QPushButton *saveButton = newButton(QStringLiteral("保存动物园"));
    QPushButton *loadButton = newButton(QStringLiteral("加载动物园"));
    queryButtons->addWidget(refreshButton);
    queryButtons->addWidget(saveButton);
    queryButtons->addWidget(loadButton);
    queryForm->addRow(QStringLiteral("类别"), m.zooFilterCategory);
    queryForm->addRow(QStringLiteral("编号"), m.zooFilterId);
    queryForm->addRow(queryButtons);
    connect(m.zooFilterCategory, &QComboBox::currentTextChanged, this, [this](const QString &) { refreshZooTable(); });
    connect(m.zooFilterId, &QLineEdit::returnPressed, this, &MainWindow::refreshZooTable);
    connect(refreshButton, &QPushButton::clicked, this, &MainWindow::refreshZooTable);
    connect(saveButton, &QPushButton::clicked, this, [this]() {
        try {
            animalManager.save();
            showInfo(QStringLiteral("动物园数据已保存（animals.txt / records.txt）。"));
        } catch (...) {
            showError(QStringLiteral("动物园数据保存失败。"));
        }
    });
    connect(loadButton, &QPushButton::clicked, this, [this]() {
        try {
            animalManager.clear();
            animalManager.load();
            refreshAll();
            showInfo(QStringLiteral("动物园数据已加载。"));
        } catch (...) {
            showError(QStringLiteral("动物园数据加载失败，请检查文件格式。"));
        }
    });

    top->addWidget(addGroup, 1);
    top->addWidget(infoGroup, 2);
    top->addWidget(queryGroup, 1);
    root->addLayout(top);

    // ---- 动物表格 ----
    m.zooTable = newTable(QStringList() << QStringLiteral("编号") << QStringLiteral("姓名")
                                        << QStringLiteral("类别") << QStringLiteral("物种")
                                        << QStringLiteral("翼展/食性") << QStringLiteral("基本工资")
                                        << QStringLiteral("银行账号"));
    root->addWidget(m.zooTable, 2);
    connect(m.zooTable, &QTableWidget::itemSelectionChanged, this, &MainWindow::onZooSelectionChanged);

    // ---- 饲养 / 健康记录 ----
    QGroupBox *recordGroup = newGroup(QStringLiteral("饲养 / 健康记录"));
    QGridLayout *recordGrid = new QGridLayout(recordGroup);
    m.feedAnimalEdit = newLine(QStringLiteral("动物编号"));
    m.feedDateEdit = new QDateEdit(QDate::currentDate());
    m.feedDateEdit->setCalendarPopup(true);
    m.feedDateEdit->setDisplayFormat(QStringLiteral("yyyy-MM-dd"));
    m.feedAmountEdit = newLine(QStringLiteral("数量 kg"));
    QPushButton *feedButton = newButton(QStringLiteral("添加饲养记录"));
    m.healthAnimalEdit = newLine(QStringLiteral("动物编号"));
    m.healthDateEdit = new QDateEdit(QDate::currentDate());
    m.healthDateEdit->setCalendarPopup(true);
    m.healthDateEdit->setDisplayFormat(QStringLiteral("yyyy-MM-dd"));
    m.healthStatusCombo = new QComboBox();
    m.healthStatusCombo->addItems(QStringList() << QStringLiteral("健康") << QStringLiteral("亚健康") << QStringLiteral("生病"));
    m.healthNoteEdit = newLine(QStringLiteral("备注"));
    QPushButton *healthButton = newButton(QStringLiteral("添加健康记录"));
    QPushButton *recordButton = newButton(QStringLiteral("查看选中动物记录"));

    recordGrid->addWidget(new QLabel(QStringLiteral("饲养：动物编号")), 0, 0);
    recordGrid->addWidget(m.feedAnimalEdit, 0, 1);
    recordGrid->addWidget(new QLabel(QStringLiteral("日期")), 0, 2);
    recordGrid->addWidget(m.feedDateEdit, 0, 3);
    recordGrid->addWidget(new QLabel(QStringLiteral("数量 kg")), 0, 4);
    recordGrid->addWidget(m.feedAmountEdit, 0, 5);
    recordGrid->addWidget(feedButton, 0, 6);
    recordGrid->addWidget(new QLabel(QStringLiteral("健康：动物编号")), 1, 0);
    recordGrid->addWidget(m.healthAnimalEdit, 1, 1);
    recordGrid->addWidget(new QLabel(QStringLiteral("日期")), 1, 2);
    recordGrid->addWidget(m.healthDateEdit, 1, 3);
    recordGrid->addWidget(m.healthStatusCombo, 1, 4);
    recordGrid->addWidget(m.healthNoteEdit, 1, 5);
    recordGrid->addWidget(healthButton, 1, 6);
    recordGrid->addWidget(recordButton, 2, 6);
    m.recordView = new QTextEdit();
    m.recordView->setReadOnly(true);
    m.recordView->setMaximumHeight(120);
    recordGrid->addWidget(m.recordView, 2, 0, 1, 6);
    root->addWidget(recordGroup);

    m.zooStatusLabel = newHint(QStringLiteral("提示：删除动物会直接删除对象；有历史业务关联时请谨慎。"));
    root->addWidget(m.zooStatusLabel);

    connect(feedButton, &QPushButton::clicked, this, &MainWindow::addFeedRecord);
    connect(healthButton, &QPushButton::clicked, this, &MainWindow::addHealthRecord);
    connect(recordButton, &QPushButton::clicked, this, &MainWindow::showAnimalRecords);

    return page;
}

// ---------------------------------------------------------------------------
// [刷新] 动物园表格：按类别和编号筛选；专属列用 dynamic_cast 读取翼展/食性。
// ---------------------------------------------------------------------------
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
        if (!idFilter.isEmpty() && !id.contains(idFilter, Qt::CaseInsensitive)) {
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
    m.zooSalaryEdit->setText(QString::fromStdString(moneyToString(a->basicSalary())));
    m.zooAccountEdit->setText(QString::fromStdString(a->bankAccountNo()));
    m.feedAnimalEdit->setText(QString::fromStdString(a->getid()));
    m.healthAnimalEdit->setText(QString::fromStdString(a->getid()));
}

// ---------------------------------------------------------------------------
// [功能] 动物园：添加、保存资料、改名、删除、饲养、健康、查看记录
// ---------------------------------------------------------------------------
void MainWindow::addAnimal()
{
    const QString id = m.zooIdEdit->text().trimmed();
    const QString name = m.zooNameEdit->text().trimmed();
    const QString type = m.zooTypeCombo->currentData().toString();

    // [校验] 编号唯一、姓名非空；鸟类翼展必须为正数。
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

    animalManager.addanimal(a);   // 所有权移交 zoo
    try {
        animalManager.save();
    } catch (...) {
    }
    refreshAll();
    selectZooRow(id.toStdString());
    m.zooIdEdit->clear();
    m.zooNameEdit->clear();
    m.zooWingEdit->clear();
    showStatus(QStringLiteral("动物添加成功：") + id);
}

void MainWindow::updateAnimalInfo()
{
    animal *a = selectedAnimal();
    if (!a) {
        showError(QStringLiteral("请先在表格中选择一只动物。"));
        return;
    }
    Money salary = 0;
    QString error;
    if (!readMoney(m.zooSalaryEdit, salary, error)) {
        showError(QStringLiteral("基本工资：") + error);
        return;
    }
    a->setBasicSalary(salary);
    a->setBankAccountNo(m.zooAccountEdit->text().trimmed().toStdString());
    try {
        animalManager.save();
    } catch (...) {
    }
    refreshAll();
    showStatus(QStringLiteral("工资/账号已保存。"));
}

void MainWindow::renameAnimal()
{
    animal *a = selectedAnimal();
    if (!a) {
        showError(QStringLiteral("请先在表格中选择一只动物。"));
        return;
    }
    const QString newName = m.zooNewNameEdit->text().trimmed();
    if (newName.isEmpty()) {
        showError(QStringLiteral("请输入新姓名。"));
        return;
    }
    if (!animalManager.updateanimal(a->getid(), newName.toStdString())) {
        showError(QStringLiteral("改名失败。"));
        return;
    }
    try {
        animalManager.save();
    } catch (...) {
    }
    refreshAll();
    showStatus(QStringLiteral("改名成功。"));
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

// ============================================================================
// [界面] 考勤管理标签页
// ============================================================================
QWidget *MainWindow::createAttendanceTab()
{
    QWidget *page = new QWidget();
    QVBoxLayout *root = new QVBoxLayout(page);
    root->setContentsMargins(8, 8, 8, 8);
    root->setSpacing(6);

    QHBoxLayout *top = new QHBoxLayout();

    // [界面] 管理者只做查询和修改；签到、请假由员工页面发起。
    QGroupBox *editGroup = newGroup(QStringLiteral("查询 / 修改"));
    QFormLayout *editForm = new QFormLayout(editGroup);
    m.attNewStatusCombo = new QComboBox();
    m.attNewStatusCombo->addItem(QStringLiteral("缺勤"), 0);
    m.attNewStatusCombo->addItem(QStringLiteral("出勤"), 1);
    m.attNewStatusCombo->addItem(QStringLiteral("请假"), 2);
    QHBoxLayout *modifyButtons = new QHBoxLayout();
    QPushButton *changeButton = newButton(QStringLiteral("修改选中状态"));
    QPushButton *deleteButton = newButton(QStringLiteral("删除选中记录"));
    modifyButtons->addWidget(changeButton);
    modifyButtons->addWidget(deleteButton);
    editForm->addRow(newHint(QStringLiteral("先在下方表格选中一条记录，再修改状态或删除。"
                                             "签到、请假由员工在“我的考勤”中操作。")));
    editForm->addRow(QStringLiteral("改为"), m.attNewStatusCombo);
    editForm->addRow(modifyButtons);
    connect(changeButton, &QPushButton::clicked, this, &MainWindow::changeAttendance);
    connect(deleteButton, &QPushButton::clicked, this, &MainWindow::deleteAttendance);

    QGroupBox *sumGroup = newGroup(QStringLiteral("月度统计"));
    QFormLayout *sumForm = new QFormLayout(sumGroup);
    m.attSumEmployeeCombo = new QComboBox();
    m.attYearSpin = new QSpinBox();
    m.attYearSpin->setRange(2000, 2100);
    m.attYearSpin->setValue(QDate::currentDate().year());
    m.attMonthSpin = new QSpinBox();
    m.attMonthSpin->setRange(1, 12);
    m.attMonthSpin->setValue(QDate::currentDate().month());
    m.attWorkdayCheck = new QCheckBox(QStringLiteral("应出勤只算周一至周五"));
    m.attWorkdayCheck->setChecked(true);
    QPushButton *sumButton = newButton(QStringLiteral("统计"));
    m.attSummaryLabel = new QLabel(QStringLiteral("尚未统计"));
    m.attSummaryLabel->setWordWrap(true);
    m.attSummaryLabel->setStyleSheet(QStringLiteral("color:#2E5C8A;"));
    sumForm->addRow(QStringLiteral("员工"), m.attSumEmployeeCombo);
    QHBoxLayout *ym = new QHBoxLayout();
    ym->addWidget(m.attYearSpin);
    ym->addWidget(new QLabel(QStringLiteral("年")));
    ym->addWidget(m.attMonthSpin);
    ym->addWidget(new QLabel(QStringLiteral("月")));
    sumForm->addRow(QStringLiteral("年月"), ym);
    sumForm->addRow(m.attWorkdayCheck);
    sumForm->addRow(sumButton);
    sumForm->addRow(m.attSummaryLabel);
    connect(sumButton, &QPushButton::clicked, this, &MainWindow::summarizeAttendance);

    QGroupBox *fileGroup = newGroup(QStringLiteral("查询 / 文件"));
    QVBoxLayout *fileLayout = new QVBoxLayout(fileGroup);
    m.attFilterEdit = newLine(QStringLiteral("日期或员工编号，可空"));
    m.attFilterStatus = new QComboBox();
    m.attFilterStatus->addItem(QStringLiteral("全部状态"), -1);
    m.attFilterStatus->addItem(QStringLiteral("缺勤"), 0);
    m.attFilterStatus->addItem(QStringLiteral("出勤"), 1);
    m.attFilterStatus->addItem(QStringLiteral("请假"), 2);
    QPushButton *filterButton = newButton(QStringLiteral("筛选表格"));
    QPushButton *saveButton = newButton(QStringLiteral("保存考勤"));
    QPushButton *loadButton = newButton(QStringLiteral("加载考勤"));
    fileLayout->addWidget(m.attFilterEdit);
    fileLayout->addWidget(m.attFilterStatus);
    fileLayout->addWidget(filterButton);
    fileLayout->addWidget(saveButton);
    fileLayout->addWidget(loadButton);
    fileLayout->addWidget(newHint(QStringLiteral("文件：attendance.txt")));
    connect(filterButton, &QPushButton::clicked, this, &MainWindow::refreshAttendanceTable);
    connect(m.attFilterEdit, &QLineEdit::returnPressed, this, &MainWindow::refreshAttendanceTable);
    connect(m.attFilterStatus, &QComboBox::currentTextChanged, this, [this](const QString &) { refreshAttendanceTable(); });
    connect(saveButton, &QPushButton::clicked, this, &MainWindow::saveAttendanceFile);
    connect(loadButton, &QPushButton::clicked, this, &MainWindow::loadAttendanceFile);

    top->addWidget(editGroup, 2);
    top->addWidget(sumGroup, 2);
    top->addWidget(fileGroup, 1);
    root->addLayout(top);

    m.attTable = newTable(QStringList() << QStringLiteral("日期")
                                        << QStringLiteral("员工编号")
                                        << QStringLiteral("状态"));
    root->addWidget(m.attTable, 1);
    m.attStatusLabel = newHint(QStringLiteral("提示：同一日期同一员工只能有一条记录；没有记录表示“未录入”。"));
    root->addWidget(m.attStatusLabel);

    return page;
}

void MainWindow::refreshAttendanceTable()
{
    if (!m.attTable) {
        return;
    }
    const QString filter = m.attFilterEdit ? m.attFilterEdit->text().trimmed() : QString();
    const int statusFilter = m.attFilterStatus ? m.attFilterStatus->currentData().toInt() : -1;
    m.attTable->setRowCount(0);
    for (int i = 0; i < attendance.dayCount(); ++i) {
        const dayRecord *d = attendance.dayAt(i);
        if (!d) {
            continue;
        }
        const QString date = QString::fromStdString(d->date.toString());
        for (int j = 0; j < d->Recordcnt; ++j) {
            const QString id = QString::fromStdString(d->dayrecords[j].ID);
            if (!filter.isEmpty() &&
                !date.contains(filter, Qt::CaseInsensitive) &&
                !id.contains(filter, Qt::CaseInsensitive)) {
                continue;
            }
            if (statusFilter >= 0 && d->dayrecords[j].status != statusFilter) {
                continue;
            }
            const int row = m.attTable->rowCount();
            m.attTable->insertRow(row);
            m.attTable->setItem(row, 0, new QTableWidgetItem(date));
            m.attTable->setItem(row, 1, new QTableWidgetItem(id));
            m.attTable->setItem(row, 2, new QTableWidgetItem(statusText(d->dayrecords[j].status)));
        }
    }
}

void MainWindow::changeAttendance()
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
    const int newStatus = m.attNewStatusCombo->currentData().toInt();
    if (!attendance.changeRecord(date, id, newStatus)) {
        showError(QStringLiteral("修改失败：记录不存在或状态非法。"));
        return;
    }
    refreshAttendanceTable();
    showStatus(QStringLiteral("考勤状态已修改。"));
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
    refreshAttendanceTable();
    showStatus(QStringLiteral("考勤记录已删除。"));
}

void MainWindow::summarizeAttendance()
{
    if (!m.attSumEmployeeCombo || m.attSumEmployeeCombo->count() == 0) {
        showError(QStringLiteral("还没有员工。"));
        return;
    }
    const std::string id = m.attSumEmployeeCombo->currentData().toString().toStdString();
    const int year = m.attYearSpin->value();
    const int month = m.attMonthSpin->value();
    const std::vector<Date> dates = buildScheduledDates(year, month, m.attWorkdayCheck->isChecked());

    AttendanceSummary summary;
    std::string message;
    if (!attendance.summarize(id, year, month, dates, summary, message)) {
        m.attSummaryLabel->setText(QStringLiteral("统计失败：") + QString::fromStdString(message));
        return;
    }
    m.attSummaryLabel->setText(
        QStringLiteral("应出勤 %1 天；出勤 %2；缺勤 %3；请假 %4；未录入 %5")
            .arg(summary.scheduledDays).arg(summary.presentDays)
            .arg(summary.absentDays).arg(summary.leaveDays).arg(summary.missingDays));
}

void MainWindow::saveAttendanceFile()
{
    if (attendanceFile.Save(attendance, kAttendanceFile)) {
        showInfo(QStringLiteral("考勤已保存到 attendance.txt。"));
    } else {
        showError(QStringLiteral("考勤保存失败，请检查数据目录。"));
    }
}

void MainWindow::loadAttendanceFile()
{
    if (attendanceFile.Load(attendance, kAttendanceFile)) {
        refreshAttendanceTable();
        showInfo(QStringLiteral("考勤已从 attendance.txt 加载。"));
    } else {
        showError(QStringLiteral("考勤加载失败：文件不存在或格式错误。"));
    }
}

// ============================================================================
// [界面] 银行管理标签页
// ============================================================================
QWidget *MainWindow::createBankTab()
{
    QWidget *page = new QWidget();
    QVBoxLayout *root = new QVBoxLayout(page);
    root->setContentsMargins(8, 8, 8, 8);
    root->setSpacing(6);

    QHBoxLayout *top = new QHBoxLayout();

    QGroupBox *openGroup = newGroup(QStringLiteral("开户"));
    QFormLayout *openForm = new QFormLayout(openGroup);
    m.bankOpenNoEdit = newLine(QStringLiteral("6~19 位数字"));
    m.bankOpenOwnerCombo = new QComboBox();
    m.bankOpenTypeCombo = new QComboBox();
    m.bankOpenTypeCombo->addItem(QStringLiteral("储蓄账户"), 1);
    m.bankOpenTypeCombo->addItem(QStringLiteral("信用账户"), 2);
    m.bankOpenInitialEdit = newLine(QStringLiteral("首次存款，元"));
    m.bankOpenExtraEdit = newLine(QStringLiteral("最低留存余额，元"));
    QPushButton *openButton = newButton(QStringLiteral("开户"));
    openForm->addRow(QStringLiteral("账号"), m.bankOpenNoEdit);
    openForm->addRow(QStringLiteral("员工"), m.bankOpenOwnerCombo);
    openForm->addRow(QStringLiteral("类型"), m.bankOpenTypeCombo);
    openForm->addRow(QStringLiteral("首次存款"), m.bankOpenInitialEdit);
    openForm->addRow(QStringLiteral("额外参数"), m.bankOpenExtraEdit);
    openForm->addRow(openButton);
    connect(m.bankOpenTypeCombo, &QComboBox::currentTextChanged, this, [this](const QString &) {
        if (m.bankOpenTypeCombo->currentData().toInt() == 2) {
            m.bankOpenExtraEdit->setPlaceholderText(QStringLiteral("信用额度，元"));
        } else {
            m.bankOpenExtraEdit->setPlaceholderText(QStringLiteral("最低留存余额，元"));
        }
    });
    connect(openButton, &QPushButton::clicked, this, &MainWindow::openBankAccount);

    QGroupBox *moneyGroup = newGroup(QStringLiteral("存取款 / 转账 / 结息"));
    QGridLayout *moneyGrid = new QGridLayout(moneyGroup);
    m.bankDepositAccountEdit = newLine(QStringLiteral("账号"));
    m.bankDepositAmountEdit = newLine(QStringLiteral("金额，元"));
    m.bankWithdrawAccountEdit = newLine(QStringLiteral("账号"));
    m.bankWithdrawAmountEdit = newLine(QStringLiteral("金额，元"));
    m.bankFromEdit = newLine(QStringLiteral("转出账号"));
    m.bankToEdit = newLine(QStringLiteral("转入账号"));
    m.bankTransferAmountEdit = newLine(QStringLiteral("金额，元"));
    QPushButton *depositButton = newButton(QStringLiteral("存款"));
    QPushButton *withdrawButton = newButton(QStringLiteral("取款"));
    QPushButton *transferButton = newButton(QStringLiteral("转账"));
    QPushButton *interestButton = newButton(QStringLiteral("结息"));
    moneyGrid->addWidget(m.bankDepositAccountEdit, 0, 0);
    moneyGrid->addWidget(m.bankDepositAmountEdit, 0, 1);
    moneyGrid->addWidget(depositButton, 0, 2);
    moneyGrid->addWidget(m.bankWithdrawAccountEdit, 1, 0);
    moneyGrid->addWidget(m.bankWithdrawAmountEdit, 1, 1);
    moneyGrid->addWidget(withdrawButton, 1, 2);
    moneyGrid->addWidget(m.bankFromEdit, 2, 0);
    moneyGrid->addWidget(m.bankToEdit, 2, 1);
    moneyGrid->addWidget(m.bankTransferAmountEdit, 2, 2);
    moneyGrid->addWidget(transferButton, 2, 3);
    moneyGrid->addWidget(interestButton, 3, 3);
    connect(depositButton, &QPushButton::clicked, this, &MainWindow::depositMoney);
    connect(withdrawButton, &QPushButton::clicked, this, &MainWindow::withdrawMoney);
    connect(transferButton, &QPushButton::clicked, this, &MainWindow::transferMoney);
    connect(interestButton, &QPushButton::clicked, this, &MainWindow::settleInterest);

    QGroupBox *queryGroup = newGroup(QStringLiteral("查询 / 保存"));
    QFormLayout *queryForm = new QFormLayout(queryGroup);
    m.bankFilterAccountEdit = newLine(QStringLiteral("账号/员工编号，可空"));
    QHBoxLayout *dateRange = new QHBoxLayout();
    m.txBeginDateEdit = new QDateEdit(QDate::currentDate().addMonths(-1));
    m.txBeginDateEdit->setCalendarPopup(true);
    m.txBeginDateEdit->setDisplayFormat(QStringLiteral("yyyy-MM-dd"));
    m.txEndDateEdit = new QDateEdit(QDate::currentDate());
    m.txEndDateEdit->setCalendarPopup(true);
    m.txEndDateEdit->setDisplayFormat(QStringLiteral("yyyy-MM-dd"));
    dateRange->addWidget(m.txBeginDateEdit);
    dateRange->addWidget(new QLabel(QStringLiteral("至")));
    dateRange->addWidget(m.txEndDateEdit);
    QPushButton *filterButton = newButton(QStringLiteral("查询流水"));
    QPushButton *saveButton = newButton(QStringLiteral("保存银行数据"));
    QPushButton *loadButton = newButton(QStringLiteral("加载银行数据"));
    QPushButton *closeButton = newButton(QStringLiteral("销户选中账户"));
    queryForm->addRow(QStringLiteral("账号/员工筛选"), m.bankFilterAccountEdit);
    queryForm->addRow(QStringLiteral("日期范围"), dateRange);
    queryForm->addRow(filterButton);
    queryForm->addRow(closeButton);
    QHBoxLayout *fileButtons = new QHBoxLayout();
    fileButtons->addWidget(saveButton);
    fileButtons->addWidget(loadButton);
    queryForm->addRow(fileButtons);
    connect(filterButton, &QPushButton::clicked, this, &MainWindow::filterBankTransactions);
    connect(closeButton, &QPushButton::clicked, this, &MainWindow::closeBankAccount);
    connect(saveButton, &QPushButton::clicked, this, &MainWindow::saveBankData);
    connect(loadButton, &QPushButton::clicked, this, &MainWindow::loadBankData);

    top->addWidget(openGroup, 1);
    top->addWidget(moneyGroup, 2);
    top->addWidget(queryGroup, 1);
    root->addLayout(top);

    m.accountTable = newTable(QStringList() << QStringLiteral("账号") << QStringLiteral("员工编号")
                                             << QStringLiteral("姓名") << QStringLiteral("类型")
                                             << QStringLiteral("余额") << QStringLiteral("最低余额/信用额度")
                                             << QStringLiteral("开户日期") << QStringLiteral("上次结息"));
    root->addWidget(new QLabel(QStringLiteral("账户列表")));
    root->addWidget(m.accountTable, 1);

    m.txTable = newTable(QStringList() << QStringLiteral("流水号") << QStringLiteral("账号")
                                       << QStringLiteral("类型") << QStringLiteral("金额")
                                       << QStringLiteral("交易后余额") << QStringLiteral("日期")
                                       << QStringLiteral("备注"));
    root->addWidget(new QLabel(QStringLiteral("交易流水")));
    root->addWidget(m.txTable, 1);

    m.bankStatusLabel = newHint(QStringLiteral("提示：银行数据文件位于 data/accounts.csv、data/transactions.csv。"));
    root->addWidget(m.bankStatusLabel);

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
        // [刷新] 账号、员工编号、姓名任意一个包含筛选词都显示，支持按员工查询。
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
        if (!filter.isEmpty() && !QString::fromStdString(t.accountNo()).contains(filter, Qt::CaseInsensitive)) {
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
    Employee owner(ownerId, ownerAnimal->getname());
    std::string message;
    if (!bank.openAccount(no.toStdString(), owner, type, initial, extra, message)) {
        showError(QString::fromStdString(message));
        return;
    }

    // [功能] 开户成功后把账号绑定到员工，工资/食物购买才能找到账户。
    ownerAnimal->setBankAccountNo(no.toStdString());
    try {
        animalManager.save();
    } catch (...) {
    }
    refreshAll();
    showInfo(QString::fromStdString(message));
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

void MainWindow::saveBankData()
{
    if (bank.saveData()) {
        showInfo(QStringLiteral("银行数据已保存到 data/。"));
    } else {
        showError(QStringLiteral("银行数据保存失败。"));
    }
}

void MainWindow::loadBankData()
{
    bank.loadData();
    refreshAll();
    showInfo(QStringLiteral("银行数据已加载。"));
}

void MainWindow::filterBankTransactions()
{
    refreshBankAccounts();
    refreshBankTransactions();
}

// ============================================================================
// [界面] 工资管理标签页
// ============================================================================
QWidget *MainWindow::createPayrollTab()
{
    QWidget *page = new QWidget();
    QVBoxLayout *root = new QVBoxLayout(page);
    root->setContentsMargins(8, 8, 8, 8);
    root->setSpacing(6);

    QGroupBox *genGroup = newGroup(QStringLiteral("汇总考勤并生成工资单"));
    QGridLayout *genGrid = new QGridLayout(genGroup);
    m.payEmployeeCombo = new QComboBox();
    m.payYearSpin = new QSpinBox();
    m.payYearSpin->setRange(2000, 2100);
    m.payYearSpin->setValue(QDate::currentDate().year());
    m.payMonthSpin = new QSpinBox();
    m.payMonthSpin->setRange(1, 12);
    m.payMonthSpin->setValue(QDate::currentDate().month());
    m.payWorkdayCheck = new QCheckBox(QStringLiteral("应出勤只算周一至周五"));
    m.payWorkdayCheck->setChecked(true);
    m.payAbsentEdit = newLine(QStringLiteral("缺勤每天扣款，元"));
    m.payAbsentEdit->setText(QStringLiteral("0"));
    m.payLeaveEdit = newLine(QStringLiteral("请假每天扣款，元"));
    m.payLeaveEdit->setText(QStringLiteral("0"));
    m.payBonusEdit = newLine(QStringLiteral("奖金，元"));
    m.payBonusEdit->setText(QStringLiteral("0"));
    QPushButton *generateButton = newButton(QStringLiteral("计算并生成工资单"));
    m.paySummaryLabel = new QLabel(QStringLiteral("尚未生成"));
    m.paySummaryLabel->setWordWrap(true);
    genGrid->addWidget(new QLabel(QStringLiteral("员工")), 0, 0);
    genGrid->addWidget(m.payEmployeeCombo, 0, 1);
    genGrid->addWidget(new QLabel(QStringLiteral("年月")), 0, 2);
    genGrid->addWidget(m.payYearSpin, 0, 3);
    genGrid->addWidget(new QLabel(QStringLiteral("年")), 0, 4);
    genGrid->addWidget(m.payMonthSpin, 0, 5);
    genGrid->addWidget(new QLabel(QStringLiteral("月")), 0, 6);
    genGrid->addWidget(new QLabel(QStringLiteral("缺勤扣款")), 1, 0);
    genGrid->addWidget(m.payAbsentEdit, 1, 1);
    genGrid->addWidget(new QLabel(QStringLiteral("请假扣款")), 1, 2);
    genGrid->addWidget(m.payLeaveEdit, 1, 3);
    genGrid->addWidget(new QLabel(QStringLiteral("奖金")), 1, 4);
    genGrid->addWidget(m.payBonusEdit, 1, 5);
    genGrid->addWidget(m.payWorkdayCheck, 1, 6);
    genGrid->addWidget(generateButton, 2, 0, 1, 2);
    genGrid->addWidget(m.paySummaryLabel, 2, 2, 1, 5);
    connect(generateButton, &QPushButton::clicked, this, &MainWindow::generatePayroll);

    QHBoxLayout *buttons = new QHBoxLayout();
    QPushButton *payButton = newButton(QStringLiteral("发放选中工资单"));
    QPushButton *saveButton = newButton(QStringLiteral("保存工资单"));
    QPushButton *loadButton = newButton(QStringLiteral("加载工资单"));
    buttons->addWidget(payButton);
    buttons->addWidget(saveButton);
    buttons->addWidget(loadButton);
    buttons->addStretch();
    connect(payButton, &QPushButton::clicked, this, &MainWindow::paySelectedPayroll);
    connect(saveButton, &QPushButton::clicked, this, &MainWindow::savePayrollFile);
    connect(loadButton, &QPushButton::clicked, this, &MainWindow::loadPayrollFile);

    root->addWidget(genGroup);
    root->addLayout(buttons);
    m.payTable = newTable(QStringList() << QStringLiteral("工资单号") << QStringLiteral("员工")
                                        << QStringLiteral("年月") << QStringLiteral("基本工资")
                                        << QStringLiteral("缺勤扣") << QStringLiteral("请假扣")
                                        << QStringLiteral("奖金") << QStringLiteral("实发")
                                        << QStringLiteral("状态") << QStringLiteral("银行流水号"));
    root->addWidget(m.payTable, 1);
    m.payStatusLabel = newHint(QStringLiteral("提示：同一员工同一年月只能生成一张工资单；已发放工资单不能重复发放。"));
    root->addWidget(m.payStatusLabel);
    return page;
}

void MainWindow::refreshPayrollTable()
{
    if (!m.payTable) {
        return;
    }
    m.payTable->setRowCount(0);
    const std::vector<PayrollRecord> &records = payroll.getPayrollRecords();
    for (size_t i = 0; i < records.size(); ++i) {
        const PayrollRecord &r = records[i];
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
    const std::vector<Date> dates = buildScheduledDates(year, month, m.payWorkdayCheck->isChecked());

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
}

void MainWindow::paySelectedPayroll()
{
    if (!m.payTable || m.payTable->currentRow() < 0) {
        showError(QStringLiteral("请先选择一张工资单。"));
        return;
    }
    const int row = m.payTable->currentRow();
    if (row >= (int)payroll.getPayrollRecords().size()) {
        showError(QStringLiteral("工资单索引无效。"));
        return;
    }
    PayrollRecord record = payroll.getPayrollRecords()[row];
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

void MainWindow::savePayrollFile()
{
    if (payroll.savePayrolls(kPayrollFile)) {
        showInfo(QStringLiteral("工资单已保存到 payroll.txt。"));
    } else {
        showError(QStringLiteral("工资单保存失败。"));
    }
}

void MainWindow::loadPayrollFile()
{
    if (payroll.loadPayrolls(kPayrollFile)) {
        refreshPayrollTable();
        showInfo(QStringLiteral("工资单已从 payroll.txt 加载。"));
    } else {
        showError(QStringLiteral("工资单加载失败：文件不存在或格式错误。"));
    }
}

// ============================================================================
// [界面] 食物管理标签页
// ============================================================================
QWidget *MainWindow::createFoodTab()
{
    QWidget *page = new QWidget();
    QVBoxLayout *root = new QVBoxLayout(page);
    root->setContentsMargins(8, 8, 8, 8);
    root->setSpacing(6);

    QHBoxLayout *top = new QHBoxLayout();

    QGroupBox *catalogGroup = newGroup(QStringLiteral("食物目录"));
    QFormLayout *catalogForm = new QFormLayout(catalogGroup);
    m.foodIdEdit = newLine(QStringLiteral("如 F001"));
    m.foodNameEdit = newLine(QStringLiteral("食物名称"));
    m.foodPriceEdit = newLine(QStringLiteral("单价，元/kg"));
    QPushButton *addButton = newButton(QStringLiteral("添加食物"));
    catalogForm->addRow(QStringLiteral("编号"), m.foodIdEdit);
    catalogForm->addRow(QStringLiteral("名称"), m.foodNameEdit);
    catalogForm->addRow(QStringLiteral("单价"), m.foodPriceEdit);
    catalogForm->addRow(addButton);
    connect(addButton, &QPushButton::clicked, this, &MainWindow::addFood);

    QGroupBox *buyGroup = newGroup(QStringLiteral("购买食物"));
    QFormLayout *buyForm = new QFormLayout(buyGroup);
    m.foodBuyEmployeeCombo = new QComboBox();
    m.foodBuyFoodCombo = new QComboBox();
    m.foodQtySpin = new QSpinBox();
    m.foodQtySpin->setRange(1, 10000);
    m.foodQtySpin->setValue(1);
    QPushButton *buyButton = newButton(QStringLiteral("购买并从账户扣款"));
    buyForm->addRow(QStringLiteral("员工"), m.foodBuyEmployeeCombo);
    buyForm->addRow(QStringLiteral("食物"), m.foodBuyFoodCombo);
    buyForm->addRow(QStringLiteral("数量 kg"), m.foodQtySpin);
    buyForm->addRow(buyButton);
    connect(buyButton, &QPushButton::clicked, this, &MainWindow::purchaseFood);

    QGroupBox *fileGroup = newGroup(QStringLiteral("文件"));
    QVBoxLayout *fileLayout = new QVBoxLayout(fileGroup);
    QPushButton *saveButton = newButton(QStringLiteral("保存食物/购买记录"));
    QPushButton *loadButton = newButton(QStringLiteral("加载食物/购买记录"));
    fileLayout->addWidget(saveButton);
    fileLayout->addWidget(loadButton);
    fileLayout->addWidget(newHint(QStringLiteral("文件：foods.txt、purchases.txt")));
    connect(saveButton, &QPushButton::clicked, this, &MainWindow::saveFoodFile);
    connect(loadButton, &QPushButton::clicked, this, &MainWindow::loadFoodFile);

    top->addWidget(catalogGroup, 2);
    top->addWidget(buyGroup, 2);
    top->addWidget(fileGroup, 1);
    root->addLayout(top);

    m.foodTable = newTable(QStringList() << QStringLiteral("编号") << QStringLiteral("名称") << QStringLiteral("单价 元/kg"));
    root->addWidget(new QLabel(QStringLiteral("食物目录")));
    root->addWidget(m.foodTable, 1);

    m.purchaseTable = newTable(QStringList() << QStringLiteral("购买单") << QStringLiteral("员工")
                                              << QStringLiteral("食物") << QStringLiteral("数量 kg")
                                              << QStringLiteral("金额") << QStringLiteral("账号")
                                              << QStringLiteral("日期") << QStringLiteral("状态")
                                              << QStringLiteral("银行流水"));
    root->addWidget(new QLabel(QStringLiteral("购买记录")));
    root->addWidget(m.purchaseTable, 1);
    m.foodStatusLabel = newHint(QStringLiteral("提示：购买成功后会自动写一条饲养记录；购买单保存在 purchases.txt。"));
    root->addWidget(m.foodStatusLabel);

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
    m.purchaseTable->setRowCount(0);
    const std::vector<FoodPurchaseRecord> &records = foodService.getPurchaseRecords();
    for (size_t i = 0; i < records.size(); ++i) {
        const FoodPurchaseRecord &r = records[i];
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
}

void MainWindow::purchaseFood()
{
    if (!m.foodBuyEmployeeCombo || m.foodBuyEmployeeCombo->count() == 0) {
        showError(QStringLiteral("还没有员工。"));
        return;
    }
    if (!m.foodBuyFoodCombo || m.foodBuyFoodCombo->count() == 0) {
        showError(QStringLiteral("还没有食物，请先添加食物。"));
        return;
    }
    const std::string employeeId = m.foodBuyEmployeeCombo->currentData().toString().toStdString();
    const std::string foodId = m.foodBuyFoodCombo->currentData().toString().toStdString();
    animal *a = animalManager.findbyid(employeeId);
    if (!a) {
        showError(QStringLiteral("员工不存在。"));
        return;
    }
    if (a->bankAccountNo().empty()) {
        showError(QStringLiteral("该员工还没有绑定银行账号，请先在“银行管理”中开户。"));
        return;
    }
    const int quantity = m.foodQtySpin->value();
    int transactionId = 0;
    std::string message;
    if (!foodService.purchaseFood(bank, *a, foodId, quantity, transactionId, message)) {
        showError(QString::fromStdString(message));
        return;
    }

    // [功能] 购买成功：写进食记录 + 保存购买单/动物园/银行数据。
    animalManager.feedanimal(employeeId, Date::today().toString(), quantity);
    try {
        animalManager.save();
    } catch (...) {
    }
    foodService.savePurchases(kPurchaseFile);
    refreshAll();
    showInfo(QString::fromStdString(message) + QStringLiteral("\n银行流水号：") + QString::number(transactionId));
}

void MainWindow::saveFoodFile()
{
    const bool a = foodService.saveFoods(kFoodFile);
    const bool b = foodService.savePurchases(kPurchaseFile);
    if (a && b) {
        showInfo(QStringLiteral("食物目录和购买记录已保存。"));
    } else {
        showError(QStringLiteral("食物数据保存失败。"));
    }
}

void MainWindow::loadFoodFile()
{
    const bool a = foodService.loadFoods(kFoodFile);
    const bool b = foodService.loadPurchases(kPurchaseFile);
    fillFoodCombos();
    refreshFoodTable();
    refreshPurchaseTable();
    if (a || b) {
        showInfo(QStringLiteral("食物数据已加载。"));
    } else {
        showError(QStringLiteral("没有找到食物数据文件，或文件格式错误。"));
    }
}
