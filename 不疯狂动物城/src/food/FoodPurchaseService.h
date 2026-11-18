#pragma once
#include <string>
#include <vector>
#include "Food.h"
#include "FoodPurchaseRecord.h"
#include "../common/Employee.h"

using namespace std;
class BankService; 
class FoodPurchaseService {
private:
    vector<Food> foods;
    vector<FoodPurchaseRecord> purchases;  // [Qt 前端支持] 已完成的购买单，用于界面历史表。
public:
    bool addFood(const Food& food, string& message);
    int findFoodIndex(const string& foodId) const;
    const vector<Food>& getFoods() const;
    bool saveFoods(const string& filename);
    bool loadFoods(const string& filename);
    const vector<FoodPurchaseRecord>& getPurchaseRecords() const;
    bool savePurchases(const string& filename);
    bool loadPurchases(const string& filename);
     bool purchaseFood(BankService& bank, const Employee& employee, const string& foodId, int quantity, int& transactionId, string& message);
};


