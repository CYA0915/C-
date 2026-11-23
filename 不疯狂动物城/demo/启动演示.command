#!/bin/zsh
set -eu

demo_root="${0:A:h}"
project_root="${demo_root:h}"
demo_cmake="$(command -v cmake || true)"
if [[ -z "$demo_cmake" ]]; then
    demo_cmake="$HOME/Qt/Tools/CMake/CMake.app/Contents/bin/cmake"
fi
if [[ ! -x "$demo_cmake" ]]; then
    print "没有找到 CMake，请先安装 Qt 开发套件，或在 Qt Creator 中按使用说明运行。"
    exit 1
fi
demo_qt_prefix="${ZOO_QT_PREFIX:-}"
if [[ -z "$demo_qt_prefix" ]]; then
    for candidate in "$HOME"/Qt/*/macos(N); do
        demo_qt_prefix="$candidate"
    done
fi
if [[ -z "$demo_qt_prefix" ]]; then
    print "没有找到 Qt。请设置 ZOO_QT_PREFIX 为 Qt 安装目录后重试。"
    exit 1
fi

demo_tmp_base="${TMPDIR:-/tmp/}"
demo_workdir="$(mktemp -d "${demo_tmp_base%/}/zoocity-demo.XXXXXX")"
python3 "$demo_root/准备演示数据.py" --output "$demo_workdir"
"$demo_cmake" -S "$project_root/QtApp" -B "$project_root/QtApp/build-demo" -DCMAKE_PREFIX_PATH="$demo_qt_prefix" -DCMAKE_BUILD_TYPE=Debug
"$demo_cmake" --build "$project_root/QtApp/build-demo" --parallel 4
print "每次启动使用新的演示目录，不修改正式数据。关闭程序后可在上述目录查看保存结果。"
"$project_root/QtApp/build-demo/QtApp.app/Contents/MacOS/QtApp" --data-dir "$demo_workdir"
