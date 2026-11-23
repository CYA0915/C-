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

class QCloseEvent;
class QComboBox;
class QDateEdit;
class QDialog;
class QFormLayout;
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

// 统一管理业务对象和身份页面，输入表单通过对话框打开。
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    // 关闭窗口时自动保存全部模块。
    void closeEvent(QCloseEvent *event) override;

private:
    // 管理者页面的控件指针。按标签页分组，便于 refreshXxx() 统一刷新。
    struct ManagerUi {
        QTabWidget *tabs = nullptr;
        QLabel *welcomeLabel = nullptr;
        QPushButton *logoutButton = nullptr;

        // 动物园管理
        QComboBox *zooTypeCombo = nullptr;
        QLineEdit *zooIdEdit = nullptr;
        QLineEdit *zooNameEdit = nullptr;
        QLineEdit *zooWingEdit = nullptr;
        QLineEdit *zooInitialSalaryEdit = nullptr;
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

        // 考勤管理
        QComboBox *attEmployeeCombo = nullptr;
        QDateEdit *attDateEdit = nullptr;
        QComboBox *attNewStatusCombo = nullptr;
        QTableWidget *attTable = nullptr;

        // 银行管理
        QTableWidget *accountTable = nullptr;
        QTableWidget *txTable = nullptr;
        QDialog *bankOpenDialog = nullptr;
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

        // 工资管理
        QComboBox *payEmployeeCombo = nullptr;
        QSpinBox *payYearSpin = nullptr;
        QSpinBox *payMonthSpin = nullptr;
        QLineEdit *payAbsentEdit = nullptr;
        QLineEdit *payLeaveEdit = nullptr;
        QLineEdit *payBonusEdit = nullptr;
        QTableWidget *payTable = nullptr;
        QLabel *paySummaryLabel = nullptr;

        // 食物管理
        QTableWidget *foodTable = nullptr;
        QTableWidget *purchaseTable = nullptr;
        QLineEdit *foodIdEdit = nullptr;
        QLineEdit *foodNameEdit = nullptr;
        QLineEdit *foodPriceEdit = nullptr;
        QLineEdit *purchaseFilterEdit = nullptr;
    };

    // 员工页面的控件指针。员工只能看到自己的考勤、账户、工资和购买记录。
    struct EmployeeUi {
        QLabel *welcomeLabel = nullptr;
        QLabel *infoLabel = nullptr;
        QPushButton *logoutButton = nullptr;
        QTabWidget *tabs = nullptr;

        QTableWidget *attTable = nullptr;
        QDateEdit *leaveDateEdit = nullptr;
        QSpinBox *attYearSpin = nullptr;
        QSpinBox *attMonthSpin = nullptr;
        QLabel *attSummaryLabel = nullptr;

        QTableWidget *accountTable = nullptr;
        QTableWidget *txTable = nullptr;
        QComboBox *accountCombo = nullptr;
        QLineEdit *accountAmountEdit = nullptr;
        QLabel *balanceLabel = nullptr;

        QTableWidget *payTable = nullptr;

        QComboBox *foodCombo = nullptr;
        QSpinBox *foodQtySpin = nullptr;
        QTableWidget *purchaseTable = nullptr;
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

    // 固定数据文件名（相对于 main.cpp 选定的数据目录）。
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

    // 三个页面的搭建
    void showLoginPage();
    void showManagerPage();
    void showEmployeePage(const std::string &employeeId);
    void setupLoginPage();
    void setupManagerPage();
    void setupEmployeePage();
    QWidget *createZooTab();
    QWidget *createAttendanceTab();
    QWidget *createBankTab();
    QWidget *createFoodTab();
    QWidget *createEmployeeAttendanceTab();
    QWidget *createEmployeeAccountTab();
    QWidget *createEmployeePayrollTab();
    QWidget *createEmployeeFoodTab();

    // 登录/登出/数据
    void handleLogin();
    void logout();
    bool loadAllData();
    bool saveAllData(bool showResult = true);
    bool initializeAttendance(int year, int month);

    // 统一刷新入口
    void refreshAll();
    void refreshManagerUi();
    void refreshWorkTables();
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

    // 动物园
    void addAnimal();
    void updateAnimalInfo();
    void deleteAnimal();
    void addFeedRecord();
    void addHealthRecord();
    void showAnimalRecords();
    void onZooSelectionChanged();

    // 考勤
    void changeAttendance();
    void deleteAttendance();
    void summarizeAttendance();

    // 银行
    void openBankAccount();
    void closeBankAccount();
    void depositMoney();
    void withdrawMoney();
    void transferMoney();
    void settleInterest();
    void filterBankTransactions();

    // 工资
    void generatePayroll();
    void paySelectedPayroll();

    // 食物
    void addFood();

    // 员工页
    void employeeSignIn();
    void employeeRequestLeave();
    void employeeSummarizeAttendance();
    void employeeDeposit();
    void employeeWithdraw();
    void employeePurchaseFood();

    // 表格、按钮、输入框、金额和日期工具。
    QTableWidget *newTable(const QStringList &headers);
    QPushButton *newButton(const QString &text);
    QLineEdit *newLine(const QString &placeholder = QString());
    QLabel *newHint(const QString &text);
    QGroupBox *newGroup(const QString &title);
    QDialog *newDialog(const QString &title);
    QPushButton *dialogAction(QDialog *dialog, QFormLayout *form, const QString &text);
    QPushButton *newDataButton(bool manager);
    void closeOperationDialog();
    QString statusText(int status) const;
    bool readMoney(QLineEdit *edit, Money &amount, QString &error) const;
    bool readDate(QDateEdit *edit, Date &date) const;
    Date dateFromQDate(const QDate &date) const;
    std::vector<Date> buildScheduledDates(int year, int month) const;
    animal *selectedAnimal() const;
    BankAccount *selectedAccount() const;
    void selectZooRow(const std::string &id);
    void showInfo(const QString &text);
    void showError(const QString &text);
    void showStatus(const QString &text);
};

#endif // MAINWINDOW_H
