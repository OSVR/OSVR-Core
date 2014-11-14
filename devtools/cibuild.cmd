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

call "%WORKSPACE%\vendor\jenkins-ctest-plugin\run-test-and-save.bat" -C Debug
call "%WORKSPACE%\vendor\jenkins-ctest-plugin\run-test-and-save.bat" -C RelWithDebInfo

cmake --build . --config Debug --target INSTALL
cmake --build . --config RelWithDebInfo --target INSTALL

cmake --build . --config RelWithDebInfo --target ogvrPluginKit -- /p:RunCodeAnalysis=True

cmake --build . --config RelWithDebInfo --target PACKAGE

rem bin\Debug\LoadTest.exe --gtest_output=xml:test_details.Debug.xml
rem bin\RelWithDebInfo\LoadTest.exe --gtest_output=xml:test_details.RelWithDebInfo.xml

pause