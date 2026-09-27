@echo off
rem The test program prints UTF-8 Chinese test names. A console whose code page
rem is 936/GBK renders those bytes as mojibake, so switch this console to UTF-8
rem first.
rem
rem This file MUST stay ASCII-only and BOM-free: cmd.exe parses .bat using the
rem OEM code page, so non-ASCII bytes here would be mangled, and a leading BOM
rem would break the very first line.
chcp 65001 >nul
setlocal
pushd "%~dp0"

rem ---------------------------------------------------------------------------
rem Auto-detect the workspace-local toolchain.
rem Everything lives in ..\tools\ ON PURPOSE, instead of being installed
rem system-wide: on 2026-09-20 the system-wide MinGW in C:\main was wiped by
rem something outside the project, which broke the build for no good reason.
rem Keeping the toolchain inside the workspace makes the build self-contained.
rem
rem   ..\tools\mingw64\bin  -> g++                (portable MinGW-w64)
rem   ..\tools\cmake\bin    -> cmake / ctest / ninja
rem   ..\tools\ninja\bin    -> ninja              (older layout, kept for compat)
rem
rem If a folder is missing we fall back to whatever is already on PATH.
rem ---------------------------------------------------------------------------
set "TOOLS=%~dp0..\tools"
if exist "%TOOLS%\mingw64\bin\g++.exe" set "PATH=%TOOLS%\mingw64\bin;%PATH%"
if exist "%TOOLS%\cmake\bin\cmake.exe" set "PATH=%TOOLS%\cmake\bin;%PATH%"
if exist "%TOOLS%\ninja\bin\ninja.exe" set "PATH=%TOOLS%\ninja\bin;%PATH%"

rem ---------------------------------------------------------------------------
rem IMPORTANT: every error check below uses
rem     if %ERRORLEVEL% neq 0
rem and NOT
rem     if errorlevel 1
rem "errorlevel 1" is only true for exit codes >= 1, so it MISSES negative
rem codes such as -1073741515 (0xC0000135, "DLL not found"). A test binary that
rem crashes on startup would then be reported as "Test OK".
rem ---------------------------------------------------------------------------

rem Usage: build.bat [gpp^|cmake^|test^|clean]     (no argument = gpp)
set "MODE=%~1"
if "%MODE%"=="" set "MODE=gpp"

if /i "%MODE%"=="gpp"   goto :gpp
if /i "%MODE%"=="cmake" goto :cmake
if /i "%MODE%"=="test"  goto :test
if /i "%MODE%"=="clean" goto :clean

echo Unknown argument: %MODE%
echo Usage: build.bat [gpp^|cmake^|test^|clean]
goto :error

rem ---------------------------------------------------------------------------
rem Mode 1: plain g++. Fastest, no CMake required.
rem ---------------------------------------------------------------------------
:gpp
where g++ 1>nul 2>nul
if %ERRORLEVEL% neq 0 (
    echo g++ not found.
    echo Expected the portable toolchain at:
    echo     %~dp0..\tools\mingw64\bin\g++.exe
    echo Restore that folder, or put any C++ compiler on PATH.
    goto :error
)

if not exist build mkdir build

echo [1/2] Building mini_infer_app...
rem -static links libstdc++ / libgcc / winpthread INTO the exe.
rem Without it the .exe needs MinGW's runtime DLLs to be on PATH, so
rem double-clicking build\mini_infer_app.exe fails with 0xC0000135
rem (DLL not found) even though the build itself succeeded.
g++ -std=c++17 -Wall -Wextra -static -Iinclude src\main.cpp src\tensor.cpp -o build\mini_infer_app.exe
if %ERRORLEVEL% neq 0 goto :error

echo [2/2] Building test_tensor...
g++ -std=c++17 -Wall -Wextra -static -Iinclude tests\test_tensor.cpp src\tensor.cpp -o build\test_tensor.exe
if %ERRORLEVEL% neq 0 goto :error

echo.
echo Build OK.
echo Run:  build\mini_infer_app.exe
echo Test: build\test_tensor.exe
goto :done

rem ---------------------------------------------------------------------------
rem Mode 2: full CMake + Ninja + CTest
rem ---------------------------------------------------------------------------
:cmake
where cmake 1>nul 2>nul
if %ERRORLEVEL% neq 0 (
    echo cmake not found. Run this first:
    echo     call ..\tools\env.cmd
    goto :error
)

cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
if %ERRORLEVEL% neq 0 goto :error

cmake --build build
if %ERRORLEVEL% neq 0 goto :error

ctest --test-dir build --output-on-failure
if %ERRORLEVEL% neq 0 goto :error

echo.
echo CMake build OK.
goto :done

rem ---------------------------------------------------------------------------
rem Mode 3: run the already-built test binary
rem ---------------------------------------------------------------------------
:test
if not exist build\test_tensor.exe (
    echo build\test_tensor.exe not found. Run "build.bat" first.
    goto :error
)

rem Warn if the binary is older than the sources: a stale binary can "pass"
rem while the code you just edited was never compiled.
rem The timestamp format is yyyy/MM/dd HH:mm, so plain string compare is valid.
for %%A in (build\test_tensor.exe)  do set "EXE_T=%%~tA"
for %%A in (src\tensor.cpp)         do set "SRC_T=%%~tA"
for %%A in (tests\test_tensor.cpp)  do set "TST_T=%%~tA"
if "%EXE_T%" LSS "%SRC_T%" set "STALE=1"
if "%EXE_T%" LSS "%TST_T%" set "STALE=1"
if defined STALE (
    echo WARNING: build\test_tensor.exe is OLDER than the source files.
    echo          You are about to test a STALE binary - run "build.bat" first.
    echo.
)

build\test_tensor.exe
if %ERRORLEVEL% neq 0 goto :error

echo.
echo Test OK.
goto :done

:clean
if exist build rmdir /s /q build
echo Removed build\
goto :done

:error
echo.
echo Build failed. (exit code %ERRORLEVEL%)
popd
endlocal
if "%~1"=="" pause
exit /b 1

:done
popd
endlocal
if "%~1"=="" pause
exit /b 0
