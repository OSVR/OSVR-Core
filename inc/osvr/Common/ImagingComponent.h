/** @file
    @brief Header

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

#ifndef INCLUDED_ImagingComponent_h_GUID_BA26C922_01FD_43B3_8EB7_A9AB2777CEBC
#define INCLUDED_ImagingComponent_h_GUID_BA26C922_01FD_43B3_8EB7_A9AB2777CEBC

// Internal Includes
#include <osvr/Common/Export.h>
#include <osvr/Common/DeviceComponent.h>
#include <osvr/Common/SerializationTags.h>
#include <osvr/Util/ChannelCountC.h>
#include <osvr/Util/ImagingReportTypesC.h>
#include <osvr/Common/IPCRingBuffer.h>
#include <osvr/Common/ImagingComponentConfig.h>

// Library/third-party includes
#include <vrpn_BaseClass.h>

// Standard includes
// - none

namespace osvr {
namespace common {
    typedef shared_ptr<OSVR_ImageBufferElement> ImageBufferPtr;
    struct ImageData {
        OSVR_ChannelCount sensor;
        OSVR_ImagingMetadata metadata;
        ImageBufferPtr buffer;
    };
    namespace messages {
        class ImageRegion : public MessageRegistration<ImageRegion> {
          public:
            class MessageSerialization;

            static const char *identifier();
        };
#ifdef OSVR_COMMON_IN_PROCESS_IMAGING
        class ImagePlacedInProcessMemory
            : public MessageRegistration<ImagePlacedInProcessMemory> {
          public:
            class MessageSerialization;
            static const char *identifier();
        };
#endif
        class ImagePlacedInSharedMemory
            : public MessageRegistration<ImagePlacedInSharedMemory> {
          public:
            class MessageSerialization;
            static const char *identifier();
        };
    } // namespace messages

    /// @brief BaseDevice component
    class ImagingComponent : public DeviceComponent {
      public:
        /// @brief Factory method
        ///
        /// Required to ensure that allocation and deallocation stay on the same
        /// side of a DLL line.
        static OSVR_COMMON_EXPORT shared_ptr<ImagingComponent>
        create(OSVR_ChannelCount numSensor = 0);

        /// @brief Message from server to client, containing some image data.
        messages::ImageRegion imageRegion;

        /// @brief Message from server to client, notifying of image data in the
        /// shared memory ring buffer.
        messages::ImagePlacedInSharedMemory imagePlacedInSharedMemory;

#ifdef OSVR_COMMON_IN_PROCESS_IMAGING
        /// @brief Message from server to client, notifying of image data in process
        /// memory (assumes joint client kit)
        messages::ImagePlacedInProcessMemory imagePlacedInProcessMemory;
#endif

        OSVR_COMMON_EXPORT void sendImageData(
            OSVR_ImagingMetadata metadata, OSVR_ImageBufferElement *imageData,
            OSVR_ChannelCount sensor, OSVR_TimeValue const &timestamp);

        typedef std::function<void(ImageData const &,
                                   util::time::TimeValue const &)> ImageHandler;
        OSVR_COMMON_EXPORT void registerImageHandler(ImageHandler cb);

      private:
        ImagingComponent(OSVR_ChannelCount numChan);
        virtual void m_parentSet();

        /// @return true if we could send it.
        bool m_sendImageDataViaSharedMemory(OSVR_ImagingMetadata metadata,
                                            OSVR_ImageBufferElement *imageData,
                                            OSVR_ChannelCount sensor,
                                            OSVR_TimeValue const &timestamp);

        /// @return true if we could send it.
        bool m_sendImageDataOnTheWire(OSVR_ImagingMetadata metadata,
                                      OSVR_ImageBufferElement *imageData,
                                      OSVR_ChannelCount sensor,
                                      OSVR_TimeValue const &timestamp);

#ifdef OSVR_COMMON_IN_PROCESS_IMAGING
        /// @return true if we could send it.
        bool m_sendImageDataViaInProcessMemory(OSVR_ImagingMetadata metadata,
                                               OSVR_ImageBufferElement *imageData,
                                               OSVR_ChannelCount sensor,
                                               OSVR_TimeValue const &timestamp);
#endif

        static int VRPN_CALLBACK
        m_handleImageRegion(void *userdata, vrpn_HANDLERPARAM p);

        static int VRPN_CALLBACK
        m_handleImagePlacedInSharedMemory(void *userdata, vrpn_HANDLERPARAM p);

#ifdef OSVR_COMMON_IN_PROCESS_IMAGING
        static int VRPN_CALLBACK
        m_handleImagePlacedInProcessMemory(void *userdata, vrpn_HANDLERPARAM p);
#endif

        void m_checkFirst(OSVR_ImagingMetadata const &metadata);
        void m_growShmVecIfRequired(OSVR_ChannelCount sensor);

        OSVR_ChannelCount m_numSensor;
        std::vector<ImageHandler> m_cb;
        bool m_gotOne;
        /// @brief One for each sensor
        std::vector<IPCRingBufferPtr> m_shmBuf;
    };
} // namespace common
} // namespace osvr

#endif // INCLUDED_ImagingComponent_h_GUID_BA26C922_01FD_43B3_8EB7_A9AB2777CEBC
