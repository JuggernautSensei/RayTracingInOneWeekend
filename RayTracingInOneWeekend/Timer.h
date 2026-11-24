#pragma once

enum class eTimeUnit
{
    Sec,
    Ms,
    Us,
    Ns,
};

[[nodiscard]] double NsToDouble(Int64 _ns, eTimeUnit _unit);
[[nodiscard]] float  NsToFloat(Int64 _ns, eTimeUnit _unit);

class Timer
{
public:
    void Start();
    void Stop();
    void Lap();
    void Reset();
    void Restart();

    [[nodiscard]] bool   IsRunning() const;
    [[nodiscard]] double GetTotal(eTimeUnit _unit) const;
    [[nodiscard]] float  GetTotalFloat(eTimeUnit _unit) const;
    [[nodiscard]] double GetInterval(eTimeUnit _unit) const;
    [[nodiscard]] float  GetIntervalFloat(eTimeUnit _unit) const;
    [[nodiscard]] Int64  GetTotalNs() const;
    [[nodiscard]] Int64  GetIntervalNs() const;

protected:
    std::chrono::steady_clock::time_point m_startTimePoint = {};

    Int64 m_totalNs    = 0;
    Int64 m_intervalNs = 0;
    bool  m_bRunning   = false;
};

// time utilities
struct TimeStamp
{
    [[nodiscard]] static TimeStamp Current();

    Int32 year  = 0;
    Int32 month = 0;
    Int32 day   = 0;
    Int32 hour  = 0;
    Int32 min   = 0;
    Int32 src   = 0;
    Int32 ms    = 0;
};
