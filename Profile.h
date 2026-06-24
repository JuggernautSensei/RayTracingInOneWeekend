#pragma once
#include "Timer.h"

class ScopedProfile
{
public:
    explicit ScopedProfile(std::string_view _name, eTimeUnit _prefix = eTimeUnit::Ms);
    ~ScopedProfile();

    ScopedProfile(const ScopedProfile&)                = delete;
    ScopedProfile& operator=(const ScopedProfile&)     = delete;
    ScopedProfile(ScopedProfile&&) noexcept            = delete;
    ScopedProfile& operator=(ScopedProfile&&) noexcept = delete;

private:
    inline static Int32 s_spacing = 0;

    Timer            m_timer  = {};
    eTimeUnit        m_prefix = eTimeUnit::Ms;
    std::string_view m_name   = {};
};

#define PRETTY_FUNCTION __FUNCSIG__

#define CONCAT_(_x, _y) _x##_y
#define CONCAT(_x, _y)  CONCAT_(_x, _y)

#ifndef PROFILE_ENABLED
#    ifdef DEBUG_BUILD
#        define PROFILE_ENABLED 1
#    else
#        define PROFILE_ENABLED 0
#    endif
#endif

#define PROFILE_TIME_UNIT_SEC 0
#define PROFILE_TIME_UNIT_MS  1
#define PROFILE_TIME_UNIT_US  2
#define PROFILE_TIME_UNIT_NS  3

#ifndef PROFILE_TIME_UNIT
#    define PROFILE_TIME_UNIT PROFILE_TIME_UNIT_MS
#endif

#if (PROFILE_ENABLED == 1)

#    define SCOPED_PROFILE_UNIT(_name, _unit) \
        ScopedProfile CONCAT(profile_, __LINE__) { _name, _unit }

#    define SCOPED_PROFILE(_name) \
        SCOPED_PROFILE_UNIT(_name, static_cast<eTimeUnit>(PROFILE_TIME_UNIT))

#    define FUNCTION_PROFILE_UNIT(_unit) \
        SCOPED_PROFILE_UNIT(PRETTY_FUNCTION, _unit)

#    define FUNCTION_PROFILE() \
        SCOPED_PROFILE(PRETTY_FUNCTION)

#else

#    define SCOPED_PROFILE_UNIT(_name, _unit) __noop
#    define SCOPED_PROFILE(_name)             __noop
#    define FUNCTION_PROFILE_UNIT(_unit)      __noop
#    define FUNCTION_PROFILE()                __noop

#endif