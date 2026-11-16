#pragma once

#include <array>
#include <cstdint>
#include <optional>
#include <span>

struct IPv4Header
{
    std::array<unsigned char, 4> source{};
    std::array<unsigned char, 4> destination{};
    std::uint8_t header_length{}; // 8 bits, initialize to 0
    std::uint16_t total_length{}; // IPv4 header + IPv4 payload
    std::uint8_t ttl{};
    std::uint8_t protocol{};
};

std::optional<IPv4Header> parse_ipv4(
    std::span<const unsigned char> packet // packet is a span containing both a pointer to the first byte and the number of bytes.
);