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

# 清理构建
make clean
```

### 单独运行测试

本项目没有独立的单元测试框架。测试通过以下方式进行：
- 运行 `buildwasm.sh` 或 `buildcart.sh` 查看运行时行为
- 在 TIC-80 模拟器中加载 `wasmdemo.wasmp` 并导入 `cart.wasm` 进行交互测试
- 修改代码后重新编译并运行，观察运行时效果

### Lint/类型检查

项目使用 CMake 编译，默认启用严格编译选项：
- `-Wall -Wextra` - 启用所有常见警告
- `-Wconversion -Wsign-conversion` - 类型转换警告
- 编译输出中的 warning 即为 lint 检查结果

---

## C 版本与 C++ 版本的核心差异

### 内存管理模型

**C 版本 - 直接操作 TIC-80 内存**
```c
// tic80.c 中定义内存指针，直接映射 TIC-80 虚拟内存地址
VRAM* FRAMEBUFFER = (VRAM*)0;
uint8_t* TILES = (uint8_t*)0x04000;
uint8_t* SPRITES = (uint8_t*)0x06000;
uint8_t* MAP = (uint8_t*)0x08000;
```
- 使用 `map_get_tile()` / `map_set_tile()` 直接读写 TIC-80 的 MAP 内存区域
- 渲染时直接操作 FRAMEBUFFER 或调用 TIC-80 API

**C++ 版本 - 内部数组 + TIC-80 API**
```cpp
// cpp/src/rendering/Map/Map.hpp
std::array<uint8_t, core::MAP_SIZE * core::MAP_SIZE> m_tiles{};
```
- 使用 `std::array` 内部数组存储地图数据，与 TIC-80 内存隔离
- 通过 `tic80::spr()`、`tic80::cls()` 等 API 函数间接渲染
- 需要调用 `sync()` 函数将数据同步到 TIC-80 内存（如果需要）

### 渲染流程对比

| 特性 | C 版本 | C++ 版本 |
|------|--------|----------|
| 地图存储 | TIC-80 MAP 内存 (0x08000) | 内部 `std::array` |
| 精灵/瓦片 | **运行时写入 TILES 内存** | 内部数组（未实现） |
| 屏幕绘制 | FRAMEBUFFER + TIC80 API | 仅调用 TIC80 API |
| 数据同步 | 天然同步 | 需手动 sync() |

### 精灵/瓦片运行时生成 (C 版本)

C 版本通过 `assets_init()` 直接操作 TIC-80 内存生成精灵：

```c
// assets.c - 直接写入 TILES 内存
uint8_t* ptr = TILES + addr;  // TILES = (uint8_t*)0x04000
*ptr = (*ptr & 0xF0) | (uint8_t)(color & 0x0F);
```

每个 8x8 瓦片占用 32 字节，复合精灵(16x16)由多个瓦片组成。

**C++ 版本当前缺失**：需要实现类似逻辑，通过 `tic80::poke()` 或直接内存写入生成精灵。

### TIC-80 API 完整性对比

**C 版本 - 完整实现** (`c/src/tic80.h/c`)
- 完整的常量定义：TILE_SIZE, WIDTH, HEIGHT, WIDTH_TILES, HEIGHT_TILES, BPP
- 完整的枚举定义：KEYCODES, BUTTON_CODES
- 完整的结构体：VRAM, Mouse
- 完整的内存指针声明：FRAMEBUFFER, TILES, SPRITES, MAP, GAMEPADS 等
- 完整的内存大小常量：TILES_SIZE, SPRITES_SIZE 等
- 完整的 API 函数声明（通过 WASM_IMPORT 导入）

**C++ 版本 - 不完整** (`cpp/src/tic80/Tic80.hpp`)
- 仅有 extern "C" 块的 API 声明
- namespace tic80 中的 wrapper 实现不完整，缺失：
  - `circ`, `circb`, `clip`, `elli`, `ellib`, `font`, `tri`, `trib`, `ttri`
  - `key`, `keyp`, `mouse`
  - `music`, `sfx`
  - `pmem`, `peek1/2/4`, `poke1/2/4`, `sync`, `vbank`
  - `fget`, `fset`, `tstamp`
- 缺失常量、枚举、结构体定义（应从 Config.hpp 获取或自行定义）
- 缺失内存指针声明（如需直接操作内存）

### 项目结构差异

**C 版本 - 函数式/过程式**
```
c/src/
├── main.c                      # 入口 (BOOT/TIC)
├── tic80.h/.c                 # TIC-80 内存映射
├── gameplay/scene/scene.c     # 场景系统 (scene_update/render 在这里)
├── rendering/map/map.c        # 地图操作
```

**C++ 版本 - 面向对象**
```
cpp/src/
├── main.cpp                    # 入口 + scene 函数 (extern "C")
├── gameplay/Scene/SceneManager.hpp  # 面向对象场景管理
├── rendering/Map/Map.hpp       # 单例 Map 类，内部数组
```

---

## 代码风格指南 (C 版本)

### 项目结构

```
c/src/
├── main.c                 # 入口点 (BOOT/TIC 函数)
├── tic80.h/.c            # TIC-80 API 绑定 + 内存指针
├── core/                 # 核心系统
│   ├── config.h          # 配置常量
│   ├── types.h           # 类型定义
│   ├── context.h         # 上下文
│   └── entity/           # 实体管理系统
├── rendering/            # 渲染系统
│   ├── assets/           # 资源加载
│   ├── camera/           # 相机系统
│   ├── map/              # 地图系统 (直接操作 MAP 内存)
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

