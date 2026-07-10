spl_add_source(src/brightness_controller.c)
spl_add_test_source(test/test_brightness_controller.cc)
spl_add_required_interface(components/rte)
spl_create_component(LONG_NAME "Brightness Controller")

if(BUILD_TESTING)
    target_compile_options(components_brightness_controller PRIVATE
        -include ${PROJECT_SOURCE_DIR}/components/platform_types/src/platform_types.h)
    target_compile_options(components_brightness_controller_test PRIVATE
        -include ${PROJECT_SOURCE_DIR}/components/platform_types/src/platform_types.h)
endif()
