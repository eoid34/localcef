
#ifndef LOCAL_SCHEME_HANDLER_H
#define LOCAL_SCHEME_HANDLER_H

#include "include/cef_scheme.h"
#include <string>
#include <fstream>

// 资源处理器：负责读取具体文件内容
class LocalResourceHandler : public CefResourceHandler {
public:
    LocalResourceHandler(const std::string& file_path);

    bool ProcessRequest(CefRefPtr<CefRequest> request, CefRefPtr<CefCallback> callback) override;
    void GetResponseHeaders(CefRefPtr<CefResponse> response, int64& response_length, CefString& redirectUrl) override;
    bool ReadResponse(void* data_out, int bytes_to_read, int& bytes_read, CefRefPtr<CefCallback> callback) override;
    void Cancel() override;

private:
    std::string file_path_;
    std::ifstream file_stream_;
    std::string mime_type_;
    IMPLEMENT_REFCOUNTING(LocalResourceHandler);
};

// 工厂类：根据 URL 创建对应的 ResourceHandler
class LocalSchemeHandlerFactory : public CefSchemeHandlerFactory {
public:
    CefRefPtr<CefResourceHandler> Create(CefRefPtr<CefBrowser> browser,
                                         CefRefPtr<CefFrame> frame,
                                         const CefString& scheme_name,
                                         CefRefPtr<CefRequest> request) override;
    
    IMPLEMENT_REFCOUNTING(LocalSchemeHandlerFactory);
};

#endif // LOCAL_SCHEME_HANDLER_H
