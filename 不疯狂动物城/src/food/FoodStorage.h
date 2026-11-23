#pragma once
#include <string>
#include <vector>
#include "Food.h"
#include "FoodPurchaseRecord.h"
using namespace std;

class FoodStorage {
public:
    bool SaveFood(const vector<Food>& foods, const string& filename);
    bool LoadFood(vector<Food>& foods, const string& filename);
    // 购买记录的持久化；竖线分隔，避免名称中的空格破坏字段。
    bool SavePurchases(const vector<FoodPurchaseRecord>& records, const string& filename);
    bool LoadPurchases(vector<FoodPurchaseRecord>& records, const string& filename);
};
