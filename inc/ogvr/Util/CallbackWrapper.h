/** @file
    @brief Header

    @date 2014

    @author
    Ryan Pavlik
    <ryan@sensics.com>;
    <http://sensics.com>
*/

// Copyright 2014 Sensics, Inc.
//
// All rights reserved.
//
// (Final version intended to be licensed under
// the Apache License, Version 2.0)

#ifndef INCLUDED_CallbackWrapper_h_GUID_6169ADE2_5BA1_4A81_47C9_9E492F6405ED
#define INCLUDED_CallbackWrapper_h_GUID_6169ADE2_5BA1_4A81_47C9_9E492F6405ED

// Internal Includes
// - none

// Library/third-party includes
#include <boost/type_traits/function_traits.hpp>
#include <boost/type_traits/remove_pointer.hpp>

// Standard includes
#include <type_traits>
#include <utility>

namespace ogvr {

template <typename FunctionPtrType> class CallbackWrapper {
  public:
    CallbackWrapper(FunctionPtrType f, void *userData)
        : m_f(f), m_ud(userData) {}
    typedef typename boost::remove_pointer<FunctionPtrType>::type FunctionType;
    typedef
        typename boost::function_traits<FunctionType>::result_type ReturnType;

    /// @brief Function call operator with non-void return
    template <typename... Args> ReturnType operator()(Args &&... args) const {
        return (*m_f)(std::forward<Args>(args)..., m_ud);
    }

  private:
    FunctionPtrType m_f;
    void *m_ud;
};

} // end of namespace ogvr
#endif // INCLUDED_CallbackWrapper_h_GUID_6169ADE2_5BA1_4A81_47C9_9E492F6405ED
