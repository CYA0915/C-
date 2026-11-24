// 原位置：员工考勤系统/attendance/FileOperation.h。
// 交付版调整：集中管理考勤文件读写，沿用日期、员工编号和状态的原格式。
#pragma once

#include <filesystem>

class Basic;

class AttendanceStorage {
public:
    static bool save(const Basic& attendance, const std::filesystem::path& path);
    static bool load(Basic& attendance, const std::filesystem::path& path);
};
