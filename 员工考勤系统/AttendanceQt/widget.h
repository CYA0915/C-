#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <Basic.h>

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

private:
    Ui::Widget *ui;
    Basic basic;
    void refreshTable();
};
#endif // WIDGET_H
