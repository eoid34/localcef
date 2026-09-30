# 便捷封装：真正编译走 CMake。Windows 下需有 Visual Studio 2022 + CMake + GNU Make。
# 主要用法：
#   make cef      # 下载并解压 CEF 二进制到 ./cef_binary
#   make         # 配置并 Release 编译
#   make clean   # 清理
# 也可以完全不用本 Makefile，直接：
#   cmake -S . -B build -G "Visual Studio 17 2022" -A x64 -DCEF_DIR=<cef路径>
#   cmake --build build --config Release

CEF_VERSION  ?= 151.3.15
CEF_HASH     ?= g57a32ea
CEF_CHROMIUM ?= 151.0.7922.76
CEF_DIR      ?= cef_binary

.PHONY: all cef build clean

all: build

cef:
	@echo "Downloading CEF $(CEF_VERSION) ..."
	powershell -NoProfile -Command "Invoke-WebRequest -Uri 'https://cef-builds.spotifycdn.com/cef_binary_$(CEF_VERSION)%2B$(CEF_HASH)%2Bchromium-$(CEF_CHROMIUM)_windows64.tar.bz2' -OutFile cef.tar.bz2"
	powershell -NoProfile -Command "tar -xf cef.tar.bz2"
	powershell -NoProfile -Command "$$d=Get-ChildItem -Directory cef_binary_* | Select-Object -First 1; if($$d){ Move-Item $$d.FullName cef_binary -Force }"
	@echo "CEF extracted to ./cef_binary"

build:
	cmake -S . -B build -G "Visual Studio 17 2022" -A x64 -DCEF_DIR="$(CEF_DIR)"
	cmake --build build --config Release

clean:
	cmake --build build --config Release --target clean
	-rm -rf build
