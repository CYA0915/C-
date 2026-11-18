#ifndef SQLITEEXPORTER_H
#define SQLITEEXPORTER_H

#include <QString>

class zoo;
class Basic;
class BankService;
class PayrollService;
class FoodPurchaseService;

// ============================================================================
// SqliteExporter
//   把当前内存中的全部业务数据导出成一个 SQLite 数据库（.db）。
//
// 导出的表：
//   employees        动物员工（编号、姓名、类别、物种、专属信息、工资、账号）
//   feed_records     饲养记录
//   health_records   健康记录
//   attendance       考勤记录
//   bank_accounts    银行账户
//   transactions     银行流水
//   payrolls         工资单（含考勤快照）
//   foods            食物目录
//   food_purchases   食物购买记录
//   export_info      导出时间、数据目录、各表行数
//
// 说明：只读业务对象，不修改界面和内存数据；导出失败会通过 error 返回原因。
// ============================================================================
class SqliteExporter
{
public:
    // 返回 true 表示数据库创建、建表、写入、提交全部成功。
    static bool exportAll(const QString &filePath,
                          zoo &animalManager,
                          Basic &attendance,
                          BankService &bank,
                          PayrollService &payroll,
                          FoodPurchaseService &foodService,
                          QString &error);
};

#endif // SQLITEEXPORTER_H
