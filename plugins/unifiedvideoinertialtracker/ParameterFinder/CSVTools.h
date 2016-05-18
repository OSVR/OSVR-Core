/** @file
    @brief Header

    @date 2016

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2016 Sensics, Inc.
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

#ifndef INCLUDED_CSVTools_h_GUID_82FA298C_196A_46AA_B2D6_059F2A035687
#define INCLUDED_CSVTools_h_GUID_82FA298C_196A_46AA_B2D6_059F2A035687

// Internal Includes
// - none

// Library/third-party includes
// - none

// Standard includes
#include <cassert>
#include <cstddef>
#include <iosfwd>
#include <iterator>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace csvtools {

static const char COMMA_CHAR = ',';

static const char DOUBLEQUOTE_CHAR = '"';

inline std::string getCleanLine(std::istream &is) {
    std::string ret;
    std::getline(is, ret);
    while (!ret.empty() && (ret.back() == '\n' || ret.back() == '\r')) {
        ret.pop_back();
    }
    return ret;
}

namespace string_fields {

    inline std::size_t getBeginningOfField(std::string const &line,
                                           std::size_t field) {
        if (0 == field) {
            return 0;
        }
        std::size_t pos = 0;
        for (std::size_t i = 0; i < field && pos != std::string::npos; ++i) {
            pos = line.find(COMMA_CHAR, pos + 1);
        }
        if (pos != std::string::npos) {
            if (pos + 1 < line.size()) {
                // must advance past the comma.
                pos++;
            } else {
                // if we can't advance past the comma, it's as though we
                // couldn't
                // find the field.
                pos = std::string::npos;
            }
        }
        return pos;
    }
}

#define CSV_TESTING

#ifdef CSV_TESTING
#define CSV_VERIFY(X, MSG)                                                     \
    do {                                                                       \
        if (!(X)) {                                                            \
            throw std::logic_error(MSG);                                       \
        }                                                                      \
    } while (0)
#else
#define CSV_VERIFY(X, MSG)
#endif

class StringField {
  public:
    using iterator = std::string::const_iterator;
    StringField(std::string const &line, const std::size_t b,
                const std::size_t e)
        : line_(line), b_(b), e_(e), ve_(virtualEndPos(line_, e_)) {
        checkInvariants();
    }

    StringField(StringField const &other, const std::size_t b,
                const std::size_t e)
        : line_(other.line_), b_(b), e_(e_), ve_(virtualEndPos(line_, e_)) {
        checkInvariants();
    }

    StringField(StringField const &other) = default;

    /// return the beginning position, which is a valid position in the string.
    std::size_t beginPos() const { return b_; }

    /// return the virtual one-past-the-end position - may point one past the
    /// end of the array but isn't std::string::npos.
    std::size_t virtualEndPos() const { return ve_; }

    /// return the one-past-the-end position, which may be std::string::npos if
    /// the range includes the rest of the string.
    std::size_t endPos() const { return e_; }

    /// gets the length of the field
    std::size_t realLength() const { return virtualEndPos() - b_; }

    /// like realLength, except returns std::string::npos when the end includes
    /// the rest of the string.
    std::size_t lengthForSubstr() const {
        return (e_ == std::string::npos) ? std::string::npos : e_ - b_;
    }

    /// Is the field/range empty?
    bool empty() const { return beginPos() == virtualEndPos(); }

    /// Access (by reference) the full line.
    std::string const &getLine() const { return line_; }

    /// Make a copy of the range as a substring.
    std::string substr() const {
        return line_.substr(beginPos(), lengthForSubstr());
    }

    /// Get a begin iterator.
    iterator begin() const { return getIteratorAt(beginPos()); }

    /// Get an end iterator
    iterator end() const {
        if (e_ == std::string::npos) {
            return line_.cend();
        } else {
            return getIteratorAt(endPos());
        }
    }

    char front() const {
        if (empty()) {
            throw std::out_of_range(
                "can't get front element of an empty string field");
        }
        return line_[beginPos()];
    }

    char back() const {
        if (empty()) {
            throw std::out_of_range(
                "can't get back element of an empty string field");
        }
        return line_[virtualEndPos() - 1];
    }

    /// Get a StringField object based on this one where outer quotation marks,
    /// if present, have been removed.
    StringField stripQuotes() const {
        if (realLength() < 2) {
            /// Too short to be wrapped in quotes.
            return StringField(*this);
        }
        if (front() != DOUBLEQUOTE_CHAR || back() != DOUBLEQUOTE_CHAR) {
            /// Not wrapped in quotes.
            return StringField(*this);
        }
        auto newB = beginPos() + 1;
        auto newE = virtualEndPos() - 1;
        return StringField(*this, newB, newE);
    }

    void loadIntoStringStream(std::istringstream &iss) const {
        /// @todo might be able to do stuff with streambuf::sputn to make this
        /// more efficient, but...
        iss.str(substr());
        iss.clear();
    }

  private:
    iterator getIteratorAt(const std::size_t pos) const {
        auto it = line_.cbegin();
        std::advance(it, pos);
        return it;
    }
    static std::size_t virtualEndPos(std::string const &line,
                                     const std::size_t e) {
        return (e == std::string::npos) ? line.size() : e;
    }
    void checkInvariants() const {
        assert((e_ == std::string::npos || e_ < line_.size()) &&
               "past the end position must be npos or a valid position");
        CSV_VERIFY((e_ == std::string::npos || e_ < line_.size()),
                   "past the end position must be npos or a valid position");
        CSV_VERIFY((b_ < line_.size()),
                   "Begin position must be a valid position");
        CSV_VERIFY((e_ == std::string::npos || b_ < e_),
                   "Begin position must be strictly before past the end.");
        CSV_VERIFY((b_ < ve_), "Begin position must be strictly before the "
                               "virtual past the end.");
#if 0
        /// only if we're restricting this to full fields.
        CSV_VERIFY((e_ == std::string::npos || line_[e_] == COMMA_CHAR),
                   "past the end position must be npos or hold a delimiter.");
#endif
    }
    std::string const &line_;
    const std::size_t b_;
    const std::size_t e_;
    const std::size_t ve_;
};

class FieldParserHelper {
  public:
    template <typename T>
    inline bool getField(StringField const &field, T &output) {
        field.loadIntoStringStream(iss_);
        return static_cast<bool>(iss_ >> output);
    }

    template <typename T>
    inline std::pair<bool, T> getFieldAs(StringField const &field) {
        T ret = T{};
        field.loadIntoStringStream(iss_);
        auto success = static_cast<bool>(iss_ >> ret);
        return std::make_pair(success, ret);
    }

  private:
    std::istringstream iss_;
};

/// fieldFunc(std::string const &line, std::size_t beginPos, std::size_t
/// endPos
/// /* or npos if end of string */), return true to continue to next field.
template <typename F>
inline void iterateFields(F &&fieldFunc, std::string const &line,
                          std::size_t numFields = std::string::npos,
                          std::size_t first = 0) {
    /// "begin" iterator/position
    std::size_t b = string_fields::getBeginningOfField(line, first);
    /// initial "one past the end" iterator/position
    auto e = b;
    std::size_t len = 0;
    bool keepGoing = true;
    const auto n = line.size();
    /// the condition on b < n is because we update b = e + 1, and e might
    /// be the last character in the string.
    for (std::size_t i = 0; i < numFields && keepGoing && b < n; ++i) {
        e = line.find(COMMA_CHAR, b);
        keepGoing = fieldFunc(line, b, e);
        if (e == std::string::npos) {
            keepGoing = false;
        }
        if (keepGoing) {
            b = e + 1;
        }
    }
}

