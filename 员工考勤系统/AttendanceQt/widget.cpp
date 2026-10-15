#include "widget.h"
#include "./ui_widget.h"
#include <QDate>
#include <QMessageBox>
#include <QTableWidgetItem>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    ui->dateEdit->setDate(QDate::currentDate());

    ui->tableWidget->setColumnCount(3);
    ui->tableWidget->setHorizontalHeaderLabels(
        {"日期","ID","状态"}
    );
}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_addRecord_clicked()
{
    QDate selected = ui->dateEdit->date();
    Date date = {
        selected.year(),
        selected.month(),
        selected.day()
    };

    string ID = ui->idEdit->text().trimmed().toStdString();

    if(ID.empty()){
        QMessageBox::warning(this,"提示","请输入ID");
        return;
    }

    int status = ui->statusCombo->currentIndex();
    basic.addRecord(date,ID,status);
    refreshTable();
}

void Widget::refreshTable()
{
    QDate selected = ui->dateEdit->date();

    Date date = {
        selected.year(),
        selected.month(),
        selected.day()
    };

    Record records[MAX_RECORDS];
    int count = basic.getRecord(date, records);//复制到result，同时获取count

    ui->tableWidget->setRowCount(count);
    for(int i = 0; i < count; i++){
        QString status = "未知";

        if(records[i].status >= 0 && records[i].status <= 2) {
            status = ui->statusCombo->itemText(records[i].status);
        }

        ui->tableWidget->setItem(
            i,0,
            new QTableWidgetItem(selected.toString("yyyy-MM-dd"))
        );

        ui->tableWidget->setItem(
            i, 1,
            new QTableWidgetItem(
                QString::fromStdString(records[i].ID)
            )
        );
        ui->tableWidget->setItem(
            i, 2,
            new QTableWidgetItem(status)
        );
    }

}


void Widget::on_showRecord_clicked()
{
    refreshTable();
}

