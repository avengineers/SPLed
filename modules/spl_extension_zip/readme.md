## How to integrate the extension

To integrate the extension one needs to:

* get the extension locally into the repository
	* this can be done either in the bootstrap step or in the configure step?
	* which extensions are enabled
	* currently the extension is just part of the repository
* install the extension required dependencies
  * we added the extension Python dependencies manually to the project Pipfile
  * the install mandatory script runs pipenv, which installs all dependencies in Pipfile in a virtual environment in `.venv`
* include the extension configuration in the project configuration (source it in the KConfig file)
  * We manually add an extra `menu` item in the project KConfig file to include the extension
* include the extension in the project build system (include the CMake file)
  * just include the extension `index.cmake` to the project `CMakeLists.txt`
* call the extension in the build system (e.g. add the extension as a dependency to the target)
  * the extension `index.cmake` automatically calls the extension to generate its own CMake file and includes it
  * the extension requires the following CMake variables to be set:
    * CMAKE_SOURCE_DIR - the project root dir is required to be able to determine the correct paths relevant for the projects
    * VARIANT - the selected variant
    * BUILD_KIT - the selected build configuration 


## Requirements

* implemented in Python
* have a minimum cmake file just to call the extension with the `–generate` target
* the generate target shall generate the cmake file containing all targets and their dependencies required for an extension.
* an extension shall support a `–run` target to execute some steps in case a complex orchestration is required or some dynamic dependencies are involved.
* read the variant configuration to determine the relevant artifacts to generate
