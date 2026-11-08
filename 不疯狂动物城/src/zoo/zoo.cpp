#include "zoo.h"
#include <fstream>
#include <iostream>
using namespace std;

static vector<string> split(const string& s, char sep) {
    vector<string> parts;
    string cur;
    for (size_t i = 0; i < s.size(); ++i) {
        if (s[i] == sep) { parts.push_back(cur); cur = ""; }
        else cur += s[i];
    }
    parts.push_back(cur);
    return parts;
}

zoo::~zoo() {
    for (size_t i = 0; i < animals.size(); ++i) delete animals[i];
    animals.clear();
}

void zoo::addanimal(animal* a) {
    if (a == nullptr) return;
    animals.push_back(a);
    cout << "添加成功 " << a->getname() << "(" << a->getcategory() << ")" << endl;
}

void zoo::showall() const {
    if (animals.empty()) { cout << "暂无动物" << endl; return; }
    for (size_t i = 0; i < animals.size(); ++i) animals[i]->display();
}

animal* zoo::findbyid(const string& id) const {
    for (size_t i = 0; i < animals.size(); ++i)
        if (animals[i]->getid() == id) return animals[i];
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

int zoo::count() const { return (int)animals.size(); }

void zoo::printbycategory() const {
    if (animals.empty()) { cout << "暂无动物" << endl; return; }
    vector<string> categories;
    vector<vector<animal*>> groups;
    for (size_t i = 0; i < animals.size(); ++i) {
        string cat = animals[i]->getcategory();
        int pos = -1;
        for (size_t j = 0; j < categories.size(); ++j)
            if (categories[j] == cat) { pos = (int)j; break; }
        if (pos == -1) {
            categories.push_back(cat);
            vector<animal*> g;
            g.push_back(animals[i]);
            groups.push_back(g);
        }
        else {
            groups[pos].push_back(animals[i]);
        }
    }
    for (size_t i = 0; i < categories.size(); ++i) {
        cout << "\n【" << categories[i] << "】共 " << groups[i].size() << " 只" << endl;
        for (size_t j = 0; j < groups[i].size(); ++j) groups[i][j]->display();
    }
}

bool zoo::updateanimal(const string& id, const string& newname) {
    animal* a = findbyid(id);
    if (a == nullptr) return false;
    a->setname(newname);
    return true;
}

bool zoo::feedanimal(const string& id, const string& date, double amount) {
    animal* a = findbyid(id);
    if (a == nullptr) return false;
    a->addfeedrecord(date, amount);
    return true;
}

bool zoo::checkhealth(const string& id, const string& date, const string& status, const string& note) {
    animal* a = findbyid(id);
    if (a == nullptr) return false;
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

void zoo::save() const { // [需补全] 保存动物员工公共信息及进食关联。
// [补全思路] 与load采用相同格式，并返回完整保存结果；本次保留原函数体。
    ofstream fout("animals.txt");
    if (!fout) { cout << "保存动物失败" << endl; return; }
    for (size_t i = 0; i < animals.size(); ++i) fout << animals[i]->saveline() << endl;
    fout.close();
    ofstream rout("records.txt");
    if (!rout) { cout << "保存记录失败" << endl; return; }
    for (size_t i = 0; i < animals.size(); ++i) {
        vector<string> lines = animals[i]->saverecords();
        for (size_t j = 0; j < lines.size(); ++j) rout << lines[j] << endl;
    }
    rout.close();
    cout << "已保存 " << animals.size() << " 只动物及记录" << endl;
}

void zoo::load() { // [需补全] 读取新增员工字段并保护原有动物数据。
// [补全思路] 同步各子类构造和saveline格式，校验重复编号、数字转换及旧格式默认值。
    ifstream fin("animals.txt");
    if (!fin) return;
    string line;
    while (getline(fin, line)) {
        if (line.empty()) continue;
        vector<string> p = split(line, '|');
        if (p.size() < 3) continue;
        string type = p[0], id = p[1], name = p[2];
        animal* a = nullptr;
        if (type == "eagle" && p.size() >= 4) a = new eagle(id, name, stod(p[3]));
        else if (type == "parrot" && p.size() >= 4) a = new parrot(id, name, stod(p[3]));
        else if (type == "lion") a = new lion(id, name);
        else if (type == "elephant") a = new elephant(id, name);
        if (a != nullptr) animals.push_back(a);
    }
    fin.close();
    ifstream rin("records.txt");
    if (!rin) return;
    while (getline(rin, line)) {
        if (line.empty()) continue;
        vector<string> p = split(line, '|');
        if (p.size() < 3) continue;
        string id = p[0], rtype = p[1];
        animal* a = findbyid(id);
        if (a == nullptr) continue;
        if (rtype == "feed" && p.size() >= 4) a->addfeedrecord(p[2], stod(p[3]));
        else if (rtype == "health" && p.size() >= 5) a->addhealthrecord(p[2], p[3], p[4]);
    }
    rin.close();
    cout << "已从文件加载 " << animals.size() << " 只动物" << endl;
}