// v1.1：复用原有动物园、考勤、银行、工资和食物存储，统一恢复和保存。
#pragma once

#include <filesystem>
#include <string>

class CityService;

class CityStorage {
public:
    explicit CityStorage(std::filesystem::path directory);

    bool loadAll(CityService& city, std::string& message);
    bool saveAll(const CityService& city, std::string& message);

private:
    bool loadFiles(CityService& city, const std::filesystem::path& directory,
                   bool checkSnapshot, std::string& message) const;
    bool saveFiles(const CityService& city, const std::filesystem::path& directory,
                   std::string& message) const;

    std::filesystem::path directory_;
    long long latestSeen_ = 0;
};
