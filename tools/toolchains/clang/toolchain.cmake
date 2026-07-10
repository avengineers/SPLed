set(CMAKE_C_COMPILER clang CACHE STRING "C Compiler")
set(CMAKE_CXX_COMPILER clang++ CACHE STRING "CXX Compiler")
set(CMAKE_ASM_COMPILER ${CMAKE_C_COMPILER} CACHE STRING "ASM Compiler")

# Link libraries required for POSIX functions like nanosleep
if(NOT WIN32)
    # On Linux/Unix systems, link with real-time library
    link_libraries(rt pthread)
elseif(WIN32 AND NOT MSVC)
    # On Windows with MinGW/Clang, link with pthread library
    link_libraries(pthread)
endif()
