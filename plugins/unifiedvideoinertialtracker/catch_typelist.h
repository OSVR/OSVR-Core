/** @file
    @brief Header

    @date 2016

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

/*
 *  Copyright 2016 Sensics, Inc. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef INCLUDED_catch_typelist_h_GUID_B8BF6209_253A_478E_0B05_37796B95C0DE
#define INCLUDED_catch_typelist_h_GUID_B8BF6209_253A_478E_0B05_37796B95C0DE

#include "catch.hpp"

namespace Catch {
/// Can specialize to get better names.
template <typename T> struct TypelistTypeNameTrait {
    static const char *get() { return typeid(T).name(); }
};

template <typename C, typename T>
class TypeParameterizedTestCase : public Catch::SharedImpl<Catch::ITestCase> {

  public:
    TypeParameterizedTestCase() {}

    virtual void invoke() const {
        C obj;
        obj.template test<T>();
    }

  private:
    virtual ~TypeParameterizedTestCase() {}
};

template <typename C, typename T>
inline void registerTypeParameterizedTestCase(SourceLineInfo const &lineInfo,
                                              NameAndDesc const &nameAndDesc) {
    getMutableRegistryHub().registerTest(
        makeTestCase(new TypeParameterizedTestCase<C, T>, "",
                     std::string(nameAndDesc.name) + " <" +
                         TypelistTypeNameTrait<T>::get() + ">",
                     nameAndDesc.description, lineInfo));
}
template <typename C, typename... List> struct TypelistTestCaseAutoReg;
/// base case
template <typename C> struct TypelistTestCaseAutoReg<C> {

    TypelistTestCaseAutoReg(SourceLineInfo const & /*lineInfo*/,
                            NameAndDesc const & /*nameAndDesc*/) {}
};
/// recursive case
template <typename C, typename Head, typename... List>
struct TypelistTestCaseAutoReg<C, Head, List...>
    : TypelistTestCaseAutoReg<C, List...> {
    using Base = TypelistTestCaseAutoReg<C, List...>;
    TypelistTestCaseAutoReg(SourceLineInfo const &lineInfo,
                            NameAndDesc const &nameAndDesc)
        : TypelistTestCaseAutoReg<C, List...>(lineInfo, nameAndDesc) {
        registerTypeParameterizedTestCase<C, Head>(lineInfo, nameAndDesc);
    }
};
} // namespace Catch

#define INTERNAL_CATCH_TYPELIST_TESTCASE2(TestName, Name, Desc, ...)           \
    namespace {                                                                \
        struct TestName {                                                      \
            template <typename TypeParam> void test();                         \
        };                                                                     \
        Catch::TypelistTestCaseAutoReg<TestName, __VA_ARGS__>                  \
            INTERNAL_CATCH_UNIQUE_NAME(autoRegistrar)(                         \
                CATCH_INTERNAL_LINEINFO, Catch::NameAndDesc(Name, Desc));      \
    }                                                                          \
    template <typename TypeParam> void TestName::test()
#define INTERNAL_CATCH_TYPELIST_TESTCASE(Name, Desc, ...)                      \
    INTERNAL_CATCH_TYPELIST_TESTCASE2(                                         \
        INTERNAL_CATCH_UNIQUE_NAME(____C_A_T_C_H____T_E_S_T____), Name, Desc,  \
        __VA_ARGS__)

#define CATCH_TYPELIST_TESTCASE(Name, ...)                                     \
    INTERNAL_CATCH_TYPELIST_TESTCASE(Name, "", __VA_ARGS__)
#define CATCH_TYPELIST_DESCRIBED_TESTCASE(Name, Desc, ...)                     \
    INTERNAL_CATCH_TYPELIST_TESTCASE(Name, Desc, __VA_ARGS__)
#endif // INCLUDED_catch_typelist_h_GUID_B8BF6209_253A_478E_0B05_37796B95C0DE
