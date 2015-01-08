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
#include <osvr/VRPN/MessageHandler.h>

// Library/third-party includes
#include <boost/optional.hpp>

// Standard includes
#include <vector>

namespace osvr {
namespace vrpn {

    class CallbackRecord : boost::noncopyable {
      public:
        CallbackRecord(vrpn_ConnectionPtr &conn,
                       MessageHandlerCallback callback,
                       std::string const &device = std::string(),
                       std::string const &messageType = std::string())
            : m_cb(callback), m_vrpnConnection(conn) {
            if (!device.empty()) {
                m_sender = m_vrpnConnection->register_sender(device.c_str());
            }
            if (!messageType.empty()) {
                m_msgType =
                    m_vrpnConnection->register_message_type(device.c_str());
            }
            m_vrpnConnection->register_handler(
                m_getMsgType(), &CallbackRecord::s_messageHandler, this,
                m_getSender());
        }
        ~CallbackRecord() {
            m_vrpnConnection->unregister_handler(
                m_getMsgType(), &CallbackRecord::s_messageHandler, this,
                m_getSender());
        }

      private:
        vrpn_int32 m_getMsgType() const {
            return m_msgType.get_value_or(vrpn_ANY_SENDER);
        }
        vrpn_int32 m_getSender() const {
            return m_sender.get_value_or(vrpn_ANY_SENDER);
        }
        static int VRPN_CALLBACK
        s_messageHandler(void *userdata, vrpn_HANDLERPARAM param) {
            CallbackRecord *record = static_cast<CallbackRecord *>(userdata);
            std::string msgType(
                record->m_vrpnConnection->message_type_name(param.type));
            std::string sender(
                record->m_vrpnConnection->sender_name(param.sender));
            OSVR_TimeValue timestamp;
            osvrStructTimevalToTimeValue(&timestamp, &(param.msg_time));
            std::string msg(param.buffer, param.payload_len);
            record->m_cb(sender, msgType, timestamp, msg);
            return 0;
        }

        boost::optional<vrpn_int32> m_msgType;
        boost::optional<vrpn_int32> m_sender;
        MessageHandlerCallback m_cb;
        vrpn_ConnectionPtr m_vrpnConnection;
    };

    MessageHandler::MessageHandler(vrpn_ConnectionPtr conn)
        : m_vrpnConnection(conn) {}
    MessageHandler::~MessageHandler() {}

    void MessageHandler::registerCallback(MessageHandlerCallback const &cb,
                                          std::string const &device,
                                          std::string const &messageType) {
        m_callbackRecords.emplace_back(
            new CallbackRecord(m_vrpnConnection, cb, device, messageType));
    }
} // namespace vrpn
} // namespace osvr