#pragma once

#ifndef ASSERT_ENABLED
#    ifdef DEBUG_BUILD
#        define ASSERT_ENABLED 1
#    else
#        define ASSERT_ENABLED 0
#    endif
#endif

#if (ASSERT_ENABLED == 1)

#    define ASSERT(_cond, ...)                                              \
        do                                                                  \
        {                                                                   \
            if (!(_cond))                                                   \
            {                                                               \
                std::cerr << std::format("Assertion failed: " __VA_ARGS__); \
                std::abort();                                               \
            }                                                               \
        } while (0)

#else

#    define ASSERT(_cond, ...) __noop

#endif

#define UNREACHABLE                                 \
    do                                              \
    {                                               \
        std::cerr << "Unreachable code reached.\n"; \
        std::terminate();                           \
    } while (0)