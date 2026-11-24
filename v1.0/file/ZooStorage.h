// 原位置：动物园管理系统/zoo.h 中的 zoo::load 和 zoo::save。
// 交付版调整：从动物管理类拆出文件读写，兼容只含动物资料的旧文件。
#pragma once

#include <filesystem>

class zoo;

class ZooStorage {
public:
    static bool load(zoo& animals, const std::filesystem::path& directory);
    static bool save(const zoo& animals, const std::filesystem::path& directory);
};
