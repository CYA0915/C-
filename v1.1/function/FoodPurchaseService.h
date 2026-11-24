// 整合新增：首见于不疯狂动物城/src/food/FoodPurchaseService.h。
// 业务接口：通过动物园员工的账户，复用银行管理系统.rar 内的 src/bank/BankService.h 的资金操作。
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
    friend class CityStorage;
    vector<Food> foods;
    vector<FoodPurchaseRecord> purchases;  // 已完成的购买单，用于界面历史表。
public:
    bool addFood(const Food& food, string& message);
    int findFoodIndex(const string& foodId) const;
    const vector<Food>& getFoods() const;
    const vector<FoodPurchaseRecord>& getPurchaseRecords() const;
    // 原位置：食品目录和购买单的 save/load 接口已合入 file/CityStorage。
    bool purchaseFood(BankService& bank, const Employee& employee, const string& foodId, int quantity, int& transactionId, string& message);
};
