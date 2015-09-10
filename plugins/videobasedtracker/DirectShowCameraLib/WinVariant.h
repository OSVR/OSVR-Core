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

#ifndef INCLUDED_WinVariant_h_GUID_A0A043D9_A146_4693_D351_06F6A04ABADA
#define INCLUDED_WinVariant_h_GUID_A0A043D9_A146_4693_D351_06F6A04ABADA

#ifdef _WIN32

// Internal Includes
// - none

// Library/third-party includes
#include <oaidl.h>

// Standard includes
#include <type_traits>
#include <stdexcept>
#include <memory>
#include <utility>

namespace detail {

/// @brief A template alias for use in static asserts..
template <typename T>
using is_variant_type =
    std::integral_constant<bool, std::is_same<T, VARIANT>::value ||
                                     std::is_same<T, VARIANTARG>::value>;

/// @brief An enable_if_t-alike that has the specific condition built in as well
/// as a default result type.
template <typename T, typename Result = void *>
using enable_for_variants_t =
    typename std::enable_if<is_variant_type<T>::value, Result>::type;

template <typename OutputType> struct VariantTypeTraits;
/// @todo extend the VariantTypeTraits implementation
/// https://msdn.microsoft.com/en-us/library/windows/desktop/ms221170(v=vs.85).aspx

#if 0 // I don't actually see any members in the union to hold these...
template <> struct VariantTypeTraits<const wchar_t *> {
    static const auto vt = VT_LPWSTR;
};
template <> struct VariantTypeTraits<const char *> {
    static const auto vt = VT_LPSTR;
};
#endif

template <> struct VariantTypeTraits<BSTR> {
    static const auto vt = VT_BSTR;
    template <typename T> static BSTR get(T &v) { return v.bstrVal; }
};

// std::wstring is the "C++-side" wrapper for a BSTR here.
template <> struct VariantTypeTraits<std::wstring> : VariantTypeTraits<BSTR> {
    template <typename T> static std::wstring get(T &v) {
        auto bs = v.bstrVal;
        return std::wstring(static_cast<const wchar_t *>(bs), SysStringLen(bs));
    }
};

/// @brief Low-level variant holder: just handles creation and destruction.
///
/// Could be used by itself on the stack, but not terribly likely.
template <typename T = VARIANT> struct VariantHolder {
  public:
    static_assert(is_variant_type<T>::value,
                  "Only valid type parameters are VARIANT or VARIANTARG");
    using type = VariantHolder<T>;
    using unique_type = std::unique_ptr<type>;
    static unique_type make_unique() {
        unique_type ret(new type);
        return ret;
    }
    VariantHolder() { VariantInit(getPtr()); }
    ~VariantHolder() {
        auto hr = VariantClear(getPtr());
        switch (hr) {
        case S_OK:
            break;
        case DISP_E_ARRAYISLOCKED:
            throw std::runtime_error("VariantClear failed on variant "
                                     "destruction: Variant "
                                     "contains an array that is locked");
            break;
        case DISP_E_BADVARTYPE:
            throw std::runtime_error("VariantClear failed on variant "
                                     "destruction: Variant is "
                                     "not a valid type");
            break;
        case E_INVALIDARG:
            // shouldn't happen.
            throw std::invalid_argument("VariantClear failed on variant "
                                        "destruction: invalid "
                                        "argument.");
            break;
        }
    }
    /// non-copyable
    VariantHolder(VariantHolder const &) = delete;
    /// non-assignable
    VariantHolder &operator=(VariantHolder const &) = delete;
    T &get() { return data_; }
    T const &get() const { return data_; }
    T *getPtr() { return &data_; }
    T const *getPtr() const { return &data_; }

  private:
    T data_;
};

} // namespace detail

