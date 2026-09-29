
#include <windows.h>
#include <iostream>
#include "include/cef_app.h"
#include "app.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow) {
    // 启用高DPI支持
    CefEnableHighDPISupport();

    // 提供CEF命令行参数
    CefMainArgs main_args(hInstance);

    // 检查是否为子进程
    auto exit_code = CefExecuteProcess(main_args, nullptr, nullptr);
    if (exit_code >= 0) {
        return exit_code;
    }

    // 配置CEF设置
    CefSettings settings;
    settings.no_sandbox = true; // 简化示例，生产环境建议启用沙箱
    
    // 设置用户数据目录为 exe所在目录/data/user-data
    // 注意：CefString需要宽字符或UTF8，这里使用简单的相对路径处理
    // 在实际应用中，可能需要获取exe绝对路径后拼接
    wchar_t exePath[MAX_PATH];
    GetModuleFileNameW(NULL, exePath, MAX_PATH);
    std::wstring exeDir(exePath);
    exeDir = exeDir.substr(0, exeDir.find_last_of(L"\\/"));
    std::wstring userDataDir = exeDir + L"\\data\\user-data";
    
    CefString(&settings.root_cache_path).FromWString(userDataDir);
    CefString(&settings.user_data_path).FromWString(userDataDir);

    // 创建应用程序实例
    CefRefPtr<CefApp> app(new SimpleApp());

    // 初始化CEF
    if (!CefInitialize(main_args, settings, app.get(), nullptr)) {
        return 1;
    }

    // 运行消息循环
    CefRunMessageLoop();

    // 关闭CEF
    CefShutdown();

    return 0;
}
