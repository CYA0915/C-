#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDate>
#include <QString>
#include <QStringList>

#include <string>
#include <vector>

#include "../src/zoo/zoo.h"
#include "../src/attendance/Basic.h"
#include "../src/attendance/FileOperation.h"
#include "../src/bank/BankService.h"
#include "../src/payroll/PayrollService.h"
#include "../src/food/FoodPurchaseService.h"

class QCheckBox;
class QCloseEvent;
class QComboBox;
class QDateEdit;
class QGroupBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QSpinBox;
class QTableWidget;
class QTabWidget;
class QTextEdit;
class QWidget;

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

// ============================================================================
// MainWindow：整个 Qt 前端的唯一窗口。
//
// 设计原则（课程要求：只用基本控件）：
//   - 页面切换：QStackedWidget + 三个 QWidget（登录、管理者、员工）。
//   - 功能分区：QTabWidget + QGroupBox + QFormLayout/QGridLayout。
//   - 输入：QLineEdit、QComboBox、QSpinBox、QDateEdit、QCheckBox。
//   - 展示：QTableWidget、QLabel、QTextEdit。
//   - 提示：QMessageBox、状态栏。
//
// 业务逻辑不重复实现：窗口只调用 src/ 下的 zoo、Basic、BankService、
// PayrollService、FoodPurchaseService；界面层负责输入校验、刷新和错误提示。
//
// 代码标注约定：
//   [界面]   纯界面搭建
//   [功能]   按钮/菜单对应的业务操作
//   [刷新]   从业务对象重新读取数据填表
//   [校验]   界面层输入检查
//   [Qt 支持] 为界面补充的只读访问器/重载
// ============================================================================
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    // 关闭窗口时自动保存全部模块，避免演示数据丢失。
    void closeEvent(QCloseEvent *event) override;

