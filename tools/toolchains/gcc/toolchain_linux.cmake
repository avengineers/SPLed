set(CMAKE_C_COMPILER gcc CACHE STRING "C Compiler")
set(CMAKE_CXX_COMPILER g++ CACHE STRING "CXX Compiler")
set(CMAKE_ASM_COMPILER ${CMAKE_C_COMPILER} CACHE STRING "ASM Compiler")

# Linux GCC doesn't support -mbig-obj (that's a MinGW flag)
# Use appropriate Linux GCC flags instead
set(COMPILE_CXX_FLAGS "")
add_compile_options(
    "$<$<COMPILE_LANGUAGE:CXX>:${COMPILE_CXX_FLAGS}>"
)
