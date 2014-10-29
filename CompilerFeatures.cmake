# TODO make these functional tests
# shared_ptr/unique_ptr
# perfect forwarding
# lambdas

include(CheckCXXCompilerFlag)
if(NOT MSVC)
    check_cxx_compiler_flag("-std=c++11" COMPILER_SUPPORTS_CXX11)
    check_cxx_compiler_flag("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
endif()

if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_REQUIRED_FLAGS "-std=c++11")
elif(COMPILER_SUPPORTS_CXX0X)
    set(CMAKE_REQUIRED_FLAGS "-std=c++0x")
endif()


include(CheckCXXSourceCompiles)
function(check_cxx_type_exists TYPE HEADER VAR)
    check_cxx_source_compiles("#include <${HEADER}>\n int main() { ${TYPE} test; return 0;}" ${VAR})
    set(${VAR} ${${VAR}} PARENT_SCOPE)
endfunction()

check_cxx_type_exists("std::shared_ptr<int>" "memory" OGVR_HAVE_STD_SHARED_PTR)
check_cxx_type_exists("std::unique_ptr<int>" "memory" OGVR_HAVE_STD_UNIQUE_PTR)

#include(CheckCXXSymbolExists)
#check_cxx_symbol_exists("std::shared_ptr<int>" "memory" OGVR_HAVE_STD_SHARED_PTR)
#check_cxx_symbol_exists("std::unique_ptr<int>" "memory" OGVR_HAVE_STD_UNIQUE_PTR)

if(OGVR_HAVE_STD_SHARED_PTR AND OGVR_HAVE_STD_UNIQUE_PTR)
    set(OGVR_CXX11_FLAGS ${CMAKE_REQUIRED_FLAGS})
else()
    message(FATAL_ERROR "Sorry, your compiler does not support the C++11 features required to compile the library core.")
endif()

set(CMAKE_REQUIRED_FLAGS)
