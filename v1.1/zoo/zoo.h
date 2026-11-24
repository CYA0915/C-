// 原位置：动物园管理系统/zoo.h。
#ifndef ZOO_H
#define ZOO_H
#include <vector>
#include <string>
#include "animal.h"
using namespace std;

class zoo {
private:
    friend class ZooStorage;
    vector<animal*> animals;
public:
    zoo() = default;
    zoo(const zoo&) = delete;
    zoo& operator=(const zoo&) = delete;
    ~zoo();
    void addanimal(animal* a);
    void showall() const;
    animal* findbyid(const string& id) const;
    bool removeanimal(const string& id);
    int count() const;
    // 只读访问动物集合，供界面遍历/刷新表格；不转移所有权。
    const vector<animal*>& getanimals() const;
    // 清空集合；加载文件前使用，避免重复追加。
    void clear();
    void printbycategory() const;
    bool updateanimal(const string& id, const string& newname);
    bool feedanimal(const string& id, const string& date, double amount);
    bool checkhealth(const string& id, const string& date, const string& status, const string& note);
    void showrecords(const string& id) const;
    // 动物园管理系统/zoo.h 的 zoo::save / zoo::load 现由 file/ZooStorage 提供。
};

#endif
