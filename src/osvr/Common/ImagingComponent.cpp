/** @file
    @brief Implementation

    @date 2015

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2015 Sensics, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// Internal Includes
#include <osvr/Common/ImagingComponent.h>
#include <osvr/Common/BaseDevice.h>
#include <osvr/Common/Serialization.h>
#include <osvr/Common/Buffer.h>
#include <osvr/Util/OpenCVTypeDispatch.h>
#include <osvr/Util/Flag.h>
#include <osvr/Util/Verbosity.h>

// Library/third-party includes
#include <opencv2/core/core.hpp>

// Standard includes
#include <sstream>
#include <utility>

namespace osvr {
namespace common {
    static inline uint32_t getBufferSize(OSVR_ImagingMetadata const &meta) {
        return meta.height * meta.width * meta.depth * meta.channels;
    }
    namespace messages {
        namespace {
            template <typename T>
            void process(OSVR_ImagingMetadata &meta, T &p) {
                p(meta.height);
                p(meta.width);
                p(meta.channels);
                p(meta.depth);
                p(meta.type,
                  serialization::EnumAsIntegerTag<OSVR_ImagingValueType,
                                                  uint8_t>());
            }
        } // namespace
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
                process(m_meta, p);
                auto bytes = getBufferSize(m_meta);

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

#ifdef OSVR_COMMON_IN_PROCESS_IMAGING
        namespace {
            struct InProcessMemoryMessage {
                OSVR_ImagingMetadata metadata;
                OSVR_ChannelCount sensor;
                int buffer;
            };
            template <typename T>
            void process(InProcessMemoryMessage &ipmmMsg, T &p) {
                process(ipmmMsg.metadata, p);
                p(ipmmMsg.sensor);
                p(ipmmMsg.buffer);
            }
        } // namespace

        const char *ImagePlacedInProcessMemory::identifier() {
            return "com.osvr.imaging.imageplacedinprocessmemory";
        }

        class ImagePlacedInProcessMemory::MessageSerialization {
        public:
            MessageSerialization() {}
            explicit MessageSerialization(InProcessMemoryMessage &&msg)
                : m_msgData(std::move(msg)) {}

#if defined(_MSC_VER) && defined(_PREFAST_)
            // @todo workaround for apparent bug in VS2013 /analyze
            explicit MessageSerialization(InProcessMemoryMessage const &msg)
                : m_msgData(msg) { }
#endif
            template <typename T> void processMessage(T &p) {
                process(m_msgData, p);
            }

            InProcessMemoryMessage const &getMessage() { return m_msgData; }

        private:
            InProcessMemoryMessage m_msgData;
        };
#endif

        namespace {
            struct SharedMemoryMessage {
                OSVR_ImagingMetadata metadata;
                IPCRingBuffer::sequence_type seqNum;
                OSVR_ChannelCount sensor;
                IPCRingBuffer::abi_level_type abiLevel;
                IPCRingBuffer::BackendType backend;
                std::string shmName;
            };
            template <typename T>
            void process(SharedMemoryMessage &shmMsg, T &p) {
                process(shmMsg.metadata, p);
                p(shmMsg.seqNum);
                p(shmMsg.sensor);
                p(shmMsg.abiLevel);
                p(shmMsg.backend);
                p(shmMsg.shmName);
            }

        } // namespace
        class ImagePlacedInSharedMemory::MessageSerialization {
          public:
            MessageSerialization() {}
            explicit MessageSerialization(SharedMemoryMessage &&msg)
                : m_msgData(std::move(msg)) {}

#if defined(_MSC_VER) && defined(_PREFAST_)
            /// @todo workaround for apparent bug in VS2013 /analyze
            explicit MessageSerialization(SharedMemoryMessage const &msg)
                : m_msgData(msg) {}
#endif

            template <typename T> void processMessage(T &p) {
                process(m_msgData, p);
            }

            SharedMemoryMessage const &getMessage() { return m_msgData; }

          private:
            SharedMemoryMessage m_msgData;
        };

        const char *ImagePlacedInSharedMemory::identifier() {
            return "com.osvr.imaging.imageplacedinsharedmemory";
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

        util::Flag dataSent;

#ifdef OSVR_COMMON_IN_PROCESS_IMAGING
        dataSent +=
            m_sendImageDataViaInProcessMemory(metadata, imageData, sensor, timestamp);
#else
        dataSent += m_sendImageDataViaSharedMemory(metadata, imageData, sensor,
                                                   timestamp);
#endif
        dataSent +=
            m_sendImageDataOnTheWire(metadata, imageData, sensor, timestamp);
        if (dataSent) {
            m_checkFirst(metadata);
        }
    }

#ifdef OSVR_COMMON_IN_PROCESS_IMAGING
    bool ImagingComponent::m_sendImageDataViaInProcessMemory(
        OSVR_ImagingMetadata metadata, OSVR_ImageBufferElement *imageData,
        OSVR_ChannelCount sensor, OSVR_TimeValue const &timestamp) {

        auto imageBufferSize = getBufferSize(metadata);
        auto imageBufferCopy = reinterpret_cast<OSVR_ImageBufferElement*>(cv::fastMalloc(imageBufferSize));
        memcpy(imageBufferCopy, imageData, imageBufferSize);

        Buffer<> buf;
        messages::ImagePlacedInProcessMemory::MessageSerialization serialization(
            messages::InProcessMemoryMessage{ metadata, sensor, reinterpret_cast<int>(imageBufferCopy) });

        serialize(buf, serialization);
        m_getParent().packMessage(
            buf, imagePlacedInProcessMemory.getMessageType(), timestamp);

        return true;
    }
#endif

    bool ImagingComponent::m_sendImageDataViaSharedMemory(
        OSVR_ImagingMetadata metadata, OSVR_ImageBufferElement *imageData,
        OSVR_ChannelCount sensor, OSVR_TimeValue const &timestamp) {

        m_growShmVecIfRequired(sensor);
        uint32_t imageBufferSize = getBufferSize(metadata);
        if (!m_shmBuf[sensor] ||
            m_shmBuf[sensor]->getEntrySize() != imageBufferSize) {
            // create or replace the shared memory ring buffer.
            auto makeName =
                [](OSVR_ChannelCount sensor, std::string const &devName) {
                    std::ostringstream os;
                    os << "com.osvr.imaging/" << devName << "/" << int(sensor);
                    return os.str();
                };
            m_shmBuf[sensor] = IPCRingBuffer::create(
                IPCRingBuffer::Options(
                    makeName(sensor, m_getParent().getDeviceName()))
                    .setEntrySize(imageBufferSize));
        }
        if (!m_shmBuf[sensor]) {
            OSVR_DEV_VERBOSE(
                "Some issue creating shared memory for imaging, skipping out.");
            return false;
        }
        auto &shm = *(m_shmBuf[sensor]);
        auto seq = shm.put(imageData, imageBufferSize);

        Buffer<> buf;
        messages::ImagePlacedInSharedMemory::MessageSerialization serialization(
            messages::SharedMemoryMessage{metadata, seq, sensor,
                                          IPCRingBuffer::getABILevel(),
                                          shm.getBackend(), shm.getName()});
        serialize(buf, serialization);
        m_getParent().packMessage(
            buf, imagePlacedInSharedMemory.getMessageType(), timestamp);

        return true;
    }

    bool ImagingComponent::m_sendImageDataOnTheWire(
        OSVR_ImagingMetadata metadata, OSVR_ImageBufferElement *imageData,
        OSVR_ChannelCount sensor, OSVR_TimeValue const &timestamp) {
        /// @todo currently only handle 8bit data over network
        if (metadata.depth != 1) {
            return false;
        }
        Buffer<> buf;
        messages::ImageRegion::MessageSerialization msg(metadata, imageData,
                                                        sensor);
        serialize(buf, msg);
        if (buf.size() > vrpn_CONNECTION_TCP_BUFLEN) {
#if 0
            OSVR_DEV_VERBOSE("Skipping imaging message: size is "
                             << buf.size() << " vs the maximum of "
                             << vrpn_CONNECTION_TCP_BUFLEN);
#endif
            return false;
        }
        m_getParent().packMessage(buf, imageRegion.getMessageType(), timestamp);
        m_getParent().sendPending();
        return true;
    }

    int VRPN_CALLBACK
    ImagingComponent::m_handleImageRegion(void *userdata, vrpn_HANDLERPARAM p) {
        auto self = static_cast<ImagingComponent *>(userdata);
        auto bufReader = readExternalBuffer(p.buffer, p.payload_len);

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

#ifdef OSVR_COMMON_IN_PROCESS_IMAGING
    int VRPN_CALLBACK
    ImagingComponent::m_handleImagePlacedInProcessMemory(void *userdata,
                                                         vrpn_HANDLERPARAM p) {
        auto self = static_cast<ImagingComponent *>(userdata);
        auto bufReader = readExternalBuffer(p.buffer, p.payload_len);

        messages::ImagePlacedInProcessMemory::MessageSerialization msgSerialize;
        deserialize(bufReader, msgSerialize);
        auto msg = msgSerialize.getMessage();
        ImageData data;
        data.sensor = msg.sensor;
        data.metadata = msg.metadata;
        data.buffer.reset(reinterpret_cast<OSVR_ImageBufferElement*>(msg.buffer), &cv::fastFree);
        auto timestamp = util::time::fromStructTimeval(p.msg_time);

        self->m_checkFirst(msg.metadata);
        for (auto const &cb : self->m_cb) {
            cb(data, timestamp);
        }
        return 0;
    }
#endif

    int VRPN_CALLBACK
    ImagingComponent::m_handleImagePlacedInSharedMemory(void *userdata,
                                                        vrpn_HANDLERPARAM p) {
        auto self = static_cast<ImagingComponent *>(userdata);
        auto bufReader = readExternalBuffer(p.buffer, p.payload_len);

        messages::ImagePlacedInSharedMemory::MessageSerialization msgSerialize;
        deserialize(bufReader, msgSerialize);
        auto &msg = msgSerialize.getMessage();
        auto timestamp = util::time::fromStructTimeval(p.msg_time);

        if (IPCRingBuffer::getABILevel() != msg.abiLevel) {
            /// Can't interoperate with this server over shared memory
            OSVR_DEV_VERBOSE("Can't handle SHM ABI level " << msg.abiLevel);
            return 0;
        }
        self->m_growShmVecIfRequired(msg.sensor);
        auto checkSameRingBuf = [](messages::SharedMemoryMessage const &msg,
                                   IPCRingBufferPtr &ringbuf) {
            return (msg.backend == ringbuf->getBackend()) &&
                   (ringbuf->getEntrySize() == getBufferSize(msg.metadata)) &&
                   (ringbuf->getName() == msg.shmName);
        };
        if (!self->m_shmBuf[msg.sensor] ||
            !checkSameRingBuf(msg, self->m_shmBuf[msg.sensor])) {
            self->m_shmBuf[msg.sensor] = IPCRingBuffer::find(
                IPCRingBuffer::Options(msg.shmName, msg.backend));
        }
        if (!self->m_shmBuf[msg.sensor]) {
            /// Can't find the shared memory referred to - possibly not a local
            /// client
            OSVR_DEV_VERBOSE("Can't find desired IPC ring buffer "
                             << msg.shmName);
            return 0;
        }

        auto &shm = self->m_shmBuf[msg.sensor];
        auto getResult = shm->get(msg.seqNum);
        if (getResult) {
            auto bufptr = getResult.getBufferSmartPointer();
            self->m_checkFirst(msg.metadata);
            auto data = ImageData{msg.sensor, msg.metadata, bufptr};

            for (auto const &cb : self->m_cb) {
                cb(data, timestamp);
            }
        }
        return 0;
    }

    void ImagingComponent::registerImageHandler(ImageHandler handler) {
        if (m_cb.empty()) {
            m_registerHandler(&ImagingComponent::m_handleImageRegion, this,
                              imageRegion.getMessageType());

            m_registerHandler(
                &ImagingComponent::m_handleImagePlacedInSharedMemory, this,
                imagePlacedInSharedMemory.getMessageType());

#ifdef OSVR_COMMON_IN_PROCESS_IMAGING
            m_registerHandler(
                &ImagingComponent::m_handleImagePlacedInProcessMemory, this,
                imagePlacedInProcessMemory.getMessageType());
#endif
        }
        m_cb.push_back(handler);
    }
    void ImagingComponent::m_parentSet() {
        m_getParent().registerMessageType(imageRegion);
        m_getParent().registerMessageType(imagePlacedInSharedMemory);
#ifdef OSVR_COMMON_IN_PROCESS_IMAGING
        m_getParent().registerMessageType(imagePlacedInProcessMemory);
#endif
    }

    void ImagingComponent::m_checkFirst(OSVR_ImagingMetadata const &metadata) {
        if (m_gotOne) {
            return;
        }
        m_gotOne = true;

        OSVR_DEV_VERBOSE("Sending/receiving first frame: width="
                         << metadata.width << " height=" << metadata.height);
    }
    void ImagingComponent::m_growShmVecIfRequired(OSVR_ChannelCount sensor) {
        if (m_shmBuf.size() <= sensor) {
            m_shmBuf.resize(sensor + 1);
        }
    }
} // namespace common
} // namespace osvr
