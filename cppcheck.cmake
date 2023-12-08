FetchContent_Declare(
    cpp-check
    GIT_REPOSITORY https://github.com/danmar/cppcheck.git
    GIT_TAG 2.12.1
)
FetchContent_Populate(cpp-check)

set(CPPCHECK_OUTPUT_FILE "${CMAKE_BINARY_DIR}/cppcheck-report.xml")

find_program(CPPCHECK cppcheck)
if(NOT CPPCHECK)
  message(FATAL_ERROR "cppcheck not found")
endif()

add_custom_command(
  OUTPUT ${CPPCHECK_OUTPUT_FILE}
  COMMAND ${CPPCHECK}
  --project=${CMAKE_BINARY_DIR}/compile_commands.json
  -i ${CMAKE_BINARY_DIR}
  -i build
  --enable=all
  --xml-version=2
  2> ${CPPCHECK_OUTPUT_FILE}
  COMMENT "Running CppCheck and generating report"
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

add_custom_target(
  cppcheck-xml
  DEPENDS ${CPPCHECK_OUTPUT_FILE}
  COMMENT "CppCheck target"
)

add_custom_command(
  OUTPUT ${CPPCHECK_OUTPUT_FILE}.html
  COMMAND python ${CMAKE_SOURCE_DIR}/build/modules/cpp-check-src/htmlreport/cppcheck-htmlreport --file=${CPPCHECK_OUTPUT_FILE} --report-dir=${CMAKE_BINARY_DIR}/reports/cppcheck --source-dir=${CMAKE_SOURCE_DIR}
  DEPENDS ${CPPCHECK_OUTPUT_FILE}
  COMMENT "Generating HTML report from CppCheck output"
)

add_custom_target(
  cppcheck-html
  DEPENDS ${CPPCHECK_OUTPUT_FILE}.html
  COMMENT "CppCheck HTML Report target"
)
