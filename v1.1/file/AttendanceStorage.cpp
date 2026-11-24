// 原位置：员工考勤系统/attendance/FileOperation.cpp。
// 交付版调整：集中管理考勤文件读写，沿用日期、员工编号和状态的原格式。
#include "AttendanceStorage.h"

#include <fstream>
#include <memory>
#include <sstream>
#include <utility>

#include "../attendance/Basic.h"

bool AttendanceStorage::save(const Basic& attendance, const std::filesystem::path& path) {
    std::ofstream file(path);
    for (int dayIndex = 0; dayIndex < attendance.dayCount(); ++dayIndex) {
        const dayRecord& day = *attendance.dayAt(dayIndex);
        for (int i = 0; i < day.Recordcnt; ++i) {
            const Record& record = day.dayrecords[i];
            file << day.date.year() << ' ' << day.date.month() << ' ' << day.date.day()
                 << ' ' << record.ID << ' ' << record.status << '\n';
        }
    }
    file.close();
    return !file.fail();
}

bool AttendanceStorage::load(Basic& attendance, const std::filesystem::path& path) {
    std::ifstream file(path);
    if (!file) {
        return false;
    }
    auto loaded = std::make_unique<Basic>();
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) {
            continue;
        }
        std::istringstream input(line);
        int year, month, day, status;
        std::string id;
        if (!(input >> year >> month >> day >> id >> status)) {
            return false;
        }
        const Date date(year, month, day);
        if (!date.isValid()) {
            return false;
        }
        loaded->addRecord(date, id, status);
    }
    if (file.bad()) {
        return false;
    }
    attendance = std::move(*loaded);
    return true;
}
