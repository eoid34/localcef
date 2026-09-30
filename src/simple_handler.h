#ifndef SIMPLE_HANDLER_H
#define SIMPLE_HANDLER_H

#include "include/cef_client.h"
#include "include/wrapper/cef_helpers.h"  // provides IMPLEMENT_REFCOUNTING

// 极简 CefClient：仅实现生命周期处理，最后一个窗口关闭时退出消息循环。
class SimpleHandler : public CefClient, public CefLifeSpanHandler {
 public:
  SimpleHandler();
  ~SimpleHandler() override;

  CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() override { return this; }

  void OnAfterCreated(CefRefPtr<CefBrowser> browser) override;
  void OnBeforeClose(CefRefPtr<CefBrowser> browser) override;

 private:
  int browser_count_ = 0;

  SimpleHandler(const SimpleHandler&) = delete;
  SimpleHandler& operator=(const SimpleHandler&) = delete;

  IMPLEMENT_REFCOUNTING(SimpleHandler);
};

#endif  // SIMPLE_HANDLER_H
