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

#ifndef INCLUDED_CSVCellGroup_h_GUID_F36004E7_71A4_43AD_7353_B67BCBD8045A
#define INCLUDED_CSVCellGroup_h_GUID_F36004E7_71A4_43AD_7353_B67BCBD8045A

// Internal Includes
#include <osvr/Util/CSV.h>
#include <osvr/Util/TimeValue.h>

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace util {
    struct DefaultGroupTag;
    /// A class that exists to make it easier to output composite data
    /// structures to CSV. You can overload `void
    /// operator<<(CellGroupProxy<RowProxyType, GroupTag> &group,
    /// YourThing const& d)`,
    /// doing `group << cell("Item", d.item)`, etc. then invoke it just by
    /// passing an object of that type to cellGroup(), which takes an optional
    /// first parameter, a prefix to appead to all the headings.
    template <typename RowProxyType, typename GroupTag = DefaultGroupTag>
    class CellGroupProxy {
      public:
        CellGroupProxy(RowProxyType &row, const char *groupPrefix)
            : row_(row), prefix_(groupPrefix){};

        RowProxyType getRow() { return std::forward<RowProxyType>(row_); }

        std::string const &getPrefix() const { return prefix_; }

      private:
        RowProxyType &row_;
        const std::string prefix_;
    };

    /// Handles user results from overloading
    /// `operator<<(CellGroupProxy<RowProxyType, GroupTag> &group, TheirThing)`
    /// and
    /// sending in `cell` calls within it.
    template <typename RowProxyType, typename GroupTag, typename... CellArgs>
    inline CellGroupProxy<RowProxyType, GroupTag> &
    operator<<(CellGroupProxy<RowProxyType, GroupTag> &group,
               detail::Cell<CellArgs...> &&currentCell) {
        // cell.addHeaderPrefix(group.getPrefix());
        group.getRow().add(cell(group.getPrefix() + currentCell.getHeader(),
                                currentCell.getData()));
        return group;
    }

    namespace detail {
        /// Temporary object created by the cellGroup call, captures the string
        /// literal prefix, the data reference/type, and the group tag.
        template <typename T, typename Tag = DefaultGroupTag> class CellGroup {
          public:
            CellGroup(const char *prefix, T const &data)
                : prefix_(prefix), data_(data) {}

            const char *getPrefix() const { return prefix_; }
            T const &getData() const { return data_; }

          private:
            const char *prefix_;
            T const &data_;
        };

        /// Wrapper for RowProxy lvalue-reference and CellGroup:
        /// creates the specially-typed CellGroupProxy object that will activate
        /// the user's operator<< and ours in turn, and takes care of getting
        /// the return value correct for chained calls.
        template <typename Derived, typename T, typename Tag>
        inline CSVRowProxy<Derived> &
        operator<<(CSVRowProxy<Derived> &row,
                   detail::CellGroup<T, Tag> const &group) {
            using RowProxyType = CSVRowProxy<Derived> &;
            auto proxy =
                CellGroupProxy<RowProxyType, Tag>{row, group.getPrefix()};
            proxy << group.getData();
            return row;
        }

        /// Wrapper for RowProxy rvalue-reference and CellGroup.
        /// creates the specially-typed CellGroupProxy object that will activate
        /// the user's operator<< and ours in turn, and takes care of getting
        /// the return value correct for chained calls.
        template <typename Derived, typename T, typename Tag>
        inline CSVRowProxy<Derived> &&
        operator<<(CSVRowProxy<Derived> &&row,
                   detail::CellGroup<T> const &group) {
            using RowProxyType = CSVRowProxy<Derived> &&;
            auto proxy = CellGroupProxy<RowProxyType, Tag>{
                std::forward<RowProxyType>(row), group.getPrefix()};
            proxy << group.getData();
            return std::move(row);
        }

    } // namespace detail

    /// Helper function to create a cell group with a group header prefix.
    template <typename T>
    inline detail::CellGroup<T> cellGroup(const char *groupHeader,
                                          T const &data) {
        return detail::CellGroup<T>{groupHeader, data};
    }
    /// Helper function to create a cell group with a group header prefix and
    /// explicit (non-default) tag.
    template <typename Tag, typename T>
    inline detail::CellGroup<T, Tag>
    cellGroup(const char *groupHeader, T const &data,
              Tag * /*dummy*/ = static_cast<Tag *>(nullptr)) {
        return detail::CellGroup<T, Tag>{groupHeader, data};
    }
    /// Helper function to create a cell group with no group header prefix -
    /// warning: column headers must be unique!
    template <typename T> inline detail::CellGroup<T> cellGroup(T const &data) {
        return detail::CellGroup<T>{"", data};
    }

    /// Helper function to create a cell group with no group header prefix and
    /// explicit (non-default) tag - warning: column headers must be unique!
    template <typename Tag, typename T>
    inline detail::CellGroup<T, Tag>
    cellGroup(T const &data, Tag * /*dummy*/ = static_cast<Tag *>(nullptr)) {
        return detail::CellGroup<T, Tag>{"", data};
    }

    template <typename T>
    inline void operator<<(CellGroupProxy<T, DefaultGroupTag> &group,
                           time::TimeValue const &tv) {
        group << cell("seconds", tv.seconds)
              << cell("microseconds", tv.microseconds);
    }
    template <typename T>
    inline void operator<<(CellGroupProxy<T, DefaultGroupTag> &group,
                           Eigen::Vector3d const &v) {
        group << cell("x", v.x()) << cell("y", v.y()) << cell("z", v.z());
    }

    template <typename T>
    inline void operator<<(CellGroupProxy<T, DefaultGroupTag> &group,
                           Eigen::Quaterniond const &q) {
        group << cell("qw", q.w()) << cell("qx", q.x()) << cell("qy", q.y())
              << cell("qz", q.z());
    }

} // namespace util
} // namespace osvr
#endif // INCLUDED_CSVCellGroup_h_GUID_F36004E7_71A4_43AD_7353_B67BCBD8045A
