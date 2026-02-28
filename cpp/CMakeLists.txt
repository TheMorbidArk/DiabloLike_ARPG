cmake_minimum_required(VERSION 3.20)
project(TIC80_Isometric_ARPG CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT DEFINED ENV{WASI_SDK_PATH})
    message(FATAL_ERROR "请设置 WASI_SDK_PATH 环境变量")
endif()
set(WASI_SDK_PATH $ENV{WASI_SDK_PATH})

set(GLOBAL_BASE 98304)
set(INITIAL_MEMORY 262144)
set(STACK_SIZE 8192)

add_compile_options(
    -Wall -Wextra -Wno-unused -Wconversion -Wsign-conversion
    -fno-exceptions -fno-rtti
)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(DEBUG)
    add_compile_options(-O0 -g)
else()
    add_compile_definitions(NDEBUG)
    add_compile_options(-O3 -flto)
endif()

file(GLOB_RECURSE SOURCES 
    "src/*.cpp"
    "src/*.c"
)
include_directories(src)

add_executable(cart.wasm ${SOURCES})

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
    "-Wl,--export-all"
    "-mexec-model=reactor"
)

if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_link_options(cart.wasm PRIVATE "-Wl,--gc-sections" "-Wl,--lto-O3")
endif()

if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_link_options(cart.wasm PRIVATE "-Wl,--lto-O3")
endif()

find_program(WASM_OPT wasm-opt)
# if(WASM_OPT AND NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
#     add_custom_command(TARGET cart.wasm POST_BUILD
#         COMMAND ${WASM_OPT} -Oz --zero-filled-memory --strip-producers 
#                 $<TARGET_FILE:cart.wasm> -o $<TARGET_FILE:cart.wasm>
#         COMMENT "Optimizing WASM with wasm-opt..."
#     )
# endif()
