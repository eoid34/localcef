#ifndef SIMPLE_APP_H
#define SIMPLE_APP_H

#include <string>
#include "include/cef_app.h"
#include "include/wrapper/cef_resource_manager.h"
#include "include/wrapper/cef_helpers.h"  // provides IMPLEMENT_REFCOUNTING

// 主应用：注册自定义 Scheme 并在初始化后用 CefResourceManager 提供本地文件。
class SimpleApp : public CefApp, public CefBrowserProcessHandler {
 public:
  explicit SimpleApp(const std::string& exe_dir);
  ~SimpleApp() override;

  CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() override {
    return this;
  }

  void OnRegisterCustomSchemes(
      CefRawPtr<CefSchemeRegistrar> registrar) override;
  void OnContextInitialized() override;

 private:
  std::string exe_dir_;
  CefRefPtr<CefResourceManager> resource_manager_;

  SimpleApp(const SimpleApp&) = delete;
  SimpleApp& operator=(const SimpleApp&) = delete;

  IMPLEMENT_REFCOUNTING(SimpleApp);
};

#endif  // SIMPLE_APP_H
