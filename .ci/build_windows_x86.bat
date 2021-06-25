@echo off
set DIR=%~dp0

set ROOT_DIR=%DIR%..\
set BUILD_DIR=%ROOT_DIR%build\windows_release_x86\

if not exist %BUILD_DIR% mkdir %BUILD_DIR% || goto :error

pushd %BUILD_DIR% || goto :error
cmake %ROOT_DIR% -A Win32 || goto :error
cmake --build . --config Release || goto :error
ctest --verbose || goto :error
popd || goto :error

goto :EOF

:error
echo Failed with error #%errorlevel%.
exit /b %errorlevel%
