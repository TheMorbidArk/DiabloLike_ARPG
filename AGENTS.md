# AGENTS.md - 代码代理指南

本文档为在此代码库中工作的 AI 代理提供开发指南。

## 项目概述

本项目是一个基于 TIC-80 的等距视角 ARPG 游戏，使用 C 语言开发，目标编译为 WebAssembly (WASM)。
同时提供 C++20 移植版本在 `cpp/` 目录下。

## 构建/测试命令

### 环境要求

- [WASI SDK](https://github.com/WebAssembly/wasi-sdk) - 必须设置 `WASI_SDK_PATH` 环境变量
- TIC-80 模拟器
- CMake 3.20+
- make

### C 项目构建命令

```bash
# 进入 C 项目目录
cd c

# 构建 WASM (Debug)
make

# 构建 WASM (Release)
make build TYPE=Release

# 清理构建
make clean

# 便捷脚本：构建并运行 WASM
./buildwasm.sh

# 便捷脚本：构建 TIC-80 卡带并运行
./buildcart.sh
```

### C++ 项目构建命令

```bash
# 进入 C++ 项目目录
cd cpp

# 构建 WASM (Debug)
make

# 构建 WASM (Release)
make build TYPE=Release
```

### 单独运行测试

本项目没有独立的单元测试框架。测试通过以下方式进行：
- 运行 `buildwasm.sh` 或 `buildcart.sh` 查看运行时行为
- 在 TIC-80 模拟器中加载 `wasmdemo.wasmp` 并导入 `cart.wasm` 进行交互测试

## 代码风格指南 (C 版本)

### 项目结构

```
c/src/
├── main.c                 # 入口点 (BOOT/TIC 函数)
├── tic80.h/.c            # TIC-80 API 绑定
├── core/                 # 核心系统
│   ├── config.h          # 配置常量
│   ├── types.h           # 类型定义
│   ├── context.h         # 上下文
│   └── entity/           # 实体管理系统
├── rendering/            # 渲染系统
│   ├── assets/           # 资源加载
│   ├── camera/           # 相机系统
│   ├── map/              # 地图系统
│   └── renderer/         # 主渲染器
├── gameplay/            # 游戏逻辑
│   ├── player/           # 玩家系统
│   ├── scene/            # 场景系统
│   │   └── scenes/       # 具体场景实现
│   └── battle/           # 战斗系统
└── utils/                # 工具函数
    └── math_iso.h/.c     # 等距数学库
```

### 命名约定

- **文件命名**: 使用下划线分隔的小写字母 (`renderer.h`, `entity_manager.c`)
- **类型命名**: 使用下划线分隔，末尾加 `_t` (`Entity`, `EntityType`, `EntityID`)
- **函数命名**: 使用下划线分隔 (`entity_add`, `map_get_tile`)
- **常量/宏**: 全大写加下划线 (`WIDTH`, `HEIGHT`, `MAP_SIZE`)
- **变量**: 小写下划线 (`player_pos`, `cam_x`)

### 头文件保护

所有头文件必须使用 include guard：

```c
#ifndef RENDERER_H
#define RENDERER_H
// 内容
#endif
```

### 导入顺序

头文件导入顺序（从局部到全局）：
1. 当前模块对应的头文件
2. 项目内部头文件（按目录深度排序）
3. 第三方库头文件
4. C 标准库头文件

```c
#include "renderer.h"           // 当前模块
#include "../../tic80.h"        // 父目录模块
#include "../../core/config.h"  // 深层模块
#include "../../utils/math_iso.h"
#include <stdio.h>              // 标准库
```

### 代码格式化

- 缩进：4 空格（不要使用 Tab）
- 行长度：尽量控制在 100 字符以内
- 大括号：函数体使用 K&R 风格，其他使用 Allman 风格
- 运算符前后保留空格：`a + b`, `x = y`

### 类型系统

- 优先使用固定宽度整数类型 (`uint8_t`, `int32_t`, `float` 等)
- 避免使用 `int`（除非用于循环计数器）
- 显式指定有符号/无符号

### 错误处理

- 本项目为 WASM 嵌入式项目，无异常处理
- 使用条件检查进行错误处理
- 关键函数返回值检查

### 编译选项

CMake 中启用的严格编译选项：
- `-Wall -Wextra` - 启用所有常见警告
- `-Wconversion -Wsign-conversion` - 类型转换警告
- `-Wno-unused` - 允许未使用变量
- `-fno-exceptions` - 禁用异常

### WASM 导出约定

- 使用 `WASM_EXPORT("FunctionName")` 宏导出函数
- 入口点：`BOOT()`（初始化）和 `TIC()`（每帧更新）
- 导出函数必须使用 C 调用约定

## C++20 移植版本指南 (cpp/)

### 项目结构

```
cpp/
├── CMakeLists.txt
├── src/
│   ├── main.cpp
│   ├── tic80/
│   │   └── Tic80.hpp         # TIC-80 API 绑定
│   ├── core/
│   │   ├── Config.hpp        # 配置常量
│   │   ├── Types.hpp         # 类型定义 + concepts
│   │   ├── Context.hpp       # 单例上下文
│   │   └── Entity/
│   │       └── EntityManager.hpp
│   ├── rendering/
│   │   ├── Assets/
│   │   ├── Camera/
│   │   ├── Map/
│   │   └── Renderer/
│   ├── gameplay/
│   │   ├── Player/
│   │   ├── Scene/
│   │   └── Battle/
│   └── utils/
│       └── MathIso.hpp
```

### C++20 命名规范

| 类型 | 规范 | 示例 |
|------|------|------|
| 类/结构体 | PascalCase | `EntityManager`, `PlayerState` |
| 方法/函数 | camelCase | `getPosition()`, `updateScene()` |
| 常量 | k前缀+PascalCase | `kMaxEntities`, `kPi` |
| 枚举类 | PascalCase | `EntityType::Player` |
| 成员变量 | m_前缀 | `m_position`, `m_active` |
| 文件 | PascalCase.hpp | `Types.hpp`, `MathIso.hpp` |

### C++20 特性使用

```cpp
// 1. Concepts 约束模板
template <typename T>
concept Numeric = std::integral<T> || std::floating_point<T>;

// 2. Constexpr 和 consteval
constexpr float kPi = 3.14159f;
consteval int square(int x) { return x * x; }

// 3. std::array / std::span
std::array<uint8_t, 256> m_tiles{};
std::span<EntityData> getArray();

// 4. std::optional
std::optional<std::reference_wrapper<EntityData>> getPlayer();

// 5. std::variant (场景系统)
using SceneVariant = std::variant<ExplorationScene, BattleScene>;

// 6. 结构化绑定
auto [x, y, z] = player.position;

// 7. 命名空间单例模式
class Camera {
public:
    static Camera& get() { static Camera instance; return instance; }
private:
    Camera() = default;
};
```

### 头文件规范

```cpp
#pragma once

#include "core/Types.hpp"     // 项目内部
#include "utils/MathIso.hpp"  // 工具类
#include <array>              // STL
#include <vector>
#include <span>
#include <optional>
#include <variant>
#include <concepts>
```

### WASM 导出 (C++)

```cpp
#define WASM_EXPORT(name) __attribute__((export_name(name)))
#define WASM_IMPORT(name) __attribute__((import_name(name)))

extern "C" {

WASM_EXPORT("BOOT")
void BOOT() {
    // 初始化代码
}

WASM_EXPORT("TIC")
void TIC() {
    // 每帧更新
}

}
```

## 常用命令速查

| 操作 | C 命令 | C++ 命令 |
|------|--------|----------|
| 构建 Debug | `cd c && make` | `cd cpp && make` |
| 构建 Release | `cd c && make build TYPE=Release` | `cd cpp && make build TYPE=Release` |
| 清理 | `cd c && make clean` | `cd cpp && make clean` |
| 构建并运行 | `./c/buildwasm.sh` | - |

## 开发提示

1. 修改 `c/src/main.c` 或 `cpp/src/main.cpp` 实现游戏逻辑
2. 使用 `map_generate(seed, true)` / `Map::get().generate(seed, true)` 生成地图
3. 地图坐标系统：96x96 网格
4. 等距投影：使用 `world_to_screen()` 和 `screen_to_world()` 转换坐标
5. 实体通过 `entity_add()` 添加，使用 `entity_manager` 管理系统
