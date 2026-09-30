#include "simple_handler.h"

#include "include/cef_browser.h"
#include "include/cef_app.h"
#include "include/wrapper/cef_helpers.h"

SimpleHandler::SimpleHandler() {}
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
