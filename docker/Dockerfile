# 使用官方 WASI SDK 镜像
FROM ghcr.io/webassembly/wasi-sdk:latest

# 避免交互式安装提示
ENV DEBIAN_FRONTEND=noninteractive

# 1. 安装基础工具，增加 binaryen (包含 wasm-opt)
RUN apt-get update && apt-get install -y --no-install-recommends \
    wget \
    ca-certificates \
    make \
    binaryen \
    && rm -rf /var/lib/apt/lists/*

# 2. 下载指定的 TIC-80 版本
RUN wget https://github.com/nesbox/TIC-80/releases/download/v1.1.2837/tic80-v1.1-linux.deb -O /tmp/tic80.deb

# 3. 安装 TIC-80 及其依赖
RUN apt-get update && \
    apt-get install -y /tmp/tic80.deb --no-install-recommends && \
    rm -rf /var/lib/apt/lists/* /tmp/tic80.deb

# 4. 设置环境变量
ENV WASI_SDK_PATH=/opt/wasi-sdk

WORKDIR /project

CMD ["make"]