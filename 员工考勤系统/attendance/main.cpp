#include "FileOperation.h"
#include <iostream>

using namespace std;

int main() {
    Basic basic;
    FileOperation file;
    string filename = "testdata.txt";
    Date date1 = {2026,10,14};

    basic.addRecord(date1, "E001", 0);
    basic.addRecord(date1, "E002", 1);
    
    if(!file.Save(basic, filename)) {
        cout << "保存失败" << endl;
        return 1;
    }
    cout<<"保存成功"<<endl;
    
    Basic restored;
    if(!file.Load(restored, filename)) {
        cout << "读取失败" << endl;
        return 1;
    }
    cout << "文件读取完成" << endl;
    restored.searchRecord(date1);
    return 0;
}