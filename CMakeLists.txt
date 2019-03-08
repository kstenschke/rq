cmake_minimum_required(VERSION 3.9)
project(rq
        VERSION 0.1)
set (Rq_VERSION_MAJOR 0)
set (Rq_VERSION_MINOR 1)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ../)

# configure config.h to be generated from config.h, with injected version number
configure_file (
        "${PROJECT_SOURCE_DIR}/src/config.h.in"
        "${PROJECT_SOURCE_DIR}/src/config.h"
)

add_executable(rq
        src/helper/helper_file.cc
        src/helper/helper_string.cc
        src/config_json_parser.cc
        src/helper/helper_curl.cc
        src/main.cc src/config.h)

find_package(PkgConfig)
find_package_handle_standard_args(JSON-C DEFAULT_MSG JSON-C_LIBRARY JSON-C_INCLUDE_DIR)
mark_as_advanced(JSON-C_INCLUDE_DIR JSON-C_LIBRARY)
# if compilation fails, try: sudo apt-get install libjson0 libjson0-dev

target_link_libraries(rq curl json-c)
