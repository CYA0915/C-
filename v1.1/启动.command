#!/bin/zsh
set -eu

project_dir="${0:A:h}"
cmake_bin="$(command -v cmake || true)"
if [[ -z "$cmake_bin" ]]; then
    cmake_bin="$HOME/Qt/Tools/CMake/CMake.app/Contents/bin/cmake"
fi
if [[ ! -x "$cmake_bin" ]]; then
    print '找不到 CMake，请安装 CMake 后重试。'
    exit 1
fi

"$cmake_bin" -S "$project_dir" -B "$project_dir/.build"
"$cmake_bin" --build "$project_dir/.build" --parallel 4
exec "$project_dir/.build/zoocity_v11" "${1:-$project_dir/runtime}"
