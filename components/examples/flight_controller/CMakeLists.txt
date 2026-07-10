spl_add_source(src/flight_controller.c)
spl_add_test_source(test/test_flight_controller.cc)
spl_add_required_interface(components/rte)
spl_create_component(LONG_NAME "Flight Controller")
