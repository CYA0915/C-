#ifndef ZOO_H
#define ZOO_H
#include <vector>
#include <string>
#include "animal.h"
using namespace std;

class zoo { // [需补全] 统一管理动物员工对象。
// [补全思路] 现有animals集合是员工对象唯一来源；其他模块通过findbyid查询，再按Employee引用使用，避免复制成独立员工。
// [补全思路] 为文件路径增加统一参数；当前固定相对路径先保留，原数据目录已删除，恢复持久化时需创建数据目录。
private:
    vector<animal*> animals;
public:
    ~zoo();
    void addanimal(animal* a); // [需补全] 检查编号唯一并明确指针所有权。
    // [补全思路] 先按getid查询，重复则拒绝；约定失败时由调用者释放，成功后由zoo负责释放，避免泄漏或重复删除。
    void showall() const;
    animal* findbyid(const string& id) const;
    bool removeanimal(const string& id); // [需补全] 保护历史考勤、账户和工资关联。
    // [补全思路] 存在历史业务的动物改为离职或停用；有余额、欠款、待发工资时拒绝直接物理删除。
    int count() const;
    void printbycategory() const;
    bool updateanimal(const string& id, const string& newname);
    bool feedanimal(const string& id, const string& date, double amount); // [需补全] 连接购买食物与原有饲养记录。
    // [补全思路] 增加食物/购买编号参数；检查动物、食性、数量和支付结果，再调用addfeedrecord；同一购买只记录一次。
    bool checkhealth(const string& id, const string& date, const string& status, const string& note);
    void showrecords(const string& id) const;
    void save() const; // [需补全] 保存扩展后的动物员工及记录。
    // [补全思路] 增加基本工资、账户绑定、在职状态等字段；返回明确的保存结果；写临时文件并检查失败，勿把多个文件依次保存视为整体原子操作。
    void load(); // [需补全] 恢复员工公共字段和动物子类。
    // [补全思路] 按物种构造对象并恢复工资、账户、状态及进食关联；检查重复编号和数字转换异常，约定旧格式的默认值。
};

#endif
