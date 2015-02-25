/** @file
    @brief Header

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

#ifndef INCLUDED_SerializationTags_h_GUID_2697261C_2AFA_4982_E73F_9817EBF96628
#define INCLUDED_SerializationTags_h_GUID_2697261C_2AFA_4982_E73F_9817EBF96628

// Internal Includes
// - none

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace common {

    namespace serialization {

        /// @brief The default "type tag" for specifying serialization behavior.
        ///
        /// Usage of tag types and matching type traits allows us to serialize
        /// the same underlying type different ways when requested. If no type
        /// tag is explicitly passed, default behavior based on the type will be
        /// invoked.
        template <typename T> struct DefaultSerializationTag {
            typedef T type;
        };

        /// @brief A tag for use for raw data (bytestream), optionally aligned
        ///
        /// Here, the tag is also used to transport size information to
        /// serialization/deserialization
        template <size_t Alignment = 1> struct AlignedDataBufferTag {
          public:
            AlignedDataBufferTag(size_t length) : m_length(length) {}
            size_t length() const { return m_length; }

          private:
            size_t m_length;
        };

        /// @brief A tag for use when the only field in a message is a string so
        /// the length prefix is unnecessary
        struct StringOnlyMessageTag {};

    } // namespace serialization

} // namespace common
} // namespace osvr

#endif // INCLUDED_SerializationTags_h_GUID_2697261C_2AFA_4982_E73F_9817EBF96628
