#include "mainwindow.h"

#include <QApplication>
#include <QDir>
#include <QFont>
#include <QMessageBox>
#include <QStandardPaths>

// 选择数据目录后启动统一 Qt 窗口；--data-dir 可用于独立演示。
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QApplication::setStyle(QStringLiteral("Fusion"));

    QFont font = app.font();
    font.setPointSize(10);
    app.setFont(font);

    QString dataDir;
    const QStringList arguments = app.arguments();
    for (int i = 1; i < arguments.size(); ++i) {
        if (arguments[i] == "--data-dir") {
            if (i + 1 == arguments.size()) {
                QMessageBox::warning(nullptr, QStringLiteral("启动失败"), QStringLiteral("--data-dir 后需要填写数据目录。"));
                return 1;
            }
            dataDir = QDir(arguments[++i]).absolutePath();
        }
    }
    if (dataDir.isEmpty()) {
        dataDir = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
        if (dataDir.isEmpty()) {
            dataDir = QDir::homePath();
        }
        dataDir += QStringLiteral("/不疯狂动物城数据");
    }
    if (!QDir().mkpath(dataDir + "/data") || !QDir::setCurrent(dataDir)) {
        QMessageBox::warning(nullptr, QStringLiteral("启动失败"), QStringLiteral("无法创建或打开数据目录：") + dataDir);
        return 1;
    }

    MainWindow window;
    window.show();
    return app.exec();
}
