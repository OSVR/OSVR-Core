foreach(testname
    Apply
    BasicListStructure
    BooleanAnd
    BooleanOr
    Contains
    Fold
    IfExpressions
    Push
    SplitList)
    add_executable(TypePack-${testname} ${testname}.cpp)
    target_link_libraries(TypePack-${testname} osvrTypePack)
    osvr_setup_gtest(TypePack-${testname})
endforeach()
