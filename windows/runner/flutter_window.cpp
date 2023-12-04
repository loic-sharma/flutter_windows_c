#include "flutter_window.h"

#include <optional>

#include "flutter/generated_plugin_registrant.h"

FlutterWindow::FlutterWindow(FlutterDesktopEngineRef engine)
    : engine_(engine) {}

FlutterWindow::~FlutterWindow() {}

bool FlutterWindow::OnCreate() {
  if (!Win32Window::OnCreate()) {
    return false;
  }

  RECT frame = GetClientArea();

  // The size here must match the window dimensions to avoid unnecessary surface
  // creation / destruction in the startup path.
  int width = frame.right - frame.left;
  int height = frame.bottom - frame.top;

  controller_ = FlutterDesktopViewControllerCreate(width, height, engine_);  
  if (controller_ == nullptr) {
    return false;
  }

  FlutterDesktopViewRef view{FlutterDesktopViewControllerGetView(controller_)};
  // RegisterPlugins(flutter_controller_->engine());
  SetChildContent(FlutterDesktopViewGetHWND(view));

  FlutterDesktopEngineSetNextFrameCallback(
    engine_,
    [](void* user_data) {
      FlutterWindow* self = static_cast<FlutterWindow*>(user_data);
      self->Show();
    },
    this);

  // Flutter can complete the first frame before the "show window" callback is
  // registered. The following call ensures a frame is pending to ensure the
  // window is shown. It is a no-op if the first frame hasn't completed yet.
  FlutterDesktopViewControllerForceRedraw(controller_);
  return true;
}

void FlutterWindow::OnDestroy() {
  if (controller_) {
    FlutterDesktopViewControllerDestroy(controller_);
  }

  Win32Window::OnDestroy();
}

LRESULT
FlutterWindow::MessageHandler(HWND hwnd, UINT const message,
                              WPARAM const wparam,
                              LPARAM const lparam) noexcept {
  // Give Flutter, including plugins, an opportunity to handle window messages.
  if (controller_) {
    LRESULT result;
    bool handled = FlutterDesktopViewControllerHandleTopLevelWindowProc(
      controller_,
      hwnd,
      message,
      wparam,
      lparam,
      &result);

    if (handled) {
      return result;
    }
  }

  switch (message) {
    case WM_FONTCHANGE:
      FlutterDesktopEngineReloadSystemFonts(engine_);
      break;
  }

  return Win32Window::MessageHandler(hwnd, message, wparam, lparam);
}
