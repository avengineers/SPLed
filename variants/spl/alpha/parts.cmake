spl_add_include(${PROJECT_SOURCE_DIR}/legacy/platform_a/some_folder)
spl_add_include(${PROJECT_SOURCE_DIR}/legacy/platform_a/some_other_folder)

spl_add_component(src/main)
spl_add_component(src/app/component/var_a)
spl_add_component(src/app/component2/var_a)
spl_add_component(legacy/platform_a)