// 整合新增：首见于不疯狂动物城/src/payroll/PayrollStorage.h，读写工资单及考勤快照。
#pragma once
#include <filesystem>
#include <vector>
#include "../function/PayrollRecord.h"

using namespace std;
class PayrollStorage {
public:
    bool Save(const vector<PayrollRecord>& records, const std::filesystem::path& filename);
    bool Load(vector<PayrollRecord>& records, const std::filesystem::path& filename);
};
