// 交付版新增：统一调度三个基础系统以及工资、食物模块的文件读写。
// 基础来源：员工考勤系统/attendance/FileOperation.cpp、动物园管理系统/zoo.cpp 的存档接口、
// 以及银行管理系统.rar 内的 src/bank/BankStorage.cpp 的账户和流水文件接口。
#include "CityStorage.h"

#include <array>
#include <utility>

#include "AttendanceStorage.h"
#include "BankStorage.h"
#include "FoodStorage.h"
#include "PayrollStorage.h"
#include "ZooStorage.h"
#include "../function/CityService.h"

namespace fs = std::filesystem;

namespace {

constexpr const char* animalsFile = "animals.txt";
constexpr const char* recordsFile = "records.txt";
constexpr const char* attendanceFile = "attendance.txt";
constexpr const char* payrollFile = "payroll.txt";
constexpr const char* foodFile = "foods.txt";
constexpr const char* purchaseFile = "purchases.txt";
constexpr const char* bankDirectory = "data";
constexpr std::array<const char*, 8> dataFiles = {
    animalsFile, recordsFile, attendanceFile, payrollFile, foodFile, purchaseFile,
    "data/accounts.csv", "data/transactions.csv"
};

struct StagedFiles {
    fs::path directory;
    bool keepForRecovery = false;

    ~StagedFiles() {
        if (!keepForRecovery) {
            std::error_code ignored;
            fs::remove_all(directory, ignored);
        }
    }
};

// 逐个替换同一组文件。发布失败时将此前已替换的文件恢复到保存前。
bool publish(StagedFiles& stage, const fs::path& destination, std::string& message) {
    const fs::path backup = stage.directory / "previous";
    fs::create_directories(backup / bankDirectory);
    std::array<bool, dataFiles.size()> existed{};
    for (std::size_t i = 0; i < dataFiles.size(); ++i) {
        const auto current = destination / dataFiles[i];
        existed[i] = fs::exists(current);
        if (existed[i]) {
            fs::copy_file(current, backup / dataFiles[i]);
        }
    }

    for (std::size_t i = 0; i < dataFiles.size(); ++i) {
        std::error_code error;
        fs::rename(stage.directory / dataFiles[i], destination / dataFiles[i], error);
        if (!error) {
            continue;
        }
        for (std::size_t j = i; j > 0; --j) {
            const auto index = j - 1;
            std::error_code restoreError;
            const auto target = destination / dataFiles[index];
            if (existed[index]) {
                fs::rename(backup / dataFiles[index], target, restoreError);
            } else {
                fs::remove(target, restoreError);
            }
            stage.keepForRecovery = stage.keepForRecovery || static_cast<bool>(restoreError);
        }
        message = std::string("保存失败：") + dataFiles[i] + "，" + error.message();
        if (stage.keepForRecovery) {
            message += "。部分文件恢复失败，备份保留在 " + stage.directory.string();
        }
        return false;
    }
    return true;
}

}

CityStorage::CityStorage(fs::path directory) : directory_(std::move(directory)) {
}

bool CityStorage::loadAll(CityService& city, std::string& message) const {
    CityService loaded;
    auto& state = *loaded.state_;
    PayrollStorage payroll;
    FoodStorage food;
    const auto failed = [&](const char* area) {
        message = std::string("读取失败：") + area;
        return false;
    };

    try {
        if (fs::exists(directory_ / animalsFile) && !ZooStorage::load(state.animals, directory_)) {
            return failed(animalsFile);
        }
        if (fs::exists(directory_ / attendanceFile) &&
            !AttendanceStorage::load(state.attendance, directory_ / attendanceFile)) {
            return failed(attendanceFile);
        }
        if (!BankStorage(directory_ / bankDirectory).load(state.bank)) {
            return failed(bankDirectory);
        }
        if (fs::exists(directory_ / payrollFile) &&
            !payroll.Load(state.payroll.records, directory_ / payrollFile)) {
            return failed(payrollFile);
        }
        if (fs::exists(directory_ / foodFile) && !food.LoadFood(state.food.foods, directory_ / foodFile)) {
            return failed(foodFile);
        }
        if (fs::exists(directory_ / purchaseFile) &&
            !food.LoadPurchases(state.food.purchases, directory_ / purchaseFile)) {
            return failed(purchaseFile);
        }
    } catch (const fs::filesystem_error& error) {
        return failed(error.what());
    }

    city.state_.swap(loaded.state_);
    message = "已加载动物、考勤、账户、工资和食物数据";
    return true;
}

bool CityStorage::saveAll(const CityService& city, std::string& message) const {
    const auto& state = *city.state_;
    try {
        fs::create_directories(directory_ / bankDirectory);
        const auto stageDirectory = directory_ / ".zoocity-save";
        if (!fs::create_directory(stageDirectory)) {
            message = "已有保存操作或待恢复的文件：" + stageDirectory.string();
            return false;
        }
        StagedFiles stage{stageDirectory};
        fs::create_directories(stage.directory / bankDirectory);
        PayrollStorage payroll;
        FoodStorage food;
        if (!ZooStorage::save(state.animals, stage.directory)
            || !AttendanceStorage::save(state.attendance, stage.directory / attendanceFile)
            || !BankStorage(stage.directory / bankDirectory).save(state.bank)
            || !payroll.Save(state.payroll.records, stage.directory / payrollFile)
            || !food.SaveFood(state.food.foods, stage.directory / foodFile)
            || !food.SavePurchases(state.food.purchases, stage.directory / purchaseFile)) {
            message = "数据写入暂存目录失败";
            return false;
        }
        if (!publish(stage, directory_, message)) {
            return false;
        }
    } catch (const fs::filesystem_error& error) {
        message = std::string("保存失败：") + error.what();
        return false;
    }
    message = "已保存全部业务数据";
    return true;
}
