cd /d "%~dp0"
cd ..
mkdir build
cd build
cmake .. -G "Visual Studio 12" "-DCMAKE_INSTALL_PREFIX=%WORKSPACE%\install"
cd ..
cmake --build build --config RelWithDebInfo -- /p:RunCodeAnalysis=True
