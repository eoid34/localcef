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

# ---- 定位 CEF C++ 封装层源码目录 ----
# 不同 CEF 版本目录名不同：旧版叫 libcef_dll_wrapper，新版（约 112+）改名为 libcef_dll。
# 这里两个名字都探测，避免版本升级后再次踩坑。
if(EXISTS "${CEF_DIR}/libcef_dll")
  set(CEF_WRAPPER_DIR "${CEF_DIR}/libcef_dll")
elseif(EXISTS "${CEF_DIR}/libcef_dll_wrapper")
  set(CEF_WRAPPER_DIR "${CEF_DIR}/libcef_dll_wrapper")
else()
  message(FATAL_ERROR
    "找不到 CEF 封装层源码目录：既无 ${CEF_DIR}/libcef_dll，也无 "
    "${CEF_DIR}/libcef_dll_wrapper。请确认 CEF 二进制包已正确解压"
    "（Standard 发行版应包含该目录的源码）。")
endif()
message(STATUS "Using CEF wrapper source dir = ${CEF_WRAPPER_DIR}")

# ---- 直接编译 CEF C++ 封装层为静态库 ----
# 用递归 GLOB，兼容源码直接在目录内或位于子目录（如 libcef_dll/wrapper）的情况。
# 注意：必须用正斜杠，CMake 的 file(GLOB) 会把反斜杠当转义符。
file(GLOB_RECURSE CEF_WRAPPER_SOURCES
  "${CEF_WRAPPER_DIR}/*.cc"
  "${CEF_WRAPPER_DIR}/*.cpp")
if(NOT CEF_WRAPPER_SOURCES)
  message(FATAL_ERROR
    "未在 ${CEF_WRAPPER_DIR} 下找到任何 .cc/.cpp 源文件。"
    "请确认 CEF 二进制包已正确解压。")
endif()
list(LENGTH CEF_WRAPPER_SOURCES CEF_WRAPPER_COUNT)
message(STATUS "Found ${CEF_WRAPPER_COUNT} CEF wrapper source files")

add_library(libcef_dll_wrapper STATIC ${CEF_WRAPPER_SOURCES})
# NOMINMAX：屏蔽 Windows 头文件的 min/max 宏，避免与 CEF/STL 冲突（CEF 官方示例同样要求）。
target_compile_definitions(libcef_dll_wrapper PUBLIC UNICODE _UNICODE NOMINMAX)
# 把 CEF 发行版根目录也加入包含路径：wrapper 内部可能用 "include/..." 或 "libcef_dll/..." 形式包含。
target_include_directories(libcef_dll_wrapper PUBLIC
  "${CEF_DIR}"
  "${CEF_INCLUDE}"
  "${CEF_INCLUDE}/wrapper"
  "${CEF_WRAPPER_DIR}")

# ---- 我们的 EXE ----
add_executable(cef_local_scheme WIN32
  src/main.cc
  src/simple_app.cc
  src/simple_handler.cc)
target_include_directories(cef_local_scheme PRIVATE
  "${CEF_DIR}"
  "${CEF_INCLUDE}"
  "${CEF_INCLUDE}/wrapper"
  "${CEF_WRAPPER_DIR}")
target_compile_definitions(cef_local_scheme PRIVATE UNICODE _UNICODE NOMINMAX)
target_link_libraries(cef_local_scheme
  libcef_dll_wrapper
  "${CEF_LIB_RELEASE}"
  comctl32.lib
  shlwapi.lib
  rpcrt4.lib)

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