inline std::vector<std::string>
getFields(std::string const &line, std::size_t numFields = std::string::npos,
          std::size_t first = 0) {
    std::vector<std::string> ret;
    auto fieldFunc = [&](std::string const &line, std::size_t beginPos,
                         std::size_t endPos) {
        bool lambdaRet = true;
        std::size_t len;
        if (endPos == std::string::npos) {
            // indicate to substring we want the rest of the line.
            len = std::string::npos;
            // quit after this field
            lambdaRet = false;
        } else {
            len = endPos - beginPos;
        }
        ret.emplace_back(line.substr(beginPos, len));
        return lambdaRet;
    };
    iterateFields(fieldFunc, line, numFields, first);

    return ret;
}

inline void stripQuotes(std::string &field) {
    if (field.size() > 1 && field.front() == DOUBLEQUOTE_CHAR &&
        field.back() == DOUBLEQUOTE_CHAR) {
        /// pop back first, so we have less to "slide up"
        field.pop_back();
        /// then remove the first character
        field.erase(0, 1);
    }
}

inline void stripQuotes(std::vector<std::string> &fields) {
    for (auto &field : fields) {
        stripQuotes(field);
    }
}
} // csvtools

#endif // INCLUDED_CSVTools_h_GUID_82FA298C_196A_46AA_B2D6_059F2A035687