private:
    // ---------------------------------------------------------------------
    // 管理者页面的控件指针。按标签页分组，便于 refreshXxx() 统一刷新。
    // ---------------------------------------------------------------------
    struct ManagerUi {
        QTabWidget *tabs = nullptr;
        QLabel *welcomeLabel = nullptr;
        QPushButton *saveAllButton = nullptr;
        QPushButton *logoutButton = nullptr;

        // 动物园管理
        QComboBox *zooTypeCombo = nullptr;
        QLineEdit *zooIdEdit = nullptr;
        QLineEdit *zooNameEdit = nullptr;
        QLineEdit *zooWingEdit = nullptr;
        QComboBox *zooFilterCategory = nullptr;
        QLineEdit *zooFilterId = nullptr;
        QTableWidget *zooTable = nullptr;
        QLabel *zooSelectedLabel = nullptr;
        QLineEdit *zooSalaryEdit = nullptr;
        QLineEdit *zooAccountEdit = nullptr;
        QLineEdit *zooNewNameEdit = nullptr;
        QLineEdit *feedAnimalEdit = nullptr;
        QDateEdit *feedDateEdit = nullptr;
        QLineEdit *feedAmountEdit = nullptr;
        QLineEdit *healthAnimalEdit = nullptr;
        QDateEdit *healthDateEdit = nullptr;
        QComboBox *healthStatusCombo = nullptr;
        QLineEdit *healthNoteEdit = nullptr;
        QTextEdit *recordView = nullptr;
        QLabel *zooStatusLabel = nullptr;

        // 考勤管理
        QComboBox *attNewStatusCombo = nullptr;
        QTableWidget *attTable = nullptr;
        QLineEdit *attFilterEdit = nullptr;
        QComboBox *attFilterStatus = nullptr;
        QComboBox *attSumEmployeeCombo = nullptr;
        QSpinBox *attYearSpin = nullptr;
        QSpinBox *attMonthSpin = nullptr;
        QCheckBox *attWorkdayCheck = nullptr;
        QLabel *attSummaryLabel = nullptr;
        QLabel *attStatusLabel = nullptr;

        // 银行管理
        QTableWidget *accountTable = nullptr;
        QTableWidget *txTable = nullptr;
        QLineEdit *bankOpenNoEdit = nullptr;
        QComboBox *bankOpenOwnerCombo = nullptr;
        QComboBox *bankOpenTypeCombo = nullptr;
        QLineEdit *bankOpenInitialEdit = nullptr;
        QLineEdit *bankOpenExtraEdit = nullptr;
        QLineEdit *bankDepositAccountEdit = nullptr;
        QLineEdit *bankDepositAmountEdit = nullptr;
        QLineEdit *bankWithdrawAccountEdit = nullptr;
        QLineEdit *bankWithdrawAmountEdit = nullptr;
        QLineEdit *bankFromEdit = nullptr;
        QLineEdit *bankToEdit = nullptr;
        QLineEdit *bankTransferAmountEdit = nullptr;
        QLineEdit *bankFilterAccountEdit = nullptr;
        QDateEdit *txBeginDateEdit = nullptr;
        QDateEdit *txEndDateEdit = nullptr;
        QLabel *bankStatusLabel = nullptr;

        // 工资管理
        QComboBox *payEmployeeCombo = nullptr;
        QSpinBox *payYearSpin = nullptr;
        QSpinBox *payMonthSpin = nullptr;
        QCheckBox *payWorkdayCheck = nullptr;
        QLineEdit *payAbsentEdit = nullptr;
        QLineEdit *payLeaveEdit = nullptr;
        QLineEdit *payBonusEdit = nullptr;
        QTableWidget *payTable = nullptr;
        QLabel *paySummaryLabel = nullptr;
        QLabel *payStatusLabel = nullptr;

        // 食物管理
        QTableWidget *foodTable = nullptr;
        QTableWidget *purchaseTable = nullptr;
        QLineEdit *foodIdEdit = nullptr;
        QLineEdit *foodNameEdit = nullptr;
        QLineEdit *foodPriceEdit = nullptr;
        QComboBox *foodBuyEmployeeCombo = nullptr;
        QComboBox *foodBuyFoodCombo = nullptr;
        QSpinBox *foodQtySpin = nullptr;
        QLabel *foodStatusLabel = nullptr;
    };

    // ---------------------------------------------------------------------
    // 员工页面的控件指针。员工只能看到自己的考勤、账户、工资和购买记录。
    // ---------------------------------------------------------------------
    struct EmployeeUi {
        QLabel *welcomeLabel = nullptr;
        QLabel *infoLabel = nullptr;
        QPushButton *saveAllButton = nullptr;
        QPushButton *logoutButton = nullptr;
        QTabWidget *tabs = nullptr;

        QTableWidget *attTable = nullptr;
        QDateEdit *leaveDateEdit = nullptr;
        QSpinBox *attYearSpin = nullptr;
        QSpinBox *attMonthSpin = nullptr;
        QCheckBox *attWorkdayCheck = nullptr;
        QLabel *attSummaryLabel = nullptr;

        QTableWidget *accountTable = nullptr;
        QTableWidget *txTable = nullptr;
        QComboBox *accountCombo = nullptr;
        QLineEdit *depositAmountEdit = nullptr;
        QLineEdit *withdrawAmountEdit = nullptr;
        QLabel *accountStatusLabel = nullptr;

        QTableWidget *payTable = nullptr;
        QLabel *payStatusLabel = nullptr;

        QComboBox *foodCombo = nullptr;
        QSpinBox *foodQtySpin = nullptr;
        QTableWidget *purchaseTable = nullptr;
        QLabel *foodStatusLabel = nullptr;
    };

    Ui::MainWindow *ui = nullptr;

    // 业务对象：与源文件中的类一一对应，界面不复制业务状态。
    zoo animalManager;
    Basic attendance;
    FileOperation attendanceFile;
    BankService bank;
    PayrollService payroll;
    FoodPurchaseService foodService;

    // 登录身份：员工编号 + “员工/管理者”角色。
    std::string currentEmployeeId;
    std::string currentRole;
    bool dataLoaded = false;

    // 固定数据文件名（相对于程序当前工作目录，main.cpp 会切到文档目录）。
    static const char *kAttendanceFile;
    static const char *kPayrollFile;
    static const char *kFoodFile;
    static const char *kPurchaseFile;

    // 登录页
    QComboBox *loginRoleCombo = nullptr;
    QLineEdit *loginAccountEdit = nullptr;
    QLineEdit *loginPasswordEdit = nullptr;
    QLabel *loginHintLabel = nullptr;

    ManagerUi m;
    EmployeeUi e;

    // [界面] 三个页面的搭建
    void showLoginPage();
    void showManagerPage();
    void showEmployeePage(const std::string &employeeId);
    void setupLoginPage();
    void setupManagerPage();
    void setupEmployeePage();
    QWidget *createZooTab();
    QWidget *createAttendanceTab();
    QWidget *createBankTab();
    QWidget *createPayrollTab();
    QWidget *createFoodTab();
    QWidget *createEmployeeInfoTab();
    QWidget *createEmployeeAttendanceTab();
    QWidget *createEmployeeAccountTab();
    QWidget *createEmployeePayrollTab();
    QWidget *createEmployeeFoodTab();

    // [功能] 登录/登出/数据
    void handleLogin();
    void logout();
    bool loadAllData();
    bool saveAllData(bool showResult = true);
    // [功能] 把全部业务数据导出成一个 SQLite 数据库。
    void exportSqlite();

    // [刷新] 统一刷新入口
    void refreshAll();
    void refreshManagerUi();
    void refreshEmployeeUi();
    void refreshZooTable();
    void refreshAttendanceTable();
    void refreshBankAccounts();
    void refreshBankTransactions();
    void refreshPayrollTable();
    void refreshFoodTable();
    void refreshPurchaseTable();
    void refreshEmployeeInfo();
    void refreshEmployeeAttendance();
    void refreshEmployeeAccounts();
    void refreshEmployeeTransactions();
    void refreshEmployeePayroll();
    void refreshEmployeePurchases();
    void fillEmployeeCombos();
    void fillFoodCombos();

    // [功能] 动物园
    void addAnimal();
    void updateAnimalInfo();
    void renameAnimal();
    void deleteAnimal();
    void addFeedRecord();
    void addHealthRecord();
    void showAnimalRecords();
    void onZooSelectionChanged();

    // [功能] 考勤
    void changeAttendance();
    void deleteAttendance();
    void summarizeAttendance();
    void saveAttendanceFile();
    void loadAttendanceFile();

    // [功能] 银行
    void openBankAccount();
    void closeBankAccount();
    void depositMoney();
    void withdrawMoney();
    void transferMoney();
    void settleInterest();
    void saveBankData();
    void loadBankData();
    void filterBankTransactions();

    // [功能] 工资
    void generatePayroll();
    void paySelectedPayroll();
    void savePayrollFile();
    void loadPayrollFile();

    // [功能] 食物
    void addFood();
    void purchaseFood();
    void saveFoodFile();
    void loadFoodFile();

    // [功能] 员工页
    void employeeSignIn();
    void employeeRequestLeave();
    void employeeSummarizeAttendance();
    void employeeDeposit();
    void employeeWithdraw();
    void employeePurchaseFood();
    void refreshEmployeeSelection();

    // [工具] 表格/按钮/输入框/金额/日期
    QTableWidget *newTable(const QStringList &headers);
    QPushButton *newButton(const QString &text);
    QLineEdit *newLine(const QString &placeholder = QString());
    QLabel *newHint(const QString &text);
    QGroupBox *newGroup(const QString &title);
    QString moneyText(Money amount) const;
    QString statusText(int status) const;
    bool readMoney(QLineEdit *edit, Money &amount, QString &error) const;
    bool readDate(QDateEdit *edit, Date &date) const;
    Date dateFromQDate(const QDate &date) const;
    QDate qdateFromDate(const Date &date) const;
    std::vector<Date> buildScheduledDates(int year, int month, bool workdays) const;
    animal *selectedAnimal() const;
    BankAccount *selectedAccount() const;
    void selectZooRow(const std::string &id);
    void showInfo(const QString &text);
    void showError(const QString &text);
    void showStatus(const QString &text);
};

#endif // MAINWINDOW_H
