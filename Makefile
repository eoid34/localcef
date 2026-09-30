# 便捷封装：真正编译走 CMake。
# 推荐用 Ninja + MSVC（不依赖 “Visual Studio” CMake 生成器，最稳）：
#   1) 打开 “x64 Native Tools Command Prompt for VS 2022”（保证 cl.exe / Ninja 在 PATH）
#   2) make cef      # 下载并解压 CEF 二进制到 ./cef_binary
#   3) make         # 配置(Release)并编译
#   4) make clean   # 清理
#
# 也可以完全不用本 Makefile，直接在 VS 的 x64 Native Tools 命令行里：
#   cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -DCEF_DIR=cef_binary
#   cmake --build build
# （若你确实装好了 “Visual Studio 17 2022 + 使用C++的桌面开发” 工作负荷，也可改用
#   cmake -S . -B build -G "Visual Studio 17 2022" -A x64 -DCEF_DIR=cef_binary
#   cmake --build build --config Release）

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
	cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -DCEF_DIR="$(CEF_DIR)"
	cmake --build build

clean:
	cmake --build build --target clean
	-rm -rf build
