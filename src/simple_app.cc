#include "simple_app.h"

#include "include/cef_browser.h"
#include "include/cef_scheme.h"
#include "include/internal/cef_win.h"
#include "include/wrapper/cef_helpers.h"
#include "simple_handler.h"

SimpleApp::SimpleApp(const std::string& exe_dir) : exe_dir_(exe_dir) {}

SimpleApp::~SimpleApp() {}

void SimpleApp::OnRegisterCustomSchemes(
    CefRawPtr<CefSchemeRegistrar> registrar) {
  registrar->AddCustomScheme(
      "local",
      CEF_SCHEME_OPTION_STANDARD | CEF_SCHEME_OPTION_LOCAL |
          CEF_SCHEME_OPTION_SECURE | CEF_SCHEME_OPTION_CORS_ENABLED |
          CEF_SCHEME_OPTION_CSP_BYPASSING);
}

void SimpleApp::OnContextInitialized() {
  CEF_REQUIRE_UI_THREAD();

  resource_manager_ = new CefResourceManager();

  // 把 local:// 请求的 URL 路径映射到 <exe_dir>/www 目录。
  // URL "local://app/index.html" 解析为 origin=local://app, path=/index.html。
  // 新版 AddDirectoryProvider 的 url_path 需包含 origin（而非旧版的纯 path 前缀
  // "/"），并使用 order + identifier 两个整/字符串参数代替旧 HandlerOptions：
  //   local://app/index.html   -> www/index.html
  //   local://app/style.css    -> www/style.css
  //   local://app/app.js       -> www/app.js
  std::string www_dir = exe_dir_ + "/www";
  resource_manager_->AddDirectoryProvider("local://app", www_dir, 0, "");

  CefRefPtr<SimpleHandler> client(new SimpleHandler(resource_manager_));
  CefBrowserSettings browser_settings;

  CefWindowInfo window_info;
  window_info.SetAsPopup(nullptr, "CEF Local Scheme");

  CefBrowserHost::CreateBrowser(window_info, client, "local://app/index.html",
                                browser_settings, nullptr, nullptr);
}
