#ifndef HEALTHRECORD_H
#define HEALTHRECORD_H
#include <string>
using namespace std;

struct healthrecord { // [需补全] 统一日期校验和头文件格式。
// [补全思路] 日期可保留文本但统一为YYYY-MM-DD并用公共Date校验；文件末尾现有#endif#pragma once待后续修正。
    string date;
    string status;
    string note;
};

#endif
