// 整合新增：首见于不疯狂动物城/src/food/FoodStorage.cpp，读写食物目录及购买单。
#include "FoodStorage.h"
#include <fstream>
#include <sstream>

bool FoodStorage::SaveFood(const vector<Food>& foods, const std::filesystem::path& filename) {
    ofstream file(filename);
    if(!file.is_open()) {
        return false;
    }
    int n = foods.size();
    for(int i = 0; i < n; i++){
        file << foods[i].getFoodId() << " "
             << foods[i].getFoodName() << " "
             << foods[i].getPricePerKg() << endl;
    }
    file.close();
    if(file.fail()){
        return false;
    }
    return true;
}

bool FoodStorage::LoadFood(vector<Food>& foods, const std::filesystem::path& filename){
    ifstream file(filename);
    if(!file.is_open()) {
        return false;
    }
    vector<Food> tempFoods;
    string line;
    while(getline(file, line)) {
        if(line.empty()) {
            continue;
        }
        istringstream iss(line);
        string id;
        string nameAndPrice;
        if (!(iss >> id) || !getline(iss >> ws, nameAndPrice)) {
            return false;
        }
        // 沿用整合版目录格式。首字段是编号，末字段是价格，中间是名称。
        const auto end = nameAndPrice.find_last_not_of(" \t\r");
        const auto delimiter = nameAndPrice.find_last_of(" \t", end);
        if (end == string::npos || delimiter == string::npos) {
            return false;
        }
        const auto nameEnd = nameAndPrice.find_last_not_of(" \t", delimiter);
        istringstream priceText(nameAndPrice.substr(delimiter + 1));
        Money price;
        char extra;
        if (nameEnd == string::npos || !(priceText >> price) || (priceText >> extra)) {
            return false;
        }
        tempFoods.emplace_back(id, nameAndPrice.substr(0, nameEnd + 1), price);
    }
    if(file.bad()){
        return false;
    }
    foods = tempFoods;
    file.close();
    return true;
}

// 购买记录保存：每行 11 个字段，用 | 分隔。
bool FoodStorage::SavePurchases(const vector<FoodPurchaseRecord>& records, const std::filesystem::path& filename) {
    ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    for (size_t i = 0; i < records.size(); ++i) {
        const FoodPurchaseRecord& r = records[i];
        file << r.purchaseId << '|'
             << r.employeeId << '|'
             << r.foodId << '|'
             << r.foodName << '|'
             << r.quantity << '|'
             << r.unitPrice << '|'
             << r.amount << '|'
             << r.accountNo << '|'
             << r.date << '|'
             << r.status << '|'
             << r.bankTransactionId << '\n';
    }
    file.close();
    return !file.fail();
}

// 购买记录读取：先读入临时 vector，全部成功后再替换，避免半份数据。
bool FoodStorage::LoadPurchases(vector<FoodPurchaseRecord>& records, const std::filesystem::path& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    vector<FoodPurchaseRecord> temp;
    string line;
    while (getline(file, line)) {
        if (line.empty()) {
            continue;
        }
        istringstream iss(line);
        FoodPurchaseRecord r;
        string quantity, unitPrice, amount, transactionId;
        if (!getline(iss, r.purchaseId, '|') ||
            !getline(iss, r.employeeId, '|') ||
            !getline(iss, r.foodId, '|') ||
            !getline(iss, r.foodName, '|') ||
            !getline(iss, quantity, '|') ||
            !getline(iss, unitPrice, '|') ||
            !getline(iss, amount, '|') ||
            !getline(iss, r.accountNo, '|') ||
            !getline(iss, r.date, '|') ||
            !getline(iss, r.status, '|') ||
            !getline(iss, transactionId, '|')) {
            return false;
        }
        try {
            r.quantity = stoi(quantity);
            r.unitPrice = stoll(unitPrice);
            r.amount = stoll(amount);
            r.bankTransactionId = stoi(transactionId);
        } catch (...) {
            return false;
        }
        temp.push_back(r);
    }
    if (file.bad()) {
        return false;
    }
    records = temp;
    return true;
}
