cmake_minimum_required(VERSION 3.16)

project(QtApp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

add_executable(QtApp
    main.cpp
    mainwindow.h
    mainwindow.cpp
    managerpage.cpp
    employeepage.cpp
    mainwindow.ui

    ../src/common/Date.h
    ../src/common/Date.cpp
    ../src/common/Employee.h
    ../src/common/Employee.cpp
    ../src/common/Money.h
    ../src/common/Money.cpp

    ../src/zoo/animal.h
    ../src/zoo/animal.cpp
    ../src/zoo/zoo.h
    ../src/zoo/zoo.cpp
    ../src/zoo/feedrecord.h
    ../src/zoo/healthrecord.h

    ../src/attendance/Basic.h
    ../src/attendance/Basic.cpp
    ../src/attendance/Record.h
    ../src/attendance/AttendanceSummary.h
    ../src/attendance/FileOperation.h
    ../src/attendance/FileOperation.cpp

    ../src/bank/BankAccount.h
    ../src/bank/BankAccount.cpp
    ../src/bank/BankService.h
    ../src/bank/BankService.cpp
    ../src/bank/BankStorage.h
    ../src/bank/BankStorage.cpp
    ../src/bank/Transaction.h

    ../src/payroll/PayrollRecord.h
    ../src/payroll/SalaryPolicy.h
    ../src/payroll/SalaryPolicy.cpp
    ../src/payroll/PayrollService.h
    ../src/payroll/PayrollService.cpp
    ../src/payroll/PayrollStorage.h
    ../src/payroll/PayrollStorage.cpp

    ../src/food/Food.h
    ../src/food/Food.cpp
    ../src/food/FoodPurchaseRecord.h
    ../src/food/FoodPurchaseService.h
    ../src/food/FoodPurchaseService.cpp
    ../src/food/FoodStorage.h
    ../src/food/FoodStorage.cpp
)

target_include_directories(QtApp PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(QtApp PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

set_target_properties(QtApp PROPERTIES
    AUTOMOC_PATH_PREFIX ON
    MACOSX_BUNDLE TRUE
    MACOSX_BUNDLE_BUNDLE_NAME "不疯狂动物城"
    MACOSX_BUNDLE_GUI_IDENTIFIER "com.course.zoocity"
    WIN32_EXECUTABLE TRUE
)
