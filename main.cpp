
#include "include/cef_app.h"
#include "client_handler.h"
#include "local_scheme_handler.h"
#include <iostream>
#include <windows.h>

// 应用程序入口
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR lpCmdLine,
                     _In_ int nCmdShow) {
    
    // 1. 初始化 CEF 设置
    CefMainArgs main_args(hInstance);
    CefRefPtr<CefApp> app;

    // 2. 执行子进程逻辑（如果是渲染进程等，直接返回）
    int exit_code = CefExecuteProcess(main_args, app, nullptr);
    if (exit_code >= 0) {
        return exit_code;
    }

    CefSettings settings;
    settings.no_sandbox = true; // 简化开发环境，生产环境建议启用沙箱并正确配置
    settings.multi_threaded_message_loop = false; // 使用外部消息循环

    // 3. 初始化 CEF
    CefInitialize(main_args, settings, app, nullptr);

    // 4. 注册自定义 Scheme Handler
    // 协议名: local, 域名: app
    CefRegisterSchemeHandlerFactory("local", "app", new LocalSchemeHandlerFactory());

    // 5. 创建浏览器窗口
    CefWindowInfo window_info;
    window_info.SetAsPopup(nullptr, "My CEF App with Custom Scheme");

    CefBrowserSettings browser_settings;
    
    // 创建 ClientHandler
    CefRefPtr<ClientHandler> handler = new ClientHandler();

    // 加载初始页面
    // 注意：这里假设 d:\cef\index.html 存在，或者你可以修改为相对路径逻辑
    // 为了演示，我们加载一个固定的本地路径，实际项目中应动态获取 exe 所在目录
    std::string initial_url = "local://app/index.html";
    
    CefBrowserHost::CreateBrowser(window_info, handler, initial_url, browser_settings, nullptr, nullptr);

    // 6. 运行消息循环
    CefRunMessageLoop();

    // 7. 关闭 CEF
    CefShutdown();

    return 0;
}
