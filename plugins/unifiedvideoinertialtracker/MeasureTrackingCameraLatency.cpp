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
//        http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#define OSVR_HAVE_BOOST

// Internal Includes
#include "ImageSources/ImageSource.h"
#include "ImageSources/ImageSourceFactories.h"
#include <osvr/Util/MiniArgsHandling.h>

// Library/third-party includes
#include <opencv2/highgui/highgui.hpp>
#include <vrpn_SerialPort.h>

// Standard includes
#include <chrono>
#include <iostream>
#include <memory>
#include <sstream>
#include <thread>

using SerialBufType = const unsigned char;

class SerialIRLED {
  public:
    void trigger() { handle_trigger(getPort()); }
    virtual ~SerialIRLED() {}

  protected:
    SerialIRLED(vrpn_SerialPort & port) : port_(std::ref(port)) {}
    virtual void handle_trigger(vrpn_SerialPort &port) = 0;

    vrpn_SerialPort &getPort() { return port_.get(); }

  private:
    std::reference_wrapper<vrpn_SerialPort> port_;
};

template <typename T, std::size_t N>
void writeStringWithoutNullTerminator(vrpn_SerialPort &port, T (&buf)[N],
                                      std::size_t repetitions = 1) {
    static_assert(sizeof(T) == 1, "Can only pass character/byte arrays");
    std::size_t len = N;
    if ('\0' == buf[len - 1]) {
        len--;
    }
    for (std::size_t i = 0; i < repetitions; ++i) {
        port.write(reinterpret_cast<const unsigned char *>(&(buf[0])), len);
    }
}

template <std::size_t N>
void writeBinaryArray(vrpn_SerialPort &port, SerialBufType (&buf)[N],
                      std::size_t repetitions = 1) {
    std::size_t len = N;
    for (std::size_t i = 0; i < repetitions; ++i) {
        port.write(reinterpret_cast<const unsigned char *>(&(buf[0])), N);
    }
}

class ArduinoLED : public SerialIRLED {
  public:
    ArduinoLED(vrpn_SerialPort & port) : SerialIRLED(port) {}

  private:
    void handle_trigger(vrpn_SerialPort &port) override {
        // write any byte to trigger a pulse
        const SerialBufType buf[] = "a";
        // comPort.write(reinterpret_cast<const unsigned char *>(buf), 1);
        port.write(buf, 1);
        port.drain_output_buffer();
    }
};

class BusPirateAuxLED : public SerialIRLED {
  public:
    static const std::uint8_t ONE = 0x01;
    static const std::uint8_t ZERO = 0x00;

	// 01001111: aka 0x4f configure everything except AUX as input.
	static const std::uint8_t PORT_MODE = 0x4f;
            // 11000000: aka 0xc0 turn on the power only
	static const std::uint8_t ONLY_POWER_ON = 0xc0;
	// 11010000: aka 0xd0 turn on the power and AUX
	static const std::uint8_t POWER_AND_AUX_ON = 0xd0;
	// 00001111: aka 0x0f reset bus pirate to terminal mode.
	static const std::uint8_t RESET_TO_TERMINAL = 0x0f;

    BusPirateAuxLED(vrpn_SerialPort & port) : SerialIRLED(port) {
        enterBitBangMode(port);

        const std::uint8_t configPortModes[] = {
            PORT_MODE,
            ONLY_POWER_ON};
        writeBinaryArray(port, configPortModes);
        // wait until everything has been sent.
        port.drain_output_buffer();
        // throw away replies.
        port.flush_input_buffer();
	
	}
    virtual ~BusPirateAuxLED() {
        const std::uint8_t resetToTerminal[] = {RESET_TO_TERMINAL};
        writeBinaryArray(getPort(), resetToTerminal);
        getPort().drain_output_buffer();
    }

