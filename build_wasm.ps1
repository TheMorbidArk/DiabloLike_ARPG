# --- 配置区域 ---
$PROJECT_NAME = "c"                       # 你的 C 代码所在的文件夹名称 (c 或 cpp)
$TIC80_EXE = ".\tic80\tic80.exe"          # TIC-80 路径
$DOCKER_IMAGE = "tic80-pro-builder"
# 确保路径指向包含 CMakeLists.txt 的文件夹
$WASM_PROJECT_PATH = Join-Path $PSScriptRoot $PROJECT_NAME

# --- 参数解析 ---
$debugMode = $false
$releaseMode = $false
$packageMode = $false
$makeMode = $false
$cppMode = $false

if ($args.Count -gt 0) {
    foreach ($arg in $args) {
        switch ($arg.ToLower()) {
            "make" { $makeMode = $true }
            "debug" { $debugMode = $true }
            "release" { $releaseMode = $true }
            "package" { $packageMode = $true }
            "cpp" { $cppMode = $true }
            default {
                Write-Host "Error: Unknown argument '$arg'" -ForegroundColor Red
                Write-Host "Valid arguments: debug, release, package, cpp" -ForegroundColor Yellow
                exit 1
            }
        }
    }
}

# 如果没有指定参数，默认使用 release 模式
if (-not $debugMode -and -not $releaseMode -and -not $packageMode -and -not $makeMode) {
    $releaseMode = $true
}

# 如果指定了 cpp 参数，切换到 cpp 目录
if ($cppMode) {
    $PROJECT_NAME = "cpp"
    $WASM_PROJECT_PATH = Join-Path $PSScriptRoot $PROJECT_NAME
    Write-Host "Using C++ project (cpp/)" -ForegroundColor Cyan
}

# --- 辅助函数 ---
function trace {
    param ($message)
    if ($debugMode) {
        Write-Host "[DEBUG] $message" -ForegroundColor Magenta
    }
}

# --- 脚本逻辑 ---
trace "Starting build script with mode: $(if($debugMode) {"Debug"} elseif($releaseMode) {"Release"} elseif($packageMode) {"Package"})"

# 1. 检查 CMakeLists.txt 是否存在 (预防性检查)
if (-not (Test-Path (Join-Path $WASM_PROJECT_PATH "CMakeLists.txt"))) {
    Write-Host "Error: Cannot find CMakeLists.txt in $WASM_PROJECT_PATH" -ForegroundColor Red
    exit 1
}

trace "CMakeLists.txt found in $WASM_PROJECT_PATH"

# 2. 清理旧的构建目录
Write-Host "Step 1: Preparing build directory..." -ForegroundColor Cyan
trace "Removing old build directory if exists"
$buildPath = Join-Path $WASM_PROJECT_PATH "build"
if (Test-Path $buildPath) { Remove-Item -Recurse -Force $buildPath }

# 3. 运行 Docker 进行编译
Write-Host "Step 2: Building WASM via CMake..." -ForegroundColor Cyan

# 解释：
# -v "${WASM_PROJECT_PATH}:/project" 将你的 'c' 文件夹映射到容器的 /project
# cmake 会在 /project 目录下寻找 CMakeLists.txt
$buildType = if ($debugMode) { "Debug" } else { "Release" }

trace "Starting Docker build with CMAKE_BUILD_TYPE=$buildType"
docker run --rm -t -v "${WASM_PROJECT_PATH}:/project" `
    -e CLICOLOR_FORCE=1 `
    $DOCKER_IMAGE `
    sh -c "cmake -B build -S . -DCMAKE_BUILD_TYPE=$buildType -DCMAKE_COLOR_DIAGNOSTICS=ON -DCMAKE_TOOLCHAIN_FILE=/opt/wasi-sdk/share/cmake/wasi-sdk.cmake && cmake --build build"

if ($LASTEXITCODE -ne 0) {
    Write-Host "Build failed!" -ForegroundColor Red
    exit $LASTEXITCODE
}

trace "Build completed successfully"

# 根据参数模式执行不同操作
if ($packageMode) {
    # 直接进入打包模式
    $outputName = "game.tic"
    Write-Host "Packaging build into $outputName..." -ForegroundColor Cyan
    trace "Running TIC-80 packaging commands"
    & $TIC80_EXE --fs $WASM_PROJECT_PATH --cmd "load wasmdemo.wasmp & import binary build/cart.wasm & save $outputName & exit"
} else {
    # 根据参数模式执行相应操作
    if ($debugMode) {
        Write-Host "Debug mode completed. Running in TIC-80..." -ForegroundColor Green
        trace "Running TIC-80 with the WASM binary"
        & $TIC80_EXE --fs $WASM_PROJECT_PATH --cmd "load wasmdemo.wasmp & import binary build/cart.wasm & run"
    }
    elseif ($releaseMode) {
        Write-Host "Release mode completed. Running in TIC-80..." -ForegroundColor Green
        & $TIC80_EXE --fs $WASM_PROJECT_PATH --cmd "load wasmdemo.wasmp & import binary build/cart.wasm & run"
    }
    elseif ($packageMode) {
        $outputName = "game.tic"
        Write-Host "Packaging build into $outputName..." -ForegroundColor Cyan
        trace "Running TIC-80 packaging commands"
        & $TIC80_EXE --fs $WASM_PROJECT_PATH --cmd "load wasmdemo.wasmp & import binary build/cart.wasm & save $outputName & exit"
    }
    elseif($makeMode){
        Write-Host "Make mode completed. You Can Running in TIC-80..." -ForegroundColor Green
    }
}
