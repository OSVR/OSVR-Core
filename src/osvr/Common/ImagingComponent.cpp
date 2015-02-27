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
#include <osvr/Common/ImagingComponent.h>
#include <osvr/Common/BaseDevice.h>
#include <osvr/Common/Serialization.h>
#include <osvr/Common/Buffer.h>
#include <osvr/Util/OpenCVTypeDispatch.h>

#include <osvr/Util/Verbosity.h>

// Library/third-party includes
#include <opencv2/core/core.hpp>

// Standard includes
// - none

namespace osvr {
namespace common {
    namespace messages {
        class ImageRegion::MessageSerialization {
          public:
            MessageSerialization(OSVR_ImagingMetadata const &meta,
                                 OSVR_ImageBufferElement *imageData,
                                 OSVR_ChannelCount sensor)
                : m_meta(meta),
                  m_imgBuf(imageData,
                           [](OSVR_ImageBufferElement *) {
                           }), // That's a null-deleter right there for you.
                  m_sensor(sensor) {}

            MessageSerialization() : m_imgBuf(nullptr) {}

            template <typename T>
            void allocateBuffer(T &, size_t bytes, std::true_type const &) {
                m_imgBuf.reset(reinterpret_cast<OSVR_ImageBufferElement *>(
                                   cv::fastMalloc(bytes)),
                               &cv::fastFree);
            }

            template <typename T>
            void allocateBuffer(T &, size_t, std::false_type const &) {
                // Does nothing if we're serializing.
            }

            template <typename T> void processMessage(T &p) {
                p(m_meta.height);
                p(m_meta.width);
                p(m_meta.channels);
                p(m_meta.depth);
                p(m_meta.type,
                  serialization::EnumAsIntegerTag<OSVR_ImagingValueType,
                                                  uint8_t>());

                auto bytes = m_meta.height * m_meta.width * m_meta.depth *
                             m_meta.channels;

                /// Allocate the matrix backing data, if we're deserializing
                /// only.
                allocateBuffer(p, bytes, p.isDeserialize());
                p(m_imgBuf.get(),
                  serialization::AlignedDataBufferTag(bytes, m_meta.depth));
            }
            ImageData getData() const {
                ImageData ret;
                ret.sensor = m_sensor;
                ret.metadata = m_meta;
                ret.buffer = m_imgBuf;
                return ret;
            }

          private:
            OSVR_ImagingMetadata m_meta;
            ImageBufferPtr m_imgBuf;
            OSVR_ChannelCount m_sensor;
        };
        const char *ImageRegion::identifier() {
            return "com.osvr.imaging.imageregion";
        }
    } // namespace messages

    shared_ptr<ImagingComponent>
    ImagingComponent::create(OSVR_ChannelCount numChan) {
        shared_ptr<ImagingComponent> ret(new ImagingComponent(numChan));
        return ret;
    }
    ImagingComponent::ImagingComponent(OSVR_ChannelCount numChan)
        : m_numSensor(numChan) {}

    void ImagingComponent::sendImageData(OSVR_ImagingMetadata metadata,
                                         OSVR_ImageBufferElement *imageData,
                                         OSVR_ChannelCount sensor,
                                         OSVR_TimeValue const &timestamp) {
        m_checkFirst(metadata);
        Buffer<> buf;
        messages::ImageRegion::MessageSerialization msg(metadata, imageData,
                                                        sensor);
        serialize(buf, msg);
        m_getParent().packMessage(buf, imageRegion.getMessageType(), timestamp);
        m_getParent().sendPending();
    }

    int VRPN_CALLBACK
    ImagingComponent::m_handleImageRegion(void *userdata, vrpn_HANDLERPARAM p) {
        auto self = static_cast<ImagingComponent *>(userdata);
        auto bufwrap = ExternalBufferReadingWrapper<unsigned char>(
            reinterpret_cast<unsigned char const *>(p.buffer), p.payload_len);
        auto bufReader = BufferReader<decltype(bufwrap)>(bufwrap);

        messages::ImageRegion::MessageSerialization msg;
        deserialize(bufReader, msg);
        auto data = msg.getData();
        auto timestamp = util::time::fromStructTimeval(p.msg_time);

        self->m_checkFirst(data.metadata);
        for (auto const &cb : self->m_cb) {
            cb(data, timestamp);
        }
        return 0;
    }

    void ImagingComponent::registerImageHandler(ImageHandler handler) {
        if (m_cb.empty()) {
            m_registerHandler(&ImagingComponent::m_handleImageRegion, this,
                              imageRegion.getMessageType());
        }
        m_cb.push_back(handler);
    }
    void ImagingComponent::m_parentSet() {
        m_getParent().registerMessageType(imageRegion);
    }

    void ImagingComponent::m_checkFirst(OSVR_ImagingMetadata const &metadata) {
        if (m_gotOne) {
            return;
        }
        m_gotOne = true;

        OSVR_DEV_VERBOSE("First frame sent: width="
                         << metadata.width << " height=" << metadata.height);
    }
} // namespace common
} // namespace osvr