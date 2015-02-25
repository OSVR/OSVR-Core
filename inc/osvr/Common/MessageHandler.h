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

#ifndef INCLUDED_MessageHandler_h_GUID_A7A258A7_A369_46BA_C44F_6FFE56DB464A
#define INCLUDED_MessageHandler_h_GUID_A7A258A7_A369_46BA_C44F_6FFE56DB464A

// Internal Includes
#include <osvr/Common/RawMessageType.h>
#include <osvr/Common/RawSenderType.h>
#include <osvr/Util/UniquePtr.h>

// Library/third-party includes
#include <boost/optional.hpp>

// Standard includes
#include <vector>

namespace osvr {
namespace common {
    /// @brief RAII class template managing a message handler callback.
    ///
    /// @tparam MessageTraits Policy class, providing a `handler_type` typedef
    /// (function pointer type of this hander) and a `registration_type` type
    /// (holder class, passed in and stored by value on registration. Must
    /// contain `registerHandler` and `unregisterHandler` methods taking the
    /// arguments (HandlerType, void*, RawSenderType, RawMessageType) )
    template <typename MessageTraits>
    class MessageHandler : boost::noncopyable {
      public:
        typedef typename MessageTraits::handler_type handler_type;
        typedef typename MessageTraits::registration_type registration_type;

        /// @brief Constructor taking your handler's basic info, and optionally
        /// a sender, and optionally a message type
        MessageHandler(handler_type handler, void *userdata = nullptr,
                       RawSenderType sender = RawSenderType(),
                       RawMessageType msgType = RawMessageType())
            : m_handler(handler), m_data(userdata), m_sender(sender),
              m_msgType(msgType) {}

        /// @brief Constructor taking your handler's basic info and a message
        /// type
        ///
        /// Provided as an overload to take care of the case when only a message
        /// type is passed.
        MessageHandler(handler_type handler, void *userdata,
                       RawMessageType msgType)
            : m_handler(handler), m_data(userdata), m_sender(sender),
              m_msgType(msgType) {}

        /// @brief Method if you want to manually unregister your handler.
        ///
        /// Automatically done when object is destroyed. No effect if called
        /// before registerHandler()
        void unregisterHandler() {
            if (m_reg.is_initialized()) {
                m_reg->unregisterHandler(m_handler, m_data, m_sender,
                                         m_msgType);
                m_reg.reset();
            }
        }

        /// @brief Register handler on the given registration policy value.
        void registerHandler(registration_type reg) {
            unregisterHandler();
            m_reg = reg;
            m_reg->registerHandler(m_handler, m_data, m_sender, m_msgType);
        }

        /// @brief destructor: automatically unregisters handler.
        ~MessageHandler() { unregisterHandler(); }

      private:
        handler_type m_handler;
        void *m_data;
        RawSenderType m_sender;
        RawMessageType m_msgType;
        boost::optional<registration_type> m_reg;
    };

    template <typename MessageTraits>
    using MessageHandlerPtr = unique_ptr<MessageHandler<MessageTraits> >;

    template <typename MessageTraits>
    using MessageHandlerList = std::vector<MessageHandlerPtr<MessageTraits> >;
} // namespace common
} // namespace osvr
#endif // INCLUDED_MessageHandler_h_GUID_A7A258A7_A369_46BA_C44F_6FFE56DB464A
