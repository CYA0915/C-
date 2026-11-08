#pragma once
//函数声明
#include "Record.h"
#include <vector>
#include "AttendanceSummary.h"

using namespace std;

const int MAX_DAYS = 366;
const int MAX_RECORDS = 100;

class Basic{ // [需补全] 增加按动物员工、月份汇总的接口。
// [补全思路] 建议summarize(employeeId,year,month,排班信息)返回AttendanceSummary；遍历该月记录，仅统计应出勤日期，并单列未录入天数。
// [补全思路] 统一Date后同步修改Basic.cpp所有日期比较与显示；新增接口返回结果，不只向cout打印。
    friend class FileOperation;
private:
    dayRecord day[MAX_DAYS];
    int Daycnt;//天数
public:
    Basic();
    void addRecord(Date date, string ID,int status); // [需补全] 校验动物员工存在和输入范围。
    // [补全思路] 调用方或传入的zoo查询确认ID有效，再检查Date合法、状态0到2、当日唯一；返回成功或失败原因。
    void deleteRecord(Date date, string ID,int status); // [需补全] 保护已发工资的历史考勤。
    // [补全思路] 核对所属月份是否已结算；已发放月份拒绝直接删除，需要单独更正流程；未结算记录正常删除并返回结果。
    void searchRecord(Date date, string ID);
    void changeRecord(Date date, string ID); // [需补全] 把新状态作为参数传入并保护已结算月份。
    // [补全思路] 新增newStatus参数，去掉业务函数内部cin；检查0到2及工资状态，修改成功后返回结果。
    //search重载部分
    void searchRecord(Date date);
    void searchRecord(string ID);
    bool summarize(
        const string& employeed,
        int year,
        int month,
        const vector<Date>& scheduledDates,
        AttendanceSummary& result,
        string& message
    )const;

    int getRecord(Date date, Record result[]) const; // [需补全] 支持安全读取并供月度统计复用。
    // [补全思路] 明确结果数组容量，或改为返回vector<Record>；按月统计可逐日读取，避免越界复制。
};