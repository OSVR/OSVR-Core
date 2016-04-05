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
    template <typename Derived> class CSVBase;
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

        /// Returned by calls to .row() on a CSV object (you'll never need
        /// instantiate manually), and only really interacted with using the
        /// operator<< and cell() calls.
        ///
        /// Intended only to survive a single statement, and for only one to be
        /// alive (for a given CSV object) at a time. You have been warned.
        template <typename Derived> class CSVRowProxy {
          public:
            using BaseCSVType = CSVBase<Derived>;
            friend class CSVBase<Derived>;
            using CSV = Derived;
            using type = CSVRowProxy<Derived>;

          private:
            /// Constructor - called by CSVBase<Derived>.row()
            explicit CSVRowProxy(BaseCSVType &csv) : csv_(csv) {}

          public:
            /// non-copyable
            CSVRowProxy(CSVRowProxy const &) = delete;
            /// non-assignable
            CSVRowProxy &operator=(CSVRowProxy const &) = delete;
            /// move constructible
            CSVRowProxy(CSVRowProxy &&other)
                : csv_(other.csv_), preparedRow_(other.preparedRow_),
                  active_(other.active_) {
                /// When moving, don't let the moved-from proxy perform
                /// finalization of this row.
                other.active_ = false;
            }

            /// Destructor - finalizes the row in the CSV object if we've got
            /// some data and haven't been moved-from.
            ~CSVRowProxy() {
                /// @todo Does it make sense to skip finalizing if we haven't
                /// had any data added? Think so...
                if (active_ && preparedRow_) {
                    csv_.finalizeLatestRow();
                }
            }

            /// Main function, used by the operator<< to add your cell to the
            /// row in progress. All the non-type-dependent stuff is pulled out
            /// into separate methods to let the compiler optionally reduce code
            /// size.
            template <typename T> void add(Cell<T> const &c) {
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

            BaseCSVType &csv_;
            std::ostringstream os_;
            bool preparedRow_ = false;
            bool active_ = true;
        };

        namespace csv {
            static const char COMMA[] = ",";
            static const char DOUBLEQUOTE[] = "\"";
            static const char DOUBLEQUOTE_COMMA[] = "\",";
            static const char NEWLINE[] = "\n";
        } // namespace csv

        /// Truly shared base class for all CSV implementations.
        class CSVCommonBase {
          public:
            using DataRow = std::vector<std::string>;
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
            column_id numColumns() const { return columns_.size(); }

          protected:
            void outputHeaders(std::ostream &os) const {
                for (auto &colName : columns_) {
                    os << csv::DOUBLEQUOTE << colName << csv::DOUBLEQUOTE_COMMA;
                }
                os << csv::NEWLINE;
            }

          private:
            std::vector<std::string> columns_;
            std::unordered_map<std::string, column_id> columnsMap_;
        };

        /// Left-shift/redirection operator to add a cell to a row proxy object
        template <typename Derived, typename T>
        inline CSVRowProxy<Derived> &&operator<<(CSVRowProxy<Derived> &&row,
                                                 Cell<T> const &cell) {
            row.add(cell);
            return std::move(row);
        }

        /// Left-shift/redirection operator to add a cell to a row proxy object
        template <typename Derived, typename T>
        inline CSVRowProxy<Derived> &operator<<(CSVRowProxy<Derived> &row,
                                                Cell<T> const &cell) {
            row.add(cell);
            return row;
        }
    } // namespace detail

    /// (Base) Class for easily outputting CSV files. Just call the .row()
    /// method and feed it
    /// << cell("header", yourdata) << cell("anotherheader", moredata);
    /// for as many rows as you want (one row call per row, of course).
    ///
    /// Methods of outputting the CSV data to an ostream vary between
    /// subclasses.  This is a "curiously-recurring template pattern" base class
    /// for compile-time polymorphism.
    template <typename Derived> class CSVBase : public detail::CSVCommonBase {
      public:
        friend class detail::CSVRowProxy<Derived>;
        using RowProxy = detail::CSVRowProxy<Derived>;

        /// Main call for the CSV object: returns a proxy object that you can
        /// redirect "cells" into, in order to add them to a new row in the CSV
        /// table.
        RowProxy row() { return RowProxy(*this); }

        /// Gets the number of rows in the internal data storage. Note that if
        /// the particular specialized CSV class you're using doesn't put all
        /// rows in the data storage (such as StreamCSV), this won't necessarily
        /// be equal to the number of rows that have been added.
        std::size_t numDataRows() const { return data_.size(); }

        /// Gets the total number of rows that have been streamed/added to this
        /// CSV object, whether or not they're in the internal data storage.
        /// That is, `numRows() >= numDataRows()`
        std::size_t numRows() const { return rows_; }

      protected:
        /// Called by CSVRowProxy life cycle, on row creation.
        void prepareForRow() { latestRow().clear(); }
        /// Called by CSVRowProxy life cycle, on cell addition.
        void dataForLatestRow(std::string const &heading,
                              std::string const &data) {
            auto col = getColumn(heading);
            ensureLatestRowCanHoldColId(col);
            latestRow()[col] = data;
        }
        /// Called by CSVRowProxy life cycle, on destruction of active row
        /// proxy. Delegates to derived classes, since this is a variance
        /// between them.
        void finalizeLatestRow() {
            rows_++;
            derived().finalizeLatestRow();
        }

        /// Called internally and potentially by derived classes for access to
        /// the "latest row" temporary storage.
        DataRow &latestRow() { return latestRow_; }

        /// Called by outputData() and by derived classes to format individual
        /// rows.
        void outputRow(std::ostream &os, DataRow const &row) const {
            for (auto &cell : row) {
                os << cell << detail::csv::COMMA;
            }
            os << detail::csv::NEWLINE;
        }

        /// Called by derived classes to output stored data rows.
        void outputData(std::ostream &os) const {
            for (auto &row : data_) {
                outputRow(os, row);
            }
        }

        /// utility function for use in derived finalizeLatestRow()
        void moveLatestRowToData() {
            data_.emplace_back(std::move(latestRow()));
        }

      private:
        /// Internal utility used by dataForLatestRow.
        void ensureLatestRowCanHoldColId(column_id id) {
            if (id >= latestRow().size()) {
                latestRow().resize(id + 1);
            }
        }

        /// CRTP derived class accessor
        Derived const &derived() const {
            return *static_cast<Derived const *>(this);
        }
        /// CRTP derived class accessor (const)
        Derived &derived() { return *static_cast<Derived *>(this); }
        std::size_t rows_ = 0;
        std::vector<DataRow> data_;
        DataRow latestRow_;
    };

    /// The "traditional" CSV class: get all your data set up ahead of time.
    /// When you're ready to get your output, hand an ostream (like std::cout or
    /// your favorite std::ofstream) to .output()
    class CSV : public CSVBase<CSV> {
        using Base = CSVBase<CSV>;
        friend class CSVBase<CSV>;

      public:
        /// Outputs all the stored rows and columns, with the union of all
        /// headers in the first row, quoted, to the std::ostream given.
        void output(std::ostream &os) const {
            Base::outputHeaders(os);
            Base::outputData(os);
        }

      private:
        /// This class always moves rows into the data storage.
        void finalizeLatestRow() { Base::moveLatestRowToData(); }
    };

    /// A CSV object taking a reference to an ostream that should remain valid
    /// throughout the entire lifetime of this object.
    ///
    /// Once you start output, it will output the headers and any existing rows,
    /// and from then on, it will will output each row as it is finalized. Of
    /// course, any additional headers acquired after you start streaming can't
    /// be added to the first row retroactively, but they will be consistent
    /// (leaving blanks if appropriate, etc.)
    class StreamCSV : public CSVBase<StreamCSV> {
        using Base = CSVBase<StreamCSV>;
        friend class CSVBase<StreamCSV>;

      public:
        /// Constructor
        /// @param os Reference to the desired output stream, which must remain
        /// valid for the lifetime of this object.
        explicit StreamCSV(std::ostream &os) : m_stream(os) {}

        /// Outputs all the stored rows and columns, with the union of all
        /// headers in the first row, quoted, to the std::ostream given at
        /// construction, and starts streaming any additional rows.
        void startOutput() {
            Base::outputHeaders(m_stream);
            outputData(m_stream);
            m_streaming = true;
        }

      private:
        /// Reference to a stream, which must remain valid for the lifetime of
        /// this object.
        std::ostream &m_stream;

        /// Flag indicating whether we received a startOutput() call and should
        /// direct all new finalized rows directly to output, rather than data
        /// storage.
        bool m_streaming = false;

        /// This class outputs a row to the stream if we've already started
        /// streaming, otherwsie it moves the row into data storage
        void finalizeLatestRow() {
            if (m_streaming) {
                /// Output it directly
                Base::outputRow(m_stream, Base::latestRow());
            } else {
                Base::moveLatestRowToData();
            }
        }
    };

    /// Helper free function to make a CSV cell
    template <typename T>
    inline detail::Cell<T> cell(const char *header, T const &data) {
        return detail::Cell<T>{header, data};
    }

    /// Helper free function to make a CSV cell
    template <typename T>
    inline detail::Cell<T> cell(std::string const &header, T const &data) {
        return detail::Cell<T>{header, data};
    }

} // namespace util
} // namespace osvr

#endif // INCLUDED_CSV_h_GUID_30FA2504_8CCF_4B87_71C7_5974F968BC19
