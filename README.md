# OpenAnimationReplacer

> **Localization (汉化版)** — 本仓库是 [ersh1/OpenAnimationReplacer](https://github.com/ersh1/OpenAnimationReplacer) 的汉化版（I18N），基于 **3.2.1**。
>
> - 翻译外置于 `SKSE/Plugins/OpenAnimationReplacer.json`（扁平 JSON：key 为英文原文，value 为译文），找不到词条时回退英文
> - 界面字符串通过 `src/PCH.h` 的 `#define _T(s) Localization::Translate(s)` 走翻译表
> - 中文字体：启动时从 `SKSE/Plugins/OpenAnimationReplacer/fonts/` 读取第一个 `.ttf/.otf`
> - 提供 en / ru / ja / ko / fr / de / es / zh-CN 八种语言（FOMOD 多语言包）
> - `src/CMakeLists.txt` 额外加了 `/FS`（PDB 并发写入防护）
>
> 上游原始说明见下。

A SKSE framework plugin that replaces animations depending on configurable conditions. In-game editor. Backwards compatible with more features. Extensible by other SKSE plugins. Supports SE/AE/VR. Open source.

[Nexus](https://www.nexusmods.com/skyrimspecialedition/mods/92109)

## Requirements

- [CMake](https://cmake.org/)
  - Add this to your `PATH`
- [PowerShell](https://github.com/PowerShell/PowerShell/releases/latest)
- [Vcpkg](https://github.com/microsoft/vcpkg)
  - Add the environment variable `VCPKG_ROOT` with the value as the path to the folder containing vcpkg
- [Visual Studio Community 2019](https://visualstudio.microsoft.com/)
  - Desktop development with C++
- [CommonLibSSE-NG](https://github.com/alandtse/CommonLibSSE-NG)
  - Optional: set `CommonLibSSEPath` to a shared checkout. When it points to a valid checkout, it is used before the `extern/CommonLibSSE` submodule.

## User Requirements

- [Address Library for SKSE](https://www.nexusmods.com/skyrimspecialedition/mods/32444)
  - Needed for SSE/AE
- [VR Address Library for SKSEVR](https://www.nexusmods.com/skyrimspecialedition/mods/58101)
  - Needed for VR

## Register Visual Studio as a Generator

- Open `x64 Native Tools Command Prompt`
- Run `cmake`
- Close the cmd window

## Building

```
git clone https://github.com/ersh1/OpenAnimationReplacer.git
cd OpenAnimationReplacer
# pull commonlib /extern to override the path settings
git submodule init
# to update submodules to checked in build
git submodule update
#configure cmake
cmake --preset vs2022-windows
#build dll
cmake --build build --config Release
```

## License

[GPL-3.0-or-later](COPYING) WITH [Modding Exception AND GPL-3.0 Linking Exception (with Corresponding Source)](EXCEPTIONS). Specifically, the Modded Code is Skyrim (and its variants) and Modding Libraries include [SKSE](https://skse.silverlock.org/) and Commonlib (and variants).
