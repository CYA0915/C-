// 原位置：动物园管理系统/zoo.cpp 中的 zoo::load 和 zoo::save。
// 交付版调整：从动物管理类拆出文件读写，兼容只含动物资料的旧文件。
#include "ZooStorage.h"

#include <fstream>
#include <memory>
#include <stdexcept>

#include "../zoo/zoo.h"

namespace {

std::vector<std::string> splitFields(const std::string& line) {
    std::vector<std::string> fields;
    std::size_t begin = 0;
    for (std::size_t i = 0; i <= line.size(); ++i) {
        if (i == line.size() || line[i] == '|') {
            fields.push_back(line.substr(begin, i - begin));
            begin = i + 1;
        }
    }
    return fields;
}

std::unique_ptr<animal> readAnimal(const std::vector<std::string>& fields) {
    const auto& type = fields[0];
    const bool isBird = type == "eagle" || type == "parrot";
    const std::size_t salaryIndex = isBird ? 4 : 3;
    const bool hasEmployeeFields = fields.size() == salaryIndex + 2;
    if (!hasEmployeeFields && fields.size() != salaryIndex) {
        return nullptr;
    }

    const auto& id = fields[1];
    const auto& name = fields[2];
    std::unique_ptr<animal> employee;
    if (type == "eagle") {
        employee = std::make_unique<eagle>(id, name, std::stod(fields[3]));
    } else if (type == "parrot") {
        employee = std::make_unique<parrot>(id, name, std::stod(fields[3]));
    } else if (type == "lion") {
        employee = std::make_unique<lion>(id, name);
    } else if (type == "elephant") {
        employee = std::make_unique<elephant>(id, name);
    }
    // 独立动物园的旧记录没有工资和银行账号时，保留 Employee 的默认值。
    if (employee && hasEmployeeFields) {
        employee->setBasicSalary(std::stoll(fields[salaryIndex]));
        employee->setBankAccountNo(fields[salaryIndex + 1]);
    }
    return employee;
}

void dropCarriageReturn(std::string& line) {
    if (!line.empty() && line.back() == '\r') {
        line.pop_back();
    }
}

}

bool ZooStorage::save(const zoo& animals, const std::filesystem::path& directory) {
    std::ofstream roster(directory / "animals.txt");
    std::ofstream history(directory / "records.txt");
    if (!roster || !history) {
        return false;
    }
    for (const animal* employee : animals.getanimals()) {
        roster << employee->saveline() << '\n';
        for (const auto& record : employee->saverecords()) {
            history << record << '\n';
        }
    }
    roster.close();
    history.close();
    return !roster.fail() && !history.fail();
}

bool ZooStorage::load(zoo& animals, const std::filesystem::path& directory) {
    std::ifstream roster(directory / "animals.txt");
    if (!roster) {
        return false;
    }
    zoo loaded;
    std::string line;
    while (std::getline(roster, line)) {
        dropCarriageReturn(line);
        const auto fields = splitFields(line);
        if (fields.size() < 3 || loaded.findbyid(fields[1])) {
            continue;
        }
        try {
            auto employee = readAnimal(fields);
            if (employee) {
                loaded.animals.push_back(employee.get());
                employee.release();
            }
        } catch (const std::invalid_argument&) {
            continue;
        } catch (const std::out_of_range&) {
            continue;
        }
    }
    if (roster.bad()) {
        return false;
    }

    const auto historyPath = directory / "records.txt";
    if (std::filesystem::exists(historyPath)) {
        std::ifstream history(historyPath);
        if (!history) {
            return false;
        }
        while (std::getline(history, line)) {
            dropCarriageReturn(line);
            const auto fields = splitFields(line);
            if (fields.size() < 4) {
                continue;
            }
            animal* employee = loaded.findbyid(fields[0]);
            if (!employee) {
                continue;
            }
            if (fields[1] == "feed") {
                try {
                    employee->addfeedrecord(fields[2], std::stod(fields[3]));
                } catch (const std::invalid_argument&) {
                    continue;
                } catch (const std::out_of_range&) {
                    continue;
                }
            } else if (fields[1] == "health" && fields.size() >= 5) {
                employee->addhealthrecord(fields[2], fields[3], fields[4]);
            }
        }
        if (history.bad()) {
            return false;
        }
    }
    animals.animals.swap(loaded.animals);
    return true;
}
