#ifndef ANIMAL_H
#define ANIMAL_H
#include <iostream>
#include <string>
#include <vector>
#include "feedrecord.h"
#include "healthrecord.h"
#include "../common/Employee.h"
using namespace std;

class animal : public Employee{
protected:
    vector<feedrecord> feedrecords;
    vector<healthrecord> healthrecords;
public:
    animal(const string& id, const string& name);
    virtual ~animal();
    string getid() const; 
    string getname() const; 
    virtual void display() const = 0;
    virtual string getcategory() const = 0;
    virtual string getspecies() const = 0;
    virtual string saveline() const = 0;
    virtual vector<string> saverecords() const;
    void setname(const string& n) { 
        Employee::setName(n); 
    } 
    void setid(const string& i) { 
        Employee::setId(i); 
    } 
    void addfeedrecord(const string& date, double amount); 
    void addhealthrecord(const string& date, const string& status, const string& note);
    void showrecords() const;
    // [Qt 前端支持] 只读记录访问，供界面文本区展示；不修改内部 vector。
    const vector<feedrecord>& getfeedrecords() const;
    const vector<healthrecord>& gethealthrecords() const;
};

class bird : public animal { // [需补全] 适配员工构造链与公共字段访问。
// [补全思路] bird构造参数增加员工资料并传给animal；具体动物display/saveline用父类id()/name()，不再读取已移除的id/name。
protected:
    double winglength;
public:
    bird(const string& id, const string& name, double winglength);
    string getcategory() const override;
    // [Qt 前端支持] 只读访问翼展，供表格显示。
    double getwinglength() const { return winglength; }
};

class mammal : public animal { // [需补全] 适配员工构造链与公共字段访问。
// [补全思路] mammal构造参数增加员工资料并传给animal；具体动物display/saveline用父类id()/name()，不再读取已移除的id/name。
protected:
    string dietType;
public:
    mammal(const string& id, const string& name, const string& dietType);
    string getcategory() const override;
    // [Qt 前端支持] 只读访问食性，供表格显示。
    string getdiettype() const { return dietType; }
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