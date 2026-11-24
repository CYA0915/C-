// 原位置：动物园管理系统/zoo.cpp。
#include "zoo.h"
#include <iostream>
using namespace std;

//   动物园集合管理。原稿由邓编写；整合时陈适配了 Employee 继承、
//   工资/账号字段和 Qt 前端只读访问，并修复了源文件中的中文乱码。

zoo::~zoo() {
    clear();
}

void zoo::addanimal(animal* a) {
    if (a == nullptr) {
        return;
    }
    animals.push_back(a);
    cout << "添加成功 " << a->getname() << "(" << a->getcategory() << ")" << endl;
}

void zoo::showall() const {
    if (animals.empty()) {
        cout << "暂无动物" << endl;
        return;
    }
    for (size_t i = 0; i < animals.size(); ++i) {
        animals[i]->display();
    }
}

animal* zoo::findbyid(const string& id) const {
    for (size_t i = 0; i < animals.size(); ++i) {
        if (animals[i]->getid() == id) {
            return animals[i];
        }
    }
    return nullptr;
}

bool zoo::removeanimal(const string& id) {
    for (size_t i = 0; i < animals.size(); ++i) {
        if (animals[i]->getid() == id) {
            delete animals[i];
            animals.erase(animals.begin() + i);
            return true;
        }
    }
    return false;
}

int zoo::count() const {
    return (int)animals.size();
}

const vector<animal*>& zoo::getanimals() const {
    return animals;
}

void zoo::clear() {
    for (size_t i = 0; i < animals.size(); ++i) {
        delete animals[i];
    }
    animals.clear();
}

void zoo::printbycategory() const {
    if (animals.empty()) {
        cout << "暂无动物" << endl;
        return;
    }
    vector<string> categories;
    vector<vector<animal*> > groups;
    for (size_t i = 0; i < animals.size(); ++i) {
        string cat = animals[i]->getcategory();
        int pos = -1;
        for (size_t j = 0; j < categories.size(); ++j) {
            if (categories[j] == cat) {
                pos = (int)j;
                break;
            }
        }
        if (pos == -1) {
            categories.push_back(cat);
            vector<animal*> g;
            g.push_back(animals[i]);
            groups.push_back(g);
        } else {
            groups[pos].push_back(animals[i]);
        }
    }
    for (size_t i = 0; i < categories.size(); ++i) {
        cout << "\n【" << categories[i] << "】共 " << groups[i].size() << " 只" << endl;
        for (size_t j = 0; j < groups[i].size(); ++j) {
            groups[i][j]->display();
        }
    }
}

bool zoo::updateanimal(const string& id, const string& newname) {
    animal* a = findbyid(id);
    if (a == nullptr) {
        return false;
    }
    a->setname(newname);
    return true;
}

bool zoo::feedanimal(const string& id, const string& date, double amount) {
    animal* a = findbyid(id);
    if (a == nullptr) {
        return false;
    }
    a->addfeedrecord(date, amount);
    return true;
}

bool zoo::checkhealth(const string& id, const string& date, const string& status, const string& note) {
    animal* a = findbyid(id);
    if (a == nullptr) {
        return false;
    }
    a->addhealthrecord(date, status, note);
    return true;
}

void zoo::showrecords(const string& id) const {
    for (size_t i = 0; i < animals.size(); ++i) {
        if (animals[i]->getid() == id) {
            animals[i]->showrecords();
            return;
        }
    }
    cout << "未找到编号 " << id << " 的动物" << endl;
}

// 动物园管理系统/zoo.cpp 的 zoo::save 和 zoo::load 已迁到 file/ZooStorage.cpp。
