// 交付版新增：统一调度三个基础系统以及工资、食物模块的文件读写。
// 基础来源：员工考勤系统/attendance/FileOperation.h、动物园管理系统/zoo.h 的存档接口、
// 以及银行管理系统.rar 内的 src/bank/BankStorage.h 的账户和流水文件接口。
#pragma once

#include <filesystem>
#include <string>

class CityService;

class CityStorage {
public:
    explicit CityStorage(std::filesystem::path directory);

    // 空目录按新项目读取。已有文件读失败时，保留当前业务状态。
    bool loadAll(CityService& city, std::string& message) const;
    // 调用者决定保存时机。暂存全部业务文件后再替换，保留旧文件格式。
    bool saveAll(const CityService& city, std::string& message) const;

private:
    std::filesystem::path directory_;
};
