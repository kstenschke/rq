cmake_minimum_required(VERSION 3.9)
project(rq
        VERSION 0.1)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ../)

add_executable(rq
        src/helper/helper_file.cc
        src/helper/helper_string.cc
        src/config.cc
        src/curl_wrapper.cc
        src/main.cc)

find_package(PkgConfig)
find_package_handle_standard_args(JSON-C DEFAULT_MSG JSON-C_LIBRARY JSON-C_INCLUDE_DIR)
mark_as_advanced(JSON-C_INCLUDE_DIR JSON-C_LIBRARY)
# if compilation fails, try: sudo apt-get install libjson0 libjson0-dev

target_link_libraries(rq curl json-c)
