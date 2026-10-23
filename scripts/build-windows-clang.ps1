param(
    [Parameter(Mandatory = $true)]
    [ValidateSet("Configure", "Build", "Run")]
    [string] $Action
)

$ErrorActionPreference = "Stop"
$repositoryRoot = Split-Path -Parent $PSScriptRoot
$workspaceRoot = Split-Path -Parent $repositoryRoot
$buildDirectory = Join-Path $workspaceRoot "build-win-clang"

if ($Action -eq "Run") {
    $gameDirectory = Join-Path $buildDirectory "game"
    $executable = Join-Path $gameDirectory "gourmandoangband.exe"
    if (-not (Test-Path -LiteralPath $executable)) {
        throw "Built game executable not found: $executable"
    }

    Push-Location $gameDirectory
    try {
        & $executable
        exit $LASTEXITCODE
    } finally {
        Pop-Location
    }
}

$vswhere = Join-Path ([Environment]::GetFolderPath("ProgramFilesX86")) `
    "Microsoft Visual Studio\Installer\vswhere.exe"
$visualStudioRoot = & $vswhere -latest -products "*" -property installationPath |
    Select-Object -First 1
if (-not $visualStudioRoot) {
    throw "Visual Studio installation not found by vswhere: $vswhere"
}
$visualStudioRoot = $visualStudioRoot.Trim()

$developerCommand = Join-Path $visualStudioRoot "Common7\Tools\VsDevCmd.bat"
$vcpkgToolchain = Join-Path $visualStudioRoot `
    "VC\vcpkg\scripts\buildsystems\vcpkg.cmake"
if (-not (Test-Path -LiteralPath $developerCommand)) {
    throw "Visual Studio developer command script not found: $developerCommand"
}
if (-not (Test-Path -LiteralPath $vcpkgToolchain)) {
    throw "Visual Studio vcpkg CMake toolchain not found: $vcpkgToolchain"
}

$command = 'call "' + $developerCommand + '" -arch=x64 -host_arch=x64'
if ($Action -eq "Configure") {
    $installedDirectory = Join-Path $buildDirectory "vcpkg_installed"
    $command += ' && cmake -G Ninja -DCMAKE_C_COMPILER=clang-cl ' +
        '-DCMAKE_RC_COMPILER=llvm-rc -DCMAKE_TOOLCHAIN_FILE="' +
        $vcpkgToolchain + '" -DVCPKG_TARGET_TRIPLET=x64-windows-static ' +
        '-DVCPKG_INSTALLED_DIR="' + $installedDirectory + '" ' +
        '-DSUPPORT_WINDOWS_FRONTEND=ON -DSUPPORT_SDL2_FRONTEND=OFF ' +
        '-DSUPPORT_SDL_FRONTEND=OFF -DSUPPORT_STATIC_LINKING=OFF ' +
        '-S "' + $repositoryRoot + '" -B "' + $buildDirectory + '"'
} else {
    $command += ' && cmake --build "' + $buildDirectory + '"'
}

& $env:ComSpec /d /c $command
exit $LASTEXITCODE
