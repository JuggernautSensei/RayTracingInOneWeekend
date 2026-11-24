#include "pch.h"

#include "Timer.h"

double NsToDouble(
    const Int64     _ns,
    const eTimeUnit _unit)
{
    switch (_unit)
    {
        case eTimeUnit::Ns: return static_cast<double>(_ns);
        case eTimeUnit::Us: return static_cast<double>(_ns) * 1e-3;
        case eTimeUnit::Ms: return static_cast<double>(_ns) * 1e-6;
        case eTimeUnit::Sec: return static_cast<double>(_ns) * 1e-9;
        default:
            UNREACHABLE;
    }
}

float NsToFloat(
    const Int64     _ns,
    const eTimeUnit _unit)
{
    switch (_unit)
    {
        case eTimeUnit::Ns:
            return static_cast<float>(_ns);
        case eTimeUnit::Us:
            return static_cast<float>(_ns) * 1e-3f;
        case eTimeUnit::Ms:
            return static_cast<float>(_ns) * 1e-6f;
        case eTimeUnit::Sec:
            return static_cast<float>(_ns) * 1e-9f;
        default:
            UNREACHABLE;
    }
}

void Timer::Start()
{
    ASSERT(!m_bRunning, "Timer is already running. Call Stop() first.");
    m_startTimePoint = std::chrono::steady_clock::now();
    m_bRunning     = true;
}

void Timer::Stop()
{
    ASSERT(m_bRunning, "Timer is not running. Call Init() first.");
    const auto counter = std::chrono::steady_clock::now();
    m_totalNs += std::chrono::duration_cast<std::chrono::nanoseconds>(counter - m_startTimePoint).count();
    m_bRunning = false;
}

void Timer::Lap()
{
    ASSERT(m_bRunning, "Timer is not running. Call Init() first.");
    const auto endCounter = std::chrono::steady_clock::now();
    m_intervalNs          = std::chrono::duration_cast<std::chrono::nanoseconds>(endCounter - m_startTimePoint).count();
    m_startTimePoint        = endCounter;
    m_totalNs += m_intervalNs;
}

void Timer::Reset()
{
    m_totalNs    = 0;
    m_intervalNs = 0;
    m_bRunning   = false;
}

void Timer::Restart()
{
    Reset();
    Start();
}

bool Timer::IsRunning() const
{
    return m_bRunning;
}

double Timer::GetTotal(const eTimeUnit _unit) const
{
    return NsToDouble(m_totalNs, _unit);
}

float Timer::GetTotalFloat(
    const eTimeUnit _unit) const
{
    return NsToFloat(m_totalNs, _unit);
}

double Timer::GetInterval(const eTimeUnit _unit) const
{
    return NsToDouble(m_intervalNs, _unit);
}

float Timer::GetIntervalFloat(
    const eTimeUnit _unit) const
{
    return NsToFloat(m_intervalNs, _unit);
}

Int64 Timer::GetTotalNs() const
{
    return m_totalNs;
}

Int64 Timer::GetIntervalNs() const
{
    return m_intervalNs;
}

TimeStamp TimeStamp::Current()
{
    using namespace std::chrono;
    const auto        now = system_clock::now();
    const auto        ms  = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;
    const std::time_t t   = system_clock::to_time_t(now);

    std::tm   tm    = {};
    TimeStamp stamp = {};
    if (::localtime_s(&tm, &t) == 0)
    {
        stamp.year        = 1900 + tm.tm_year;
        stamp.month       = 1 + tm.tm_mon;
        stamp.day         = tm.tm_mday;
        stamp.hour        = tm.tm_hour;
        stamp.min      = tm.tm_min;
        stamp.src      = tm.tm_sec;
        stamp.ms = static_cast<int>(ms.count());
    }

    return stamp;
}
