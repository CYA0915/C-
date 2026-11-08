#include "widget.h"
#include "./ui_widget.h"
#include <QDate>
#include <QMessageBox>
#include <QTableWidgetItem>
#include <QFileDialog>


Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    ui->tableWidget->setEditTriggers(
        QAbstractItemView::NoEditTriggers
        );
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

    if (ID.find_first_of(" \t\r\n") != string::npos) {
        QMessageBox::warning(this, "提示", "ID不能包含空格");
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



void Widget::on_saveRecord_clicked()
{
    QString filename = QFileDialog::getSaveFileName(
        this,
        "保存考勤记录",
        "attendance.txt",
        "文本文件(*.txt)"
    );

    if(filename.isEmpty()) {
        return;
    }

    bool success = file.Save(basic,filename.toStdString());

    if(success) {
        QMessageBox::information(this,"提示","保存成功");
    }
    else {
        QMessageBox::warning(this,"提示","保存失败");
    }
}


void Widget::on_loadRecord_clicked()
{
    QString filename = QFileDialog::getOpenFileName(
        this,
        "读取考勤记录",
        "",
        "文本文件(*.txt)"
    );

    if(filename.isEmpty()){
        return;
    }

    bool success = file.Load(basic,filename.toStdString());

    if(success) {
        refreshTable();
        QMessageBox::information(this,"提示","文件读取完成");
    }
    else {
        QMessageBox::warning(this,"提示","读取失败");
    }
}



void Widget::on_deleteRecord_clicked()
{

}

