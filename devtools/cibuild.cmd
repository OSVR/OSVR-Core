cd /d "%~dp0"
cd ..
rmdir /s /q install
rmdir /s /q build
mkdir build
cd build
cmake .. -G "Visual Studio 12" "-DCMAKE_INSTALL_PREFIX=%WORKSPACE%\install"
cmake --build . --config Debug
cmake --build . --config RelWithDebInfo

call "%WORKSPACE%\vendor\jenkins-ctest-plugin\run-test-and-save.bat" -C Debug
call "%WORKSPACE%\vendor\jenkins-ctest-plugin\run-test-and-save.bat" -C RelWithDebInfo

cmake --build . --config Debug --target INSTALL
cmake --build . --config RelWithDebInfo --target INSTALL

cmake --build . --config RelWithDebInfo --target ogvrPluginKit -- /p:RunCodeAnalysis=True
