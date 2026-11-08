#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <Basic.h>
#include "FileOperation.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget() override;

private slots:
    void on_addRecord_clicked();

    void on_showRecord_clicked();

    void on_saveRecord_clicked();

    void on_loadRecord_clicked();


    void on_deleteRecord_clicked();

private:
    Ui::Widget *ui;
    Basic basic;
    FileOperation file;
    void refreshTable();
};
#endif // WIDGET_H
