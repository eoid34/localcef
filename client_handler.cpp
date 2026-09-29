
#include "client_handler.h"
#include <iostream>

ClientHandler::ClientHandler() {}

void ClientHandler::OnTitleChange(CefRefPtr<CefBrowser> browser, const CefString& title) {
    // 可以在这里更新原生窗口标题
    // 由于是 Popup 窗口，CEF 会自动处理标题，这里仅做日志输出
    std::wcout << L"Title changed to: " << title.ToWString() << std::endl;
}

void ClientHandler::OnAfterCreated(CefRefPtr<CefBrowser> browser) {
    browser_ = browser;
}

bool ClientHandler::DoClose(CefRefPtr<CefBrowser> browser) {
    return false;
}

void ClientHandler::OnBeforeClose(CefRefPtr<CefBrowser> browser) {
    browser_ = nullptr;
}
