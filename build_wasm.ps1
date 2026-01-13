# --- 配置区域 ---
$PROJECT_NAME = "c"                       # 你的 C 代码所在的文件夹名称
$TIC80_EXE = ".\tic80\tic80.exe"          # TIC-80 路径
$DOCKER_IMAGE = "tic80-pro-builder" 
# 确保路径指向包含 CMakeLists.txt 的文件夹
$WASM_PROJECT_PATH = Join-Path $PSScriptRoot $PROJECT_NAME

# --- 脚本逻辑 ---

# 1. 检查 CMakeLists.txt 是否存在 (预防性检查)
if (-not (Test-Path (Join-Path $WASM_PROJECT_PATH "CMakeLists.txt"))) {
    Write-Host "Error: Cannot find CMakeLists.txt in $WASM_PROJECT_PATH" -ForegroundColor Red
    exit 1
}

# 2. 清理旧的构建目录
Write-Host "Step 1: Preparing build directory..." -ForegroundColor Cyan
$buildPath = Join-Path $WASM_PROJECT_PATH "build"
if (Test-Path $buildPath) { Remove-Item -Recurse -Force $buildPath }

# 3. 运行 Docker 进行编译
Write-Host "Step 2: Building WASM via CMake..." -ForegroundColor Cyan

# 解释：
# -v "${WASM_PROJECT_PATH}:/project" 将你的 'c' 文件夹映射到容器的 /project
# cmake 会在 /project 目录下寻找 CMakeLists.txt
docker run --rm -t -v "${WASM_PROJECT_PATH}:/project" `
    -e CLICOLOR_FORCE=1 `
    $DOCKER_IMAGE `
    sh -c "cmake -B build -S . -DCMAKE_BUILD_TYPE=Release -DCMAKE_COLOR_DIAGNOSTICS=ON -DCMAKE_TOOLCHAIN_FILE=/opt/wasi-sdk/share/cmake/wasi-sdk.cmake && cmake --build build"

if ($LASTEXITCODE -ne 0) {
    Write-Host "Build failed!" -ForegroundColor Red
    exit $LASTEXITCODE
}

# 4. 运行或打包
$action = Read-Host "Build Success! [1] Run in TIC-80 [2] Pack [Any] Exit"

# 注意：TIC-80 的工作目录需要设为 $WASM_PROJECT_PATH 才能找到 build/cart.wasm
if ($action -eq "1") {
    & $TIC80_EXE --fs $WASM_PROJECT_PATH --cmd "load wasmdemo.wasmp & import binary build/cart.wasm & run"
} 
elseif ($action -eq "2") {
    $outputName = Read-Host "Enter name (default: game.tic)"
    if ($outputName -eq "") { $outputName = "game.tic" }
    & $TIC80_EXE --fs $WASM_PROJECT_PATH --cmd "load wasmdemo.wasmp & import binary build/cart.wasm & save $outputName & exit"
}