#include "Hash.h"

/****************************************************************************//*!
@brief Implementations the FNV-1a hashing algorithm.
The fnv-1a implementation provides better avalanche characteristics
against the fnv-1.
The FNV hash was designed for fast hash tableand checksum use, not cryptography.
Find out more
https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function

@return returns the hashed fnv-1a output.
*//*****************************************************************************/

namespace util
{
    //inline constexpr size_t StringHash::const_strlen(const char* s)
    //{
    //    size_t size = 0;
    //    while (s[size]) { size++; };
    //    return size;
    //}
    //
    //inline constexpr StringHash::size_type StringHash::GenerateFNV1aHash(const char* str)
    //{
    //    // Also C++ does not like static constexpr
    //    constexpr size_type FNV_PRIME = 16777619u;
    //    constexpr size_type OFFSET_BASIS = 2166136261u;
    //
    //    const size_t length = const_strlen(str) + 1;
    //    size_type hash = OFFSET_BASIS;
    //    for (size_t i = 0; i < length; ++i)
    //    {
    //        hash ^= *str++;
    //        hash *= FNV_PRIME;
    //    }
    //    return hash;
    //}
    //
    //inline constexpr StringHash::size_type StringHash::GenerateFNV1aHash(std::string_view string)
    //{
    //    return GenerateFNV1aHash(string.data());
    //}

    constexpr StringHash::StringHash(size_type hash) noexcept : computedHash(hash) {}

    constexpr StringHash::StringHash(const char* s) noexcept : computedHash(0)
    {
        computedHash = GenerateFNV1aHash(s);
    }

    constexpr StringHash::StringHash(std::string_view s) noexcept : computedHash(0)
    {
        computedHash = GenerateFNV1aHash(s.data());
    }

    constexpr StringHash::operator size_type() noexcept
    {
        return computedHash;
    }

    constexpr bool StringHash::operator==(const StringHash other) noexcept
    {
        return computedHash == other.computedHash;
    }
}