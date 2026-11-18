#include "FoodStorage.h"
#include <fstream>
#include <sstream>

bool FoodStorage::SaveFood(const vector<Food>& foods, const string& filename) {
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

bool FoodStorage::LoadFood(vector<Food>& foods, const string& filename){
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
        string name;
        Money price;
        if(!(iss >> id >> name >> price)){
            return false; 
        }
        Food food(id, name, price);
        tempFoods.push_back(food);
    }
    if(file.bad()){
        return false; 
    }
    foods = tempFoods;
    file.close();
    return true;
}


// [Qt 前端支持] 购买记录保存：每行 11 个字段，用 | 分隔；先写临时容器由调用方决定是否替换。
bool FoodStorage::SavePurchases(const vector<FoodPurchaseRecord>& records, const string& filename) {
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

// [Qt 前端支持] 购买记录读取：先读入临时 vector，全部成功后再替换，避免半份数据。
bool FoodStorage::LoadPurchases(vector<FoodPurchaseRecord>& records, const string& filename) {
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
