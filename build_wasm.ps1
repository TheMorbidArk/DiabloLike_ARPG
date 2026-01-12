# --- 配置区域 ---
$PROJECT_NAME = "c"                       # C 项目所在的文件夹名称
$TIC80_EXE = ".\tic80\tic80.exe"          # 本地 Windows 版 TIC-80 路径
$DOCKER_IMAGE = "tic80-pro-builder"      # Docker 镜像名称
$WASM_PROJECT_PATH = "${PWD}\$PROJECT_NAME" # 映射到容器的绝对路径

# --- 脚本逻辑 ---

# 1. 清理旧编译产物 (Make Clean)
Write-Host "Step 1: Cleaning project..." -ForegroundColor Cyan
docker run --rm -v "${WASM_PROJECT_PATH}:/project" $DOCKER_IMAGE make clean

# 2. 编译 WASM (Make)
Write-Host "Step 2: Building WASM..." -ForegroundColor Cyan
docker run --rm -v "${WASM_PROJECT_PATH}:/project" $DOCKER_IMAGE make

if ($LASTEXITCODE -ne 0) {
    Write-Host "Build failed! Please check your C code." -ForegroundColor Red
    exit $LASTEXITCODE
}

# 3. 询问操作
$action = Read-Host "Build Success! Choose action: [1] Run in TIC-80  [2] Pack to .tic  [Any other] Exit"

if ($action -eq "1") {
    # 运行并测试
    Write-Host "Running in TIC-80..." -ForegroundColor Green
    & $TIC80_EXE --fs $WASM_PROJECT_PATH --cmd "load wasmdemo.wasmp & import binary build/cart.wasm & run"
} 
elseif ($action -eq "2") {
    # 打包为 .tic 卡带
    $outputName = Read-Host "Enter cartridge name (default: game.tic)"
    if ($outputName -eq "") { $outputName = "game.tic" }
    
    Write-Host "Packing to $outputName..." -ForegroundColor Green
    & $TIC80_EXE --fs $WASM_PROJECT_PATH --cmd "load wasmdemo.wasmp & import binary build/cart.wasm & save $outputName & exit"
    
    Write-Host "Done! Your cartridge is at: $PROJECT_NAME\$outputName" -ForegroundColor Yellow
}