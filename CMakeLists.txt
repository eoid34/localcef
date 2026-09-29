
cmake_minimum_required(VERSION 3.15)
project(CefLocalSchemeApp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 查找CEF包，假设CEF_ROOT环境变量已设置或手动指定
# 在实际CI环境中，通常需要先下载并解压CEF到指定目录
if(NOT DEFINED CEF_ROOT)
    set(CEF_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/cef_binary")
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CEF_ROOT}/cmake")
find_package(CEF REQUIRED)

# 添加可执行文件
add_executable(${PROJECT_NAME}
    src/main.cpp
    src/app.cpp
    src/scheme_handler.cpp
)

# 链接CEF库
target_link_libraries(${PROJECT_NAME} libcef_dll_wrapper ${CEF_STANDARD_LIBS})

# 包含头文件
target_include_directories(${PROJECT_NAME} PRIVATE ${CEF_INCLUDE_DIRS} src)

# 复制CEF资源文件到输出目录
PRINT_CEF_CONFIG()
COPY_CEF_RESOURCES(${PROJECT_NAME})

# 创建www目录和index.html用于测试
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/www)
file(WRITE ${CMAKE_BINARY_DIR}/www/index.html "<html><body><h1>Hello from local:// scheme!</h1></body></html>")
