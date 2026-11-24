// v1.1：沿用 v1.0 的各类文件读写，每次保存发布一份完整数据并保留上一份。
#include "CityStorage.h"

#include <algorithm>
#include <array>
#include <chrono>
#include <cstdint>
#include <fstream>
#include <iterator>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <utility>
#include <vector>

#include "AttendanceStorage.h"
#include "BankStorage.h"
#include "FoodStorage.h"
#include "PayrollStorage.h"
#include "ZooStorage.h"
#include "../function/CityService.h"

namespace fs = std::filesystem;

namespace {

const std::string snapshotPrefix = "snapshot-";
const char* animalsFile = "animals.txt";
const char* attendanceFile = "attendance.txt";
const char* payrollFile = "payroll.txt";
const char* foodFile = "foods.txt";
const char* purchasesFile = "purchases.txt";
const char* bankDirectory = "data";
const std::array<const char*, 8> dataFiles = {
    animalsFile, "records.txt", attendanceFile, payrollFile, foodFile, purchasesFile,
    "data/accounts.csv", "data/transactions.csv"
};

// 用简单的按字节校验检查八个文件是否完整、内容是否发生变化。
bool snapshotChecks(const fs::path& directory, std::string& result) {
    std::ostringstream checks;
    for (const char* name : dataFiles) {
        std::ifstream input(directory / name, std::ios::binary);
        if (!input) {
            return false;
        }
        std::uint64_t sum = 14695981039346656037ULL;
        char byte;
        while (input.get(byte)) {
            sum ^= static_cast<unsigned char>(byte);
            sum *= 1099511628211ULL;
        }
        if (input.bad()) {
            return false;
        }
        checks << name << ' ' << sum << '\n';
    }
    result = checks.str();
    return true;
}

struct Snapshot {
    long long number;
    fs::path path;
};

std::vector<Snapshot> listSnapshots(const fs::path& directory) {
    std::vector<Snapshot> snapshots;
    if (!fs::exists(directory)) {
        return snapshots;
    }
    for (const auto& entry : fs::directory_iterator(directory)) {
        const auto name = entry.path().filename().string();
        if (name.compare(0, snapshotPrefix.size(), snapshotPrefix) != 0 ||
            entry.is_symlink() || !entry.is_directory()) {
            continue;
        }
        const auto numberText = name.substr(snapshotPrefix.size());
        if (numberText.empty() || numberText.find_first_not_of("0123456789") != std::string::npos) {
            continue;
        }
        try {
            const long long number = std::stoll(numberText);
            if (number > 0 && std::to_string(number) == numberText) {
                snapshots.push_back({number, entry.path()});
            }
        } catch (const std::out_of_range&) {
            continue;
        }
    }
    std::sort(snapshots.begin(), snapshots.end(),
              [](const Snapshot& a, const Snapshot& b) { return a.number > b.number; });
    return snapshots;
}

struct TemporaryDirectory {
    fs::path path;
    ~TemporaryDirectory() {
        std::error_code ignored;
        fs::remove_all(path, ignored);
    }
};

}

CityStorage::CityStorage(fs::path directory) : directory_(std::move(directory)) {
}

// 原位置：v1.0/file/CityStorage.cpp 的 loadAll 与 saveAll 对各存储类的调用。
bool CityStorage::loadFiles(CityService& city, const fs::path& directory, bool checkSnapshot,
                            std::string& message) const {
    if (checkSnapshot) {
        std::ifstream manifest(directory / "snapshot.check", std::ios::binary);
        std::string actual;
        if (!manifest || !snapshotChecks(directory, actual) ||
            std::string(std::istreambuf_iterator<char>(manifest), {}) != actual) {
            message = "快照文件缺失或校验值不符";
            return false;
        }
    }
    auto& state = *city.state_;
    const auto failed = [&](const char* name) {
        message = std::string("读取失败：") + name;
        return false;
    };
    PayrollStorage payroll;
    FoodStorage food;
    if (fs::exists(directory / animalsFile) && !ZooStorage::load(state.animals, directory)) {
        return failed(animalsFile);
    }
    if (fs::exists(directory / attendanceFile) &&
        !AttendanceStorage::load(state.attendance, directory / attendanceFile)) {
        return failed(attendanceFile);
    }
    if (!BankStorage(directory / bankDirectory).load(state.bank)) {
        return failed(bankDirectory);
    }
    if (fs::exists(directory / payrollFile) &&
        !payroll.Load(state.payroll.records, directory / payrollFile)) {
        return failed(payrollFile);
    }
    if (fs::exists(directory / foodFile) && !food.LoadFood(state.food.foods, directory / foodFile)) {
        return failed(foodFile);
    }
    if (fs::exists(directory / purchasesFile) &&
        !food.LoadPurchases(state.food.purchases, directory / purchasesFile)) {
        return failed(purchasesFile);
    }
    return true;
}

