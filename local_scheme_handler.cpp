
#include "local_scheme_handler.h"
#include <algorithm>
#include <iostream>

// 简单的 MIME 类型映射
std::string GetMimeType(const std::string& path) {
    if (path.find(".html") != std::string::npos || path.find(".htm") != std::string::npos) return "text/html";
    if (path.find(".css") != std::string::npos) return "text/css";
    if (path.find(".js") != std::string::npos) return "application/javascript";
    if (path.find(".png") != std::string::npos) return "image/png";
    if (path.find(".jpg") != std::string::npos || path.find(".jpeg") != std::string::npos) return "image/jpeg";
    if (path.find(".gif") != std::string::npos) return "image/gif";
    if (path.find(".json") != std::string::npos) return "application/json";
    return "application/octet-stream";
}

// --- LocalResourceHandler 实现 ---

LocalResourceHandler::LocalResourceHandler(const std::string& file_path)
    : file_path_(file_path) {}

bool LocalResourceHandler::ProcessRequest(CefRefPtr<CefRequest> request, CefRefPtr<CefCallback> callback) {
    callback->Continue();
    return true;
}

void LocalResourceHandler::GetResponseHeaders(CefRefPtr<CefResponse> response, int64& response_length, CefString& redirectUrl) {
    file_stream_.open(file_path_, std::ios::binary | std::ios::ate);
    if (file_stream_.is_open()) {
        response_length = static_cast<int64>(file_stream_.tellg());
        file_stream_.seekg(0, std::ios::beg);
        
        mime_type_ = GetMimeType(file_path_);
        response->SetMimeType(mime_type_);
        response->SetStatus(200);
        response->SetStatusText("OK");
    } else {
        response_length = 0;
        response->SetStatus(404);
        response->SetStatusText("Not Found");
    }
}

bool LocalResourceHandler::ReadResponse(void* data_out, int bytes_to_read, int& bytes_read, CefRefPtr<CefCallback> callback) {
    if (!file_stream_.is_open() || file_stream_.eof()) {
        bytes_read = 0;
        if (file_stream_.is_open()) file_stream_.close();
        return false;
    }

    file_stream_.read(static_cast<char*>(data_out), bytes_to_read);
    bytes_read = static_cast<int>(file_stream_.gcount());
    return true;
}

void LocalResourceHandler::Cancel() {
    if (file_stream_.is_open()) {
        file_stream_.close();
    }
}

// --- LocalSchemeHandlerFactory 实现 ---

CefRefPtr<CefResourceHandler> LocalSchemeHandlerFactory::Create(CefRefPtr<CefBrowser> browser,
                                                                CefRefPtr<CefFrame> frame,
                                                                const CefString& scheme_name,
                                                                CefRefPtr<CefRequest> request) {
    std::string url = request->GetURL();
    
    // 解析 URL: local://app/path/to/file.html
    std::string prefix = "local://app/";
    if (url.find(prefix) == 0) {
        std::string relative_path = url.substr(prefix.length());
        
        // 【重要】在实际应用中，这里应该基于 exe 所在目录构建路径
        // 为了演示 GitHub Action 编译后的运行，我们假设用户会在 exe 同级目录下创建一个 www 文件夹
        // 或者你可以硬编码一个测试路径，但最好动态获取
        
        // 获取当前模块所在目录 (简化版，实际应使用 GetModuleFileName)
        char buffer[MAX_PATH];
        GetModuleFileNameA(NULL, buffer, MAX_PATH);
        std::string exe_path(buffer);
        std::string exe_dir = exe_path.substr(0, exe_path.find_last_of("\\/"));
        
        // 假设资源放在 exe 同级的 www 目录下
        std::string full_path = exe_dir + "\\www\\" + relative_path;

        // 替换斜杠
        std::replace(full_path.begin(), full_path.end(), '/', '\\');

        return new LocalResourceHandler(full_path);
    }

    return nullptr;
}
