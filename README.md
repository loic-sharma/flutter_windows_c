# Flutter Windows C API

Flutter Windows apps interact with Flutter using a C++ API. However, this C++
API is actually a wrapper built on top of Flutter Window's C API. This sample
shows how to use Flutter Window's C API directly.

Noteworthy files:

1. `windows/runner/main.cpp`
2. `windows/runner/flutter_window.h`
3. `windows/runner/flutter_window.cpp`

### Background

C++ does not have a stable ABI and two libraries built separately might
not interoperate. Flutter Windows avoids this problem by exposing
a low-level C API.

When you build your app, the Flutter tool injects C++ source code into the app's
`flutter/ephemeral/cpp_client_wrapper` directory. This injected code "wraps"
Flutter Window's C API and provides nicer C++ abstractions.

Here's pseudocode showing the C API:

```cpp
FlutterDesktopEngineProperties engine_properties = {};

// Create the engine.
auto engine = FlutterDesktopEngineCreate(&engine_properties);

// Create a view controller. The view controller takes ownership of the engine.
auto controller = FlutterDesktopViewControllerCreate(width, height, engine);

// Run app...

// Destroy the view controller. This also destroys the engine.
FlutterDesktopViewControllerDestroy(controller);
```

Here's pseudocode showing the C++ API:

```cpp
// Create a view controller. This also creates an engine.
flutter::DartProject project{L"data"};
flutter::FlutterViewController controller{width, height, &project};

// Run app...
```