/// @brief A wrapper for VARIANT/VARIANTARG on Windows that is both copy and
/// move aware, and does not require any Visual Studio libraries (unlike
/// comutil.h)
template <typename T = VARIANT> class VariantWrapper {

  public:
    static_assert(detail::is_variant_type<T>::value,
                  "Only valid type parameters are VARIANT or VARIANTARG");
    using variant_type = T;
    using type = VariantWrapper<T>;
    using holder_type = detail::VariantHolder<T>;
    using holder_unique_ptr = typename holder_type::unique_type;

    /// @brief Default constructor - sets up an empty variant
    VariantWrapper() : data_(holder_type::make_unique()) {}

    /// @brief Constructor from nullptr - does not allocate a variant.
    explicit VariantWrapper(std::nullptr_t) {}

    /// @brief Copy constructor - copies a variant (without following
    /// indirection of VT_BYREF)
    VariantWrapper(VariantWrapper const &other) : VariantWrapper() {
        if (!other) {
            throw std::logic_error("On variant copy-construction: tried to "
                                   "copy from a variant in an invalid state "
                                   "(moved-from)!");
        }
        (*this) = other;
    }

    /// @brief Move constructor - moves the variant out of the other object,
    /// leaving it as if initialized with nullptr.
    VariantWrapper(VariantWrapper &&other) : data_(std::move(other.data_)) {}

    /// @brief Move-assignment - moves the variant out of the other object,
    /// leaving it as if initialized with nullptr
    type &operator=(type &&other) {
        dealloc();
        data_ = std::move(other.data_);
        return *this;
    }

    /// @brief Copy-assignment - copies a variant (without following indirection
    /// of VT_BYREF)
    type &operator=(VariantWrapper const &other) {
        if (!other) {
            throw std::logic_error("On variant copy-assignment: tried to copy "
                                   "from a variant in an invalid state "
                                   "(moved-from)!");
        }
        ensureInit();
        auto hr = VariantCopy(getPtr(), other.getPtr());
        switch (hr) {
        case S_OK:
            break;
        case DISP_E_ARRAYISLOCKED:
            throw std::runtime_error("VariantCopy failed on variant "
                                     "copy-construction/assignment: Variant "
                                     "contains an array that is locked");
            break;
        case DISP_E_BADVARTYPE:
            throw std::runtime_error("VariantCopy failed on variant "
                                     "copy-construction/assignment: Variant is "
                                     "not a valid type");
            break;
        case E_INVALIDARG:
            throw std::invalid_argument("VariantCopy failed on variant "
                                        "copy-construction/assignment: invalid "
                                        "argument.");
            break;
        case E_OUTOFMEMORY:
            throw std::runtime_error("VariantCopy failed on variant "
                                     "copy-construction/assignment: "
                                     "insufficient memory.");
            break;
        }
        return *this;
    }

    /// @brief Checks to see if this variant is in a valid state: not moved-from
    /// or initialized with nullptr.
    explicit operator bool() const { return bool(data_); }

    /// @brief Ensures the variant is initialized.
    void ensureInit() {
        if (!data_) {
            data_ = holder_type::make_unique();
        }
    }

    /// @brief Uncommon: de-allocates the variant. Usually you just let the
    /// destructor do this.
    void dealloc() { data_.reset(); }

    /// @name Access to the underlying VARIANT/VARIANTARG
    /// @{
    variant_type &get() { return data_->get(); }
    variant_type const &get() const { return data_->get(); }
    variant_type *getPtr() { return data_->getPtr(); }
    variant_type const *getPtr() const { return data_->getPtr(); }
    /// @}

  private:
    holder_unique_ptr data_;
};

/// @brief Function similar to AttachPtr, but for variants.
template <typename T> inline T *AttachVariant(VariantWrapper<T> &v) {
    return v.getPtr();
}

using WinVariant = VariantWrapper<VARIANT>;
using WinVariantArg = VariantWrapper<VARIANTARG>;

/// @brief Determines if the type of data in the variant can be described as the
/// type parameter @p Dest (without coercion)
template <typename Dest, typename T>
inline detail::enable_for_variants_t<T, bool> contains(T const &v) {
    return (v.vt == detail::VariantTypeTraits<Dest>::vt);
}
/// @overload
/// For wrapped variants.
template <typename Dest, typename T>
inline bool contains(VariantWrapper<T> const &v) {
    return v && contains<Dest>(v.get());
}

/// @brief Determines if the variant passed is "empty"
template <typename T>
inline detail::enable_for_variants_t<T, bool> is_empty(T const &v) {
    return (v.vt == VT_EMPTY);
}

/// @overload
/// For wrapped variants. Also returns true if the wrapper itself is empty.
template <typename T> inline bool is_empty(VariantWrapper<T> const &v) {
    return (!v) || is_empty(v.get());
}

/// @brief Get the data of type @p Dest from the variant: no conversion/coercion
/// applied.
template <typename Dest, typename T>
inline detail::enable_for_variants_t<T, Dest> get(T const &v) {
    if (!contains<Dest>(v)) {
        throw std::invalid_argument("Variant does not contain the type of data "
                                    "you are trying to access!");
    }
    return detail::VariantTypeTraits<Dest>::get(v);
}

/// @overload
/// For wrapped variants.
template <typename Dest, typename T>
inline Dest get(VariantWrapper<T> const &v) {
    if (!contains<Dest>(v)) {
        throw std::invalid_argument("Variant does not contain the type of data "
                                    "you are trying to access!");
    }
    return get<Dest>(v.get());
}

#endif // _WIN32

#endif // INCLUDED_WinVariant_h_GUID_A0A043D9_A146_4693_D351_06F6A04ABADA
