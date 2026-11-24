// 整合新增：首见于不疯狂动物城/src/food/Food.h，用于食物购买目录。
#pragma once
#include <string>
#include "../common/Money.h"

using namespace std;

class Food {
private:
    string foodId;
    string foodName;
    Money pricePerKg = 0;
public:
    Food();
    Food(const string& id, const string& name, Money price);
    const string& getFoodId() const;
    const string& getFoodName() const;
    Money getPricePerKg() const;
    void setFoodPrice(Money price);
};
