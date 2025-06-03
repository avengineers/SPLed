spl_add_source(src/component_a.c)
spl_add_test_source(test/test_component_a_suite_1.cc)
spl_add_test_source(test/test_component_a_suite_2.cc)
# TODO: Use spl_add_required_interface when header only components are supported (component_b has only a header).
spl_add_provided_interface(${CMAKE_CURRENT_SOURCE_DIR}/../component_b/src)
spl_create_component()
