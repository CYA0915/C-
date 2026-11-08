#ifndef FEEDRECORD_H
#define FEEDRECORD_H
#include <string>
using namespace std;

struct feedrecord { // [需补全] 关联实际购买的食物。
// [补全思路] 补充购买编号、食物编号；当前记录归属于动物对象，可由动物编号定位；数量统一换算为kg后写入现有amount。
// [补全思路] 保存和读取同步扩展；本文件末尾现有#endif#pragma once待后续拆成合法指令，本次不修改原代码。
    string date;
    double amount;
};

#endif
