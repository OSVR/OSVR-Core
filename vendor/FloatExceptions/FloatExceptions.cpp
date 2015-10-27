// Copyright (c) 2012 Cygnus Software
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy of
// this software and associated documentation files (the "Software"), to deal in
// the Software without restriction, including without limitation the rights to use,
// copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the
// Software, and to permit persons to whom the Software is furnished to do so,
// subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
// FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
// COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
// IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//

// For more details see the original blog post at:
// http://randomascii.wordpress.com/2012/04/21/exceptional-floating-point/

#include <stdio.h> // For printf
#include <Windows.h> // For structured exception handling support functions.
#include <float.h> // For _clearfp and _controlfp_s

// Declare an object of this type in a scope in order to suppress
// all floating-point exceptions temporarily. The old exception
// state will be reset at the end.
// From http://randomascii.wordpress.com/2012/04/21/exceptional-floating-point/
class FPExceptionDisabler
{
public:
    FPExceptionDisabler()
    {
        // Retrieve the current state of the exception flags. This
        // must be done before changing them.
        _controlfp_s(&mOldValues, 0, 0);
        // Set all of the exception flags, which suppresses FP
        // exceptions on the x87 and SSE units. _MCW_EM is a bit
        // mask representing all available exception masks.
        _controlfp_s(0, _MCW_EM, _MCW_EM);
    }
    ~FPExceptionDisabler()
    {
        // Clear any pending FP exceptions. This must be done
        // prior to enabling FP exceptions since otherwise there
        // may be a 'deferred crash' as soon the exceptions are
        // enabled.
        _clearfp();

        // Reset (possibly enabling) the exception status.
        _controlfp_s(0, mOldValues, _MCW_EM);
    }

private:
    unsigned int mOldValues;

    // Make the copy constructor and assignment operator private
    // and unimplemented to prohibit copying.
    FPExceptionDisabler(const FPExceptionDisabler&);
    FPExceptionDisabler& operator=(const FPExceptionDisabler&);
};

// Declare an object of this type in a scope in order to enable a
// specified set of floating-point exceptions temporarily. The old
// exception state will be reset at the end.
// This class can be nested.
// From http://randomascii.wordpress.com/2012/04/21/exceptional-floating-point/
class FPExceptionEnabler
{
public:
    // Overflow, divide-by-zero, and invalid-operation are the FP
    // exceptions most frequently associated with bugs.
    FPExceptionEnabler(unsigned int enableBits = _EM_OVERFLOW |
                _EM_ZERODIVIDE | _EM_INVALID)
    {
        // Retrieve the current state of the exception flags. This
        // must be done before changing them.
        _controlfp_s(&mOldValues, 0, 0);

        // Make sure no non-exception flags have been specified,
        // to avoid accidental changing of rounding modes, etc.
        enableBits &= _MCW_EM;

        // Clear any pending FP exceptions. This must be done
        // prior to enabling FP exceptions since otherwise there
        // may be a 'deferred crash' as soon the exceptions are
        // enabled. _MCW_EM is a bit
        // mask representing all available exception masks.
        _clearfp();

        // Zero out the specified bits, leaving other bits alone.
        _controlfp_s(0, ~enableBits, enableBits);
    }
    ~FPExceptionEnabler()
    {
        // Reset the exception state.
        _controlfp_s(0, mOldValues, _MCW_EM);
    }

private:
    unsigned int mOldValues;

    // Make the copy constructor and assignment operator private
    // and unimplemented to prohibit copying.
    FPExceptionEnabler(const FPExceptionEnabler&);
    FPExceptionEnabler& operator=(const FPExceptionEnabler&);
};

int __cdecl DescribeException(PEXCEPTION_POINTERS pData, const char *pFunction)
{
    // Clear the exception or else every FP instruction will
    // trigger it again.
    _clearfp();

    DWORD exceptionCode = pData->ExceptionRecord->ExceptionCode;
    const char* pDescription = NULL;
    switch (exceptionCode)
    {
    case STATUS_FLOAT_INVALID_OPERATION:
        pDescription = "float invalid operation";
        break;
    case STATUS_FLOAT_DIVIDE_BY_ZERO:
        pDescription = "float divide by zero";
        break;
    case STATUS_FLOAT_OVERFLOW:
        pDescription = "float overflow";
        break;
    case STATUS_FLOAT_UNDERFLOW:
        pDescription = "float underflow";
        break;
    case STATUS_FLOAT_INEXACT_RESULT:
        pDescription = "float inexact result";
        break;
    case STATUS_FLOAT_MULTIPLE_TRAPS:
        // This seems to occur with SSE code.
        pDescription = "float multiple traps";
        break;
    default:
        pDescription = "unknown exception";
        break;
    }

    void* pErrorOffset = 0;
#if defined(_M_IX86)
    void* pIP = (void*)pData->ContextRecord->Eip;
    pErrorOffset = (void*)pData->ContextRecord->FloatSave.ErrorOffset;
#elif defined(_M_X64)
    void* pIP = (void*)pData->ContextRecord->Rip;
#else
    #error Unknown processor
#endif

    printf("Crash with exception %x (%s) in %s at %p!\n",
            exceptionCode, pDescription, pFunction, pIP);

    if (pErrorOffset)
    {
        // Float exceptions may be reported in a delayed manner -- report the
        // actual instruction as well.
        printf("Faulting instruction may actually be at %p.\n", pErrorOffset);
    }

    // Return this value to execute the __except block and continue as if
    // all was fine, which is a terrible idea in shipping code.
    return EXCEPTION_EXECUTE_HANDLER;
    // Return this value to let the normal exception handling process
    // continue after printing diagnostics/saving crash dumps/etc.
    //return EXCEPTION_CONTINUE_SEARCH;
}

static float g_zero = 0;

void TryDivByZero()
{
    __try
    {
        float inf = 1.0f / g_zero;
        printf("No crash encountered, we successfully calculated %f.\n", inf);
    }
    __except(DescribeException(GetExceptionInformation(), __FUNCTION__))
    {
        // Do nothing here - DescribeException() has already done
        // everything that is needed.
    }
}

int main(int argc, char* argv[])
{
#if _M_IX86_FP == 0
    const char* pArch = "with the default FPU architecture";
#elif _M_IX86_FP == 1
    const char* pArch = "/arch:sse";
#elif _M_IX86_FP == 2
    const char* pArch = "/arch:sse2";
#else
#error Unknown FP architecture
#endif
    printf("Code is compiled for %d bits, %s.\n", sizeof(void*) * 8, pArch);

    // Do an initial divide-by-zero.
    // In the registers window if display of Floating Point
    // is enabled then the STAT register will have 4 ORed
    // into it, and the floating-point section's EIP register
    // will be set to the address of the instruction after
    // the fdiv.
    printf("\nDo a divide-by-zero in the default mode.\n");
    TryDivByZero();
    {
        // Now enable the default set of exceptions. If the
        // enabler object doesn't call _clearfp() then we
        // will crash at this point.
        FPExceptionEnabler enabled;
        printf("\nDo a divide-by-zero with FP exceptions enabled.\n");
        TryDivByZero();
        {
            // Now let's disable exceptions and do another
            // divide-by-zero.
            FPExceptionDisabler disabled;
            printf("\nDo a divide-by-zero with FP exceptions disabled.\n");
            TryDivByZero();
        }
    }

    return 0;
}
