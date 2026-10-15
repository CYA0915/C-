cmake_minimum_required(VERSION 3.16)

project(AttendanceQt VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(PROJECT_SOURCES
        main.cpp
        widget.cpp
        widget.h
        widget.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(AttendanceQt
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        /Users/cya/Desktop/大作业/C++大作业/员工考勤系统/attendance/Basic.cpp
        /Users/cya/Desktop/大作业/C++大作业/员工考勤系统/attendance/Basic.h
        /Users/cya/Desktop/大作业/C++大作业/员工考勤系统/attendance/FileOperation.cpp
        /Users/cya/Desktop/大作业/C++大作业/员工考勤系统/attendance/FileOperation.h
        /Users/cya/Desktop/大作业/C++大作业/员工考勤系统/attendance/Record.h
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET AttendanceQt APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(AttendanceQt SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(AttendanceQt
            ${PROJECT_SOURCES}
        )
    endif()
endif()
target_include_directories(AttendanceQt PRIVATE
    "/Users/cya/Desktop/大作业/C++大作业/员工考勤系统/attendance"
)

target_link_libraries(AttendanceQt PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.AttendanceQt)
endif()
set_target_properties(AttendanceQt PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS AttendanceQt
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(AttendanceQt)
endif()
