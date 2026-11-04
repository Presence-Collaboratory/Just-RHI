# Just RHI

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)
[![C++20](https://img.shields.io/badge/C%2B%2B-20-blue.svg)](https://en.cppreference.com/w/cpp/20)
[![MSBuild CI](https://github.com/NSDeathman/Just-RHI/actions/workflows/build.yml/badge.svg)](https://github.com/NSDeathman/Just-RHI/actions/workflows/build.yml)

A minimalistic render hardware interface (RHI) whose single goal is a **maximally simple API for the user**.

## Philosophy

Just RHI deliberately exposes only what you need to get pixels on screen — nothing more. No hidden state, no boilerplate pipelines, no hundred-line setup rituals. Create a device, compile a shader, draw.

```cpp
IDevice* device = CreateDevice(BackendType::DirectX12);
DeviceDesc desc;
desc.Width = 1280;
desc.Height = 720;
device->Init(hwnd, desc);

// ...create PSO, buffer, record commands...

device->Shutdown();
delete device;
```

## Features

- **Simple, flat interface** — a handful of classes, no inheritance trees to memorize
- **DirectX 12 backend** out of the box
- **C++20** required
- Windows **x64 / x86**
- Ships as a DLL with a stable C entry point (`CreateDevice`)

## Building

Requires **Visual Studio 2019+** (v142 toolset), the **Windows SDK**, and C++20 support.

```
msbuild RHI.sln /p:Configuration=Release /p:Platform=x64
```

Or just open `RHI.sln` in Visual Studio and hit Build.

A minimal **Triangle** sample lives under `Samples/Triangle` and is included in the solution.

## License

MIT — see [LICENSE](LICENSE).
