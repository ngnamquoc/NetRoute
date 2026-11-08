#pragma once // tells the compiler's preprocessing system to include this header file only once per translation unit
#include <array>
#include <cstdint>
#include <optional>
#include <span>

struct EthernetHeader
{
    std::array<unsigned char, 6> destination{}; // fix-sized arr, 1 char=1 byte,unsigned char has range 0 to 2^8-1
    std::array<unsigned char, 6> source{};
    std::uint16_t type_or_length{}; // 16 bits=2bytes, range from 0 to 2^16-1
};
/** take raw packet data and try to extract an Ethernet header */
std::optional<EthernetHeader> parse_ethernet(
    std::span<const unsigned char> packet // span is used to reference the original array
);