## Legacy Sources to Configurable Sources

- Move source files from `legacy/<variant>/App/<component>` (like *.c and *.h) to `src/App/<component>/src`
- Create the file `src/App/<component>/CMakeLists.txt` with content:

    ```
    include(parts.cmake)

    create_component()
    ```

- create `src/App/<component>/parts.cmake`. The Content will be a list of all compiled *.c files, that are required to link. The source code is specified as `add_source(my_file.c)`. The `add_source(...)` definitions are already present in `legacy/<variant>/parts.cmake`. They must be cut & pasted into `src/App/<component>/parts.cmake`. The path to the source file is relative from the component, so usually the correct path is something like `add_source(src/my_file.c)`
- Add the new created component to variant `variants/<variant>/parts.cmake` in this form: `add_component(<path to component>)`, e.g. `add_component(src/App/IsolMn)`

## Configuring 'Configurable Sources'

If a component is used in multiple variants, it might be necessary do do small configuration changes, like cycleTime 5ms in one variant vs. cycleTime 10ms in another or de/activating functional parts of the code completely based on a feature configuration. You can achieve this by adding a configuration header to your `variants/<variant>` folder, e.g. `variants/<variant>/config.h`. In this header you can set the variant-specific #defines that you need. Then make sure to add the variant folder to your include sserach path by adding `add_include(${CMAKE_CURRENT_LIST_DIR})` to the `variants/<variant>/parts.cmake`. Then you can do `#include "config.h"` to use the defines. This way you can reuse ~90% of your existing code.

> **_NOTE:_** Configuration will be generated in the future, including configuration constraints. This is a simple way of adding configuration to your components, so that it is possible to merge code with slight differences.