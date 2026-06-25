#include "pch.h"
#include "Profile.h"

namespace
{
[[nodiscard]] std::string_view ToTimeUnitString_(
    const eTimeUnit _prefix)
{
    switch (_prefix)
    {
        case eTimeUnit::Sec:
            return "s";
        case eTimeUnit::Ms:
            return "ms";
        case eTimeUnit::Us:
            return "us";
        case eTimeUnit::Ns:
            return "ns";
        default:
            UNREACHABLE;
    }
}

}   // namespace

ScopedProfile::ScopedProfile(
    const std::string_view _name,
    const eTimeUnit        _prefix)
    : m_prefix(_prefix)
    , m_name(_name)
{
    const std::string spacing(s_spacing * 2, ' ');
    std::cout << std::format("[Profile] {}▶ {}\n", spacing, m_name);

    ++s_spacing;
    m_timer.Start();
}

ScopedProfile::~ScopedProfile()
{
    m_timer.Stop();
    --s_spacing;

    const double           elapsed = m_timer.GetTotal(m_prefix);
    const std::string_view unit    = ToTimeUnitString_(m_prefix);
    const std::string      spacing(s_spacing * 2, ' ');
    std::cout << std::format("[Profile] {}◀ {:.3f} {}\n", spacing, elapsed, unit);
}
