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
    // 只读记录访问，供界面文本区展示；不修改内部 vector。
    const vector<feedrecord>& getfeedrecords() const;
    const vector<healthrecord>& gethealthrecords() const;
};

class bird : public animal {
protected:
    double winglength;
public:
    bird(const string& id, const string& name, double winglength);
    string getcategory() const override;
    // 只读访问翼展，供表格显示。
    double getwinglength() const { return winglength; }
};

class mammal : public animal {
protected:
    string dietType;
public:
    mammal(const string& id, const string& name, const string& dietType);
    string getcategory() const override;
    // 只读访问食性，供表格显示。
    string getdiettype() const { return dietType; }
};

class eagle : public bird {
public:
    eagle(const string& id, const string& name, double winglength);
    void display() const override;
    string getspecies() const override;
    string saveline() const override;
};

class parrot : public bird {
public:
    parrot(const string& id, const string& name, double winglength);
    void display() const override;
    string getspecies() const override;
    string saveline() const override;
};

class lion : public mammal {
public:
    lion(const string& id, const string& name);
    void display() const override;
    string getspecies() const override;
    string saveline() const override;
};

class elephant : public mammal {
public:
    elephant(const string& id, const string& name);
    void display() const override;
    string getspecies() const override;
    string saveline() const override;
};

#endif
