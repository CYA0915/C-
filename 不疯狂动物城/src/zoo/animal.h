#ifndef ANIMAL_H
#define ANIMAL_H
#include <iostream>
#include <string>
#include <vector>
#include "feedrecord.h"
#include "healthrecord.h"
#include "../common/Employee.h"
using namespace std;

class animal : public Employee{ // [需补全] 将animal改为公开继承Employee。
// [补全思路] 包含../common/Employee.h；删去本类重复id/name，改用父类身份；饲养、健康记录及现有动物继承体系继续保留。
// [补全思路] 增加所有动物都可查询的食性接口，鸟类也要能判断可购买的食物；可约定肉食、草食、杂食三种值。
protected:
    vector<feedrecord> feedrecords;
    vector<healthrecord> healthrecords;
public:
    animal(const string& id, const string& name); // [需补全] 通过初始化列表构造员工父类。
    // [补全思路] 调用Employee(id,name,基本工资等)，其余成员正常初始化；鸟类、哺乳类、具体动物的构造链同步传递。
    virtual ~animal();
    string getid() const; // [需补全] 沿用旧函数名，返回统一员工编号。
    // [补全思路] 函数内部改调Employee::id()；考勤、开户、工资和购买记录全部使用这个编号。
    string getname() const; // [需补全] 改为读取父类姓名。
    // [补全思路] 返回Employee::name()，避免动物姓名与员工姓名出现两份副本。
    virtual void display() const = 0;
    virtual string getcategory() const = 0;
    virtual string getspecies() const = 0;
    virtual string saveline() const = 0; // [需补全] 扩展各动物类型的保存格式。
    // [补全思路] 各子类saveline保存员工公共信息和动物专属属性；先统一字段顺序，再同步修改zoo::load。
    virtual vector<string> saverecords() const; // [需补全] 保存购买与进食的关联标识。
    // [补全思路] 为饲养记录加入食物和购买编号，并在zoo::load中恢复；约定购买后立即进食，避免本阶段引入背包。
    void setname(const string& n) { 
        Employee::setName(n); 
    } 
    void setid(const string& i) { 
        Employee::setId(i); 
    } 
    void addfeedrecord(const string& date, double amount); // [需补全] 扩展进食记录的业务来源。
    // [补全思路] 新增食物编号、购买编号等参数；付费进食只在购买支付确认成功后记录，按购买编号防止重复记入。
    void addhealthrecord(const string& date, const string& status, const string& note);
    void showrecords() const;
};

class bird : public animal { // [需补全] 适配员工构造链与公共字段访问。
// [补全思路] bird构造参数增加员工资料并传给animal；具体动物display/saveline用父类id()/name()，不再读取已移除的id/name。
protected:
    double winglength;
public:
    bird(const string& id, const string& name, double winglength);
    string getcategory() const override;
};

class mammal : public animal { // [需补全] 适配员工构造链与公共字段访问。
// [补全思路] mammal构造参数增加员工资料并传给animal；具体动物display/saveline用父类id()/name()，不再读取已移除的id/name。
protected:
    string dietType;
public:
    mammal(const string& id, const string& name, const string& dietType);
    string getcategory() const override;
};

class eagle : public bird { // [需补全] 适配员工构造链与公共字段访问。
// [补全思路] eagle构造参数增加员工资料并传给bird；具体动物display/saveline用父类id()/name()，不再读取已移除的id/name。
public:
    eagle(const string& id, const string& name, double winglength);
    void display() const override;
    string getspecies() const override;
    string saveline() const override;
};

class parrot : public bird { // [需补全] 适配员工构造链与公共字段访问。
// [补全思路] parrot构造参数增加员工资料并传给bird；具体动物display/saveline用父类id()/name()，不再读取已移除的id/name。
public:
    parrot(const string& id, const string& name, double winglength);
    void display() const override;
    string getspecies() const override;
    string saveline() const override;
};

class lion : public mammal { // [需补全] 适配员工构造链与公共字段访问。
// [补全思路] lion构造参数增加员工资料并传给mammal；具体动物display/saveline用父类id()/name()，不再读取已移除的id/name。
public:
    lion(const string& id, const string& name);
    void display() const override;
    string getspecies() const override;
    string saveline() const override;
};

class elephant : public mammal { // [需补全] 适配员工构造链与公共字段访问。
// [补全思路] elephant构造参数增加员工资料并传给mammal；具体动物display/saveline用父类id()/name()，不再读取已移除的id/name。
public:
    elephant(const string& id, const string& name);
    void display() const override;
    string getspecies() const override;
    string saveline() const override;
};

#endif