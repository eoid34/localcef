#include "simple_handler.h"

#include "include/cef_browser.h"
#include "include/cef_app.h"
#include "include/wrapper/cef_helpers.h"

SimpleHandler::SimpleHandler(CefRefPtr<CefResourceManager> rm)
    : resource_manager_(rm) {}

SimpleHandler::~SimpleHandler() {}

void SimpleHandler::OnAfterCreated(CefRefPtr<CefBrowser> browser) {
  CEF_REQUIRE_UI_THREAD();
  browser_count_++;
  (void)browser;
}

void SimpleHandler::OnBeforeClose(CefRefPtr<CefBrowser> browser) {
  CEF_REQUIRE_UI_THREAD();
  (void)browser;
  if (--browser_count_ == 0) {
    CefQuitMessageLoop();
  }
}

cef_return_value_t SimpleHandler::OnBeforeResourceLoad(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    CefRefPtr<CefRequest> request,
    CefRefPtr<CefCallback> callback) {
  CEF_REQUIRE_IO_THREAD();
  return resource_manager_->OnBeforeResourceLoad(browser, frame, request,
                                                 callback);
}

CefRefPtr<CefResourceHandler> SimpleHandler::GetResourceHandler(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    CefRefPtr<CefRequest> request) {
  CEF_REQUIRE_IO_THREAD();
  return resource_manager_->GetResourceHandler(browser, frame, request);
}
