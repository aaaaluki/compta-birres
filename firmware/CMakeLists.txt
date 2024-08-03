# For more information about build system see
# https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/build-system.html
# The following five lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.16)

set(CB_COMPONENTS_DIRS "./components"
                        "./components/display_7segment"
                        "./components/drivers" )

set(EXTRA_COMPONENT_DIRS "${EXTRA_COMPONENT_DIRS} ${CB_COMPONENTS_DIRS}")

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(compta-birres)
