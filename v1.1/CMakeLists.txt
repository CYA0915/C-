# 交付版构建配置：编译三个基础子系统、公共类型、整合功能和存储层。
cmake_minimum_required(VERSION 3.16)
project(ZooCity VERSION 1.1 LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(zoocity STATIC
    attendance/Basic.cpp
    zoo/animal.cpp
    zoo/zoo.cpp
    bank/BankAccount.cpp
    bank/BankService.cpp
    common/Date.cpp
    common/Employee.cpp
    common/Money.cpp
    function/CityService.cpp
    function/Food.cpp
    function/FoodPurchaseService.cpp
    function/PayrollService.cpp
    function/SalaryPolicy.cpp
    file/AttendanceStorage.cpp
    file/BankStorage.cpp
    file/CityStorage.cpp
    file/FoodStorage.cpp
    file/PayrollStorage.cpp
    file/ZooStorage.cpp
)

target_compile_features(zoocity PUBLIC cxx_std_17)
target_include_directories(zoocity PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

if(MSVC)
    target_compile_options(zoocity PUBLIC /utf-8)
endif()

add_executable(zoocity_v11 main.cpp)
target_link_libraries(zoocity_v11 PRIVATE zoocity)
