#ifndef SIMPLE_HANDLER_H
#define SIMPLE_HANDLER_H

#include "include/cef_client.h"
#include "include/cef_resource_request_handler.h"
#include "include/wrapper/cef_helpers.h"  // provides IMPLEMENT_REFCOUNTING
#include "include/wrapper/cef_resource_manager.h"

// 极简 CefClient：实现生命周期处理 + 请求处理。
// 最后一个窗口关闭时退出消息循环；并把 local:// 资源请求转发给
// CefResourceManager（CEF 151 新用法：通过 CefRequestHandler::GetResourceRequestHandler
// 返回的 CefResourceRequestHandler，而不是旧版直接挂在 CefRequestHandler 上的
// OnBeforeResourceLoad / GetResourceHandler —— 那两个方法在 CEF 151 已从
// CefRequestHandler 移除）。
class SimpleHandler : public CefClient,
                     public CefLifeSpanHandler,
                     public CefRequestHandler,
                     public CefResourceRequestHandler {
 public:
  explicit SimpleHandler(CefRefPtr<CefResourceManager> rm);
  ~SimpleHandler() override;

  CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() override { return this; }
  CefRefPtr<CefRequestHandler> GetRequestHandler() override { return this; }

  void OnAfterCreated(CefRefPtr<CefBrowser> browser) override;
  void OnBeforeClose(CefRefPtr<CefBrowser> browser) override;

  // CefRequestHandler：为匹配的资源请求返回本对象作为资源请求处理器。
  CefRefPtr<CefResourceRequestHandler> GetResourceRequestHandler(
      CefRefPtr<CefBrowser> browser,
      CefRefPtr<CefFrame> frame,
      CefRefPtr<CefRequest> request,
      bool is_navigation,
      bool is_download,
      const CefString& request_initiator,
      bool& disable_default_handling) override;

  // CefResourceRequestHandler：把请求转发给 CefResourceManager。
  cef_return_value_t OnBeforeResourceLoad(
      CefRefPtr<CefBrowser> browser,
      CefRefPtr<CefFrame> frame,
      CefRefPtr<CefRequest> request,
      CefRefPtr<CefCallback> callback) override;

  CefRefPtr<CefResourceHandler> GetResourceHandler(
      CefRefPtr<CefBrowser> browser,
      CefRefPtr<CefFrame> frame,
      CefRefPtr<CefRequest> request) override;

 private:
  CefRefPtr<CefResourceManager> resource_manager_;
  int browser_count_ = 0;

  SimpleHandler(const SimpleHandler&) = delete;
  SimpleHandler& operator=(const SimpleHandler&) = delete;

  IMPLEMENT_REFCOUNTING(SimpleHandler);
};

#endif  // SIMPLE_HANDLER_H
