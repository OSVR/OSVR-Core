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
//        http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef INCLUDED_CSV_h_GUID_30FA2504_8CCF_4B87_71C7_5974F968BC19
#define INCLUDED_CSV_h_GUID_30FA2504_8CCF_4B87_71C7_5974F968BC19

// Internal Includes
// - none

// Library/third-party includes
// - none

// Standard includes
#include <unordered_map>
#include <string>
#include <sstream>
#include <vector>
#include <cassert>

namespace osvr {
namespace util {
    /// Class for easily outputting CSV files
    class CSV {
      public:
        class Row {
          public:
            Row() = default;
            template <typename T> Row(std::string const &col, T &&data) {
                addCol(col, std::forward<T>(data));
            }
            template <typename T>
            void addCol(std::string const &col, T &&data) {

                std::ostringstream os;
                os << std::forward<T>(data);
                headings_.push_back(col);
                data_.push_back(os.str());
            }

          private:
            friend class CSV;
            std::vector<std::string> headings_;
            std::vector<std::string> data_;
        };

        std::size_t getColumn(std::string const &heading) {
            auto it = columnsMap_.find(heading);
            // If we don't find it, this is where it will be.
            std::size_t ret = columns_.size();
            if (end(columnsMap_) != it) {
                // OK, found it somewhere already.
                ret = it->second;
                return ret;
            }
            // didn't find it, add it.
            columns_.push_back(heading);
            columnsMap_.insert(std::make_pair(heading, ret));
            return ret;
        }

        void add(Row const &row) {
            std::vector<std::string> rowData(numColumns());

            const auto size = row.headings_.size();
            for (std::size_t i = 0; i < size; ++i) {
                auto colId = getColumn(row.headings_[i]);
                /// ensure vector is big enough.
                if (colId >= rowData.size()) {
                    rowData.resize(colId + 1);
                }
                rowData[colId] = std::move(row.data_[i]);
            }
            data_.emplace_back(std::move(rowData));
        }

        std::size_t numDataRows() const { return data_.size(); }

        std::size_t numColumns() const { return columns_.size(); }

        void output(std::ostream &os) const {
            for (auto &colName : columns_) {
                os << "\"" << colName << "\",";
            }
            os << "\n";
            for (auto &row : data_) {
                for (auto &cell : row) {
                    os << cell << ",";
                }
                os << "\n";
            }
        }

      private:
        // std::ostringstream body_;

        std::vector<std::string> columns_;
        std::unordered_map<std::string, std::size_t> columnsMap_;
        std::vector<std::vector<std::string> > data_;
    };
    namespace detail {
        template <typename T> class Cell {
          public:
            Cell(std::string const &header, T const &data)
                : header_(header), data_(data){};
            std::string const &getHeader() const { return header_; };
            T const &getData() const { return data_; }

          private:
            std::string const &header_;
            T const &data_;
        };
    } // namespace detail

    template <typename T>
    inline detail::Cell<T> cell(std::string const &header, T const &data) {
        return detail::Cell<T>{header, data};
    }

    template <typename T>
    inline CSV::Row &&operator<<(CSV::Row &&row, detail::Cell<T> const &cell) {
        row.addCol(cell.getHeader(), cell.getData());
        return std::move(row);
    }
    template <typename T>
    inline CSV::Row &operator<<(CSV::Row &row, detail::Cell<T> const &cell) {
        row.addCol(cell.getHeader(), cell.getData());
        return row;
    }
} // namespace util
} // namespace osvr

#endif // INCLUDED_CSV_h_GUID_30FA2504_8CCF_4B87_71C7_5974F968BC19
