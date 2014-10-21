cd /d "%~dp0"
cd ..
rmdir /s /q build
mkdir build
cd build
cmake .. -G "Visual Studio 12"
cmake --build . --config Debug
cmake --build . --config RelWithDebInfo
cmake --build . --config Debug --target doc

rem bin\Debug\LoadTest.exe --gtest_output=xml:test_details.Debug.xml
rem bin\RelWithDebInfo\LoadTest.exe --gtest_output=xml:test_details.RelWithDebInfo.xml

pause