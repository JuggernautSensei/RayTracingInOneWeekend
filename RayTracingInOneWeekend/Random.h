#pragma once
#include <random>

namespace Details
{

[[nodiscard]] UInt64 MakeRandomDeviceSeed() noexcept;

}   // namespace Details

struct RandomDeviceSeed
{
    struct Tag
    {
    };

    explicit constexpr RandomDeviceSeed(Tag) {}
};

inline constexpr RandomDeviceSeed kRandomDeviceSeed { RandomDeviceSeed::Tag {} };

// George Marsaglia's SHR3 (fastest)
class RandomGeneratorSHR3
{
public:
    explicit RandomGeneratorSHR3(UInt64 _seed = 342211234567ULL) noexcept;
    explicit RandomGeneratorSHR3(RandomDeviceSeed) noexcept;

    void                 Reseed(UInt64 _seed = 342211234567ULL) noexcept;
    void                 Reseed(RandomDeviceSeed) noexcept;
    [[nodiscard]] UInt64 Generate() noexcept;

private:
    UInt64 m_jsr = 0;
};

// George Marsaglia's MWC (more random)
class RandomGeneratorMWC
{
public:
    explicit RandomGeneratorMWC(UInt64 _seedZ = 123456789123ULL, UInt64 _seedW = 654356789123ULL) noexcept;
    explicit RandomGeneratorMWC(RandomDeviceSeed) noexcept;

    void                 Reseed(UInt64 _seedZ = 123456789123ULL, UInt64 _seedW = 654356789123ULL) noexcept;
    void                 Reseed(RandomDeviceSeed) noexcept;
    [[nodiscard]] UInt64 Generate() noexcept;

private:
    UInt64 m_z = 0;
    UInt64 m_w = 0;
};

// Mersenne Twister (highest quality)
class RandomGeneratorMT19937
{
public:
    explicit RandomGeneratorMT19937(UInt64 _seed = 123456789ULL) noexcept;
    explicit RandomGeneratorMT19937(RandomDeviceSeed) noexcept;

    void                 Reseed(UInt64 _seed = 123456789ULL) noexcept;
    void                 Reseed(RandomDeviceSeed) noexcept;
    [[nodiscard]] UInt64 Generate() noexcept;

private:
    std::mt19937_64 m_mt;
};

template<typename T>
concept IsRng = requires(T t) {
    { t.Generate() } -> std::same_as<UInt64>;
};

// Returns a random number in the range of [_min, _max].
template<typename T, typename TRng>
    requires IsRng<TRng> && std::is_arithmetic_v<T>
[[nodiscard]] T MakeRandom(TRng& _rng, const T _min, const T _max) noexcept
{
    ASSERT(_min <= _max, "Invalid range! min: {}, max: {}", _min, _max);

    if (_min == _max)
    {
        return _min;
    }

    if constexpr (std::is_integral_v<T>)
    {
        const UInt64 range = static_cast<UInt64>(_max - _min) + 1;
        return static_cast<T>(_min + static_cast<T>(_rng.Generate() % range));
    }
    else
    {
        using FloatT     = std::conditional_t<std::is_same_v<T, float>, float, double>;
        const FloatT f01 = static_cast<FloatT>(_rng.Generate()) / static_cast<FloatT>(Max<UInt64>());
        return static_cast<T>(_min + f01 * (_max - _min));
    }
}
