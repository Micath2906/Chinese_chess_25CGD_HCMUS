@echo off
echo ===================================
echo Building Co Tuong - Chinese Chess
echo ===================================

REM Tao thu muc build neu chua co
if not exist build mkdir build

cd build

echo.
echo [1/3] Configuring with CMake...
cmake .. -DCMAKE_BUILD_TYPE=Release

if %errorlevel% neq 0 (
    echo.
    echo Loi: Khong the configure project!
    echo Hay kiem tra SFML da duoc cai dat chua.
    pause
    exit /b 1
)

echo.
echo [2/3] Building project...
cmake --build . --config Release

if %errorlevel% neq 0 (
    echo.
    echo Loi: Khong the build project!
    pause
    exit /b 1
)

echo.
echo [3/3] Build thanh cong!
echo.
echo De chay game, go: cd build ^&^& Release\CoTuong.exe
echo.

pause
