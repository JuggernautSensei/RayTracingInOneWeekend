#include "pch.h"
#include "Random.h"

namespace
{

UInt64 MakeRandomDeviceSeed() noexcept
{
    std::random_device rd = {};
    return (static_cast<UInt64>(rd()) << 32) | rd();
}

}   // namespace

RandomGeneratorSHR3::RandomGeneratorSHR3(
    const UInt64 _seed) noexcept
    : m_jsr(_seed)
{
}

RandomGeneratorSHR3::RandomGeneratorSHR3(
    RandomDeviceSeed) noexcept
    : m_jsr(MakeRandomDeviceSeed())
{
}

void RandomGeneratorSHR3::Reseed(
    const UInt64 _seed) noexcept
{
    m_jsr = _seed;
}

void RandomGeneratorSHR3::Reseed(
    RandomDeviceSeed) noexcept
{
    m_jsr = MakeRandomDeviceSeed();
}

[[nodiscard]] UInt64 RandomGeneratorSHR3::Generate() noexcept
{
    m_jsr ^= (m_jsr << 13);
    m_jsr ^= (m_jsr >> 7);
    m_jsr ^= (m_jsr << 17);
    return m_jsr;
}

RandomGeneratorMWC::RandomGeneratorMWC(
    const UInt64 _seedZ,
    const UInt64 _seedW) noexcept
    : m_z(_seedZ)
    , m_w(_seedW)
{
}

RandomGeneratorMWC::RandomGeneratorMWC(
    RandomDeviceSeed) noexcept
    : m_z(MakeRandomDeviceSeed())
    , m_w(MakeRandomDeviceSeed())
{
}

void RandomGeneratorMWC::Reseed(
    const UInt64 _seedZ,
    const UInt64 _seedW) noexcept
{
    m_z = _seedZ;
    m_w = _seedW;
}

void RandomGeneratorMWC::Reseed(
    RandomDeviceSeed) noexcept
{
    m_z = MakeRandomDeviceSeed();
    m_w = MakeRandomDeviceSeed();
}

UInt64 RandomGeneratorMWC::Generate() noexcept
{
    m_z = 698769069ULL * (m_z & 0xFFFFFFFFULL) + (m_z >> 32);
    m_w = 362436069ULL * (m_w & 0xFFFFFFFFULL) + (m_w >> 32);
    return (m_z << 32) + m_w;
}

RandomGeneratorMT19937::RandomGeneratorMT19937(
    const UInt64 _seed) noexcept
    : m_mt(_seed)
{
}

RandomGeneratorMT19937::RandomGeneratorMT19937(
    RandomDeviceSeed) noexcept
    : m_mt(MakeRandomDeviceSeed())
{
}

void RandomGeneratorMT19937::Reseed(
    const UInt64 _seed) noexcept
{
    m_mt.seed(_seed);
}

void RandomGeneratorMT19937::Reseed(
    RandomDeviceSeed) noexcept
{
    m_mt = std::mt19937_64 { MakeRandomDeviceSeed() };
}

UInt64 RandomGeneratorMT19937::Generate() noexcept
{
    return m_mt();
}
