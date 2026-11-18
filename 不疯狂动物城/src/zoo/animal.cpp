#include "animal.h"

// ============================================================================
// animal.cpp
//   动物园继承体系的实现。原稿由邓编写；整合时改为继承 Employee，
//   并保留原有饲养/健康记录和多态显示/保存接口。
// ============================================================================

animal::animal(const string& id, const string& name) : Employee(id, name) {}
animal::~animal() {}

string animal::getid() const { return Employee::id(); }
string animal::getname() const { return Employee::name(); }

const vector<feedrecord>& animal::getfeedrecords() const {
    return feedrecords;
}

const vector<healthrecord>& animal::gethealthrecords() const {
    return healthrecords;
}

void animal::addfeedrecord(const string& date, double amount) {
    feedrecord r;
    r.date = date;
    r.amount = amount;
    feedrecords.push_back(r);
}

void animal::addhealthrecord(const string& date, const string& status, const string& note) {
    healthrecord r;
    r.date = date;
    r.status = status;
    r.note = note;
    healthrecords.push_back(r);
}

void animal::showrecords() const {
    cout << "--- " << getname() << " 饲养记录 ---" << endl;
    if (feedrecords.empty()) {
        cout << "  (暂无)" << endl;
    }
    for (size_t i = 0; i < feedrecords.size(); ++i) {
        cout << "  [" << feedrecords[i].date << "] " << feedrecords[i].amount << "kg" << endl;
    }
    cout << "--- " << getname() << " 健康记录 ---" << endl;
    if (healthrecords.empty()) {
        cout << "  (暂无)" << endl;
    }
    for (size_t i = 0; i < healthrecords.size(); ++i) {
        cout << "  [" << healthrecords[i].date << "] " << healthrecords[i].status
             << ", " << healthrecords[i].note << endl;
    }
}

vector<string> animal::saverecords() const {
    vector<string> lines;
    for (size_t i = 0; i < feedrecords.size(); ++i) {
        lines.push_back(getid() + "|feed|" + feedrecords[i].date + "|" + to_string(feedrecords[i].amount));
    }
    for (size_t i = 0; i < healthrecords.size(); ++i) {
        lines.push_back(getid() + "|health|" + healthrecords[i].date + "|" +
                        healthrecords[i].status + "|" + healthrecords[i].note);
    }
    return lines;
}

bird::bird(const string& id, const string& name, double winglength)
    : animal(id, name), winglength(winglength) {
}

string bird::getcategory() const {
    return "鸟类";
}

mammal::mammal(const string& id, const string& name, const string& dietType)
    : animal(id, name), dietType(dietType) {
}

string mammal::getcategory() const {
    return "哺乳动物";
}

eagle::eagle(const string& id, const string& name, double winglength)
    : bird(id, name, winglength) {
}

void eagle::display() const {
    cout << "编号：" << getid() << " | 名称：" << getname()
         << " | 翼展（cm）：" << winglength << endl;
}

string eagle::getspecies() const { return "eagle"; }

string eagle::saveline() const {
    return "eagle|" + getid() + "|" + getname() + "|" + to_string(winglength)
         + "|" + to_string(basicSalary()) + "|" + bankAccountNo();
}

parrot::parrot(const string& id, const string& name, double winglength)
    : bird(id, name, winglength) {
}

void parrot::display() const {
    cout << "编号：" << getid() << " | 名称：" << getname()
         << " | 翼展（cm）：" << winglength << endl;
}

string parrot::getspecies() const { return "parrot"; }

string parrot::saveline() const {
    return "parrot|" + getid() + "|" + getname() + "|" + to_string(winglength)
         + "|" + to_string(basicSalary()) + "|" + bankAccountNo();
}

lion::lion(const string& id, const string& name) : mammal(id, name, "肉食") {}

void lion::display() const {
    cout << "编号：" << getid() << " | 名称：" << getname()
         << " | 食性：" << dietType << endl;
}

string lion::getspecies() const { return "lion"; }

string lion::saveline() const {
    return "lion|" + getid() + "|" + getname()
         + "|" + to_string(basicSalary()) + "|" + bankAccountNo();
}

elephant::elephant(const string& id, const string& name) : mammal(id, name, "草食") {}

void elephant::display() const {
    cout << "编号：" << getid() << " | 名称：" << getname()
         << " | 食性：" << dietType << endl;
}

string elephant::getspecies() const { return "elephant"; }

string elephant::saveline() const {
    return "elephant|" + getid() + "|" + getname()
         + "|" + to_string(basicSalary()) + "|" + bankAccountNo();
}
