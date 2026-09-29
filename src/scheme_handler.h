
#ifndef SCHEME_HANDLER_H
#define SCHEME_HANDLER_H

#include "include/cef_scheme.h"

class LocalSchemeHandler : public CefResourceHandler {
public:
    LocalSchemeHandler();

    // CefResourceHandler methods:
    virtual bool ProcessRequest(CefRefPtr<CefRequest> request, CefRefPtr<CefCallback> callback) override;
    virtual void GetResponseHeaders(CefRefPtr<CefResponse> response, int64& response_length, CefString& redirectUrl) override;
    virtual bool ReadResponse(void* data_out, int bytes_to_read, int& bytes_read, CefRefPtr<CefCallback> callback) override;
    virtual void Cancel() override;

private:
    std::string mime_type_;
    std::string content_;
    size_t offset_;

    IMPLEMENT_REFCOUNTING(LocalSchemeHandler);
};

class LocalSchemeHandlerFactory : public CefSchemeHandlerFactory {
public:
    LocalSchemeHandlerFactory();

    // CefSchemeHandlerFactory methods:
    virtual CefRefPtr<CefResourceHandler> Create(CefRefPtr<CefBrowser> browser,
                                                 CefRefPtr<CefFrame> frame,
                                                 const CefString& scheme_name,
                                                 CefRefPtr<CefRequest> request) override;

private:
    IMPLEMENT_REFCOUNTING(LocalSchemeHandlerFactory);
};

#endif // SCHEME_HANDLER_H
