#include "zoo.h"
#include <fstream>
#include <iostream>
using namespace std;

// ============================================================================
// zoo.cpp
//   动物园集合管理。原稿由邓编写；整合时陈适配了 Employee 继承、
//   工资/账号字段和 Qt 前端只读访问，并修复了源文件中的中文乱码。
// ============================================================================

static vector<string> split(const string& s, char sep) {
    vector<string> parts;
    string cur;
    for (size_t i = 0; i < s.size(); ++i) {
        if (s[i] == sep) {
            parts.push_back(cur);
            cur.clear();
        } else {
            cur += s[i];
        }
    }
    parts.push_back(cur);
    return parts;
}

zoo::~zoo() {
    for (size_t i = 0; i < animals.size(); ++i) {
        delete animals[i];
    }
    animals.clear();
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

void zoo::save() const {
    ofstream fout("animals.txt");
    if (!fout) {
        cout << "保存动物失败" << endl;
        return;
    }
    for (size_t i = 0; i < animals.size(); ++i) {
        fout << animals[i]->saveline() << endl;
    }
    fout.close();

    ofstream rout("records.txt");
    if (!rout) {
        cout << "保存记录失败" << endl;
        return;
    }
    for (size_t i = 0; i < animals.size(); ++i) {
        vector<string> lines = animals[i]->saverecords();
        for (size_t j = 0; j < lines.size(); ++j) {
            rout << lines[j] << endl;
        }
    }
    rout.close();
    cout << "已保存 " << animals.size() << " 只动物及记录" << endl;
}

void zoo::load() {
    ifstream fin("animals.txt");
    if (!fin) {
        return;
    }
    string line;
    while (getline(fin, line)) {
        if (line.empty()) {
            continue;
        }
        vector<string> p = split(line, '|');
        if (p.size() < 3) {
            continue;
        }
        string type = p[0], id = p[1], name = p[2];
        if (findbyid(id) != nullptr) {
            continue;   // 跳过重复编号，避免集合中出现两个同号动物
        }
        animal* a = nullptr;
        try {
            if (type == "eagle" && p.size() >= 4) {
                a = new eagle(id, name, stod(p[3]));
            } else if (type == "parrot" && p.size() >= 4) {
                a = new parrot(id, name, stod(p[3]));
            } else if (type == "lion") {
                a = new lion(id, name);
            } else if (type == "elephant") {
                a = new elephant(id, name);
            }
        } catch (...) {
            delete a;
            a = nullptr;
        }
        if (a == nullptr) {
            continue;
        }

        // 兼容旧格式：鸟类 4 列、哺乳类 3 列没有工资/账号，使用默认空值。
        try {
            if (type == "eagle" || type == "parrot") {
                if (p.size() >= 6) {
                    a->setBasicSalary(stoll(p[4]));
                    a->setBankAccountNo(p[5]);
                }
            } else {
                if (p.size() >= 5) {
                    a->setBasicSalary(stoll(p[3]));
                    a->setBankAccountNo(p[4]);
                }
            }
        } catch (...) {
            // 工资/账号解析失败时保留默认值，不阻断整只动物的加载。
        }
        animals.push_back(a);
    }
    fin.close();

    ifstream rin("records.txt");
    if (!rin) {
        return;
    }
    while (getline(rin, line)) {
        if (line.empty()) {
            continue;
        }
        vector<string> p = split(line, '|');
        if (p.size() < 3) {
            continue;
        }
        string id = p[0], rtype = p[1];
        animal* a = findbyid(id);
        if (a == nullptr) {
            continue;
        }
        if (rtype == "feed" && p.size() >= 4) {
            try {
                a->addfeedrecord(p[2], stod(p[3]));
            } catch (...) {
            }
        } else if (rtype == "health" && p.size() >= 5) {
            a->addhealthrecord(p[2], p[3], p[4]);
        }
    }
    rin.close();
    cout << "已从文件加载 " << animals.size() << " 只动物" << endl;
}
