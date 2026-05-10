@echo off
set /p task="Which task do you want to run (101, 102, 103, or snake)? "

:: The paths to your SDL3 folders (Check if these match your actual folder names)
set SDL_INCLUDE="C:\Users\HP\Downloads\SDL3-devel-3.4.2-mingw\SDL3-3.4.2\i686-w64-mingw32\include"
set SDL_LIB="C:\Users\HP\Downloads\SDL3-devel-3.4.2-mingw\SDL3-3.4.2\i686-w64-mingw32\lib"
set SDL_BIN="C:\Users\HP\Downloads\SDL3-devel-3.4.2-mingw\SDL3-3.4.2\i686-w64-mingw32\bin"

if "%task%"=="101" (
    echo Compiling Task 101...
    gcc Task101.c -o Task101.exe -I%SDL_INCLUDE% -L%SDL_LIB% -lSDL3
    if %errorlevel% equ 0 (
        copy %SDL_BIN%\SDL3.dll . >nul
        Task101.exe
    )
) else if "%task%"=="102" (
    echo Compiling Task 102...
    gcc Task102.c -o Task102.exe -I%SDL_INCLUDE% -L%SDL_LIB% -lSDL3
    if %errorlevel% equ 0 (
        copy %SDL_BIN%\SDL3.dll . >nul
        Task102.exe
    )
) else if "%task%"=="103" (
    echo Compiling Task 103...
    gcc Task103.c -o Task103.exe -I%SDL_INCLUDE% -L%SDL_LIB% -lSDL3
    if %errorlevel% equ 0 (
        copy %SDL_BIN%\SDL3.dll . >nul
        Task103.exe
    )
) else if "%task%"=="snake" (
    echo Compiling Snake Game...
    gcc snake.c -o snake.exe -I%SDL_INCLUDE% -L%SDL_LIB% -lSDL3
    if %errorlevel% equ 0 (
        copy %SDL_BIN%\SDL3.dll . >nul
        snake.exe
    )
) else (
    echo Task %task% not found!
)

pause