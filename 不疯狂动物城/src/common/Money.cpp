#include "Money.h"

#include <sstream>
#include <iostream>

using namespace std;

namespace {

string trim(const string& text) {
    size_t begin = 0;
    while (begin < text.size() &&
           (text[begin] == ' ' || text[begin] == '\t' || text[begin] == '\r' || text[begin] == '\n')) {
        ++begin;
    }
    size_t end = text.size();
    while (end > begin &&
           (text[end - 1] == ' ' || text[end - 1] == '\t' || text[end - 1] == '\r' || text[end - 1] == '\n')) {
        --end;
    }
    return text.substr(begin, end - begin);
}

bool isDigits(const string& text) {
    if (text.empty()) {
        return false;
    }
    for (size_t i = 0; i < text.size(); ++i) {
        if (text[i] < '0' || text[i] > '9') {
            return false;
        }
    }
    return true;
}

}

string moneyToString(Money amount) {
    bool negative = (amount < 0);
    Money value = negative ? -amount : amount;

    Money yuan = value / 100;
    Money fen = value % 100;

    ostringstream oss;
    if (negative) {
        oss << '-';
    }
    oss << yuan << '.';
    if (fen < 10) {
        oss << '0';
    }
    oss << fen;
    return oss.str();
}

bool parseMoney(const string& text, Money& cents) {
    string s = trim(text);
    if (s.empty()) {
        return false;
    }

    if (s[0] == '+') {
        s.erase(0, 1);
    }
    if (s.empty()) {
        return false;
    }

    size_t dotPos = s.find('.');
    string intPart;
    string fracPart;
    if (dotPos == string::npos) {
        intPart = s;
    } else {

        if (s.find('.', dotPos + 1) != string::npos) {
            return false;
        }
        intPart = s.substr(0, dotPos);
        fracPart = s.substr(dotPos + 1);
    }

    if (!isDigits(intPart)) {
        return false;
    }
    if (!fracPart.empty() && !isDigits(fracPart)) {
        return false;
    }
    if (fracPart.size() > 2) {
        return false;
    }

    Money yuan = 0;
    for (size_t i = 0; i < intPart.size(); ++i) {
        yuan = yuan * 10 + (intPart[i] - '0');
        if (yuan > 1000000000000LL) {
            return false;
        }
    }

    Money fen = 0;
    if (fracPart.size() >= 1) {
        fen += (fracPart[0] - '0') * 10;
    }
    if (fracPart.size() >= 2) {
        fen += (fracPart[1] - '0');
    }

    cents = yuan * 100 + fen;
    return true;
}
