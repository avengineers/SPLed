set(CMAKE_C_COMPILER clang CACHE STRING "C Compiler")
set(CMAKE_CXX_COMPILER clang++ CACHE STRING "CXX Compiler")
set(CMAKE_ASM_COMPILER ${CMAKE_C_COMPILER} CACHE STRING "ASM Compiler")

# Define extra C/CPP compiler flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Dstatic_scope_file=static")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Dstatic_scope_file=static")
