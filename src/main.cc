#include <windows.h>
#include <string>
#include "include/cef_app.h"
#include "simple_app.h"

// 获取当前 EXE 所在目录，统一使用正斜杠。
std::string GetExeDirectory() {
  char path[MAX_PATH] = {0};
  DWORD len = GetModuleFileNameA(nullptr, path, MAX_PATH);
  std::string full(path, len);
  for (char& c : full) {
    if (c == '\\') c = '/';
  }
  size_t pos = full.find_last_of('/');
  if (pos == std::string::npos) return ".";
  return full.substr(0, pos);
}

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                      LPWSTR lpCmdLine, int nCmdShow) {
  (void)hPrevInstance;
  (void)lpCmdLine;
  (void)nCmdShow;

  CefMainArgs main_args(hInstance);

  std::string exe_dir = GetExeDirectory();
  CefRefPtr<SimpleApp> app(new SimpleApp(exe_dir));

  // 处理 CEF 子进程（renderer/gpu 等）。>=0 表示子进程已自行处理并退出。
  // 把同一个 app 传入，确保子进程也能注册自定义 Scheme。
  int exit_code = CefExecuteProcess(main_args, app, nullptr);
  if (exit_code >= 0) {
    return exit_code;
  }

  CefSettings settings;
  settings.no_sandbox = true;
  settings.multi_threaded_message_loop = false;

  // 运行时资源/语言包位于 exe 同目录。
  CefString(&settings.resources_dir_path).FromASCII(exe_dir.c_str());
  CefString(&settings.locales_dir_path).FromASCII((exe_dir + "/locales").c_str());

  CefInitialize(main_args, settings, app, nullptr);
  CefRunMessageLoop();
  CefShutdown();

  return 0;
}
