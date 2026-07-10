spl_add_source(src/spled.c)
spl_add_test_source(test/test_spled.cc)

if(BRIGHTNESS_ADJUSTMENT_ENABLED STREQUAL "True")
    spl_add_required_interface(components/brightness_controller)
endif()

spl_add_required_interface(components/console_interface)
spl_add_required_interface(components/keyboard_interface)
spl_add_required_interface(components/light_controller)
spl_add_required_interface(components/main_control_knob)
spl_add_required_interface(components/power_button)
spl_add_required_interface(components/power_signal_processing)
spl_add_required_interface(components/rte)
spl_add_required_interface(components/platform_types)

if(AUTO_OFF STREQUAL "True")
    spl_add_required_interface(components/auto_off)
endif()

spl_create_component(LONG_NAME "SPLED")

if(BUILD_TESTING)
    target_compile_options(components_spled PRIVATE
        -include ${PROJECT_SOURCE_DIR}/components/platform_types/src/platform_types.h)
    target_compile_options(components_spled_test PRIVATE
        -include ${PROJECT_SOURCE_DIR}/components/platform_types/src/platform_types.h)
endif()