bool CityStorage::saveFiles(const CityService& city, const fs::path& directory,
                            std::string& message) const {
    const auto& state = *city.state_;
    PayrollStorage payroll;
    FoodStorage food;
    fs::create_directories(directory / bankDirectory);
    if (!ZooStorage::save(state.animals, directory)
        || !AttendanceStorage::save(state.attendance, directory / attendanceFile)
        || !BankStorage(directory / bankDirectory).save(state.bank)
        || !payroll.Save(state.payroll.records, directory / payrollFile)
        || !food.SaveFood(state.food.foods, directory / foodFile)
        || !food.SavePurchases(state.food.purchases, directory / purchasesFile)) {
        message = "暂存业务数据失败";
        return false;
    }
    std::string checks;
    if (!snapshotChecks(directory, checks)) {
        message = "无法生成快照校验值";
        return false;
    }
    std::ofstream manifest(directory / "snapshot.check", std::ios::binary);
    manifest << checks;
    manifest.close();
    if (!manifest) {
        message = "无法保存快照校验清单";
        return false;
    }
    return true;
}

bool CityStorage::loadAll(CityService& city, std::string& message) {
    try {
        const auto snapshots = listSnapshots(directory_ / "snapshots");
        if (snapshots.empty()) {
            CityService loaded;
            if (!loadFiles(loaded, directory_, false, message)) {
                return false;
            }
            city.state_.swap(loaded.state_);
            latestSeen_ = 0;
            message = "已加载旧格式数据或空数据目录";
            return true;
        }

        std::string firstError;
        for (const Snapshot& snapshot : snapshots) {
            CityService loaded;
            if (loadFiles(loaded, snapshot.path, true, message)) {
                city.state_.swap(loaded.state_);
                latestSeen_ = snapshots.front().number;
                message = firstError.empty()
                    ? "已加载最新数据"
                    : "最新快照无法读取，已从上一份有效备份恢复。原因：" + firstError;
                return true;
            }
            if (firstError.empty()) {
                firstError = message;
            }
        }
        message = "没有可用的完整快照：" + firstError;
    } catch (const fs::filesystem_error& error) {
        message = std::string("读取失败：") + error.what();
    }
    return false;
}

bool CityStorage::saveAll(const CityService& city, std::string& message) {
    try {
        const auto directory = directory_ / "snapshots";
        fs::create_directories(directory);
        const auto snapshots = listSnapshots(directory);
        const long long latest = snapshots.empty() ? 0 : snapshots.front().number;
        if (latest != latestSeen_) {
            message = "磁盘上的数据版本已变化，请重新加载后再保存";
            return false;
        }
        if (latest == std::numeric_limits<long long>::max()) {
            message = "保存版本编号已用完";
            return false;
        }

        const auto timestamp = std::chrono::steady_clock::now().time_since_epoch().count();
        const auto temporaryPath = directory / (".pending-" + std::to_string(timestamp));
        if (!fs::create_directory(temporaryPath)) {
            message = "无法建立保存暂存目录";
            return false;
        }
        TemporaryDirectory staging{temporaryPath};
        if (!saveFiles(city, staging.path, message)) {
            return false;
        }
        CityService checked;
        if (!loadFiles(checked, staging.path, true, message)) {
            message = "暂存数据校验失败：" + message;
            return false;
        }

        // 只有上一份完整可读的数据可以作为备份。
        fs::path backup;
        for (const Snapshot& snapshot : snapshots) {
            CityService candidate;
            std::string ignored;
            if (loadFiles(candidate, snapshot.path, true, ignored)) {
                backup = snapshot.path;
                break;
            }
        }

        const auto destination = directory / (snapshotPrefix + std::to_string(latest + 1));
        fs::rename(staging.path, destination);
        latestSeen_ = latest + 1;
        for (const Snapshot& snapshot : snapshots) {
            if (snapshot.path != backup) {
                std::error_code ignored;
                fs::remove_all(snapshot.path, ignored);
            }
        }
        message = "已保存完整数据快照";
        return true;
    } catch (const fs::filesystem_error& error) {
        message = std::string("保存失败：") + error.what();
        return false;
    }
}
