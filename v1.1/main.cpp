// v1.1 命令行演示：管理者端和员工端共用一份动物与考勤数据。
#include "file/CityStorage.h"
#include "function/CityService.h"

#include <cmath>
#include <iostream>
#include <sstream>
#include <utility>

namespace {

std::string ask(const char* prompt) {
    std::cout << prompt << std::flush;
    std::string line;
    std::getline(std::cin, line);
    return line;
}

bool askDate(Date& date) {
    const auto input = ask("日期（YYYY-MM-DD，回车为今天）：");
    date = Date::today();
    return std::cin && (input.empty() || Date::parse(input, date));
}

int currentStatus(const Basic& attendance, const std::string& id, const Date& date) {
    Record records[MAX_RECORDS];
    const int count = attendance.getRecord(date, records);
    for (int i = 0; i < count; ++i) {
        if (records[i].ID == id) {
            return records[i].status;
        }
    }
    return -1;
}

bool changeAndSave(CityService& city, CityStorage& storage, const std::string& id,
                   const Date& date, int status, std::string& message) {
    if (!city.animals().findbyid(id)) {
        message = "没有这个员工";
        return false;
    }
    auto& attendance = city.attendance();
    auto previous = std::make_unique<Basic>(attendance);
    if (!attendance.changeRecord(date, id, status) && !attendance.addRecord(date, id, status)) {
        message = "考勤日期、状态或记录容量有问题";
        return false;
    }
    if (!storage.saveAll(city, message)) {
        attendance = std::move(*previous);
        return false;
    }
    return true;
}

void showAttendance(CityService& city, const std::string& id) {
    if (!city.animals().findbyid(id)) {
        std::cout << "没有这个员工\n";
        return;
    }
    Date date;
    if (!askDate(date)) {
        std::cout << "日期格式不正确\n";
        return;
    }
    const int status = currentStatus(city.attendance(), id, date);
    const char* statusNames[] = {"缺勤", "出勤", "请假"};
    const char* label = status >= 0 && status <= 2 ? statusNames[status] : "未登记";
    std::cout << id << ' ' << date.toString() << "：" << label << '\n';
}

void addEmployee(CityService& city, CityStorage& storage) {
    const auto id = ask("员工编号：");
    const auto name = ask("员工姓名：");
    if (id.empty() || name.empty() || city.animals().findbyid(id) ||
        id.find_first_of(" \t\r\n|") != std::string::npos || name.find('|') != std::string::npos) {
        std::cout << "请检查员工编号、姓名和编号是否重复\n";
        return;
    }
    const auto type = ask("物种（1 老鹰 / 2 鹦鹉 / 3 狮子 / 4 大象）：");
    std::unique_ptr<animal> employee;
    if (type == "1" || type == "2") {
        std::istringstream input(ask("翼展（厘米）："));
        double wing;
        char extra;
        if (!(input >> wing) || (input >> extra) || !std::isfinite(wing) || wing <= 0) {
            std::cout << "翼展必须是正数\n";
            return;
        }
        if (type == "1") {
            employee = std::make_unique<eagle>(id, name, wing);
        } else {
            employee = std::make_unique<parrot>(id, name, wing);
        }
    } else if (type == "3") {
        employee = std::make_unique<lion>(id, name);
    } else if (type == "4") {
        employee = std::make_unique<elephant>(id, name);
    } else {
        std::cout << "物种选择无效\n";
        return;
    }
    Money salary = 0;
    if (!parseMoney(ask("基本工资（元）："), salary)) {
        std::cout << "基本工资格式不正确\n";
        return;
    }
    employee->setBasicSalary(salary);
    city.animals().addanimal(employee.get());
    employee.release();
    std::string message;
    if (!storage.saveAll(city, message)) {
        city.animals().removeanimal(id);
    } else {
        message = "员工资料已保存";
    }
    std::cout << message << '\n';
}

void managerMenu(CityService& city, CityStorage& storage) {
    while (std::cin) {
        const auto choice = ask("\n管理者端：1 查考勤  2 设置考勤  3 添加员工  0 返回\n选择：");
        if (!std::cin || choice == "0") {
            return;
        }
        if (choice == "1") {
            showAttendance(city, ask("员工编号："));
        } else if (choice == "2") {
            const auto id = ask("员工编号：");
            Date date;
            if (!askDate(date)) {
                std::cout << "日期格式不正确\n";
                continue;
            }
            const auto status = ask("状态（0 缺勤 / 1 出勤 / 2 请假）：");
            if (status != "0" && status != "1" && status != "2") {
                std::cout << "状态选择无效\n";
                continue;
            }
            std::string message;
            if (changeAndSave(city, storage, id, date, status[0] - '0', message)) {
                message = "考勤已保存";
            }
            std::cout << message << '\n';
        } else if (choice == "3") {
            addEmployee(city, storage);
        } else {
            std::cout << "请选择菜单中的编号\n";
        }
    }
}

void employeeMenu(CityService& city, CityStorage& storage) {
    const auto id = ask("选择员工编号：");
    if (!city.animals().findbyid(id)) {
        std::cout << "没有这个员工，请先由管理者添加\n";
        return;
    }
    while (std::cin) {
        const auto choice = ask("\n员工端：1 今日签到  2 查本人考勤  0 返回\n选择：");
        if (!std::cin || choice == "0") {
            return;
        }
        if (choice == "2") {
            showAttendance(city, id);
        } else if (choice == "1") {
            const Date today = Date::today();
            const int previous = currentStatus(city.attendance(), id, today);
            if (previous == 1) {
                std::cout << "今天已经签到\n";
            } else if (previous == 2) {
                std::cout << "今天已经登记请假，请找管理者修改\n";
            } else {
                std::string message;
                if (changeAndSave(city, storage, id, today, 1, message)) {
                    message = "签到成功，考勤已保存";
                }
                std::cout << message << '\n';
            }
        } else {
            std::cout << "请选择菜单中的编号\n";
        }
    }
}

}

int main(int argc, char* argv[]) {
    if (argc > 2) {
        std::cerr << "用法：zoocity_v11 [数据目录]\n";
        return 1;
    }
    CityStorage storage(argc == 2 ? argv[1] : "runtime");
    CityService city;
    std::string message;
    if (!storage.loadAll(city, message)) {
        std::cerr << message << '\n';
        return 1;
    }
    std::cout << "不疯狂动物城 v1.1\n" << message << '\n';
    while (std::cin) {
        const auto choice = ask("\n进入：1 管理者端  2 员工端  0 退出\n选择：");
        if (!std::cin || choice == "0") {
            break;
        }
        if (choice == "1") {
            managerMenu(city, storage);
        } else if (choice == "2") {
            employeeMenu(city, storage);
        } else {
            std::cout << "请选择菜单中的编号\n";
        }
    }
}
