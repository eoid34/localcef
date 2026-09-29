
#include "app.h"
#include "include/cef_browser.h"
#include "include/cef_command_line.h"
#include "include/wrapper/cef_helpers.h"

SimpleApp::SimpleApp() {}

void SimpleApp::OnBeforeCommandLineProcessing(const CefString& process_type, CefRefPtr<CefCommandLine> command_line) {
    // 可以在这里添加额外的命令行参数
}

void SimpleApp::OnRegisterCustomSchemes(CefRawPtr<CefSchemeRegistrar> registrar) {
    // 注册自定义scheme "local"
    registrar->AddCustomScheme("local", CEF_SCHEME_OPTION_STANDARD | CEF_SCHEME_OPTION_LOCAL);
}

void SimpleApp::OnContextInitialized() {
    CEF_REQUIRE_UI_THREAD();

    // 创建浏览器窗口信息
    CefWindowInfo window_info;
    window_info.SetAsPopup(nullptr, "Cef Local Scheme App");

    // 浏览器设置
    CefBrowserSettings browser_settings;

    // 加载本地URL
    CefString url("local://index.html");

    // 创建浏览器
    CefBrowserHost::CreateBrowser(window_info, nullptr, url, browser_settings, nullptr, nullptr);
}
