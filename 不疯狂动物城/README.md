# 不疯狂动物城

```text
不疯狂动物城/
├── QtApp/
│   ├── CMakeLists.txt
│   ├── main.cpp
│   ├── mainwindow.cpp
│   ├── mainwindow.h
│   ├── managerpage.cpp
│   ├── employeepage.cpp
│   ├── sqliteexporter.cpp
│   ├── sqliteexporter.h
│   └── mainwindow.ui
├── docs/
│   ├── 类体系设计与程序流程.md
│   └── Qt前端使用说明书.md
├── src/
│   ├── attendance/
│   │   ├── AttendanceSummary.h
│   │   ├── Basic.cpp
│   │   ├── Basic.h
│   │   ├── FileOperation.cpp
│   │   ├── FileOperation.h
│   │   └── Record.h
│   ├── bank/
│   │   ├── BankAccount.cpp
│   │   ├── BankAccount.h
│   │   ├── BankService.cpp
│   │   ├── BankService.h
│   │   ├── BankStorage.cpp
│   │   ├── BankStorage.h
│   │   └── Transaction.h
│   ├── common/
│   │   ├── Date.cpp
│   │   ├── Date.h
│   │   ├── Employee.cpp
│   │   ├── Employee.h
│   │   ├── Money.cpp
│   │   └── Money.h
│   ├── food/
│   │   ├── Food.cpp
│   │   ├── Food.h
│   │   ├── FoodPurchaseRecord.h
│   │   ├── FoodPurchaseService.cpp
│   │   ├── FoodPurchaseService.h
│   │   ├── FoodStorage.cpp
│   │   └── FoodStorage.h
│   ├── payroll/
│   │   ├── PayrollRecord.h
│   │   ├── PayrollService.cpp
│   │   ├── PayrollService.h
│   │   ├── PayrollStorage.cpp
│   │   ├── PayrollStorage.h
│   │   ├── SalaryPolicy.cpp
│   │   └── SalaryPolicy.h
│   └── zoo/
│       ├── animal.cpp
│       ├── animal.h
│       ├── feedrecord.h
│       ├── healthrecord.h
│       ├── zoo.cpp
│       └── zoo.h
└── README.md
```


## Qt 前端

- 源码：`QtApp/`（`mainwindow.*`、`managerpage.cpp`、`employeepage.cpp`）
- 构建：用 Qt Creator 打开 `QtApp/CMakeLists.txt`，或使用 CMake + Qt6 Widgets。
- 使用说明：`docs/Qt前端使用说明书.md`（Word 版见桌面/打包文件）。
- SQLite 导出：管理者页右上角“导出 SQLite”，把全部数据写成 `.db`；需要 Qt SQL 模块（QSQLITE 驱动）。
- 考勤权限：员工在“我的考勤”签到/请假；管理者在“考勤管理”查询、修改状态和删除修正。
- 数据目录：程序启动后切换到“文档/不疯狂动物城数据”，窗口底部状态栏会显示完整路径。
