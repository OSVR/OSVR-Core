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
#include <boost/lexical_cast.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <vrpn_SerialPort.h>

// Standard includes
#include <chrono>
#include <fstream>
#include <functional>
#include <iostream>
#include <memory>
#include <random>
#include <sstream>
#include <thread>

using SerialBufType = const unsigned char;

/// Utility function to take in a (typically) string literal and write it,
/// without its null terminator, to the serial port, optionally repeatedly.
/// (Size deduced by template)
template <typename T, std::size_t N>
inline void writeStringWithoutNullTerminator(vrpn_SerialPort &port, T (&buf)[N],
                                             std::size_t repetitions = 1) {
    static_assert(sizeof(T) == 1, "Can only pass character/byte arrays");
    std::size_t len = N;
    if ('\0' == buf[len - 1]) {
        len--;
    }
    for (std::size_t i = 0; i < repetitions; ++i) {
        port.write(reinterpret_cast<SerialBufType *>(&(buf[0])), len);
    }
}

/// Utility function to take in a string literal and write it,
/// without its null terminator, followed by a newline, to the serial port,
/// optionally repeatedly. (Size deduced by template)
template <typename T, std::size_t N>
inline void writeCommand(vrpn_SerialPort &port, T (&buf)[N],
                         std::size_t repetitions = 1) {
    for (std::size_t i = 0; i < repetitions; ++i) {
        writeStringWithoutNullTerminator(port, buf);
        writeStringWithoutNullTerminator(port, "\n");
    }
}

/// Utility function to take in a binary array and write it verbatim to the
/// serial port, optionally repeatedly. (Size deduced by template)
template <std::size_t N>
inline void writeBinaryArray(vrpn_SerialPort &port, SerialBufType (&buf)[N],
                             std::size_t repetitions = 1) {
    std::size_t len = N;
    for (std::size_t i = 0; i < repetitions; ++i) {
        port.write(reinterpret_cast<SerialBufType *>(&(buf[0])), N);
    }
}

/// Abstract base class for ways of controlling an IR LED through a serial port.
class SerialIRLED {
  public:
    /// Call to cause an IR LED pulse to be emitted.
    void trigger() { handle_trigger(getPort()); }
    virtual ~SerialIRLED() {}

  protected:
    SerialIRLED(vrpn_SerialPort &port) : port_(std::ref(port)) {}
    virtual void handle_trigger(vrpn_SerialPort &port) = 0;

    vrpn_SerialPort &getPort() { return port_.get(); }

  private:
    std::reference_wrapper<vrpn_SerialPort> port_;
};
using SerialIRLEDPtr = std::unique_ptr<SerialIRLED>;
using SerialLEDControllerFactory =
    std::function<SerialIRLEDPtr(vrpn_SerialPort &)>;

/// A class for an LED connected to an Arduino running a sketch that pulses an
/// LED on then off on receiving a single character over serial.
class ArduinoLED : public SerialIRLED {
  public:
    static SerialIRLEDPtr make(vrpn_SerialPort &port) {
        SerialIRLEDPtr ret(new ArduinoLED(port));
        return ret;
    }

  private:
    ArduinoLED(vrpn_SerialPort &port) : SerialIRLED(port) {}
    void handle_trigger(vrpn_SerialPort &port) override {
        // write any byte to trigger a pulse
        writeStringWithoutNullTerminator(port, "a");
        port.drain_output_buffer();
    }
};

/// A class for an LED connected to a Bus Pirate's AUX pin (with resistor),
/// using the binary bit-bang mode to turn it on then off. Bus pirate should be
/// in normal "terminal" mode before starting.
class BusPirateAuxLED : public SerialIRLED {
  public:
    static SerialIRLEDPtr make(vrpn_SerialPort &port) {
        SerialIRLEDPtr ret;
        try {
            ret.reset(new BusPirateAuxLED(port));
        } catch (std::exception &e) {
            std::cerr << "Error setting up bus pirate: " << e.what()
                      << std::endl;
        }
        return ret;
    }

    virtual ~BusPirateAuxLED() {
        const std::uint8_t resetToTerminal[] = {RESET_TO_TERMINAL};
        writeBinaryArray(getPort(), resetToTerminal);
        getPort().drain_output_buffer();
    }

