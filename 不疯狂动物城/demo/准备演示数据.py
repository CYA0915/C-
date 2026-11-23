#!/usr/bin/env python3
"""在空目录中准备动物员工、考勤、银行和食物演示数据。"""
import argparse
import calendar
import csv
import datetime as dt
import json
from pathlib import Path


def prepare(output):
    output = Path(output).expanduser().resolve()
    output.mkdir(parents=True, exist_ok=True)
    if any(output.iterdir()):
        raise SystemExit("目标目录已有文件，请选择一个空目录，演示不会覆盖已有数据。")
    today = dt.date.today()
    last_day = today.replace(day=1) - dt.timedelta(days=1)
    first_day = last_day.replace(day=1)
    workdays = [dt.date(first_day.year, first_day.month, day)
                for day in range(1, calendar.monthrange(first_day.year, first_day.month)[1] + 1)
                if dt.date(first_day.year, first_day.month, day).weekday() < 5]
    animals = [
        "eagle|A001|阿鹰|120|500000|6222020000001",
        "lion|A002|阿狮|450000|6222020000002",
        "parrot|A003|阿鹦|45|400000|",
        "elephant|A004|阿象|600000|",
    ]
    (output / "animals.txt").write_text("\n".join(animals) + "\n", encoding="utf-8")
    (output / "records.txt").write_text(
        f"A001|feed|{first_day.isoformat()}|2\n"
        f"A001|health|{first_day.isoformat()}|健康|例行检查\n", encoding="utf-8")
    rows = []
    for index, date in enumerate(workdays):
        for employee in ["A001", "A002", "A003", "A004"]:
            status = 1
            if employee == "A001":
                status = 0 if index < 2 else (2 if index == 2 else 1)
            elif employee == "A003":
                status = 0
            rows.append(f"{date.year} {date.month} {date.day} {employee} {status}")
    (output / "attendance.txt").write_text("\n".join(rows) + "\n", encoding="utf-8")
    (output / "foods.txt").write_text("F001 牛肉 1000\nF002 水果 600\nF003 谷物 800\n", encoding="utf-8")
    (output / "payroll.txt").write_text("", encoding="utf-8")
    (output / "purchases.txt").write_text("", encoding="utf-8")
    (output / "data").mkdir()
    with (output / "data/accounts.csv").open("w", encoding="utf-8", newline="") as file:
        writer = csv.writer(file)
        writer.writerow(["账号", "员工编号", "姓名", "类型", "余额(分)", "最低余额(分)", "信用额度(分)", "开户日期", "上次结息日期"])
        writer.writerow(["6222020000001", "A001", "阿鹰", "储蓄账户", 100000, 0, 0, first_day.isoformat(), first_day.isoformat()])
        writer.writerow(["6222020000002", "A002", "阿狮", "信用账户", 50000, 0, 100000, first_day.isoformat(), first_day.isoformat()])
    with (output / "data/transactions.csv").open("w", encoding="utf-8", newline="") as file:
        writer = csv.writer(file)
        writer.writerow(["流水号", "账号", "类型", "金额(分)", "交易后余额(分)", "日期", "备注"])
        writer.writerow([1, "6222020000001", "存款", 100000, 100000, first_day.isoformat(), "开户存入"])
        writer.writerow([2, "6222020000002", "存款", 50000, 50000, first_day.isoformat(), "开户存入"])
    info = {
        "month": first_day.strftime("%Y-%m"),
        "scheduled_days": len(workdays),
        "present_days": len(workdays) - 3,
        "absent_days": 2,
        "leave_days": 1,
        "first_workday": workdays[0].isoformat(),
        "basic_salary_yuan": 5000,
        "absent_deduction_yuan": 100,
        "leave_deduction_yuan": 50,
        "salary_yuan": 4750,
        "initial_balance_yuan": 1000,
        "balance_after_salary_yuan": 5750,
        "food_id": "F001",
        "food_quantity_kg": 2,
        "food_cost_yuan": 20,
        "final_balance_yuan": 5730,
    }
    (output / "demo.json").write_text(json.dumps(info, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")
    print(f"演示数据目录：{output}")
    print(f"演示工资月份：{info['month']}")
    print(f"A001 考勤：应出勤 {len(workdays)}，出勤 {len(workdays) - 3}，缺勤 2，请假 1")
    print("管理者：admin / 123456；员工：A001 / 123456")
    print("A001 缺勤每次扣100元、请假每次扣50元、奖金0：实发4750元")
    print("银行原余额1000元，工资入账后5750元；购买2kg牛肉20元，余额5730元")
    return info


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--output", required=True, help="保存演示数据的空目录")
    arguments = parser.parse_args()
    prepare(arguments.output)
