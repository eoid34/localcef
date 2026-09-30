#include "simple_app.h"

#include "include/cef_browser.h"
#include "include/cef_scheme.h"
#include "include/cef_win.h"
#include "include/wrapper/cef_helpers.h"
#include "simple_handler.h"

namespace {

// 把 local:// 请求交给 CefResourceManager 处理。
class LocalSchemeHandlerFactory : public CefSchemeHandlerFactory {
 public:
  explicit LocalSchemeHandlerFactory(CefRefPtr<CefResourceManager> rm)
      : rm_(rm) {}

  CefRefPtr<CefResourceHandler> Create(CefRefPtr<CefBrowser> browser,
                                       CefRefPtr<CefFrame> frame,
                                       const CefString& scheme_name,
                                       CefRefPtr<CefRequest> request) override {
    CEF_REQUIRE_IO_THREAD();
    return rm_->CreateHandler(request);
  }

 private:
  CefRefPtr<CefResourceManager> rm_;
  IMPLEMENT_REFCOUNTING(LocalSchemeHandlerFactory);
};

}  // namespace

SimpleApp::SimpleApp(const std::string& exe_dir) : exe_dir_(exe_dir) {}

SimpleApp::~SimpleApp() {}

void SimpleApp::OnRegisterCustomSchemes(
    CefRefPtr<CefSchemeRegistrar> registrar) {
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
  // URL "local://app/index.html" 解析为 host=app, path=/index.html；
  // DirectoryProvider 只匹配 path（不含 host），故 url_path 用 "/" 即可：
  //   /index.html   -> www/index.html
  //   /style.css    -> www/style.css
  //   /app.js       -> www/app.js
  std::string www_dir = exe_dir_ + "/www";
  CefResourceManager::HandlerOptions options;
  resource_manager_->AddDirectoryProvider("/", www_dir, 0, options);

  CefRegisterSchemeHandlerFactory(
      "local", "", new LocalSchemeHandlerFactory(resource_manager_));

  CefRefPtr<CefClient> client(new SimpleHandler());
  CefBrowserSettings browser_settings;

  CefWindowInfo window_info;
  window_info.SetAsPopup(nullptr, "CEF Local Scheme");

  CefBrowserHost::CreateBrowser(window_info, client, "local://app/index.html",
                                browser_settings, nullptr, nullptr);
}
