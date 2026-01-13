FROM ghcr.io/webassembly/wasi-sdk:latest

ENV DEBIAN_FRONTEND=noninteractive

# 安装必要工具 (CMake 已经是基础需求)
RUN apt-get update && apt-get install -y --no-install-recommends \
    wget \
    ca-certificates \
    cmake \
    ninja-build \
    binaryen \
    && rm -rf /var/lib/apt/lists/*

# 设置环境变量
ENV WASI_SDK_PATH=/opt/wasi-sdk

WORKDIR /project

# 默认指令改为查看版本或保持空，由外部脚本控制构建过程
CMD ["cmake", "--version"]