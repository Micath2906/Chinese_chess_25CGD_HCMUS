@echo off
cd /d "%~dp0"
if exist "CoTuong.exe" (
    start "" "CoTuong.exe"
) else if exist "build-vs\Release\CoTuong.exe" (
    start "" "build-vs\Release\CoTuong.exe"
) else if exist "build-vs\Debug\CoTuong.exe" (
    start "" "build-vs\Debug\CoTuong.exe"
) else (
    echo Khong tim thay CoTuong.exe!
    pause
)
