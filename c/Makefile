ifndef WASI_SDK_PATH
$(error 请设置 WASI_SDK_PATH 环境变量，例如：export WASI_SDK_PATH=/opt/wasi-sdk)
endif

CC = "$(WASI_SDK_PATH)/bin/clang" --sysroot="$(WASI_SDK_PATH)/share/wasi-sysroot"
CXX = "$(WASI_SDK_PATH)/bin/clang++" --sysroot="$(WASI_SDK_PATH)/share/wasi-sysroot"

# ---------------------------
#      TIC-80 内存配置
# ---------------------------
GLOBAL_BASE = 98304
INITIAL_MEMORY = 262144
STACK_SIZE = 8192

# ---------------------------
#      目录与源文件配置 (新)
# ---------------------------
# 定义所有的源代码子目录
SRC_DIRS = src \
           src/utils \
           src/systems \
           src/entities

# 使用 VPATH 让 Make 能够在这些目录中找到源文件
VPATH = $(SRC_DIRS)

# 查找所有目录下的 .c 文件
SOURCES_C = $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.c))
# 查找所有目录下的 .cpp 文件 (如果有)
SOURCES_CPP = $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.cpp))

# 将源文件名映射为 build 目录下的 .o 文件 (扁平化结构：src/systems/map.c -> build/map.o)
OBJECTS = $(patsubst %.c, build/%.o, $(notdir $(SOURCES_C))) \
          $(patsubst %.cpp, build/%.o, $(notdir $(SOURCES_CPP)))

# 依赖文件 (.d)
DEPS = $(OBJECTS:.o=.d)

# ---------------------------
#      构建配置
# ---------------------------
WASM_OPT = wasm-opt
WASM_OPT_FLAGS = -Oz --zero-filled-memory --strip-producers

DEBUG = 0

# 编译选项
# -Isrc 确保能从根目录找到头文件
CFLAGS = -W -Wall -Wextra -Wno-unused -Wconversion -Wsign-conversion -MMD -MP \
         -fno-exceptions -I. -Isrc \
         -mno-bulk-memory -mno-nontrapping-fptoint -mno-sign-ext

ifeq ($(DEBUG), 1)
	CFLAGS += -DDEBUG -O0 -g
else
	CFLAGS += -DNDEBUG -O3 -flto
endif

# 链接选项
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

# ---------------------------
#      平台指令处理
# ---------------------------
ifeq ($(OS), Windows_NT)
	# Windows 下简单的创建 build 目录逻辑
	MKDIR_BUILD = if not exist build md build
	RMDIR = if exist build rd /s /q build
else
	MKDIR_BUILD = mkdir -p build
	RMDIR = rm -rf build
endif

# ---------------------------
#      构建规则
# ---------------------------

all: build/cart.wasm

# 链接步骤
build/cart.wasm: $(OBJECTS)
	@echo "Linking $@"
	@$(CXX) -o $@ $(OBJECTS) $(LDFLAGS)
ifneq ($(DEBUG), 1)
	@$(WASM_OPT) --version > /dev/null 2>&1 || (echo "Tip: wasm-opt not found, skipping optimization.")
	@$(WASM_OPT) --version > /dev/null 2>&1 && $(WASM_OPT) $(WASM_OPT_FLAGS) $@ -o $@ && echo "Optimized with wasm-opt"
endif

# 编译 C 文件
# 由于使用了 VPATH，这里只需要匹配 %.c，Make 会自动在 SRC_DIRS 中寻找路径
build/%.o: %.c
	@$(MKDIR_BUILD)
	@echo "Compiling $<..."
	@$(CC) -c $< -o $@ $(CFLAGS)

# 编译 C++ 文件
build/%.o: %.cpp
	@$(MKDIR_BUILD)
	@echo "Compiling $<..."
	@$(CXX) -c $< -o $@ $(CFLAGS)

.PHONY: clean
clean:
	$(RMDIR)
	@echo "Cleaned build directory."

# 包含依赖文件（自动处理头文件修改后的重新编译）
-include $(DEPS)