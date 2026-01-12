ifndef WASI_SDK_PATH
$(error 请设置 WASI_SDK_PATH 环境变量，例如：export WASI_SDK_PATH=/opt/wasi-sdk)
endif

CC = "$(WASI_SDK_PATH)/bin/clang" --sysroot="$(WASI_SDK_PATH)/share/wasi-sysroot"
CXX = "$(WASI_SDK_PATH)/bin/clang++" --sysroot="$(WASI_SDK_PATH)/share/wasi-sysroot"

# ---------------------------
#      TIC-80 内存配置
# ---------------------------
# TIC-80 的 WASM 自由内存起始地址是 0x18000 (96KB)
# 我们将全局变量起始地址设在这里，防止覆盖前面的 VRAM 和 MAP 区域
GLOBAL_BASE = 98304
# 初始内存大小 (通常设为 256KB)
INITIAL_MEMORY = 262144
# 栈大小 (8KB 对 ARPG 来说通常够用，如果有深度递归需调大)
STACK_SIZE = 8192

# ---------------------------
#      构建配置
# ---------------------------
WASM_OPT = wasm-opt
WASM_OPT_FLAGS = -Oz --zero-filled-memory --strip-producers

DEBUG = 0

# 编译选项：添加了禁用特性的 flag
CFLAGS = -W -Wall -Wextra -Wno-unused -Wconversion -Wsign-conversion -MMD -MP \
         -fno-exceptions -I. -Isrc/ \
         -mno-bulk-memory -mno-nontrapping-fptoint -mno-sign-ext

ifeq ($(DEBUG), 1)
	CFLAGS += -DDEBUG -O0 -g
else
	CFLAGS += -DNDEBUG -O3 -flto
endif

# 链接选项：确保 --allow-undefined 存在
LDFLAGS = -Wl,-zstack-size=$(STACK_SIZE),--no-entry,--import-memory \
          -Wl,--initial-memory=$(INITIAL_MEMORY),--max-memory=$(INITIAL_MEMORY) \
          -Wl,--global-base=$(GLOBAL_BASE) \
          -Wl,--allow-undefined \
          -Wl,--export=TIC -Wl,--export=BOOT \
          -mexec-model=reactor

ifeq ($(DEBUG), 1)
	LDFLAGS += -Wl,--export-all,--no-gc-sections
else
	LDFLAGS += -Wl,--strip-all,--gc-sections,--lto-O3
endif

# 支持 src/ 目录下的源文件，也支持根目录（如果你把 main.c 放在根目录）
SOURCES_C = $(wildcard src/*.c) $(wildcard *.c)
SOURCES_CPP = $(wildcard src/*.cpp) $(wildcard *.cpp)
OBJECTS = $(patsubst %.c, build/%.o, $(notdir $(SOURCES_C))) \
          $(patsubst %.cpp, build/%.o, $(notdir $(SOURCES_CPP)))
DEPS = $(OBJECTS:.o=.d)

# ---------------------------
#      平台指令处理
# ---------------------------
ifeq ($(OS), Windows_NT)
	MKDIR_BUILD = if not exist build md build
	RMDIR = if exist build rd /s /q build
else
	MKDIR_BUILD = mkdir -p build
	RMDIR = rm -rf build
endif

all: build/cart.wasm

# 链接
build/cart.wasm: $(OBJECTS)
	$(CXX) -o $@ $(OBJECTS) $(LDFLAGS)
ifneq ($(DEBUG), 1)
	@$(WASM_OPT) --version > /dev/null 2>&1 || (echo "Tip: wasm-opt not found, skipping optimization.")
	@$(WASM_OPT) --version > /dev/null 2>&1 && $(WASM_OPT) $(WASM_OPT_FLAGS) $@ -o $@
endif

# 编译 C (注意这里用了 vpath 或者简单的模式匹配)
build/%.o: src/%.c
	@$(MKDIR_BUILD)
	$(CC) -c $< -o $@ $(CFLAGS)

build/%.o: %.c
	@$(MKDIR_BUILD)
	$(CC) -c $< -o $@ $(CFLAGS)

# 编译 C++
build/%.o: src/%.cpp
	@$(MKDIR_BUILD)
	$(CXX) -c $< -o $@ $(CFLAGS)

build/%.o: %.cpp
	@$(MKDIR_BUILD)
	$(CXX) -c $< -o $@ $(CFLAGS)

.PHONY: clean
clean:
	$(RMDIR)

-include $(DEPS)