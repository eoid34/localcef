cmake_minimum_required(VERSION 3.21)
project(cef_local_scheme CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---- CEF 二进制发行版位置 ----
# 优先用 -DCEF_DIR=... 传入；其次用环境变量 CEF_DIR；再退回到 ./cef_binary。
if(NOT DEFINED CEF_DIR)
  if(DEFINED ENV{CEF_DIR})
    set(CEF_DIR $ENV{CEF_DIR})
  else()
    set(CEF_DIR "${CMAKE_SOURCE_DIR}/cef_binary")
  endif()
endif()

# Normalize to forward slashes: CMake's file(GLOB) treats backslashes in the
# pattern as escape characters, which breaks path matching on Windows.
string(REPLACE "\\" "/" CEF_DIR "${CEF_DIR}")
message(STATUS "Using CEF_DIR = ${CEF_DIR}")

set(CEF_INCLUDE "${CEF_DIR}/include")
set(CEF_LIB_RELEASE "${CEF_DIR}/Release/libcef.lib")
set(CEF_RESOURCES_SRC "${CEF_DIR}/Resources")
set(CEF_RUNTIME_SRC_RELEASE "${CEF_DIR}/Release")

# ---- 直接编译 CEF C++ 封装层为静态库 ----
# 注意：必须用正斜杠，CMake 的 file(GLOB) 会把反斜杠当转义符。
file(GLOB CEF_WRAPPER_SOURCES
  "${CEF_DIR}/libcef_dll_wrapper/*.cc"
  "${CEF_DIR}/libcef_dll_wrapper/*.cpp")
if(NOT CEF_WRAPPER_SOURCES)
  message(FATAL_ERROR
    "未在 ${CEF_DIR}/libcef_dll_wrapper 下找到任何 .cc/.cpp 源文件。"
    "请确认 CEF 二进制包已正确解压（Standard 发行版应包含该目录的源码）。")
endif()
add_library(libcef_dll_wrapper STATIC ${CEF_WRAPPER_SOURCES})
target_compile_definitions(libcef_dll_wrapper PUBLIC UNICODE _UNICODE)
target_include_directories(libcef_dll_wrapper PUBLIC
  "${CEF_INCLUDE}"
  "${CEF_INCLUDE}/wrapper"
  "${CEF_DIR}/libcef_dll_wrapper")

# ---- 我们的 EXE ----
add_executable(cef_local_scheme WIN32
  src/main.cc
  src/simple_app.cc
  src/simple_handler.cc)
target_include_directories(cef_local_scheme PRIVATE
  "${CEF_INCLUDE}"
  "${CEF_INCLUDE}/wrapper"
  "${CEF_DIR}/libcef_dll_wrapper")
target_link_libraries(cef_local_scheme
  libcef_dll_wrapper
  "${CEF_LIB_RELEASE}")

# ---- 构建后复制运行时文件到 EXE 同目录 ----
set(OUT "$<TARGET_FILE_DIR:cef_local_scheme>")

# DLL / bin / pak / dat（来自 Release）
file(GLOB CEF_RUNTIME_FILES
  "${CEF_RUNTIME_SRC_RELEASE}/*.dll"
  "${CEF_RUNTIME_SRC_RELEASE}/*.bin"
  "${CEF_RUNTIME_SRC_RELEASE}/*.pak"
  "${CEF_RUNTIME_SRC_RELEASE}/*.dat")
foreach(f ${CEF_RUNTIME_FILES})
  add_custom_command(TARGET cef_local_scheme POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${f}" "${OUT}")
endforeach()

# 语言包（来自 Release/locales 或 Resources/locales）
file(GLOB CEF_LOCALES_RELEASE "${CEF_RUNTIME_SRC_RELEASE}/locales/*.pak")
foreach(f ${CEF_LOCALES_RELEASE})
  add_custom_command(TARGET cef_local_scheme POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${OUT}/locales"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${f}" "${OUT}/locales")
endforeach()

file(GLOB CEF_LOCALES_RES "${CEF_RESOURCES_SRC}/locales/*.pak")
foreach(f ${CEF_LOCALES_RES})
  add_custom_command(TARGET cef_local_scheme POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${OUT}/locales"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${f}" "${OUT}/locales")
endforeach()

# 资源包（来自 Resources）
file(GLOB CEF_RES_FILES "${CEF_RESOURCES_SRC}/*.pak" "${CEF_RESOURCES_SRC}/*.dat")
foreach(f ${CEF_RES_FILES})
  add_custom_command(TARGET cef_local_scheme POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${f}" "${OUT}")
endforeach()

# 我们自己的 www 目录
add_custom_command(TARGET cef_local_scheme POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/www" "${OUT}/www")
