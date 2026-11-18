#include "mainwindow.h"

#include <QApplication>
#include <QDir>
#include <QFont>
#include <QStandardPaths>

// ============================================================================
// 程序入口：
//   1. 把当前工作目录切到“文档/不疯狂动物城数据”，保证相对路径的数据文件
//      （animals.txt、records.txt、attendance.txt、payroll.txt、foods.txt、
//       purchases.txt、data/accounts.csv、data/transactions.csv）都可写。
//   2. 创建 QApplication 和 MainWindow，进入 Qt 事件循环。
// ============================================================================
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QApplication::setStyle(QStringLiteral("Fusion"));

    QFont font = app.font();
    font.setPointSize(10);
    app.setFont(font);

    QString dataDir = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    if (dataDir.isEmpty()) {
        dataDir = QDir::homePath();
    }
    dataDir += QStringLiteral("/不疯狂动物城数据");
    QDir().mkpath(dataDir);
    QDir().mkpath(dataDir + QStringLiteral("/data"));
    QDir::setCurrent(dataDir);

    MainWindow window;
    window.show();
    return app.exec();
}
