
#include "scheme_handler.h"
#include "include/cef_parser.h"
#include <fstream>
#include <sstream>
#include <windows.h>

LocalSchemeHandler::LocalSchemeHandler() : offset_(0) {}

bool LocalSchemeHandler::ProcessRequest(CefRefPtr<CefRequest> request, CefRefPtr<CefCallback> callback) {
    CEF_REQUIRE_IO_THREAD();

    std::string url = request->GetURL();
    // 解析URL，去掉 "local://"
    std::string path = url.substr(8); // strlen("local://") == 8
    
    // 获取exe所在目录
    wchar_t exePath[MAX_PATH];
    GetModuleFileNameW(NULL, exePath, MAX_PATH);
    std::wstring exeDir(exePath);
    exeDir = exeDir.substr(0, exeDir.find_last_of(L"\\/"));
    
    // 构建完整文件路径: exeDir/www/path
    std::wstring widePath = exeDir + L"\\www\\" + CefString(path).ToWString();
    
    // 读取文件
    std::ifstream file(widePath, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    content_ = buffer.str();
    
    // 确定MIME类型
    if (path.find(".html") != std::string::npos || path.find(".htm") != std::string::npos) {
        mime_type_ = "text/html";
    } else if (path.find(".css") != std::string::npos) {
        mime_type_ = "text/css";
    } else if (path.find(".js") != std::string::npos) {
        mime_type_ = "application/javascript";
    } else {
        mime_type_ = "text/plain";
    }

    callback->Continue();
    return true;
}

void LocalSchemeHandler::GetResponseHeaders(CefRefPtr<CefResponse> response, int64& response_length, CefString& redirectUrl) {
    CEF_REQUIRE_IO_THREAD();

    response->SetStatus(200);
    response->SetStatusText("OK");
    response->SetMimeType(mime_type_);
    response_length = content_.size();
}

bool LocalSchemeHandler::ReadResponse(void* data_out, int bytes_to_read, int& bytes_read, CefRefPtr<CefCallback> callback) {
    CEF_REQUIRE_IO_THREAD();

    bytes_read = 0;
    if (offset_ < content_.size()) {
        int transfer_size = std::min(bytes_to_read, static_cast<int>(content_.size() - offset_));
        memcpy(data_out, content_.c_str() + offset_, transfer_size);
        offset_ += transfer_size;
        bytes_read = transfer_size;
    }
    return bytes_read > 0;
}

void LocalSchemeHandler::Cancel() {
    CEF_REQUIRE_IO_THREAD();
}

LocalSchemeHandlerFactory::LocalSchemeHandlerFactory() {}

CefRefPtr<CefResourceHandler> LocalSchemeHandlerFactory::Create(CefRefPtr<CefBrowser> browser,
                                                                CefRefPtr<CefFrame> frame,
                                                                const CefString& scheme_name,
                                                                CefRefPtr<CefRequest> request) {
    return new LocalSchemeHandler();
}
