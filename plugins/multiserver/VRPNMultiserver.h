/** @file
    @brief Header

    @date 2014

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2014 Sensics, Inc.
//
// All rights reserved.
//
// (Final version intended to be licensed under
// the Apache License, Version 2.0)

#ifndef INCLUDED_VRPNMultiserver_h_GUID_05913DEF_7B12_4089_FE38_379BEF4A1A7D
#define INCLUDED_VRPNMultiserver_h_GUID_05913DEF_7B12_4089_FE38_379BEF4A1A7D

// Internal Includes
// - none

// Library/third-party includes
// - none

// Standard includes
#include <map>
#include <string>

class VRPNMultiserverData {
  public:
    std::string getName(std::string const &nameStem);

  private:
    typedef std::map<std::string, size_t> NameCountMap;
    std::string assignName(std::string const &nameStem);
    size_t assignNumber(std::string const &nameStem);

    NameCountMap m_nameCount;
};

#endif // INCLUDED_VRPNMultiserver_h_GUID_05913DEF_7B12_4089_FE38_379BEF4A1A7D
