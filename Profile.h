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


#ifndef PROFILE_ENABLED
#    ifdef DEBUG_BUILD
#        define PROFILE_ENABLED 1
#    else
#        define PROFILE_ENABLED 0
#    endif
#endif

#define PROFILE_TIME_UNIT_SECOND 0
#define PROFILE_TIME_UNIT_MILLI  1
#define PROFILE_TIME_UNIT_MICRO  2
#define PROFILE_TIME_UNIT_NANOS  3

#ifndef PROFILE_TIME_UNIT
#    define PROFILE_TIME_UNIT PROFILE_TIME_UNIT_MILLI
#endif

#if (PROFILE_ENABLED == 1)

#    define SCOPED_PROFILE(_name) \
        Jam::ScopedProfile CONCAT(profile_, __LINE__) { _name, static_cast<Jam::eSIPrefix>(PROFILE_TIME_UNIT) }

#    define FUNCTION_PROFILE() \
        SCOPED_PROFILE(PRETTY_FUNCTION)

#else

#    define SCOPED_PROFILE(_name) NOOP

#endif