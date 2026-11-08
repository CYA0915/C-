#include "animal.h"

animal::animal(const string& id, const string& name) : Employee(id,name) {}
animal::~animal() {}
string animal::getid() const { return Employee::id(); }
string animal::getname() const { return Employee::name(); }

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
    cout << "--- " << getname() << " ��������¼ ---" << endl;
    if (feedrecords.empty()) cout << "  (����)" << endl;
    for (size_t i = 0; i < feedrecords.size(); ++i)
        cout << "  [" << feedrecords[i].date << "] " << feedrecords[i].amount << "kg" << endl;
    cout << "--- " << getname() << " �Ľ�����¼ ---" << endl;
    if (healthrecords.empty()) cout << "  (����)" << endl;
    for (size_t i = 0; i < healthrecords.size(); ++i)
        cout << "  [" << healthrecords[i].date << "] " << healthrecords[i].status << ", " << healthrecords[i].note << endl;
}

vector<string> animal::saverecords() const {
    vector<string> lines;
    for (size_t i = 0; i < feedrecords.size(); ++i)
        lines.push_back(getid() + "|feed|" + feedrecords[i].date + "|" + to_string(feedrecords[i].amount));
    for (size_t i = 0; i < healthrecords.size(); ++i)
        lines.push_back(getid() + "|health|" + healthrecords[i].date + "|" + healthrecords[i].status + "|" + healthrecords[i].note);
    return lines;
}

bird::bird(const string& id, const string& name, double winglength)
    : animal(id, name), winglength(winglength) {
}
string bird::getcategory() const { return "����"; }

mammal::mammal(const string& id, const string& name, const string& dietType)
    : animal(id, name), dietType(dietType) {
}
string mammal::getcategory() const { return "���鶯��"; }

eagle::eagle(const string& id, const string& name, double winglength)
    : bird(id, name, winglength) {
}
void eagle::display() const {
    cout << "��ţ�" << getid() << " | ���ƣ�" << getname() << " | ��չ��cm����" << winglength << endl;
}
string eagle::getspecies() const { return "eagle"; }
string eagle::saveline() const { return "eagle|" + getid() + "|" + getname() + "|" + to_string(winglength); }

parrot::parrot(const string& id, const string& name, double winglength)
    : bird(id, name, winglength) {
}
void parrot::display() const {
    cout << "��ţ�" << getid() << " | ƣ" << getname() << " | չcm" << winglength << endl;
}
string parrot::getspecies() const { return "parrot"; }
string parrot::saveline() const { return "parrot|" + getid() + "|" + getname() + "|" + to_string(winglength); }

lion::lion(const string& id, const string& name) : mammal(id, name, "ʳ") {}
void lion::display() const {
    cout << "��ţ�" << getid() << " | ƣ" << getname() << " | ʳԣ" << dietType << endl;
}
string lion::getspecies() const { return "lion"; }
string lion::saveline() const { return "lion|" + getid() + "|" + getname(); }

elephant::elephant(const string& id, const string& name) : mammal(id, name, "ʳ") {}
void elephant::display() const {
    cout << "ţ" << getid() << " | ƣ" << getname() << " | ʳԣ" << dietType << endl;
}
string elephant::getspecies() const { return "elephant"; }
string elephant::saveline() const { return "elephant|" + getid() + "|" + getname(); }