function(spl_extension_example)
    if(NOT SPL_EXTENSION_EXAMPLE_INPUT_VARIABLE)
        message(FATAL_ERROR "SPL_EXTENSION_EXAMPLE_INPUT_VARIABLEis not existing, but a mandatory input argument.")
    endif()

    message("SPL_EXTENSION_EXAMPLE_INPUT_VARIABLE: ${SPL_EXTENSION_EXAMPLE_INPUT_VARIABLE}")
endfunction(spl_extension_example)
