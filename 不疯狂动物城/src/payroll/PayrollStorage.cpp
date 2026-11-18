#include "PayrollStorage.h"
#include <fstream>
#include <sstream>

bool PayrollStorage::Save(const vector<PayrollRecord>& records, const string& filename){
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

bool PayrollStorage::Load(vector<PayrollRecord>& records, const string& filename){
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
        
        if(!(iss >> r.payrollId >> r.employeeId >> r.year >> r.month >> r.accountNo
             >> r.basicSalary >> r.absentDeduction >> r.leaveDeduction >> r.bonus >> r.finalsalary
             >> r.status >> r.bankTransactionId
             >> r.attendance.employeeID >> r.attendance.year >> r.attendance.month
             >> r.attendance.scheduledDays >> r.attendance.presentDays
             >> r.attendance.absentDays >> r.attendance.leaveDays
             >> r.attendance.missingDays)) {
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
