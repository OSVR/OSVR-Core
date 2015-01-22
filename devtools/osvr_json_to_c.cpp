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
// - none

// Library/third-party includes
#include <boost/program_options.hpp>

#include <json/value.h>
#include <json/reader.h>
#include <json/writer.h>

#include <boost/format.hpp>

// Standard includes
#include <string>
#include <fstream>
#include <iostream>

void outputByte(std::ostream &os, const char byteVal) {
    os << boost::format("0x%02x") % static_cast<const unsigned short>(byteVal);
}

static const size_t BYTES_PER_LINE = 20;

void outputStringAsLiteral(std::string const &literalName,
                           std::string const &value, std::ostream &output) {
    output << "static const char " << literalName << "[] = {\n";
    if (!value.empty()) {
        outputByte(output, value[0]);
        for (size_t i = 1, n = value.length(); i < n; ++i) {
            output << ",";
            if (0 == i % BYTES_PER_LINE) {
                output << "\n";
            }
            outputByte(output, value[i]);
        }
    }
    output << "};\n" << std::flush;
}
void convertJsonToLiteral(std::string const &literalName, std::istream &input,
                          std::ostream &output) {
    Json::Reader reader;
    Json::Value root;
    // false is "don't collect comments"
    if (!reader.parse(input, root, false)) {
        throw std::runtime_error("Error in parsing JSON: " +
                                 reader.getFormattedErrorMessages());
    }
    Json::FastWriter writer;
    outputStringAsLiteral(literalName, writer.write(root), output);
}

int main(int argc, char *argv[]) {
    namespace po = boost::program_options;
    std::string inFilename;
    // clang-format off
    po::options_description desc("Options");
    desc.add_options()
        ("help", "produce help message")
        ("symbol,S", po::value<std::string>()->default_value("json"), "symbol/variable name to create in generated file")
        ("output,O", po::value<std::string>(), "output file (defaults to standard out)")
        ;
    po::options_description hidden("Hidden (positional-only) options");
    hidden.add_options()
        ("input", po::value<std::string>(&inFilename))
        ;
    // clang-format on

    po::positional_options_description p;
    p.add("input", 1);
    p.add("output", 1);

    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv)
                  .options(po::options_description().add(desc).add(hidden))
                  .positional(p)
                  .run(),
              vm);
    po::notify(vm);
    if (vm.count("help") || (vm.count("input") == 0)) {
        std::cout << "Usage: osvr_json_to_cpp <input> [output]\n";
        std::cout << desc << "\n";
        return 1;
    }

    std::ifstream inFile(inFilename);
    try {
        if (vm.count("output")) {
            std::ofstream outFile(vm["output"].as<std::string>());
            convertJsonToLiteral(vm["symbol"].as<std::string>(), inFile,
                                 outFile);
        } else {
            convertJsonToLiteral(vm["symbol"].as<std::string>(), inFile,
                                 std::cout);
        }
    } catch (std::exception &e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}