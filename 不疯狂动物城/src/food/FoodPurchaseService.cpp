#include "FoodPurchaseService.h"
#include "FoodStorage.h"
#include "../bank/BankService.h"
#include "../common/Date.h"

bool FoodPurchaseService::addFood(const Food& food, string& message) {
    // 编号唯一，避免 findFoodIndex 命中错误项。
    if (findFoodIndex(food.getFoodId()) != -1) {
        message = "食物编号已存在";
        return false;
    }
    foods.push_back(food);
    message = "食物添加成功";
    return true;
}

int FoodPurchaseService::findFoodIndex(const string& foodId) const {
    int n = foods.size();
    for (int i = 0; i < n; i++) {
        if (foods[i].getFoodId() == foodId) {
            return i;
        }
    }
    return -1;
}

const vector<Food>& FoodPurchaseService::getFoods() const {
    return foods;
}

bool FoodPurchaseService::saveFoods(const string& filename) {
    FoodStorage storage;
    return storage.SaveFood(foods, filename);
}

bool FoodPurchaseService::loadFoods(const string& filename) {
    FoodStorage storage;
    return storage.LoadFood(foods, filename);
}

const vector<FoodPurchaseRecord>& FoodPurchaseService::getPurchaseRecords() const {
    return purchases;
}

bool FoodPurchaseService::savePurchases(const string& filename) {
    FoodStorage storage;
    return storage.SavePurchases(purchases, filename);
}

bool FoodPurchaseService::loadPurchases(const string& filename) {
    FoodStorage storage;
    return storage.LoadPurchases(purchases, filename);
}

bool FoodPurchaseService::purchaseFood(BankService& bank, const Employee& employee, const string& foodId, int quantity, int& transactionId, string& message) {
    transactionId = 0;

    BankAccount *account = bank.findAccount(employee.bankAccountNo());
    if (!account || account->ownerId() != employee.id()) {
        message = "购买食物必须使用该员工名下的银行账户";
        return false;
    }

    int foodIndex = findFoodIndex(foodId);
    if (foodIndex == -1) {
        message = "未找到对应食物";
        return false;
    }

    if (quantity <= 0) {
        message = "购买数量必须大于 0";
        return false;
    }
    const Food& food = foods[foodIndex];
    if (food.getPricePerKg() <= 0) {
        message = "食物单价必须大于 0";
        return false;
    }
    Money totalCost = food.getPricePerKg() * quantity;

    string info = "员工 " + employee.id() + " 购买食物 " + food.getFoodId() + " " + food.getFoodName() +
    " 数量 " + to_string(quantity) +  "kg" + ",单价" +
    moneyToString(food.getPricePerKg()) + "元/公斤，总价" + moneyToString(totalCost) + "元";

    bool success = bank.payForFood(employee.bankAccountNo(), totalCost, info, transactionId, message);
    if (!success) {
        return false;
    }

    // 支付成功后生成购买单快照；编号在当前集合内唯一。
    FoodPurchaseRecord record;
    record.purchaseId = "FP" + to_string(purchases.size() + 1);
    while (true) {
        bool duplicated = false;
        for (size_t i = 0; i < purchases.size(); ++i) {
            if (purchases[i].purchaseId == record.purchaseId) {
                duplicated = true;
                break;
            }
        }
        if (!duplicated) {
            break;
        }
        record.purchaseId += "X";
    }
    record.employeeId = employee.id();
    record.foodId = food.getFoodId();
    record.foodName = food.getFoodName();
    record.quantity = quantity;
    record.unitPrice = food.getPricePerKg();
    record.amount = totalCost;
    record.accountNo = employee.bankAccountNo();
    record.date = Date::today().toString();
    record.status = "已支付";
    record.bankTransactionId = transactionId;
    purchases.push_back(record);
    return true;
}
