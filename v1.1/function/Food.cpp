// 整合新增：首见于不疯狂动物城/src/food/Food.cpp，用于食物购买目录。
#include "Food.h"

Food::Food() : foodId(""), foodName(""), pricePerKg(0) {}

Food::Food(const string& id, const string& name, Money price) : foodId(id), foodName(name), pricePerKg(price) {}

const string& Food::getFoodId() const {
    return foodId;
}

const string& Food::getFoodName() const {
    return foodName;
}

Money Food::getPricePerKg() const {
    return pricePerKg;
}

void Food::setFoodPrice(Money price) {
    pricePerKg = price;
}
