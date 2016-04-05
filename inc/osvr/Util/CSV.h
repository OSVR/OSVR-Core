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
#include <cassert>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

namespace osvr {
namespace util {

    namespace detail {
        /// Utility class used in conjunction with osvr::util::CSV, to store a
        /// single table cell's column header and data value temporarily until
        /// it is added to the row. Construct with the `cell()` helper
        /// functions.
        template <typename T> class Cell {
          public:
            Cell(std::string const &header, T const &data)
                : header_(header), data_(data){};

            Cell(std::string &&header, T const &data)
                : header_(std::move(header)), data_(data){};

            std::string const &getHeader() const { return header_; }

            T const &getData() const { return data_; }

          private:
            std::string header_;
            T const &data_;
        };
    } // namespace detail

    /// Class for easily outputting CSV files. Just call the .row() method and
    /// feed it << cell("header", yourdata) << cell("anotherheader", moredata);
    /// for as many rows as you want (one row call per row, of course).
    ///
    /// When you're ready to get your output, hand an ostream (like std::cout or
    /// your favorite std::ofstream) to .output()
    class CSV {
      public:
        /// Returned by calls to .row() on a CSV object (you'll never need
        /// instantiate manually), and only really interacted with using the
        /// operator<< and cell() calls.
        ///
        /// Intended only to survive a single statement, and for only one to be
        /// alive (for a given CSV object) at a time. You have been warned.
        class RowProxy {
          public:
            explicit RowProxy(CSV &csv) : csv_(csv) {}
            /// non-copyable
            RowProxy(RowProxy const &) = delete;
            /// non-assignable
            RowProxy &operator=(RowProxy const &) = delete;
            /// move constructible
            RowProxy(RowProxy &&other)
                : csv_(other.csv_), preparedRow_(other.preparedRow_),
                  active_(other.active_) {
                other.active_ = false;
            }

            ~RowProxy() {
                if (active_) {
                    csv_.finalizeLatestRow();
                }
            }

            /// Main function, used by the operator<< to add your cell to the
            /// row in progress. All the non-type-dependent stuff is pulled out
            /// into separate methods to let the compiler optionally reduce code
            /// size.
            template <typename T> void add(detail::Cell<T> const &c) {
                commonPreAdd();
                os_ << c.getData();
                commonPostAdd(c.getHeader());
            }

          private:
            /// non-template-parameter-dependent things we do before
            /// type-dependent stuff in add()
            void commonPreAdd() {
                if (!preparedRow_) {
                    csv_.prepareForRow();
                    preparedRow_ = true;
                } else {
                    os_ = std::ostringstream{};
                }
            }
            /// non-template-parameter-dependent things we do after
            /// type-dependent stuff in add()
            void commonPostAdd(std::string const &header) {
                csv_.dataForLatestRow(header, os_.str());
            }

            CSV &csv_;
            std::ostringstream os_;
            bool preparedRow_ = false;
            bool active_ = true;
        };

        friend class RowProxy;

        using column_id = std::size_t;

        column_id getColumn(std::string const &heading) {
            auto it = columnsMap_.find(heading);
            // If we don't find it, this is where it will be.
            column_id ret = columns_.size();
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

        /// Main call for the CSV object: returns a proxy object that you can
        /// redirect "cells" into, in order to add them to a new row in the CSV
        /// table.
        RowProxy row() { return RowProxy(*this); }

        std::size_t numDataRows() const { return data_.size(); }

        column_id numColumns() const { return columns_.size(); }

        /// Outputs all the stored rows and columns, with the union of all
        /// headers in the first row, quoted, to the std::ostream given.
        void output(std::ostream &os) const { outputStoredData(os); }

      private:
        void prepareForRow() { latestRow().clear(); }

        void dataForLatestRow(std::string const &heading,
                              std::string const &data) {
            auto col = getColumn(heading);
            ensureLatestRowCanHoldColId(col);
            latestRow()[col] = data;
        }
        void finalizeLatestRow() { data_.emplace_back(std::move(latestRow())); }
        using DataRow = std::vector<std::string>;
        void ensureLatestRowCanHoldColId(column_id id) {
            if (id >= latestRow().size()) {
                latestRow().resize(id + 1);
            }
        }
        DataRow &latestRow() { return latestRow_; }

        void outputHeaders(std::ostream &os) const {
            for (auto &colName : columns_) {
                os << "\"" << colName << "\",";
            }
            os << "\n";
        }

        void outputRow(std::ostream &os, DataRow const &row) const {
            for (auto &cell : row) {
                os << cell << ",";
            }
            os << "\n";
        }
        void outputStoredData(std::ostream &os) const {
            outputHeaders(os);
            for (auto &row : data_) {
                outputRow(os, row);
            }
        }

        std::vector<std::string> columns_;
        std::unordered_map<std::string, column_id> columnsMap_;
        std::vector<DataRow> data_;
        DataRow latestRow_;
    };

    /// Helper function to make a CSV cell
    template <typename T>
    inline detail::Cell<T> cell(const char *header, T const &data) {
        return detail::Cell<T>{header, data};
    }

    /// Helper function to make a CSV cell
    template <typename T>
    inline detail::Cell<T> cell(std::string const &header, T const &data) {
        return detail::Cell<T>{header, data};
    }

    /// Left-shift/redirection operator to add a cell to a row proxy object
    template <typename T>
    inline CSV::RowProxy &&operator<<(CSV::RowProxy &&row,
                                      detail::Cell<T> const &cell) {
        row.add(cell);
        return std::move(row);
    }

    /// Left-shift/redirection operator to add a cell to a row proxy object
    template <typename T>
    inline CSV::RowProxy &operator<<(CSV::RowProxy &row,
                                     detail::Cell<T> const &cell) {
        row.add(cell);
        return row;
    }
} // namespace util
} // namespace osvr

#endif // INCLUDED_CSV_h_GUID_30FA2504_8CCF_4B87_71C7_5974F968BC19
