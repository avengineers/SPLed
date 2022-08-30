# TODO: caching required due to cmake tools extension issue: https://github.com/microsoft/vscode-cmake-tools/issues/1188
# TODO: get debugging running (lldb-mi not found in any current package, switching back to gcc)
set(CMAKE_C_COMPILER clang CACHE STRING "C Compiler")
# TODO: clarify why llvm-cov produces invalid gcov files (contain blank lines), releated GCOVR issue: https://github.com/gcovr/gcovr/issues/331
set(GCOVR_ADDITIONAL_OPTIONS --gcov-executable \"llvm-cov gcov\" --gcov-ignore-parse-errors --html-title \"Code Coverage Report \(tool suite: LLVM Clang\)\")

set(CMAKE_CXX_COMPILER clang++ CACHE STRING "CXX Compiler")
set(CMAKE_ASM_COMPILER ${CMAKE_C_COMPILER} CACHE STRING "ASM Compiler")
