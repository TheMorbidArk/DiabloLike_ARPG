# escape=`
FROM mcr.microsoft.com/windows/servercore:ltsc2022

# 设置 PowerShell 为默认 Shell
SHELL ["powershell", "-Command", "$ErrorActionPreference = 'Stop'; $ProgressPreference = 'SilentlyContinue';"]

# 1. 预设 Scoop 环境变量
ENV SCOOP='C:\Scoop'
RUN [Environment]::SetEnvironmentVariable('SCOOP', $env:SCOOP, [EnvironmentVariableTarget]::Machine); `
    # 将 Scoop 的 shim 路径加入系统 PATH
    $env:Path += ';C:\Scoop\shims'; `
    [Environment]::SetEnvironmentVariable('Path', $env:Path, [EnvironmentVariableTarget]::Machine)

# 2. 安装 Scoop 并切换至 Gitee 国内镜像源
RUN iex (new-object net.webclient).downloadstring('https://get.scoop.sh'); `
    scoop config SCOOP_REPO "https://gitee.com/scoop-installer/scoop"; `
    scoop update

# 3. 安装基础开发工具 (Git, CMake)
RUN scoop install git cmake

# 4. 安装 Ruby 2.6 (对应 workflow 中的版本，需要 versions 桶)
# 注意：添加桶时通常仍需访问 GitHub，建议构建时开启代理
RUN scoop bucket add versions; `
    scoop install ruby26

# 5. 安装 MSYS2 并配置 UCRT64 工具链 (对应 Setup Mingw 步骤)
RUN scoop install msys2; `
    # 进入 MSYS2 环境执行 pacman 安装 UCRT 工具链
    # 这里建议在 pacman 步骤也通过命令行设置下清华源或中科院源，提高下载速度
    $msys_bash = 'C:\Scoop\apps\msys2\current\usr\bin\bash.exe'; `
    Start-Process $msys_bash -ArgumentList '--login', '-c', 'pacman -Syu --noconfirm' -Wait; `
    Start-Process $msys_bash -ArgumentList '--login', '-c', 'pacman -S --noconfirm base-devel mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-toolchain' -Wait

# 6. 将 MSYS2/UCRT64 路径添加到 Windows PATH (对应 msys2 {0} 环境)
RUN $env:Path = 'C:\Scoop\apps\msys2\current\ucrt64\bin;C:\Scoop\apps\msys2\current\usr\bin;' + $env:Path; `
    [Environment]::SetEnvironmentVariable('Path', $env:Path, [EnvironmentVariableTarget]::Machine)

# 设置工作目录
WORKDIR C:\build

# 7. 构建步骤 (对应 workflow 中的 Build 步骤)
# 我们假设代码通过 docker run -v 挂载，或者在这里 clone
# 此处以直接运行编译命令为例
ENTRYPOINT ["powershell", "-Command"]
CMD ["cd build; cmake -G 'MSYS Makefiles' -DCMAKE_BUILD_TYPE=Release -DBUILD_IPO=ON -DBUILD_WITH_ALL=ON .. ; cmake --build . --parallel"]