// 整合新增：首见于不疯狂动物城/src/payroll/PayrollStorage.cpp，读写工资单及考勤快照。
#include "PayrollStorage.h"
#include <fstream>
#include <iterator>
#include <sstream>

bool PayrollStorage::Save(const vector<PayrollRecord>& records, const std::filesystem::path& filename){
    ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }

    int n = records.size();
    for (int i = 0; i < n; i++){
        const PayrollRecord& r = records[i];
        file << r.payrollId << " "
             << r.employeeId << " "
             << r.year << " "
             << r.month << " "
             << r.accountNo << " ";

        file << r.basicSalary << " "
             << r.absentDeduction << " "
             << r.leaveDeduction << " "
             << r.bonus << " "
             << r.finalsalary << " ";

        file << r.status << " "
             << r.bankTransactionId << " ";

        file << r.attendance.employeeID << " "
             << r.attendance.year << " "
             << r.attendance.month << " "
             << r.attendance.scheduledDays << " "
             << r.attendance.presentDays << " "
             << r.attendance.absentDays << " "
             << r.attendance.leaveDays << " "
             << r.attendance.missingDays << endl;
    }
    file.close();
    if(file.fail()){
        return false;
    }
    return true;
}

bool PayrollStorage::Load(vector<PayrollRecord>& records, const std::filesystem::path& filename){
    ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }

    vector<PayrollRecord> tempRecords;//临时存储，避免部分读取成功后覆盖原有数据
    string line;

    while (getline(file, line)) {
        if (line.empty()) {
            continue; //跳空行
        }
        istringstream iss(line);
        PayrollRecord r;

        // 兼容整合版的工资单文件。未绑定账号的旧记录少一个字段。
        istringstream fields(line);
        const auto fieldCount = std::distance(std::istream_iterator<string>(fields),
                                              std::istream_iterator<string>());
        if ((fieldCount != 19 && fieldCount != 20) ||
            !(iss >> r.payrollId >> r.employeeId >> r.year >> r.month)) {
            return false;
        }
        if (fieldCount == 20) {
            iss >> r.accountNo;
        }
        if (!(iss >> r.basicSalary >> r.absentDeduction >> r.leaveDeduction >> r.bonus >> r.finalsalary
                  >> r.status >> r.bankTransactionId
                  >> r.attendance.employeeID >> r.attendance.year >> r.attendance.month
                  >> r.attendance.scheduledDays >> r.attendance.presentDays >> r.attendance.absentDays
                  >> r.attendance.leaveDays >> r.attendance.missingDays)) {
            return false;
        }
        tempRecords.push_back(r);
    }
    if(file.bad()){
        return false;
    }
    records = tempRecords;
    return true;
}
