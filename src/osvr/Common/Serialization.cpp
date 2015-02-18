/** @file
    @brief Implementation

    @date 2015

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2015 Sensics, Inc.
//
// All rights reserved.
//
// (Final version intended to be licensed under
// the Apache License, Version 2.0)

// Internal Includes
#include <osvr/Common/Serialization.h>

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace common {
    class MyClass : public MessageSerializationBase<MyClass> {
      public:
        MyClass() : a(1), b(2), c(3) {}
        template <typename T> void processMessage(T &process) {
            /*
            process<int32_t>(a);
            process<uint32_t>(b);
            process<int8_t>(c);
            */
        }
        int32_t a;
        uint32_t b;
        int8_t c;
    };

    void trySerializing() {
        BufferWrapper<> buf;
        MyClass data;
        data.serialize(buf);
    }
} // namespace common
} // namespace osvr