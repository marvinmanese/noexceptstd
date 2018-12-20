#ifndef GUARD_NOE_STD_debug_H
#define GUARD_NOE_STD_debug_H

#ifndef NOE_NO_DEBUG
#include <iostream>
#endif // NOE_NO_DEBUG

namespace noe_std
{
#ifndef NOE_NO_DEBUG
namespace debug
{
    template<class... Args>
    void log(Args&&... args)
    {
        using expander = int[];
        expander { 0, (std::cout << std::forward<Args>(args), void(), 0)...  };
        std::cout << std::endl;
    }
}
#endif // NOE_NO_DEBUG
}

#ifndef NOE_NO_DEBUG
#define NOE_DEBUG_LOG(args...) { noe_std::debug::log(args); }
#else
#define NOE_DEBUG_LOG(...) {}
#endif // NOE_NO_DEBUG

#endif // GUARD_NOE_STD_debug_H
