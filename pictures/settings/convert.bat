@echo off
setlocal enabledelayedexpansion

:: Check if FFmpeg is available
ffmpeg -version >nul 2>&1
if errorlevel 1 (
    echo FFmpeg is not installed or not in PATH.
    pause
    exit /b
)

:: Loop through all JPG files in the current folder
for %%f in (*.jpg) do (
    echo Converting "%%f" to BMP format...
    ffmpeg -i "%%f" "%%~nf.bmp"
    if errorlevel 1 (
        echo Failed to convert "%%f". Skipping.
    )
)

echo Conversion complete.
pause
endlocal
