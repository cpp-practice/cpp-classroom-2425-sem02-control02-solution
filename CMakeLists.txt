cmake_minimum_required (VERSION 3.14)

project(control-02)

set(CMAKE_CXX_STANDARD 20)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /WX") # CMake already contains W3 in its flags
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -Wextra -Wextra-semi \
                        -O0 -g -fsanitize=address -fno-omit-frame-pointer")
endif()

add_executable(task-1-test src/task_1/buffer_smoke_test.cpp src/task_1/buffer.hpp)


if (WIN32)
    set(dl_loader_platform windows)
endif(WIN32)
if(UNIX)
    set(dl_loader_platform unix)
endif(UNIX)

set(DLLOADER_INC_PATH src/task_2/${dl_loader_platform})  # for dlloader.hpp

add_compile_definitions(PARSER_LIBRARY_DIR="${CMAKE_BINARY_DIR}")
add_executable(task-2-test src/task_2/phonebook_smoke_test.cpp src/task_2/phonebook.hpp src/task_2/${dl_loader_platform}/dlloader.hpp src/task_2/idlloader.hpp)
# target_include_directories(task-2-test PRIVATE src/contrib/nlohmann)
target_include_directories(task-2-test PRIVATE ${DLLOADER_INC_PATH} src/task_2)
add_library(json-parser SHARED src/task_2/json_parser.cpp)

target_include_directories(json-parser PRIVATE src/contrib)

# add_executable(task-3-test src/task_3/matrix_smoke_test.cpp src/task_3/matrix.hpp)
