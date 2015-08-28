/** @file
    @brief Header providing for usage of boost::intrusive_ptr with COM.

    Based on the header found at:
    <https://github.com/rengeln/nyx/blob/master/src/Prefix.h>

    It is essentially the code from the author's blog post,
    <http://code-freeze.blogspot.com/2012/02/using-intrusiveptr-with-com.html>
    , which describes usage and motivation more. However, it appears to have
    some small improvements (mainly in style, and C++11 usage), and usefully,
    it has a standard license associated with it.

    @date 2011

    @author
    Robert Engeln <engeln@gmail.com>
*/

// Excerpted from <https://github.com/rengeln/nyx/blob/master/src/Prefix.h>
//
// Original header, then accompanying LICENSE file (which matches the MIT
// license as found at http://opensource.org/licenses/mit-license) are
// included


#ifndef INCLUDED_intrusive_ptr_COM_h_GUID_BB97FC94_B320_4FB9_5379_940B0A631CA6
#define INCLUDED_intrusive_ptr_COM_h_GUID_BB97FC94_B320_4FB9_5379_940B0A631CA6

// If we haven't yet included windows.h, don't make COM do it.
#if !defined(COM_NO_WINDOWS_H) && !defined(_WINDOWS_)
#define COM_NO_WINDOWS_H
#endif
#include <unknwn.h>
#include <boost/intrusive_ptr.hpp>


//
//  Enable intrusive_ptr to work with COM objects.
//
inline void intrusive_ptr_add_ref(IUnknown* ptr)
{
    ptr->AddRef();
}

inline void intrusive_ptr_release(IUnknown* ptr)
{
    ptr->Release();
}

namespace detail {
//
//  Enable intrusive_ptr objects to be passed as parameters to functions expecting
//  a pointer-to-pointer which will be initialized by the function.
//
template <typename T>
class IntrusivePtrWrapper
{
public:
    //
    //  Constructor.
    //
    //  Parameters:
    //      [in] ref
    //          Intrusive_ptr object to wrap.
    //      [in] addRef
    //          If true then the ptr's refcount will be incremented on acquisition.
    //
    IntrusivePtrWrapper(boost::intrusive_ptr<T>& ref, bool addRef)
        : m_ref(ref), m_ptr(nullptr), m_addRef(addRef)
    {
    }

    //
    //  Destructor.
    //
    ~IntrusivePtrWrapper()
    {
        m_ref = boost::intrusive_ptr<T>(m_ptr, m_addRef);
    }

    //
    // Assignment operator: deleted
    //
    IntrusivePtrWrapper & operator=(IntrusivePtrWrapper const &) = delete;

    //
    //  Implicit conversion to T**.
    //
    operator T**()
    {
        return &m_ptr;
    }

	//
	//  Implicit conversion to T*.
	//
    operator T*()
    {
        return m_ptr;
    }

    //
    //  Implicit conversion to void**.
    //
    operator void**()
    {
        return reinterpret_cast<void**>(&m_ptr);
    }

private:
    //
    //  Properties.
    //
    boost::intrusive_ptr<T>& m_ref;
    T* m_ptr;
    bool m_addRef;
};


//
//  Helper function for constructing an IntrusivePtrWrapper.
//
//  Parameters:
//      [in] ref
//          Intrusive_ptr object to wrap.
//      [in] addRef
//          If true then the ptr's refcount will be incremented on acquisition.
//          Default is false, which matches the behavior of COM functions.
//
template <typename T>
inline IntrusivePtrWrapper<T> AttachPtr(boost::intrusive_ptr<T>& ref, bool addRef = false)
{
    return IntrusivePtrWrapper<T>(ref, addRef);
}

} // namespace detail


using detail::AttachPtr;

#endif // INCLUDED_intrusive_ptr_COM_h_GUID_BB97FC94_B320_4FB9_5379_940B0A631CA6
