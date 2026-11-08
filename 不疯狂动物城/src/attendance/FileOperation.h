#pragma once
#include "Record.h"
#include "Basic.h"

using namespace std;

class FileOperation { // [需补全] 适配公共Date和统一文件位置。
// [补全思路] 保持考勤读写职责，新增员工编号与状态校验；路径由调用者指定，数据文件在后续启用存储时创建。
public:
    bool Save(const Basic& basic, const string& filename);//const只读 // [需补全] 适配统一日期并检查完整写入。
    // [补全思路] 日期输出改为公共Date访问接口；先写临时文件，完成并检查后替换，失败返回false。
    bool Load(Basic& basic, const string& filename); // [需补全] 避免坏数据导致只导入一半。
    // [补全思路] 先读入临时Basic并检查日期、员工编号、状态和重复项，全部有效后再替换目标；明确覆盖或合并规则。
};