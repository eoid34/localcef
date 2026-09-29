
cmake_minimum_required(VERSION 3.15)
project(MyCefApp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 查找 CEF SDK
# 期望通过 -DCEF_ROOT 传入 CEF 二进制包的路径
if(NOT DEFINED CEF_ROOT)
    message(FATAL_ERROR "CEF_ROOT is not defined. Please set it to the path of the CEF binary distribution.")
endif()

# 包含 CEF 的 CMake 模块
list(APPEND CMAKE_MODULE_PATH "${CEF_ROOT}/cmake")
find_package(CEF REQUIRED)

# 添加可执行文件
add_executable(MyCefApp
    main.cpp
    client_handler.cpp
    local_scheme_handler.cpp
)

# 链接 CEF 库
target_link_libraries(MyCefApp PRIVATE libcef_dll_wrapper ${CEF_STANDARD_LIBS})

# 设置包含目录
target_include_directories(MyCefApp PRIVATE ${CEF_ROOT})

# 复制 CEF 资源文件到输出目录 (关键步骤，否则运行会崩溃)
# 这包括 dlls, pak files, locales 等
PRINT_CEF_CONFIG()
COPY_CEF_RESOURCES(${CMAKE_CURRENT_BINARY_DIR} "${CEF_ROOT}")

# 设置工作目录为输出目录，方便调试
set_target_properties(MyCefApp PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/${CEF_BINARY_DIR}")
