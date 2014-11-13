
foreach(name sample-tracker-reader)
    add_library(${name} MODULE ${name}.cpp)
    target_link_libraries(${name} ogvrPluginKitCpp)
    set_target_properties(${name} PROPERTIES
        PREFIX ""
        FOLDER "OGVR Example Clients")
endforeach()