---

## C++20 移植版本指南 (cpp/)

### 项目结构

```
cpp/
├── CMakeLists.txt
├── src/
│   ├── main.cpp               # 入口 + scene 函数 (extern "C")
│   ├── tic80/
│   │   └── Tic80.hpp          # TIC-80 API 绑定 (wrapper)
│   ├── core/
│   │   ├── Config.hpp         # 配置常量
│   │   ├── Types.hpp          # 类型定义 + concepts
│   │   ├── Context.hpp        # 单例上下文
│   │   └── Entity/
│   │       └── EntityManager.hpp
│   ├── rendering/
│   │   ├── Assets/
│   │   ├── Camera/
│   │   ├── Map/               # 内部 std::array 存储
│   │   └── Renderer/          # 调用 tic80:: API
│   ├── gameplay/
│   │   ├── Player/
│   │   ├── Scene/
│   │   │   └── SceneManager.hpp  # 场景管理器
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

### 禁止使用 C++ STL

**禁止使用以下 C++ 标准库组件**（WASM 嵌入式环境限制）：
- ❌ `std::vector`、`std::list`、`std::map`、`std::set` 等容器
- ❌ `std::string`、`std::string_view`
- ❌ `std::unique_ptr`、`std::shared_ptr`
- ❌ `std::function`
- ❌ `std::iostream`

**允许使用**：
- ✅ `std::array<T, N>` - 固定大小数组（栈上或静态）
- ✅ `std::span<T>` - 只读数组视图（轻量）
- ✅ `std::optional<T>` - 可选值
- ✅ `std::variant<T1, T2, ...>` - 类型联合
- ✅ C 标准库 (`<cstdlib>`, `<cstring>`, `<cmath>` 等)

**Map 生成算法示例**（使用静态数组替代 std::array）：

```cpp
// 错误 - 使用动态内存
void generate() {
    std::vector<uint8_t> temp(100);  // 禁止
}

// 正确 - 使用静态数组
void generate() {
    static uint8_t temp[100];  // 允许
}
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

---

## 关键设计说明

### 为什么 scene_init/scene_update 在 main.cpp 中

C++ 版本的 `main.cpp` 中定义了这些函数：

```cpp
extern "C" {
    void scene_init();
    void scene_update();
    void scene_render();
}
```

这是因为：
1. WASM 导出需要 `extern "C"` 来避免 name mangling
2. 将这些函数放在 main.cpp 可以被 TIC() 函数直接调用
3. 实际的场景逻辑封装在 `SceneManager` 类中

### 内存操作注意事项

**如果需要在 C++ 版本中使用 TIC-80 内存**：
- 使用 `tic80::peek()` / `tic80::poke()` 函数
- 或直接定义指针（需包含 `<cstdint>`）
- 使用 `tic80::sync()` 同步数据到 cartridge

**C++ 版本的推荐做法**：
- 保持内部数据结构（`std::array`）与 TIC-80 内存隔离
- 仅在需要时通过 `sync()` 同步
- 使用 `tic80::map()` 渲染时，会自动读取 TIC-80 MAP 内存

---

## 开发提示

1. 修改 `c/src/main.c` 或 `cpp/src/main.cpp` 实现游戏逻辑
2. 使用 `map_generate(seed, true)` / `Map::get().generate(seed, true)` 生成地图
3. 地图坐标系统：96x96 网格
4. 等距投影：使用 `world_to_screen()` 和 `screen_to_world()` 转换坐标
5. 实体通过 `entity_add()` 添加，使用 `entity_manager` 管理系统

---

## 常用命令速查

| 操作 | C 命令 | C++ 命令 |
|------|--------|----------|
| 构建 Debug | `cd c && make` | `cd cpp && make` |
| 构建 Release | `cd c && make build TYPE=Release` | `cd cpp && make build TYPE=Release` |
| 清理 | `cd c && make clean` | `cd cpp && make clean` |
| 构建并运行 | `./c/buildwasm.sh` | - |
