// 整合新增：首见于不疯狂动物城/src/food/FoodStorage.h，读写食物目录及购买单。
#pragma once
#include <filesystem>
#include <vector>
#include "../function/Food.h"
#include "../function/FoodPurchaseRecord.h"
using namespace std;

class FoodStorage {
public:
    bool SaveFood(const vector<Food>& foods, const std::filesystem::path& filename);
    bool LoadFood(vector<Food>& foods, const std::filesystem::path& filename);
    // 购买记录的持久化；竖线分隔，避免名称中的空格破坏字段。
    bool SavePurchases(const vector<FoodPurchaseRecord>& records, const std::filesystem::path& filename);
    bool LoadPurchases(vector<FoodPurchaseRecord>& records, const std::filesystem::path& filename);
};
