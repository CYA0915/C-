#pragma once
//日期和考勤记录结构体声明
#include <string>
#include "../common/Date.h"

using namespace std;

struct Record { // [需补全] 明确动物员工身份和状态取值。
// [补全思路] ID统一为animal的员工编号；保留0缺勤、1出勤、2请假；无记录表示未录入，不能直接算缺勤。
    string ID;
    int status;//0:缺勤 1:出勤 2:请假
    //创建数组时无参构造，添加记录使有参构造
    Record() : ID(""),  status(0) {}
    Record(string ID, int status) : ID(ID), status(status) {}
};

struct dayRecord { // [需补全] 配合月度统计和容量约束。
// [补全思路] 按日期和员工编号唯一；固定数组暂可复用，超过366个日期/每天100条时返回失败，后续需要多年记录再改容器。
    Date date;
    Record dayrecords[100];//一天最多一百条
    int Recordcnt;
    dayRecord() : Recordcnt(0) {}

};