  private:
    BusPirateAuxLED(vrpn_SerialPort &port) : SerialIRLED(port) {
        bool success = enterBitBangMode(port);
        if (success) {
            std::cout << "Bus Pirate successfully entered binary bit-bang mode!"
                      << std::endl;
        } else {
            std::cout << "Couldn't get bus pirate into binary bit-bang mode: "
                         "check that nothing else is using the port and try "
                         "resetting it."
                      << std::endl;
            throw std::runtime_error("Could not transition bus pirate to "
                                     "binary bit-bang mode: check that nothing "
                                     "else is using the port and try resetting "
                                     "it.");
        }

        const std::uint8_t configPortModes[] = {PORT_MODE, ONLY_POWER_ON};
        writeBinaryArray(port, configPortModes);
        // wait until everything has been sent.
        port.drain_output_buffer();
        // throw away replies.
        port.flush_input_buffer();
    }
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
    bool enterBitBangMode(vrpn_SerialPort &port) {

        SerialBufType enter[] = "\n";
        // throw away anything we got to read.
        port.flush_input_buffer();
        // write at least 10 enters to get out of any menus.
        writeStringWithoutNullTerminator(port, enter, 10);
        // send reset command
        writeCommand(port, "#");
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
static const auto BUS_PIRATE_ARGS = {"--bp", "--buspirate", "-b"};
static const auto ARDUINO_ARGS = {"--arduino", "-a"};
static const auto DEVICE_ARG = "--dev";
static const auto DEFAULT_DEVICE = "COM1";
static const auto RATE_ARG = "--rate";
static const auto DEFAULT_RATE = 115200;
static const auto MEASUREMENTS_ARG = "--measurements";
static const auto DEFAULT_MEASUREMENTS = 20;

static const double CUTOFF_VALUE = 200.;

struct Config {
    SerialLEDControllerFactory controllerFactory;
    std::string device = DEFAULT_DEVICE;
    std::uint32_t rate = DEFAULT_RATE;
    std::uint32_t measurements = DEFAULT_MEASUREMENTS;
} g_config;

int main(int argc, char *argv[]) {
    {
        auto withUsageError = [] {
            /// @todo add usage
            return 1;
        };
        using namespace osvr::util::args;
        auto args = makeArgList(argc, argv);
        if (handle_has_any_iswitch_of(args, BUS_PIRATE_ARGS)) {
            std::cout << "Will use the Bus Pirate, IR LED on AUX, controller."
                      << std::endl;
            g_config.controllerFactory = BusPirateAuxLED::make;
        }
        if (handle_has_any_iswitch_of(args, ARDUINO_ARGS)) {
            if (g_config.controllerFactory) {
                std::cerr << "Got an arduino arg, but already was specified to "
                             "use bus pirate!"
                          << std::endl;
                return withUsageError();
            }
            g_config.controllerFactory = ArduinoLED::make;
        }

        if (!g_config.controllerFactory) {
            std::cerr << "No controller type specified! Assuming bus pirate."
                      << std::endl;
            g_config.controllerFactory = BusPirateAuxLED::make;
        }

        handle_value_arg(
            args, [](std::string const &a) { return a == DEVICE_ARG; },
            [&](std::string const &val) {
                std::cout << "Setting serial device to " << val << std::endl;
                g_config.device = val;
            });

        handle_value_arg(
            args, [](std::string const &a) { return a == RATE_ARG; },
            [&](std::string const &val) {
                g_config.rate = boost::lexical_cast<std::uint32_t>(val);
                std::cout << "Setting serial rate to " << g_config.rate
                          << std::endl;
            });

        handle_value_arg(
            args, [](std::string const &a) { return a == MEASUREMENTS_ARG; },
            [&](std::string const &val) {
                g_config.measurements = boost::lexical_cast<std::uint32_t>(val);
                std::cout << "Setting number of measurements to "
                          << g_config.measurements << std::endl;
            });
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

    vrpn_SerialPort comPort(g_config.device.c_str(), g_config.rate);
    if (!comPort.is_open()) {
        std::cerr << "Couldn't open com port!" << std::endl;
        return -1;
    }

    std::unique_ptr<SerialIRLED> LEDController(
        g_config.controllerFactory(comPort));

    if (!LEDController) {
        std::cerr << "Could not start LEDController, exiting." << std::endl;
        return 1;
    }

    enum class State { StartedTrigger, SawFlash, AwaitingNewTrigger };

    using osvr::util::time::TimeValue;
    TimeValue triggerTime = {};
    double peakVal = 0.;

    State s = State::AwaitingNewTrigger;

    std::random_device rd;
    std::mt19937 mt(rd());
    // distribution of how many frames we'll wait before triggering a new pulse.
    std::uniform_int_distribution<int> dist(5, 50);
    auto generateNewCountdown = [&mt, &dist] { return dist(mt); };
    int countdownToTrigger = generateNewCountdown();

    std::uint32_t samples = 0;
    std::ofstream os("latency.csv");
    do {
        TimeValue tv;
        cam->retrieve(frame, grayFrame, tv);
        switch (s) {
        case State::AwaitingNewTrigger: {
            if (countdownToTrigger == 0) {
                // trigger timing
                s = State::StartedTrigger;
                peakVal = 0.;
                LEDController->trigger();
                triggerTime = osvr::util::time::getNow();
            } else {
                countdownToTrigger--;
            }
            break;
        }
        case State::StartedTrigger: {
            auto now = osvr::util::time::getNow();
            double minVal, maxVal;
            cv::minMaxIdx(grayFrame, &minVal, &maxVal);
            // if (maxVal < peakVal) {
            if (maxVal > CUTOFF_VALUE) {
                osvrTimeValueDifference(&now, &triggerTime);
                osvrTimeValueDifference(&tv, &triggerTime);
                if (tv.microseconds > 0) {
                    // we got it
                    s = State::SawFlash;
                    std::cout
                        << "   Current: "
                        << maxVal << "\n";
                    std::cout << "Latency from trigger to sample time: "
                              << tv.microseconds << "us\n";
                    std::cout << "Latency from trigger to retrieval time: "
                              << now.microseconds << "us\n";
                    cv::imwrite("triggered.png", frame);
                    samples++;
					os << tv.microseconds << std::endl;
                } else {
                    std::cout << "Got a spurious flash, negative trigger to "
                                 "sample duration"
                              << std::endl;
                }
            }
            break;
        }
        case State::SawFlash: {
            // wait for the flash to disappear
            double minVal, maxVal;
            cv::minMaxIdx(grayFrame, &minVal, &maxVal);
            if (maxVal < CUTOFF_VALUE) {
                // OK, ready to go for a new trigger at some point.
                s = State::AwaitingNewTrigger;
                countdownToTrigger = generateNewCountdown();
                std::cout << "Will wait " << countdownToTrigger
                          << " frames before triggering again\n";
            }
            break;
        }
        }
    } while (cam->grab() && samples < g_config.measurements);

    LEDController.reset();

    return 0;
}
