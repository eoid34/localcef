
#ifndef APP_H
#define APP_H

#include "include/cef_app.h"
#include "scheme_handler.h"

class SimpleApp : public CefApp, public CefBrowserProcessHandler {
public:
    SimpleApp();

    // CefApp methods:
    virtual CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() override { return this; }
    virtual void OnBeforeCommandLineProcessing(const CefString& process_type, CefRefPtr<CefCommandLine> command_line) override;
    virtual void OnRegisterCustomSchemes(CefRawPtr<CefSchemeRegistrar> registrar) override;

    // CefBrowserProcessHandler methods:
    virtual void OnContextInitialized() override;

private:
    IMPLEMENT_REFCOUNTING(SimpleApp);
};

#endif // APP_H
