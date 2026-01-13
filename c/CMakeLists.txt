cmake_minimum_required(VERSION 3.10)
project(TIC80_Isometric_ARPG C)

# --- 1. WASI-SDK 配置 ---
if(NOT DEFINED ENV{WASI_SDK_PATH})
    message(FATAL_ERROR "请设置 WASI_SDK_PATH 环境变量")
endif()
set(WASI_SDK_PATH $ENV{WASI_SDK_PATH})

# --- 2. TIC-80 内存与导出配置 ---
set(GLOBAL_BASE 98304)
set(INITIAL_MEMORY 262144)
set(STACK_SIZE 8192)

# --- 3. 编译器选项 ---
# 禁用异常、设置等轴测引擎需要的优化标志
add_compile_options(
    -W -Wall -Wextra -Wno-unused -Wconversion -Wsign-conversion
    -fno-exceptions
    -mno-bulk-memory 
    -mno-nontrapping-fptoint 
    -mno-sign-ext
)

# Debug/Release 差异化
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(DEBUG)
    add_compile_options(-O0 -g)
else()
    add_compile_definitions(NDEBUG)
    add_compile_options(-O3 -flto)
endif()

# --- 4. 源文件配置 ---
# 自动搜集子目录源码
file(GLOB_RECURSE SOURCES 
    "src/*.c"
)
include_directories(src)

# --- 5. 目标与链接配置 ---
add_executable(cart.wasm ${SOURCES})

# 强制输出后缀为 .wasm
set_target_properties(cart.wasm PROPERTIES OUTPUT_NAME "cart" SUFFIX ".wasm")

target_link_options(cart.wasm PRIVATE
    "-Wl,-zstack-size=${STACK_SIZE}"
    "-Wl,--no-entry"
    "-Wl,--import-memory"
    "-Wl,--initial-memory=${INITIAL_MEMORY}"
    "-Wl,--max-memory=${INITIAL_MEMORY}"
    "-Wl,--global-base=${GLOBAL_BASE}"
    "-Wl,--allow-undefined"
    "-Wl,--export=TIC"
    "-Wl,--export=BOOT"
    "-mexec-model=reactor"
)

if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_link_options(cart.wasm PRIVATE "-Wl,--strip-all" "-Wl,--gc-sections" "-Wl,--lto-O3")
endif()

# --- 6. 后处理 (wasm-opt) ---
find_program(WASM_OPT wasm-opt)
if(WASM_OPT AND NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_custom_command(TARGET cart.wasm POST_BUILD
        COMMAND ${WASM_OPT} -Oz --zero-filled-memory --strip-producers 
                $<TARGET_FILE:cart.wasm> -o $<TARGET_FILE:cart.wasm>
        COMMENT "Optimizing WASM with wasm-opt..."
    )
endif()