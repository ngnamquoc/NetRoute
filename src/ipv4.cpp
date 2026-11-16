#include "ipv4.hpp"

std::optional<IPv4Header> parse_ipv4(
    std::span<const unsigned char> packet)
{
    constexpr std::size_t minimum_header_size = 20; // 160/8=20 bytes

    // Check the length before accessing any fields.
    if (packet.size() < minimum_header_size)
    {
        return std::nullopt;
    }

    // First byte: upper 4 bits = version, lower 4 bits = IHL.
    const unsigned int version = packet[0] >> 4; // e.g. 0100 0101 -> 0000 0100
    // 0x0F is 00001111 in binary. The bitwise AND operation keeps only the last four bits
    const unsigned int ihl = packet[0] & 0x0F; // Internet Header Length

    if (version != 4 || ihl < 5)
    {
        return std::nullopt;
    }

    // IHL counts groups of four bytes.
    const auto header_length =
        static_cast<std::uint8_t>(ihl * 4);

    // The capture must contain the entire header, including options.
    if (packet.size() < header_length)
    {
        return std::nullopt;
    }

    // IPv4 stores multibyte numbers with the high byte first.
    const auto total_length = static_cast<std::uint16_t>(
        (static_cast<unsigned int>(packet[2]) << 8) | static_cast<unsigned int>(packet[3]));

    // The total packet cannot be shorter than its own header.
    if (total_length < header_length)
    {
        return std::nullopt;
    }

    IPv4Header header{};
    header.header_length = header_length;
    header.total_length = total_length;
    header.ttl = packet[8];
    header.protocol = packet[9];

    for (std::size_t i = 0; i < 4; ++i)
    {
        header.source[i] = packet[12 + i];
        header.destination[i] = packet[16 + i];
    }

    return header;
}