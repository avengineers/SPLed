set(CMAKE_C_COMPILER gcc CACHE STRING "C Compiler")
set(CMAKE_CXX_COMPILER g++ CACHE STRING "CXX Compiler")
set(CMAKE_ASM_COMPILER ${CMAKE_C_COMPILER} CACHE STRING "ASM Compiler")

# Define extra C/CPP compiler flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Dstatic_scope_file= -save-temps")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Dstatic_scope_file= -save-temps")