  private:
    bool enterBitBangMode(vrpn_SerialPort &port) {

        SerialBufType enter[] = "\n";
        // throw away anything we got to read.
        port.flush_input_buffer();
        // write at least 10 enters to get out of any menus.
        writeStringWithoutNullTerminator(port, enter, 10);
        // send reset command
        writeStringWithoutNullTerminator(port, "#\n");
        // wait until everything has been sent.
        port.drain_output_buffer();
        std::cout << "Resetting bus pirate" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
        // throw away anything we got to read.
        port.flush_input_buffer();

        bool success = false;
        const std::uint8_t bitBangReset[] = {ZERO};
        static const auto desiredReply = "BBIO1";
        writeBinaryArray(port, bitBangReset, 10);
        for (int i = 0; i < 40; ++i) {
            writeBinaryArray(port, bitBangReset);
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
            // try more times to get BBIO1
            auto response = port.read_available_characters(5);
            if (desiredReply == response) {
                return true;
            } else if (!response.empty()) {
                std::cout << "When waiting for " << desiredReply
                          << " Bus Pirate sent us " << response << std::endl;
            }
        }
        return false;
    }
    void handle_trigger(vrpn_SerialPort &port) override {

        const std::uint8_t turnOn[] = {POWER_AND_AUX_ON};

        writeBinaryArray(port, turnOn);
        // wait until everything has been sent.
        port.drain_output_buffer();
        // throw away replies.
        port.flush_input_buffer();

        std::this_thread::sleep_for(std::chrono::milliseconds(5));

        const std::uint8_t turnOff[] = {ONLY_POWER_ON};

        writeBinaryArray(port, turnOff);
        // wait until everything has been sent.
        port.drain_output_buffer();
        // throw away replies.
        port.flush_input_buffer();
    }
};

static const std::string windowNameAndInstructions(
    "OSVR tracking camera preview | q or esc to quit");

int main(int argc, char *argv[]) {
    std::string devName;
    {
        using namespace osvr::util::args;
        auto args = makeArgList(argc, argv);
    }
#ifdef _WIN32
    auto cam = osvr::vbtracker::openHDKCameraDirectShow();
#else
    std::cerr << "Warning: Just using OpenCV to open Camera #0, which may not "
                 "be the tracker camera."
              << std::endl;
    auto cam = osvr::vbtracker::openOpenCVCamera(0);
#endif
    if (!cam || !cam->ok()) {
        std::cerr << "Couldn't find, open, or read from the OSVR HDK tracking "
                     "camera.\n"
                  << "Press enter to exit." << std::endl;
        std::cin.ignore();
        return -1;
    }

    auto FRAME_DISPLAY_STRIDE = 3u;
    cam->grab();

    std::cout << "Will display 1 out of every " << FRAME_DISPLAY_STRIDE
              << " frames captured." << std::endl;
    std::cout << "\nPress q or esc to quit, c to capture a frame to file.\n"
              << std::endl;

    auto frame = cv::Mat{};
    auto grayFrame = cv::Mat{};

    vrpn_SerialPort comPort("COM1", 115200);
    if (!comPort.is_open()) {
        std::cerr << "Couldn't open com port!" << std::endl;
        return -1;
    }
	
    std::unique_ptr<SerialIRLED> LEDController(new BusPirateAuxLED(comPort));

    bool startedTrigger = false;
    using osvr::util::time::TimeValue;
    TimeValue triggerTime = {};
    double peakVal = 0.;
    cv::namedWindow(windowNameAndInstructions);
    auto frameCount = std::size_t{0};
    do {
        TimeValue tv;
        cam->retrieve(frame, grayFrame, tv);

        if (startedTrigger) {
            auto now = osvr::util::time::getNow();
            double minVal, maxVal;
            cv::minMaxIdx(grayFrame, &minVal, &maxVal);
            // if (maxVal < peakVal) {
            if (maxVal > 150.) {
                // we got it
                startedTrigger = false;
                osvrTimeValueDifference(&now, &triggerTime);
                osvrTimeValueDifference(&tv, &triggerTime);
                std::cout << /* "Peak value: " << peakVal <<*/ "   Current: "
                          << maxVal << "\n";
                std::cout << "Latency from trigger to sample time: "
                          << tv.microseconds << "us\n";
                std::cout << "Latency from trigger to retrieval time: "
                          << now.microseconds << "us\n";
                cv::imwrite("triggered.png", frame);
            } /* else {
                 peakVal = maxVal;
             }*/
        }

        ++frameCount;
        if (frameCount % FRAME_DISPLAY_STRIDE == 0) {
            frameCount = 0;
            cv::imshow(windowNameAndInstructions, frame);
            char key = static_cast<char>(cv::waitKey(1)); // wait 1 ms for a key
            if ('q' == key || 'Q' == key || 27 /*esc*/ == key) {
                break;
            } else if ('t' == key) {
                // trigger timing
                startedTrigger = true;
                peakVal = 0.;
                auto buf = "a";
                comPort.write(reinterpret_cast<const unsigned char *>(buf), 1);
                comPort.drain_output_buffer();
                triggerTime = osvr::util::time::getNow();
            }
        }
    } while (cam->grab());
    return 0;
}
