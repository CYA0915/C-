// 整合新增：首见于不疯狂动物城/src/food/FoodPurchaseService.cpp。
// 业务接口：通过动物园员工的账户，复用银行管理系统.rar 内的 src/bank/BankService.cpp 的资金操作。
#include "FoodPurchaseService.h"
#include <limits>
#include <utility>
#include "../bank/BankService.h"
#include "../common/Date.h"

bool FoodPurchaseService::addFood(const Food& food, string& message) {
    // 整合时新增食物目录。这里在录入时校验编号、名称、单价和用于存档的分隔符。
    const auto& id = food.getFoodId();
    const auto& name = food.getFoodName();
    if (id.empty() || name.find_first_not_of(" \t") == string::npos || food.getPricePerKg() <= 0) {
        message = "请填写食物编号、名称和大于 0 的单价";
        return false;
    }
    if (id.find_first_of(" \t\r\n|") != string::npos || name.find_first_of("\r\n|") != string::npos) {
        message = "食物编号或名称含有文件分隔符";
        return false;
    }
    // 编号唯一，避免 findFoodIndex 命中错误项。
    if (findFoodIndex(id) != -1) {
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

const vector<FoodPurchaseRecord>& FoodPurchaseService::getPurchaseRecords() const {
    return purchases;
}

bool FoodPurchaseService::purchaseFood(BankService& bank, const Employee& employee, const string& foodId, int quantity, int& transactionId, string& message) {
    transactionId = 0;

    BankAccount* account = bank.findAccountForOwner(employee.bankAccountNo(), employee.id());
    if (!account) {
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
    const Money unitPrice = food.getPricePerKg();
    if (unitPrice <= 0) {
        message = "食物单价必须大于 0";
        return false;
    }
    if (unitPrice > std::numeric_limits<Money>::max() / quantity) {
        message = "购买总价超出金额范围";
        return false;
    }
    const Money totalCost = unitPrice * quantity;

    string info = "员工 " + employee.id() + " 购买食物 " + food.getFoodId() + " " + food.getFoodName() +
    " 数量 " + to_string(quantity) +  "kg" + ",单价" +
    moneyToString(food.getPricePerKg()) + "元/公斤，总价" + moneyToString(totalCost) + "元";

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
    record.unitPrice = unitPrice;
    record.amount = totalCost;
    record.accountNo = account->accountNo();
    record.date = Date::today().toString();
    record.status = "已支付";
    // 食物扣款是整合功能，交付版复用银行原有的取款规则与交易流水。
    if (!bank.postTransaction(account->accountNo(), totalCost, TX_FOOD_PURCHASE, info,
                              transactionId, message)) {
        return false;
    }
    record.bankTransactionId = transactionId;
    purchases.push_back(std::move(record));
    message = "购买成功，支付" + moneyToString(totalCost)
        + "元，当前余额：" + moneyToString(account->balance()) + "元";
    return true;
}
