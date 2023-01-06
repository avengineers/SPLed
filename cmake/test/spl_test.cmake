# Unit test framework to test cmake code of SPL

set(CONFIG_SPL_TEST_VERBOSE 0)
set(CONFIG_SPL_FAILED_ASSERTION_LEADS_TO FATAL_ERROR)

set(_SPL_TEST_NUM_ASSERTIONS_FAILED 0)

macro(spl_test_create_mock method)
    set(COUNT_${method} 0)

    macro(${method})
        MATH(EXPR COUNT_${method} "${COUNT_${method}}+1")
        if(CONFIG_SPL_TEST_VERBOSE GREATER 0)
            message("${method} has been called the ${COUNT_${method}} time.")
        endif()
    endmacro(${method})
endmacro(spl_test_create_mock)

# assert macro
# Note: it doesn't support lists in arguments
# Usage samples:
#   EXPECT(MyLib_FOUND)
#   EXPECT(DEFINED MyLib_INCLUDE_DIRS)
#   EXPECT( 1 GREATER 0)
macro(EXPECT)
  set(__line f${CMAKE_CURRENT_LIST_LINE})
  if(NOT (${ARGN}))
    string(REPLACE ";" " " __assert_msg "${ARGN}")
    message(AUTHOR_WARNING "${CMAKE_CURRENT_LIST_FILE}:${__line} Expectation failed: ${__assert_msg}")
  endif()
endmacro(EXPECT)


macro(spl_test_expect_is_called method times)
    if (NOT ${COUNT_${method}} EQUAL ${times})
        MATH(EXPR _SPL_TEST_NUM_ASSERTIONS_FAILED "${_SPL_TEST_NUM_ASSERTIONS_FAILED}+1")
        message(${CONFIG_SPL_FAILED_ASSERTION_LEADS_TO} "FAILED: ${method} was expected to be called ${times} but was called ${COUNT_${method}}.")
    endif()
endmacro(spl_test_expect_is_called)
