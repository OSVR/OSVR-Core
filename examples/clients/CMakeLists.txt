set(CLIENTS
    MinimalInit
    #sample-tracker-reader
)
foreach(name ${CLIENTS})
    add_library(${name} MODULE ${name}.cpp)
    target_link_libraries(${name} ogvrClient ogvrUtilCpp)
    set_target_properties(${name} PROPERTIES
        PREFIX ""
        FOLDER "OGVR Example Clients")
endforeach()