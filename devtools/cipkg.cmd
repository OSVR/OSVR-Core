cd /d "%~dp0"
cd ..
rmdir /s /q install
rmdir /s /q build
mkdir build
cd build
cmake .. -G "Visual Studio 12" "-DCMAKE_INSTALL_PREFIX=%WORKSPACE%\install" -DCPACK_GENERATOR=ZIP %*
cmake --build . --config Debug
cmake --build . --config RelWithDebInfo

rem Build external docs for packaging.
cmake --build . --config RelWithDebInfo --target doc_external

cmake --build . --config RelWithDebInfo --target PACKAGE
